#include "RenderTexture.h"                                                   

#include <GL/glut.h>

#ifdef USE_CG
#include <Cg/cgGL.h>
#endif

#include <assert.h>
#include <stdio.h>

void Reshape(int w, int h);

#ifdef USE_CG
CGprogram   textureProgram;
CGprogram   passThroughProgram;
CGparameter textureParam;
CGcontext   cgContext;
CGprofile   cgProfile;
#else
GLuint      iTextureProgram     = 0;
GLuint      iPassThroughProgram = 0;
#endif // USE_CG

RenderTexture *rt = NULL;

float       rectAngle         = 0;
float       torusAngle        = 0;
bool        bTorusMotion      = true;
bool        bRectMotion       = true;
bool        bShowDepthTexture = false;

static const char *g_modeTestStrings[] = 
{
    "rgb tex2D",
    "r=32f texRECT ctt aux=4",
    "rgba tex2D depthTex2D",
    "rgba=8 depthTexRECT ctt",
    "rgba samples=4 tex2D ctt",
    "rgba=8 tex2D mipmap",
    "rgb=5,6,5 tex2D",
    "rgba=16f texRECT",
    "rgba=32f texRECT depthTexRECT",
    "rgb=5,6,5 tex2D",
    "rgba=16f texRECT depthTexRECT ctt",
    "r=32f texRECT depth ctt",
    "rgb double tex2D",
    "r=32f texRECT ctt aux=4"
};

static int g_numModeTestStrings = sizeof(g_modeTestStrings) / sizeof(char*);
static int g_currentString      = 0;

//---------------------------------------------------------------------------
// Function     	: PrintGLerror
// Description	    : 
//---------------------------------------------------------------------------
void PrintGLerror( char *msg )
{
    GLenum errCode;
    const GLubyte *errStr;
    
    if ((errCode = glGetError()) != GL_NO_ERROR) 
    {
        errStr = gluErrorString(errCode);
        fprintf(stderr,"OpenGL ERROR: %s: %s\n", errStr, msg);
    }
}

#ifdef USE_CG
//---------------------------------------------------------------------------
// Function     	: cgErrorCallback
// Description	    : 
//---------------------------------------------------------------------------
void cgErrorCallback()
{
    CGerror lastError = cgGetError();
    
    if(lastError)
    {
        printf("%s\n\n", cgGetErrorString(lastError));
        printf("%s\n", cgGetLastListing(cgContext));
        printf("Cg error, exiting...\n");
        
        exit(0);
    }
} 
#endif // USE_CG

