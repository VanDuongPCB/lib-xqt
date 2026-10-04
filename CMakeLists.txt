cmake_minimum_required(VERSION 3.21)

project(xQt
    VERSION 0.1.0
    DESCRIPTION "Customized Qt widgets: X* subclasses of the Qt Widgets classes"
    LANGUAGES CXX
)

option(XQT_BUILD_SHARED "Build xQt as a shared library (default: static)" OFF)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_package(Qt6 6.3 REQUIRED COMPONENTS Core Gui Widgets)

# Only for this directory: the parent project may use qt_standard_project_setup().
set(CMAKE_AUTOMOC ON)

file(GLOB XQT_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB XQT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

if(XQT_BUILD_SHARED)
    add_library(xQt SHARED ${XQT_SOURCES} ${XQT_HEADERS})
    target_compile_definitions(xQt PRIVATE XQT_LIBRARY)
else()
    add_library(xQt STATIC ${XQT_SOURCES} ${XQT_HEADERS})
    target_compile_definitions(xQt PUBLIC XQT_STATIC)
endif()
add_library(xQt::xQt ALIAS xQt)

target_compile_definitions(xQt PRIVATE XQT_VERSION_STR="${PROJECT_VERSION}")

target_include_directories(xQt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(xQt PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

set_target_properties(xQt PROPERTIES
    OUTPUT_NAME   "xQt"
    DEBUG_POSTFIX "d"
    FOLDER        "Libraries"
)

if(MSVC)
    target_compile_options(xQt PRIVATE /utf-8 /W3 /MP)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${XQT_HEADERS} ${XQT_SOURCES})
