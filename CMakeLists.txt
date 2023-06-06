cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)

project(libnut)

# Source
set(SOURCE_FILES
        src/main.cc
        src/deadbeef_rand.c
        src/MMBitmap.c
        )

if (UNIX AND NOT APPLE)
    list(APPEND SOURCE_FILES
            src/linux/keycode.c
            src/linux/keypress.c
            src/linux/mouse.c
            src/linux/screen.c
            src/linux/screengrab.c
            src/linux/xdisplay.c
            src/linux/highlightwindow.c
            src/linux/window_manager.cc
            )
elseif (UNIX AND APPLE)
    list(APPEND SOURCE_FILES
            src/macos/keycode.c
            src/macos/keypress.c
            src/macos/mouse.c
            src/macos/mouse_utils.mm
            src/macos/screen.c
            src/macos/screengrab.m
            src/macos/highlightwindow.m
            src/macos/window_manager.mm
            )
elseif (WIN32)
    list(APPEND SOURCE_FILES
            src/win32/keycode.c
            src/win32/keypress.c
            src/win32/mouse.c
            src/win32/screen.c
            src/win32/screengrab.c
            src/win32/highlightwindow.c
            src/win32/window_manager.cc
            )
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

# External libs
set(LIBS "")
set(INCLUDES "")

if (UNIX AND APPLE)
    message(STATUS "macOS build")
    list(APPEND LIBS "-framework ApplicationServices" "-framework Cocoa")
elseif (WIN32)
    message(STATUS "Windows build")
elseif (UNIX AND NOT APPLE)
    message(STATUS "Linux build")
    list(APPEND LIBS "-lX11" "-lXtst")
endif()

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "No MSVC compiler in use")
    set(CMAKE_CXX_FLAGS "-Wall -Wparentheses -Winline -Wbad-function-cast -Wdisabled-optimization -Wextra")
else()
    message(STATUS "MSVC compiler in use")
    set(CMAKE_CXX_FLAGS "/Wall /W4 /EHsc")
endif()

if (WIN32)
    # Copy runtime distributable
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/3rdparty/win32/msvcp140.dll
            ${CMAKE_SOURCE_DIR}/3rdparty/win32/vcruntime140.dll
            ${CMAKE_SOURCE_DIR}/3rdparty/win32/vcruntime140_1.dll
            ${CMAKE_SOURCE_DIR}/3rdparty/win32/api-ms-win-crt-heap-l1-1-0.dll
            ${CMAKE_SOURCE_DIR}/3rdparty/win32/api-ms-win-crt-runtime-l1-1-0.dll
            ${CMAKE_SOURCE_DIR}/3rdparty/win32/api-ms-win-crt-string-l1-1-0.dll
            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/
    )
endif()

add_compile_definitions(NAPI_CPP_EXCEPTIONS)
add_compile_definitions(NAPI_VERSION=3)

# cmake-js
list(APPEND INCLUDES ${CMAKE_JS_INC})
message(STATUS "Includes: ${INCLUDES}")
list(APPEND LIBS ${CMAKE_JS_LIB})
message(STATUS "Libs: ${LIBS}")

# N-API
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api")

# Change suffix to *.node
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# BUILD
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
