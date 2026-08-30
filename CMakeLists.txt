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

# Foundation: allocation, status codes, logging
dana_module(Core
  SOURCES
    Core.c
    Log.c
)

# Data structures the model is built out of
dana_module(Collections
  SOURCES
    HashMap.c
    HashMap_Probe.c
  PRIVATE_DEPENDS Core
)

# Text in, tokens out
dana_module(Text
  SOURCES Tokenizer.c
  PRIVATE_DEPENDS Core
)

# Where the text comes from
dana_module(Corpus
  SOURCES Corpus.c
  PRIVATE_DEPENDS Core
)

# Words <-> integer ids
dana_module(Vocab
  SOURCES Vocab.c
  PRIVATE_DEPENDS Core Collections
)

# The language model itself
dana_module(NGram
  SOURCES
    NGram.c
    NGram_Sampling.c
  PUBLIC_DEPENDS Vocab
  PRIVATE_DEPENDS Core Collections
)

# Counting a corpus into the model
dana_module(Trainer
  SOURCES Trainer.c
  PUBLIC_DEPENDS NGram Vocab
  PRIVATE_DEPENDS Core Corpus Text
)

# Generating replies and the conversation loop
dana_module(Chat
  SOURCES Chat.c
  PUBLIC_DEPENDS NGram Vocab
  PRIVATE_DEPENDS Core Corpus Text
)

add_executable(Dana src/App/Private/main.c)
target_link_libraries(Dana PRIVATE Chat Core Trainer)

if(NOT MSVC)
  target_link_libraries(NGram PRIVATE m)
endif()

include(CTest)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
