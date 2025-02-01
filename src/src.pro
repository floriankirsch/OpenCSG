TEMPLATE = lib
TARGET = opencsg
VERSION = 1.7.0

isEmpty(INSTALLDIR) {
  INSTALLDIR = /usr/local
}

CONFIG += opengl warn_on release
INCLUDEPATH += ../include ../ $$INSTALLDIR/include
CONFIG -= qt
LIBS += -L$$INSTALLDIR/lib

DESTDIR = ../lib
headers.files = ../include/opencsg.h
headers.path = $$INSTALLDIR/include
target.path = $$INSTALLDIR/lib
INSTALLS += target headers

HEADERS		= ../include/opencsg.h \
		  glad/include/glad/gl.h \
		  glad/include/KHR/khrplatform.h \
		  opencsgConfig.h \
		  area.h \
		  batch.h \
		  context.h \
		  channelManager.h \
		  frameBufferObject.h \
		  frameBufferObjectExt.h \
		  occlusionQuery.h \
		  offscreenBuffer.h \
		  openglExt.h \
		  opencsgRender.h \
		  openglHelper.h \
		  primitiveHelper.h \
		  scissorMemo.h \
		  sequencer.h \
		  settings.h
SOURCES		= glad/src/gl.cpp \
		  area.cpp \
		  batch.cpp \
		  context.cpp \
		  channelManager.cpp \
		  frameBufferObject.cpp	\
		  frameBufferObjectExt.cpp \
		  occlusionQuery.cpp \
		  opencsgRender.cpp \
		  openglHelper.cpp \
		  primitive.cpp \
		  primitiveHelper.cpp \
		  renderGoldfeather.cpp \
		  renderSCS.cpp \
		  scissorMemo.cpp \
		  sequencer.cpp \
		  settings.cpp
