cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

project(libnut)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Source
set(SOURCE_FILES "src/main.cc" "src/deadbeef_rand.cc" "src/Bitmap.cc")
if (UNIX AND NOT APPLE)
    set(SOURCE_FILES "${SOURCE_FILES}" "src/linux/keycode.cc" "src/linux/keypress.cc" "src/linux/mouse.cc" "src/linux/screen.cc" "src/linux/screengrab.cc" "src/linux/xdisplay.cc" "src/linux/highlightwindow.cc" "src/linux/window_manager.cc" "src/linux/libxdo/xdo.c" "src/linux/libxdo/xdo_search.c")
elseif (UNIX AND APPLE)
    set(SOURCE_FILES "${SOURCE_FILES}" "src/macos/keycode.cc" "src/macos/keypress.cc" "src/macos/mouse.cc" "src/macos/mouse_utils.mm" "src/macos/screen.cc" "src/macos/screengrab.cc" "src/macos/highlightwindow.mm" "src/macos/window_manager.mm")
elseif (WIN32)
    set(SOURCE_FILES "${SOURCE_FILES}" "src/win32/keycode.cc" "src/win32/keypress.cc" "src/win32/mouse.cc" "src/win32/screen.cc" "src/win32/screengrab.cc" "src/win32/highlightwindow.cc" "src/win32/window_manager.cc")
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
    set(LIBS "${LIBS}" "-lXinerama")
    set(LIBS "${LIBS}" "-lxkbcommon")
endif()

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "No MSVC compiler in use")
    set(CMAKE_CXX_FLAGS "-O3 -Wall -Wparentheses -Winline -Wdisabled-optimization -Wextra")
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
