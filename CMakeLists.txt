project(Raytracer)
cmake_minimum_required(VERSION 2.8.3)

Set(CMAKE_BUILD_TYPE Release)

set(CMAKE_PREFIX_PATH "C:/Qt/5.2.0/msvc2012_64/")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets)
find_package(Assimp)
#add_definitions(${Qt5Widgets_DEFINITIONS})
#add_definitions(${Qt5Opengl_DEFINITIONS})
#include_directories(${Qt5Widgets_INCLUDES})
#include_directories(${Qt5Opengl_INCLUDES})


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -std=c++11")

# SET(OPENCL_ROOT "C:/Program Files (x86)/AMD APP SDK/2.9")
# INCLUDE_DIRECTORIES("${OPENCL_ROOT}/include")
# SET(OPENCL_LIBRARIES "C:/Program Files (x86)/AMD APP SDK/2.9/lib/x86/OpenCL.lib")
# SET(OPENCL_DLL "C:/Program Files (x86)/AMD APP SDK/2.9/bin/x86/OpenCL.dll")

QT5_WRAP_UI(UIS_HDRS
    Forms/mainwindow.ui
)

QT5_ADD_RESOURCES (ResourceSources ui.qrc)
aux_source_directory(Sources/. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${UIS_HDRS} ${ResourceSources})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ASSIMP_INCLUDE_DIR})
qt5_use_modules(${PROJECT_NAME} Widgets)
target_link_libraries(${PROJECT_NAME} ${Qt5Widgets_LIBRARIES} ${ASSIMP_LIBRARY})#"C:/Program Files/Assimp/lib/x64/assimp.lib" "C:/Program Files/Assimp/bin/x64/Assimp64.dll")
MESSAGE(STATUS ${ASSIMP_LIBRARY})