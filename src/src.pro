TEMPLATE	= lib
TARGET		= opencsg
VERSION     = 1.0.2
DESTDIR     = ../lib

CONFIG		+= opengl warn_on release
INCLUDEPATH += ../include ../glew/include ../

HEADERS		= ../include/opencsg.h \
              ../include/opencsgConfig.h \
              area.h \
			  batch.h \
			  channelManager.h \
			  occlusionQuery.h \
			  opencsgRender.h \
			  openglHelper.h \
			  primitiveHelper.h \
			  scissorMemo.h \
			  stencilManager.h \
			  ../RenderTexture/RenderTexture.h
SOURCES		= area.cpp \
			  batch.cpp \
			  channelManager.cpp \
			  occlusionQuery.cpp \
			  opencsgRender.cpp \
			  openglHelper.cpp \
			  primitive.cpp \
			  primitiveHelper.cpp \
			  renderGoldfeather.cpp \
			  renderSCS.cpp \
			  scissorMemo.cpp \
			  stencilManager.cpp \
			  ../RenderTexture/RenderTexture.cpp
