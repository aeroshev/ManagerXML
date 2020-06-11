cmake_minimum_required(VERSION 3.16)
project(AVSoft2)
set(PROJECT AVSoft2)

set(CMAKE_CXX_STANDARD 17)

set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)
set (BOOST_INCLUDEDIR "/usr/local/Cellar/boost/1.72.0_1")
set (BOOST_LIBRARYDIR "/usr/local/Cellar/boost/1.72.0_1/lib")

find_package (Boost REQUIRED COMPONENTS system filesystem)

if (Boost_FOUND)
    include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif ()

add_library(
        pugixml
        pugixml/pugixml.hpp
        pugixml/pugixml.cpp
        xml_reader
        xml_reader/xml_reader.h
        xml_reader/xml_reader.cpp
        xml_reader/xml_manager.h
        xml_reader/xml_manager.cpp
        xml_reader/combiner.h
        xml_reader/combiner.cpp
        xml_reader/xml_components.h)



add_executable(${PROJECT} main.cpp)

target_link_libraries(${PROJECT} PRIVATE pugixml)
target_link_libraries (${PROJECT} PRIVATE ${Boost_LIBRARIES})