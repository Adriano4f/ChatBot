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

if(DANA_SANITIZE)
  if(MSVC)
    message(FATAL_ERROR "DANA_SANITIZE is only supported on GCC and Clang")
  endif()
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
  add_link_options(-fsanitize=address,undefined)
endif()

include(cmake/DanaModule.cmake)

# One static library per module. src/<name>/Public is on the include path of
# whoever depends on the module, src/<name>/Private only of the module itself,
# so a module can never reach into another one's internals.
dana_module(GUtils
  SOURCES
    GUtils_Allocation.c
    GUtils_Debug.c
)

dana_module(Hash
  SOURCES
    Hash_Access.c
    Hash_Display.c
    Hash_Hash.c
    Hash_Lifecycle.c
    Hash_Modify.c
    Hash_Utils.c
  PRIVATE_DEPENDS GUtils
)

dana_module(LI_Unit
  SOURCES LI_Unit.c
  PRIVATE_DEPENDS GUtils
)

dana_module(LP_Unit
  SOURCES LP_Unit.c
  PUBLIC_DEPENDS LI_Unit # LP_Unit.h exposes LI_info
)

dana_module(CPU
  SOURCES CPU.c
  PUBLIC_DEPENDS LI_Unit LP_Unit
  PRIVATE_DEPENDS GUtils Hash
)

add_executable(Dana src/Dana/Private/main.c)
target_link_libraries(Dana PRIVATE CPU)

include(CTest)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
