add_library(opencsg
    area.cpp area.h
    batch.cpp batch.h
    channelManager.cpp channelManager.h
    context.cpp context.h
    frameBufferObject.cpp frameBufferObject.h
    frameBufferObjectExt.cpp frameBufferObjectExt.h
    glad/include/KHR/khrplatform.h
    glad/include/glad/gl.h
    glad/src/gl.cpp
    occlusionQuery.cpp occlusionQuery.h
    offscreenBuffer.h
    opencsgConfig.h
    opencsgRender.cpp opencsgRender.h
    openglExt.h
    openglHelper.cpp openglHelper.h
    primitive.cpp
    primitiveHelper.cpp primitiveHelper.h
    renderGoldfeather.cpp
    renderSCS.cpp
    scissorMemo.cpp scissorMemo.h
    sequencer.h
    settings.cpp settings.h
)

target_include_directories(opencsg PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
)

set_target_properties(opencsg PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/opencsg.h" 
)

find_package(OpenGL REQUIRED)

target_link_libraries(opencsg PRIVATE OpenGL::OpenGL)

install(TARGETS opencsg
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
