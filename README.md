# OpenCSG

OpenCSG is a library that does image-based CSG rendering using OpenGL.
It is written in C++ and supports most modern graphics hardware using Microsoft Windows or the Linux operating system.

What is CSG, anyway? CSG is short for Constructive Solid Geometry and denotes an approach to model complex 3D-shapes using simpler ones.
I.e., two shapes can be combined by taking the union of them, by intersecting them, or by subtracting one shape of the other.
The most basic shapes, which are not result of such a CSG operation, are called primitives. Primitives must be solid, i.e.,
they must have a clearly defined interior and exterior. By construction, a CSG shape is also solid then.

Image-based CSG rendering (also z-buffer CSG rendering) is a term for algorithms that render CSG shapes
without an explicit calculation of the geometric boundary of a CSG shape. Such algorithms use frame-buffer
settings of the graphics hardware, e.g., the depth and stencil buffer, to compose CSG shapes.
OpenCSG implements a variety of those algorithms, namely the Goldfeather algorithm and the SCS algorithm,
both of them in several variants.
 
See the <a href="http://www.opencsg.org">OpenCSG homepage</a> for more information.
