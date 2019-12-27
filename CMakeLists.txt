cmake_minimum_required(VERSION 3.5)

project(untitled111 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick Widgets REQUIRED)

add_library(untitledWindow
    VkMainWindow.h

    VkIbWindow.h
    VkIbWindow.cpp
)

add_executable(untitled111
    main_window.cpp
    qml.qrc
)


#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

option(WITH_IGNIMBRITE "Should use Ignimbrite library for Vulkan API rendering" YES)
#if(WITH_IGNIMBRITE)
    message(STATUS "Adding subdirectory: Ignimbrite")
    set(WITH_VULKAN ON CACHE BOOL "" FORCE)
    set(WITH_GLFW ON CACHE BOOL "" FORCE)
    set(WITH_IGNIMBRITE_QT ON CACHE BOOL "" FORCE)
    add_subdirectory(ignimbrite)
    target_link_libraries(untitled111 PUBLIC Ignimbrite)
    target_link_libraries(untitled111 PUBLIC tinyobjloader)
    target_link_libraries(untitled111 PUBLIC stb-image)

    if (WITH_VULKAN)
        message(STATUS "Linking current app to VulkanDevice")
        target_link_libraries(untitled111 PUBLIC VulkanDevice)

        # TEMPORARY
        target_link_libraries(untitledWindow PUBLIC VulkanDevice)
    endif()
#endif()

target_compile_definitions(untitled111
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(untitledWindow
  PUBLIC Qt5::Core Qt5::Quick Qt5::Widgets)

target_link_libraries(untitled111 PRIVATE untitledWindow)
