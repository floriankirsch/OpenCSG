TEMPLATE	= app
TARGET		= opencsgexample

DEFINES     += GLEW_STATIC

CONFIG	 	+= opengl warn_on release
INCLUDEPATH += ../glew/include ../include

LIBS        += -L../lib -lopencsg -lglut -L../glew/lib -lGLEW

HEADERS		= displaylistPrimitive.h
SOURCES		= displaylistPrimitive.cpp main.cpp
