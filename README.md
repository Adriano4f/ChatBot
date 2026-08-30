# ChatBot

A dependency-free C17 chatbot skeleton: a custom open-addressed hash table, a
tokeniser and the input/processing units it feeds. `Structure.txt` describes the
module layout the project is growing into.

## Build

```sh
cmake -S . -B build
cmake --build build
./build/Dana
```

## Tests

```sh
ctest --test-dir build --output-on-failure
```

## Sanitizers

```sh
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DDANA_SANITIZE=ON
cmake --build build-asan
ctest --test-dir build-asan --output-on-failure
```

## Layout

```
include/      public headers
src/core/     hash table and allocation helpers
src/cpu/      the central process loop
src/lang/     language input and processing units
tests/        ctest executables
```
