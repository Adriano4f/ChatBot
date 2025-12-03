cmake_minimum_required(VERSION 3.10)
project(Dana LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Helper function to recursively gather all .c files in a folder
function(add_module module_name)
  file(GLOB_RECURSE MODULE_SOURCES
    ${PROJECT_SOURCE_DIR}/source/${module_name}/*.c
  )
  add_library(${module_name} ${MODULE_SOURCES})
  target_include_directories(${module_name} PUBLIC ${PROJECT_SOURCE_DIR}/include)
endfunction()

add_module(Core)
add_module(Utils)

add_executable(Dana
  source/main.c
)

target_link_libraries(Dana
  PRIVATE
    Core
    Utils
)

# Global compile options
target_compile_options(Dana PRIVATE -Wall -Wextra -O2)
target_compile_options(Core PRIVATE -Wall -Wextra -O2)
target_compile_options(Utils PRIVATE -Wall -Wextra -O2)

# Copy executable to bin/
add_custom_command(
    TARGET Dana
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:Dana>
            ${PROJECT_SOURCE_DIR}/bin/$<TARGET_FILE_NAME:Dana>
)