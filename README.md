# OpenCSG

OpenCSG is a library that does image-based CSG rendering using OpenGL.
It is written in C++ and supports most modern graphics hardware using Microsoft Windows or the Linux operating system.
See the <a href="http://www.opencsg.org">OpenCSG homepage</a> for a more detailed introduction.

# Building from git

In contrast to the official source releases, this git repository comes without the OpenGL Extension Wrangler Library, GLEW.
This library is needed to compile OpenCSG. On platforms where it is not available anyway (Windows), you should download it
from the <a href="http://glew.sourceforge.net">GLEW homepage</a> and extract it into the <code>glew</code> subdirectory.
