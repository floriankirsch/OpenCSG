TEMPLATE	= lib
TARGET		= opencsg.so

CONFIG		+= opengl warn_on release
INCLUDEPATH 	+= include glew/include

HEADERS		= include/opencsg.h include/opencsgConfig.h src/area.h src/batch.h src/channelManager.h src/occlusionQuery.h src/opencsgRender.h src/openglHelper.h src/primitiveHelper.h src/scissorMemo.h src/stencilManager.h RenderTexture/RenderTexture.h
SOURCES		= RenderTexture/RenderTexture.cpp src/area.cpp src/batch.cpp src/channelManager.cpp src/occlusionQuery.cpp src/opencsgRender.cpp src/openglHelper.cpp src/primitive.cpp src/primitiveHelper.cpp src/renderGoldfeather.cpp src/renderSCS.cpp src/scissorMemo.cpp src/stencilManager.cpp
