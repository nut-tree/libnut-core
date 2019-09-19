cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project(libnut)

# Source
set(SOURCE_FILES "src/libnut.cc" "src/deadbeef_rand.c" "src/mouse.c" "src/keypress.c" "src/keycode.c" "src/screen.c" "src/screengrab.c" "src/snprintf.c" "src/MMBitmap.c")
if (LINUX)
    set(SOURCE_FILES "${SOURCE_FILES}" "src/xdisplay.c")
endif()
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# Includes
include_directories("${CMAKE_SOURCE_DIR}/src/include")

set(LIBS "")
set(INCLUDES "")

# External libs
if (APPLE)
    message(STATUS "macOS build")
    set(LIBS "${LIBS}" "-framework ApplicationServices")
    set(LIBS "${LIBS}" "-framework Carbon")
    set(LIBS "${LIBS}" "-framework CoreFoundation")
    set(LIBS "${LIBS}" "-framework OpenGL")
elseif (WIN32)
    message(STATUS "Windows build")
elseif (LINUX)
    message(STATUS "Linux build")
    set(LIBS "${LIBS}" "-lpng")
    set(LIBS "${LIBS}" "-lz")
    set(LIBS "${LIBS}" "-lX11")
    set(LIBS "${LIBS}" "-lXtst")
endif()

set(CMAKE_C_FLAGS "-Wall -Wparentheses -Winline -Wbad-function-cast -Wdisabled-optimization")
set(CMAKE_C_FLAGS_RELEASE "-O3")
if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "No MSVC compiler in use, adding compile flags -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()

# add_compile_definitions(NAPI_CPP_EXCEPTIONS)

# cmake-js
set(INCLUDES ${INCLUDES} ${CMAKE_JS_INC})
set(LIBS ${LIBS} ${CMAKE_JS_LIB})
message(STATUS "Libs: ${LIBS}")

# N-API
# target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api")

# Change suffix to *.node
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# BUILD
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
