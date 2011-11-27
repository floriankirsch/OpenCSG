TEMPLATE	= lib
TARGET		= opencsg
VERSION     = 1.3.2
DESTDIR     = ../lib

CONFIG		+= opengl warn_on release
INCLUDEPATH += ../include ../glew/include ../

HEADERS		= ../include/opencsg.h \
		  opencsgConfig.h \
		  area.h \
		  batch.h \
		  context.h \
		  channelManager.h \
		  frameBufferObject.h \
		  frameBufferObjectExt.h \
		  occlusionQuery.h \
		  offscreenBuffer.h \
		  opencsgRender.h \
		  openglHelper.h \
		  pBufferTexture.h \
		  primitiveHelper.h \
		  scissorMemo.h \
		  settings.h \
		  stencilManager.h \
		  ../RenderTexture/RenderTexture.h
SOURCES		= area.cpp \
		  batch.cpp \
		  context.cpp \
		  channelManager.cpp \
		  frameBufferObject.cpp	\
		  frameBufferObjectExt.cpp \
		  occlusionQuery.cpp \
		  offscreenBuffer.cpp \
		  opencsgRender.cpp \
		  openglHelper.cpp \
		  pBufferTexture.cpp \
		  primitive.cpp \
		  primitiveHelper.cpp \
		  renderGoldfeather.cpp \
		  renderSCS.cpp \
		  scissorMemo.cpp \
		  settings.cpp \
		  stencilManager.cpp \
		  ../RenderTexture/RenderTexture.cpp
