string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

find_package(Git QUIET)
execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} --version
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE COMPILER_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\n" " " COMPILER_VERSION ${COMPILER_VERSION})
message("COMPILER_VERSION=" ${COMPILER_VERSION})
execute_process(
        COMMAND ${GIT_EXECUTABLE} symbolic-ref --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE DISPLAY_GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    
cmake_host_system_information(RESULT THIS_HOST QUERY HOSTNAME)
cmake_host_system_information(RESULT LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
cmake_host_system_information(RESULT VIRTUAL_MEMORY QUERY AVAILABLE_VIRTUAL_MEMORY)

add_executable(event_processor
                event_processor.cpp
                event_producer.cpp
                main.cpp
               )
target_link_libraries(event_processor ${CMAKE_THREAD_LIBS_INIT})
add_custom_command( TARGET event_processor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E md5sum ${CMAKE_BINARY_DIR}/deploy/bin/event_processor
        COMMENT "Print hash")
