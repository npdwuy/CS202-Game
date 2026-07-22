cmake_minimum_required(VERSION 3.15)
project(CS202-Group5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically find all .cpp source files recursively in src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include SFML/include)

# Compiler-specific SFML configurations
if(MSVC)
    target_link_directories(${PROJECT_NAME} PRIVATE SFML/lib/msvc)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        debug sfml-graphics-d debug sfml-window-d debug sfml-system-d debug sfml-audio-d debug sfml-network-d
        optimized sfml-graphics optimized sfml-window optimized sfml-system optimized sfml-audio optimized sfml-network
    )
    add_compile_definitions(UNICODE _UNICODE)
    file(GLOB SFML_DLLS "SFML/bin/msvc/*.dll")
elseif(MINGW)
    target_link_directories(${PROJECT_NAME} PRIVATE SFML/lib/mingw)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system sfml-audio sfml-network)
    file(GLOB SFML_DLLS "SFML/bin/mingw/*.dll")
endif()

# Locate additional lib*.dll files (common in MinGW builds)
file(GLOB SFML_LIB_DLLS "SFML/bin/lib*.dll")

# Combine all runtime DLL files
set(SFML_RUNTIME_DLLS
    ${SFML_DLLS}
    ${SFML_LIB_DLLS}
)

# Copy SFML DLLs only when the list is not empty
if(SFML_RUNTIME_DLLS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SFML_RUNTIME_DLLS}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying SFML DLLs to build directory..."
    )
endif()

# Copy assets to the executable directory
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copying assets to build directory..."
    )
endif()