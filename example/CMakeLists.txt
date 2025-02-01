add_executable(opencsgexample WIN32 MACOSX_BUNDLE
    displaylistPrimitive.cpp displaylistPrimitive.h
    includeGl.h
    main.cpp
)

target_include_directories(opencsgexample PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

target_link_libraries(opencsgexample PRIVATE
    opencsg
    ${OPENGL_LIBRARIES}
    GLUT::GLUT
)

install(TARGETS opencsgexample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
