all: TestRenderTexture TestRenderTextureCg

TestRenderTexture: TestRenderTexture.cpp RenderTexture.cpp RenderTexture.h
	g++ -g -o TestRenderTexture TestRenderTexture.cpp RenderTexture.cpp -lGL -lglut -lGLEW

TestRenderTextureCg: TestRenderTexture.cpp RenderTexture.cpp RenderTexture.h
	g++ -g -DUSE_CG -o TestRenderTextureCg TestRenderTexture.cpp RenderTexture.cpp -lGL -lglut -lGLEW -lCg -lCgGL
