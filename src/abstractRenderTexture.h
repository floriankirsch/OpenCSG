#ifndef __ABSTRACTRENDERTEXTURE_HPP__
#define __ABSTRACTRENDERTEXTURE_HPP__

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

class AbstractRenderTexture
{
    
public: // interface
    // ctor / dtor
    AbstractRenderTexture(const char *strMode="rgb tex2D");
    ~AbstractRenderTexture();
    
    //! Call this once before use.  Set bShare to true to share lists, textures, 
    //! and program objects between the render texture context and the 
    //! current active GL context.
    virtual bool Initialize(int width, int height, 
                    bool shareObjects=true, 
                    bool copyContext=false);

    // !Change the render texture format.
    virtual bool Reset(const char* strMode,...);
    // !Change the size of the render texture.
    virtual bool Resize(int width, int height);
    
    // !Begin drawing to the texture. (i.e. use as "output" texture)
    virtual bool BeginCapture();
    // !End drawing to the texture.
    virtual bool EndCapture();
    
    // !Bind the texture to the active texture unit for use as an "input" texture
    virtual void Bind() const;

    //! Enables the texture target appropriate for this render texture.
    void EnableTextureTarget() const 
    { if (initialized) glEnable(textureTarget); }
    //! Disables the texture target appropriate for this render texture.
    void DisableTextureTarget() const 
    { if (initialized) glDisable(textureTarget); }
    
    //! Returns the width of the offscreen buffer.
    int GetWidth() const            { return width;  } 
    //! Returns the width of the offscreen buffer.
    int GetHeight() const           { return height; }

    /**
    * @fn IsPowerOfTwo(int n)
    * @brief Returns true if /param n is an integer power of 2.
    * 
    * Taken from Steve Baker's Cute Code Collection. 
    * http://www.sjbaker.org/steve/software/cute_code.html
    */ 
    static bool IsPowerOfTwo(int n) { return ((n&(n-1))==0); }

protected: // data
    int          width;     // width of the pbuffer
    int          height;    // height of the pbuffer
    
    // Texture stuff
    GLenum       textureTarget;
    unsigned int textureID;
    unsigned int depthID;
    unsigned int stencilID;

    unsigned int framebufferID;

    bool initialized;
};

#endif //__ABSTRACTRENDERTEXTURE_HPP__
