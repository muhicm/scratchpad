cmake_minimum_required(VERSION 3.8)

# default the build type to RelWithDebInfo
if(CMAKE_BUILD_TYPE STREQUAL "")
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()
message("BUILD TYPE " ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Debug Release RelWithDebInfo MinSizeRel")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "Configs" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/deploy/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR})

execute_process(
        COMMAND bash -c "printf \"%s\" $(cat /etc/os-release | grep -E \"^ID=[^\\n]*|^VERSION_ID=[^\\n]*\" | cut -f2 -d= | tr -d '\"')"
        OUTPUT_VARIABLE LINUX_VERSION
)

message("Linux version is '${LINUX_VERSION}'")

set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )

add_definitions(-DFMT_HEADER_ONLY -DHAVE_STD_UNIQUE_PTR -DBOOST_ALLOW_DEPRECATED_HEADERS -DOPENSSL_SUPPRESS_DEPRECATED)
set(CMAKE_CXX_FLAGS "-Wall -Werror -fno-var-tracking -Wno-maybe-uninitialized -rdynamic" CACHE STRING "Default C++ compiler flags")
set(CMAKE_CXX_FLAGS_DEBUG "-g" CACHE STRING "Debug C++ compiler flags")
set(CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING "Release C++ compiler flags")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2" CACHE STRING "RelWithDebInfo C++ compiler flags")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_FIND_LIBRARY_PREFIXES lib)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
find_library(PTHREAD_LIB NAMES libpthread.a pthread)
find_library(DL_LIB NAMES libdl.a dl)

project(scratchpad CXX)

find_package(Threads REQUIRED)

include_directories(
        src/
)

link_directories(
        system
)

add_subdirectory(src)
