#include "abstractRenderTexture.h"
#include <cassert>
#include <iostream>

    #define CHECK_FRAMEBUFFER_STATUS()                            \
      {                                                           \
        GLenum status;                                            \
        status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
        switch(status) {                                          \
          case GL_FRAMEBUFFER_COMPLETE_EXT:                       \
            std::cout << "framebuffer complete" << std::endl;     \
            break;                                                \
          case GL_FRAMEBUFFER_UNSUPPORTED_EXT:                    \
            std::cout << "framebuffer unsupported" << std::endl;  \
            /* choose different formats */                        \
            break;                                                \
          default:                                                \
            /* programming error; will fail on all hardware */    \
            assert(0);                                            \
        }                                                         \
      }

// ctor / dtor
AbstractRenderTexture::AbstractRenderTexture(const char *strMode) {
    // ignore mode string

    bool haveFBO = GLEW_EXT_framebuffer_object != 0;

    //assert(haveFBO);
}

AbstractRenderTexture::~AbstractRenderTexture() {

}

//! Call this once before use.  Set bShare to true to share lists, textures, 
//! and program objects between the render texture context and the 
//! current active GL context.
bool AbstractRenderTexture::Initialize(int width, int height, bool shareObjects, bool copyContext) {

    this->width = width;
    this->height = height;

    glGenFramebuffersEXT(1, &framebufferID);
    glGenRenderbuffersEXT(1, &depthID); 
    //glGenRenderbuffersEXT(1, &stencilID); 
    glGenTextures(1, &textureID);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferID);	
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_INT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureID, 0);

    //glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilID);
    //glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX8_EXT, 512, 512);
    //glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, stencilID);
	
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthID);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_NV, width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthID);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthID);
	
    CHECK_FRAMEBUFFER_STATUS();
    
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);	
    glBindTexture(GL_TEXTURE_2D, 0);

    textureTarget = GL_TEXTURE_2D;

    return true;
}

// !Change the render texture format.
bool AbstractRenderTexture::Reset(const char* strMode,...)
{
    glDeleteTextures(1, &textureID);
    glDeleteRenderbuffersEXT(1, &depthID); 
    glDeleteRenderbuffersEXT(1, &stencilID); 
    glDeleteFramebuffersEXT(1, &framebufferID);
    return true;
}

// !Change the size of the render texture.
bool AbstractRenderTexture::Resize(int width, int height)
{
    Reset(0);
    Resize(width, height);
    return true;

}

// !Begin drawing to the texture. (i.e. use as "output" texture)
bool AbstractRenderTexture::BeginCapture()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferID);
    return true;
}

// !End drawing to the texture.
bool AbstractRenderTexture::EndCapture()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    return true;
}

// !Bind the texture to the active texture unit for use as an "input" texture
void AbstractRenderTexture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, textureID);
}