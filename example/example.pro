TEMPLATE = app
TARGET = opencsgexample

CONFIG += opengl warn_on release
CONFIG -= qt
INCLUDEPATH += ../include
LIBS += -L../lib -lopencsg -lGLEW

INSTALLDIR = /usr/local
INSTALLS += target
target.path = $$INSTALLDIR/bin

macx {
  INCLUDEPATH += /opt/local/include
  LIBS += -framework GLUT -L/opt/local/lib
}
else {
  LIBS += -lGLU -lglut
}

HEADERS = displaylistPrimitive.h
SOURCES	= displaylistPrimitive.cpp main.cpp
