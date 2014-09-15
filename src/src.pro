TEMPLATE = lib
TARGET = opencsg
VERSION = 1.4.0

CONFIG += opengl warn_on release
INCLUDEPATH += ../include ../
CONFIG -= qt
LIBS += -lGLEW

DESTDIR = ../lib
INSTALLDIR = /usr/local
headers.files = ../include/opencsg.h
headers.path = $$INSTALLDIR/include
target.path = /usr/local/lib
INSTALLS += target headers

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
