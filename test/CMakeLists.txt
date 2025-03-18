cmake_minimum_required(VERSION 3.2)

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

set(PROJECT_NAME TestRayProof)
project(${PROJECT_NAME})

set(RAY_PROOF_HEADERS ${HEADERS})
set(RAY_PROOF_SOURCES ${SOURCES})

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(HEADERS "catch_amalgamated.hpp")
set(SOURCES
    ${SOURCE_DIR}/catch_amalgamated.cpp
    ${SOURCE_DIR}/test_radix.cpp
    ${SOURCE_DIR}/test_obj.cpp
    ${SOURCE_DIR}/main.cpp)

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR}../include)

source_group("include" FILES ${HEADERS} ${RAY_PROOF_HEADERS})
source_group("src" FILES ${SOURCES} ${RAY_PROOF_SOURCES})

set(FILES ${HEADERS} ${SOURCES} ${RAY_PROOF_HEADERS} ${RAY_PROOF_SOURCES})

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
set(CMAKE_DEBUG_POSTFIX d)

add_library(${PROJECT_NAME} STATIC ${FILES})

if(MSVC)
    set(DEFAULT_CXX_FLAGS "/DWIN32 /D_WINDOWS /D_UNICODE /DUNICODE /DDGLEW_STATIC /W4 /WX- /nologo /fp:precise /arch:AVX /Zc:wchar_t /TP /Gd /std:c++20 /std:c11")
    if(MSVC_VERSION VERSION_LESS_EQUAL "1900")
        set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /Zc:__cplusplus /std:c++latest")
    else()
        set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /Zc:__cplusplus /std:c++20")
    endif()
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm- /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Oi /GL /GR- /DNDEBUG /EHsc-")

elseif(UNIX)
    set(DEFAULT_CXX_FLAGS "-DDGLEW_STATIC -Wall -O2 -std=c++20 -std=gnu++20 -march=x86-64-v3 -fno-exceptions")
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
elseif(APPLE)
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        OUTPUT_NAME_DEBUG "${PROJECT_NAME}" OUTPUT_NAME_RELEASE "${PROJECT_NAME}"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")


