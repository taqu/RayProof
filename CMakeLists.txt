cmake_minimum_required(VERSION 3.25)

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

set(PROJECT_NAME RayProof)
project(${PROJECT_NAME})

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(AFTER ${PROJECT_ROOT}/include)
set(HEADER_DIR ${PROJECT_ROOT}/include)
set(SOURCE_DIR ${PROJECT_ROOT}/src)
set(THIRDPARTY_DIR ${PROJECT_ROOT}/thirdparty)

if(WIN32)
    set(FLTK_ROOT ${THIRDPARTY_DIR}/fltk/win)
elseif(UNIX)
    set(FLTK_ROOT ${THIRDPARTY_DIR}/fltk/linux)
else()
endif()
include_directories(AFTER ${FLTK_ROOT}/include)
link_directories(AFTER ${FLTK_ROOT}/lib)

# build thirdparties
configure_file(${PROJECT_ROOT}/CMakeLists_Thirdparty.txt.in ${THIRDPARTY_DIR}/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${THIRDPARTY_DIR})
if(result)
    message(FATAL_ERROR "Adding external projects failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Debug --target ALL_BUILD
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${THIRDPARTY_DIR})
if(result)
    message(FATAL_ERROR "Building debug external projects failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release --target ALL_BUILD
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${THIRDPARTY_DIR})
if(result)
    message(FATAL_ERROR "Building release external projects failed: ${result}")
endif()

############################################################
# mimalloc
set(MIMALLOC_ROOT ${THIRDPARTY_DIR}/mimalloc)
set(MIMALLOC_INCLUDE ${MIMALLOC_ROOT}/include/mimalloc-2.1)
set(MIMALLOC_LIB ${MIMALLOC_ROOT}/lib/mimalloc-2.1)

include_directories(AFTER ${MIMALLOC_INCLUDE})

link_directories(AFTER ${MIMALLOC_LIB})

add_library(MIMALLOC MODULE IMPORTED)
set_target_properties(MIMALLOC PROPERTIES IMPORTED_LOCATION_DEBUG "mimalloc-staticd.lib" IMPORTED_LOCATION_RELEASE "mimalloc-static.lib")

########################################################################
# Sources
set(HEADERS
    "${HEADER_DIR}/Core.h"
    "${HEADER_DIR}/Fixed16.h"
    "${HEADER_DIR}/mikktspace.h"
    "${HEADER_DIR}/Vector.h"
    "${HEADER_DIR}/AABB.h"
    "${HEADER_DIR}/Ray.h"
    "${HEADER_DIR}/Coordinate.h"
    "${HEADER_DIR}/cppimg.h"
    "${HEADER_DIR}/cppobj.h"
    "${HEADER_DIR}/szlib.h"
    "${HEADER_DIR}/Random.h"
    "${HEADER_DIR}/RadixSort.h"
    "${HEADER_DIR}/Material.h"
    "${HEADER_DIR}/HitRecord.h"
    "${HEADER_DIR}/Hittable.h"
    "${HEADER_DIR}/Sphere.h"
    "${HEADER_DIR}/Plane.h"
    "${HEADER_DIR}/LQBVH.h"
    "${HEADER_DIR}/BinQBVH.h"
    "${HEADER_DIR}/Primitive.h"
    "${HEADER_DIR}/Face.h"
    "${HEADER_DIR}/Vertex.h"
    "${HEADER_DIR}/Texture.h"
    "${HEADER_DIR}/Image.h"
    "${HEADER_DIR}/Object.h"
    "${HEADER_DIR}/Color.h"
    "${HEADER_DIR}/World.h"
    "${HEADER_DIR}/Camera.h"
    "${HEADER_DIR}/Context.h")

set(SOURCES
    "${SOURCE_DIR}/Core.cpp"
    "${SOURCE_DIR}/Fixed16.cpp"
    "${SOURCE_DIR}/mikktspace.cpp"
    "${SOURCE_DIR}/Vector.cpp"
    "${SOURCE_DIR}/AABB.cpp"
    "${SOURCE_DIR}/Ray.cpp"
    "${SOURCE_DIR}/Coordinate.cpp"
    "${SOURCE_DIR}/cppimg.cpp"
    "${SOURCE_DIR}/cppobj.cpp"
    "${SOURCE_DIR}/szlib.cpp"
    "${SOURCE_DIR}/Random.cpp"
    "${SOURCE_DIR}/RadixSort.cpp"
    "${SOURCE_DIR}/Material.cpp"
    "${SOURCE_DIR}/Sphere.cpp"
    "${SOURCE_DIR}/Plane.cpp"
    "${SOURCE_DIR}/LQBVH.cpp"
    "${SOURCE_DIR}/BinQBVH.cpp"
    "${SOURCE_DIR}/Texture.cpp"
    "${SOURCE_DIR}/Image.cpp"
    "${SOURCE_DIR}/Object.cpp"
    "${SOURCE_DIR}/Color.cpp"
    "${SOURCE_DIR}/World.cpp"
    "${SOURCE_DIR}/Camera.cpp"
    "${SOURCE_DIR}/Context.cpp")

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES} "${SOURCE_DIR}/main.cpp")

set(FILES ${HEADERS} ${SOURCES} "${SOURCE_DIR}/main.cpp")

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

add_executable(${PROJECT_NAME} ${FILES})

if(MSVC)
    set(DEFAULT_CXX_FLAGS "/DWIN32 /D_WINDOWS /D_UNICODE /DUNICODE /W4 /WX- /nologo /fp:precise /arch:AVX /Zc:wchar_t /TP /Gd /std:c++20 /std:c11")
    if(MSVC_VERSION VERSION_LESS_EQUAL "1900")
        set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /Zc:__cplusplus /std:c++latest")
    else()
        set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /Zc:__cplusplus /std:c++20")
    endif()
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm- /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Oi /GL /GR- /DNDEBUG /EHsc-")
    target_link_libraries(${PROJECT_NAME} MIMALLOC fltk.lib fltk_forms.lib fltk_images.lib fltk_jpeg.lib fltk_png.lib fltk_z.lib)

elseif(UNIX)
    set(DEFAULT_CXX_FLAGS "-Wall -O2 -std=c++20 -std=gnu++20 -march=x86-64-v3 -fno-exceptions")
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
    target_link_libraries(${PROJECT_NAME} MIMALLOC libfltk.a libfltk_forms.a libfltk_images.a libfltk_jpeg.a libfltk_png.a libfltk_z.a libX11.a libXext.a libxcb.a libXau.a libXdmcp.a)
elseif(APPLE)
elseif(APPLE)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        OUTPUT_NAME_DEBUG "${PROJECT_NAME}" OUTPUT_NAME_RELEASE "${PROJECT_NAME}"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(test)

