add_executable(opencsgexample WIN32 MACOSX_BUNDLE
    displaylistPrimitive.cpp displaylistPrimitive.h
    includeGl.h
    main.cpp
)
target_include_directories(opencsgexample PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(opencsgexample PRIVATE
    opencsg
    GL
)

if(MACOS)
    target_include_directories(opencsgexample PRIVATE
        /opt/local/include
    )
    target_link_libraries(opencsgexample PRIVATE
        "-framework GLUT"
    )
else()
    target_link_libraries(opencsgexample PRIVATE
        GLU
        glut
    )
endif()

install(TARGETS opencsgexample
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