//---------------------------------------------------------------------------
// Function     	: CreateRenderTexture
// Description	    : 
//---------------------------------------------------------------------------
RenderTexture* CreateRenderTexture(const char *initstr)
{
    printf("\nCreating with init string: \"%s\"\n", initstr);

    int texWidth = 256, texHeight = 256;

    // Test deprecated interface
    //RenderTexture *rt2 = new RenderTexture(texWidth, texHeight);
    //if (!rt2->Initialize(true,false,false,false,false,8,8,8,0))

    RenderTexture *rt2 = new RenderTexture(); 
    rt2->Reset(initstr);
    if (!rt2->Initialize(texWidth, texHeight))
    {
        fprintf(stderr, "RenderTexture Initialization failed!\n");
    }

    // for shadow mapping we still have to bind it and set the correct 
    // texture parameters using the SGI_shadow or ARB_shadow extension
    // setup the rendering context for the RenderTexture
    if (rt2->BeginCapture())
    {
        Reshape(texWidth, texHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST); 
        glClearColor(0.2, 0.2, 0.2, 1);
        rt2->EndCapture();
    }

    // enable linear filtering if available
    if (rt2->IsTexture() || rt2->IsDepthTexture())
    {
        if (rt2->IsMipmapped())
        {
            // Enable trilinear filtering so we can see the mipmapping
            if (rt2->IsTexture())
            {
                rt2->Bind();
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
            }
            
            if (rt2->IsDepthTexture())
            {
                rt2->BindDepth();
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
            }
        }   
        else if (!(rt2->IsRectangleTexture() || rt2->IsFloatTexture()))
        {
            if (rt2->IsTexture())
            {
                rt2->Bind();
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            
            if (rt2->IsDepthTexture())
            {
                rt2->BindDepth();
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(rt2->GetTextureTarget(),
                                GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        }
    }

    if (rt2->IsFloatTexture())
    {
        ;
#ifdef USE_CG
        printf("Initializing Cg...\n");

        // Setup Cg
        cgSetErrorCallback(cgErrorCallback);
        cgContext = cgCreateContext();
        cgGLSetManageTextureParameters(cgContext,CG_TRUE);

        // get the best profile for this hardware
        cgProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
        assert(cgProfile != CG_PROFILE_UNKNOWN);
        cgGLSetOptimalOptions(cgProfile);

        // [Andrew Wood] - Used to refer to global rt RenderTexture & get previous value
        const char* strTextureProgram = rt2->IsRectangleTexture() ?
            "float4 main(half2       coords   : TEX0,\n"
            "    uniform samplerRECT texture) : COLOR {\n"
            "    return f4texRECT(texture, coords); }\n" 
            :
            "float4 main(half2     coords   : TEX0,\n"
            "    uniform sampler2D texture) : COLOR {\n"
            "    return f4tex2D(texture, coords); }\n";
            
        const char* strPassThroughProgram = 
            "float4 main(float4 color : COL0) : COLOR { return color; }\n";

        textureProgram = cgCreateProgram(cgContext, CG_SOURCE, 
                                         strTextureProgram, cgProfile, 
                                         NULL, NULL);
        
        if(textureProgram != NULL)
        {
            cgGLLoadProgram(textureProgram);
            
            textureParam = cgGetNamedParameter(textureProgram, "texture");
            assert(textureParam != NULL);
        }
        
        passThroughProgram = cgCreateProgram(cgContext, CG_SOURCE, 
                                             strPassThroughProgram, 
                                             cgProfile, NULL, NULL);
        
        if(passThroughProgram != NULL)
        {
            cgGLLoadProgram(passThroughProgram);
        }
#else
        glGenProgramsARB(1, &iTextureProgram);
        glGenProgramsARB(1, &iPassThroughProgram);
        
        const char* strTextureProgram = rt2->IsRectangleTexture() ?
            "!!ARBfp1.0\n"
            "TEX result.color, fragment.texcoord[0], texture[0], RECT;\n"
            "END\n"
            :
            "!!ARBfp1.0\n"
            "TEX result.color, fragment.texcoord[0], texture[0], 2D;\n"
            "END\n";
        const char* strPassThroughProgram = 
            "!!ARBfp1.0\n"
            "MOV result.color, fragment.color.primary;\n"
            "END\n";
        
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, iTextureProgram);
        glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, 
                           GL_PROGRAM_FORMAT_ASCII_ARB,
                           strlen(strTextureProgram), strTextureProgram);
        
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, iPassThroughProgram);
        glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, 
                           GL_PROGRAM_FORMAT_ASCII_ARB,
                           strlen(strPassThroughProgram), 
                           strPassThroughProgram);
#endif // USE_CG
    }

    
    if (rt2->IsDepthTexture())
    {
        fprintf(stderr, 
            "\nPress the spacebar to toggle color / depth textures.\n");
        if (!rt2->IsTexture())
            bShowDepthTexture = true;
    }
    else 
    {
        if (rt2->IsTexture())
            bShowDepthTexture = false;
    }

    PrintGLerror("Create");
    return rt2;
}

//---------------------------------------------------------------------------
// Function     	: DestroyRenderTexture
// Description	    : 
//---------------------------------------------------------------------------
void DestroyRenderTexture(RenderTexture *rt2)
{
    if (rt2->IsFloatTexture())
    {
#ifdef USE_CG
      
        cgDestroyProgram(textureProgram);
        cgDestroyProgram(passThroughProgram);
        cgDestroyContext(cgContext);
#else
        glDeleteProgramsARB(1, &iTextureProgram);
        glDeleteProgramsARB(1, &iPassThroughProgram);
#endif // USE_CG
    }
    
    delete rt2;
}

//---------------------------------------------------------------------------
// Function     	: Keyboard
// Description	    : 
//---------------------------------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27: 
    case 'q':
        exit(0);
        break;
    case ' ':
        bShowDepthTexture = !bShowDepthTexture;
        break;
    case 13:
        ++g_currentString%=g_numModeTestStrings;
        DestroyRenderTexture(rt);
        rt = CreateRenderTexture(g_modeTestStrings[g_currentString]);
        break;
    case 't':
        bTorusMotion = !bTorusMotion;
        break;
    case 'r':
        bRectMotion = !bRectMotion;
        break;
    default:
        return;
    }
}

//---------------------------------------------------------------------------
// Function     	: Idle
// Description	    : 
//---------------------------------------------------------------------------
void Idle()
{
    // make sure we don't try to display nonexistent textures
    if (!rt->IsDepthTexture())
        bShowDepthTexture = false; 
    
    if (bRectMotion) rectAngle += 1;
    if (bTorusMotion) torusAngle += 1;
    glutPostRedisplay();
}

