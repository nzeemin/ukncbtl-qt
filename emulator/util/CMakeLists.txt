cmake_minimum_required(VERSION 3.16)

project(util LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(UTIL_SOURCES
    WavPcmFile.cpp
)

set(UTIL_HEADERS
    stdafx.h
    WavPcmFile.h
)

add_library(util STATIC
    ${UTIL_SOURCES}
    ${UTIL_HEADERS}
)

target_compile_definitions(util PRIVATE EMUBASE_LIBRARY)
