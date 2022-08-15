cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)

project(libnut)

# Source
set(SOURCE_FILES "src/main.cc" "src/deadbeef_rand.c" "src/MMBitmap.c")
if (UNIX AND NOT APPLE)
    set(SOURCE_FILES "${SOURCE_FILES}" "src/linux/keycode.c" "src/linux/keypress.c" "src/linux/mouse.c" "src/linux/screen.c" "src/linux/screengrab.c" "src/linux/xdisplay.c" "src/linux/highlightwindow.c" "src/linux/window_manager.cc")
elseif (UNIX AND APPLE)
    set(SOURCE_FILES "${SOURCE_FILES}" "src/macos/keycode.c" "src/macos/keypress.c" "src/macos/mouse.c" "src/macos/mouse_utils.mm" "src/macos/screen.c" "src/macos/screengrab.c" "src/macos/highlightwindow.m" "src/macos/window_manager.mm")
elseif (WIN32)
    set(SOURCE_FILES "${SOURCE_FILES}" "src/win32/keycode.c" "src/win32/keypress.c" "src/win32/mouse.c" "src/win32/screen.c" "src/win32/screengrab.c" "src/win32/highlightwindow.c" "src/win32/window_manager.cc")
endif()
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

set(LIBS "")
set(INCLUDES "")

# External libs
if (UNIX AND APPLE)
    message(STATUS "macOS build")
    set(LIBS "${LIBS}" "-framework ApplicationServices")
    set(LIBS "${LIBS}" "-framework Cocoa")
elseif (WIN32)
    message(STATUS "Windows build")
elseif (UNIX AND NOT APPLE)
    message(STATUS "Linux build")
    set(LIBS "${LIBS}" "-lX11")
    set(LIBS "${LIBS}" "-lXtst")
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
		${CMAKE_SOURCE_DIR}/3rdparty/win32/*
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/
	)
endif()

add_compile_definitions(NAPI_CPP_EXCEPTIONS)
add_compile_definitions(NAPI_VERSION=3)

# cmake-js
set(INCLUDES ${INCLUDES} ${CMAKE_JS_INC})
message(STATUS "Includes: ${INCLUDES}")
set(LIBS ${LIBS} ${CMAKE_JS_LIB})
message(STATUS "Libs: ${LIBS}")

# N-API
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api")

# Change suffix to *.node
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# BUILD
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
