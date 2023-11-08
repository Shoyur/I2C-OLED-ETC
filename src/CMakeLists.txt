# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(SRCS ${app_sources})

# execute_process(
#     COMMAND "date" "+%s"
#     OUTPUT_VARIABLE CURRENT_TIMESTAMP
#     OUTPUT_STRIP_TRAILING_WHITESPACE
# )

# file(WRITE ${CMAKE_BINARY_DIR}/timestamp.h "#define PRECOMPILED_TIMESTAMP ${CURRENT_TIMESTAMP}")