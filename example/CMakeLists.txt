add_executable(opencsgexample WIN32 MACOSX_BUNDLE
    displaylistPrimitive.cpp displaylistPrimitive.h
    includeGl.h
    main.cpp
)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

target_include_directories(opencsgexample PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(opencsgexample PRIVATE
    opencsg
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
)

install(TARGETS opencsgexample
    BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/Applications
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
