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

One directory per module, each split the way Unreal Engine splits its modules:

```
src/<Module>/Public/    the module's API: what other modules may include
src/<Module>/Private/   implementation and internal headers
cmake/DanaModule.cmake  the dana_module() helper every module is declared with
tests/                  ctest executables
```

Modules, bottom up:

| Module    | Purpose                                        |
|-----------|------------------------------------------------|
| `GUtils`  | allocation wrappers and debug output           |
| `Hash`    | open-addressed hash table                      |
| `LI_Unit` | language input: reading stdin and tokenising   |
| `LP_Unit` | language processing                            |
| `CPU`     | the central process loop                       |
| `Dana`    | the executable, `main.c` only                  |

Adding a module means creating `src/<Name>/{Public,Private}` and one
`dana_module()` call:

```cmake
dana_module(MyModule
  SOURCES MyModule.c        # relative to src/MyModule/Private
  PUBLIC_DEPENDS LI_Unit    # modules named in MyModule.h
  PRIVATE_DEPENDS GUtils    # modules used only by the implementation
)
```

Each module's `Private` directory is on its own include path only, so a module
cannot include another module's internal headers — the compiler enforces the
API boundary rather than a convention.
