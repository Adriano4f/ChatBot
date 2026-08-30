cmake_minimum_required(VERSION 3.16)

project(Dana LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(DANA_SANITIZE "Build with the address and undefined behaviour sanitizers" OFF)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

# Everything but main.c lives here, so the tests can link against it
add_library(dana_core STATIC
  src/core/GUtils_Allocation.c
  src/core/GUtils_Debug.c
  src/core/Hash_Access.c
  src/core/Hash_Display.c
  src/core/Hash_Hash.c
  src/core/Hash_Lifecycle.c
  src/core/Hash_Modify.c
  src/core/Hash_Utils.c
  src/cpu/CPU.c
  src/lang/LI_Unit.c
  src/lang/LP_Unit.c
)

target_include_directories(dana_core
  PUBLIC include
  PRIVATE src/core # Hash_internal.h and GUtils_internal.h
)

if(DANA_SANITIZE)
  if(MSVC)
    message(FATAL_ERROR "DANA_SANITIZE is only supported on GCC and Clang")
  endif()
  target_compile_options(dana_core PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer -g)
  target_link_options(dana_core PUBLIC -fsanitize=address,undefined)
endif()

add_executable(Dana src/main.c)
target_link_libraries(Dana PRIVATE dana_core)

include(CTest)

if(BUILD_TESTING)
  foreach(test_name test_hash test_tokenise)
    add_executable(${test_name} tests/${test_name}.c)
    target_link_libraries(${test_name} PRIVATE dana_core)
    target_include_directories(${test_name} PRIVATE src/core tests)
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()