//---------------------------------------------------------------------------
// Function     	: Reshape
// Description	    : 
//---------------------------------------------------------------------------
void Reshape(int w, int h)
{
    if (h == 0) h = 1;
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 1, 5.0);
}

//---------------------------------------------------------------------------
// Function     	: Display
// Description	    : 
//---------------------------------------------------------------------------
void display()
{
    if (rt->IsInitialized() && rt->BeginCapture())
    {
      if (rt->IsDoubleBuffered()) glDrawBuffer(GL_BACK);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        
        glRotatef(torusAngle, 1, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColor3f(1,1,0);
        
        if (rt->IsFloatTexture())
        {
#ifdef USE_CG
            cgGLBindProgram(passThroughProgram);
            cgGLEnableProfile(cgProfile);
#else
            glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, iPassThroughProgram);
            glEnable(GL_FRAGMENT_PROGRAM_ARB);
#endif // USE_CG
        }
        
        glutSolidTorus(0.25, 1, 32, 64);
        
        if (rt->IsFloatTexture())
        {
#ifdef USE_CG
            cgGLDisableProfile(cgProfile);
#else
            glDisable(GL_FRAGMENT_PROGRAM_ARB);
#endif // USE_CG
        }

        glPopMatrix();
        PrintGLerror("RT Update");

    rt->EndCapture();
    }    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1, 1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glRotatef(rectAngle / 10, 0, 1, 0);

    if(bShowDepthTexture && rt->IsDepthTexture())
        rt->BindDepth();
    else if (rt->IsTexture()) {
#ifdef _WIN32
        if (rt->IsDoubleBuffered()) rt->BindBuffer(WGL_BACK_LEFT_ARB);
#endif
        rt->Bind();
    }

#ifdef USE_CG
    if (!bShowDepthTexture && rt->IsFloatTexture())
    {
        cgGLEnableProfile(cgProfile);
        cgGLBindProgram(textureProgram);
        cgGLSetTextureParameter(textureParam, rt->GetTextureID());
	//cgGLEnableTextureParameter(textureParam);
    }
    else 
    {
        rt->EnableTextureTarget();
    }
#else 
    if (!bShowDepthTexture && rt->IsFloatTexture())
    {
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, iTextureProgram);
        glEnable(GL_FRAGMENT_PROGRAM_ARB);
        glActiveTexture(GL_TEXTURE0_ARB);
    }

    rt->EnableTextureTarget();
#endif // USE_CG

    int maxS = rt->GetMaxS();
    int maxT = rt->GetMaxT();  
    
    glBegin(GL_QUADS);
    glTexCoord2f(0,       0); glVertex2f(-1, -1);
    glTexCoord2f(maxS,    0); glVertex2f( 1, -1);
    glTexCoord2f(maxS, maxT); glVertex2f( 1,  1);
    glTexCoord2f(0,    maxT); glVertex2f(-1,  1);
    glEnd();
    
    if (!bShowDepthTexture && rt->IsFloatTexture())
    {
#ifdef USE_CG
      //cgGLDisableTextureParameter(textureParam);
        cgGLDisableProfile(cgProfile);
    }
    else
    {
        rt->DisableTextureTarget();
    }
#else
        glDisable(GL_FRAGMENT_PROGRAM_ARB);  
    }

    rt->DisableTextureTarget();
#endif // USE_CG
          
    glPopMatrix();
    
    PrintGLerror("display");
    glutSwapBuffers();
}




//---------------------------------------------------------------------------
// Function     	: main
// Description	    : 
//---------------------------------------------------------------------------
int main()
{
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(512, 512);
    glutCreateWindow("TestRenderTexture");  
    
    int err = glewInit();
    if (GLEW_OK != err)
    {
        // problem: glewInit failed, something is seriously wrong
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        exit(-1);
    }  
    
    glutDisplayFunc(display);
    glutIdleFunc(Idle);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    
    Reshape(512, 512);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 2, 0, 0, 0, 0, 1, 0);
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST); 
    glClearColor(0.4, 0.6, 0.8, 1);
    

    rt = CreateRenderTexture(g_modeTestStrings[g_currentString]);

    printf("Press Enter to change RenderTexture parameters.\n"
           "Press 'r' to toggle the rectangle's motion.\n"
           "Press 't' to toggle the torus' motion.\n");
    

    glutMainLoop();
}
