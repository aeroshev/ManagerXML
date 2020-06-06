cmake_minimum_required(VERSION 3.16)
project(AVSoft2)

set(CMAKE_CXX_STANDARD 17)

add_library(
        pugixml
        pugixml/pugixml.hpp
        pugixml/pugixml.cpp
        xml_reader
        xml_reader/xml_reader.h
        xml_reader/xml_reader.cpp)



add_executable(AVSoft2 main.cpp)

target_link_libraries(AVSoft2 PRIVATE pugixml)