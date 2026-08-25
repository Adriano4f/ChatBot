# Tests

Unit tests for the modules Dana is built from. No external dependency: the
whole harness is `TestFramework.[ch]`, the suite links every module except
`source/main.c` and a POSIX host is required (the tests redirect `stdin` and
`stdout` to check what each unit reads and prints).

```sh
make test      # build and run the suite
make coverage  # same, plus a per file line coverage report from gcov
make clean-test
```

`make coverage` leaves the gcov data and the `.gcov` files in `build/coverage`.

## Layout

| File                | Covers                                          |
| ------------------- | ----------------------------------------------- |
| `test_GUtils.c`     | `GUtils_Debug.c`, `GUtils_Allocation.c`         |
| `test_Hash.c`       | every file of `source/Hash`                     |
| `test_LI_Unit.c`    | `LI_Unit.c`                                     |
| `test_LP_Unit.c`    | `LP_Unit.c`, `IntentP.c`                        |

Adding a test: write it with the `TEST(name)` macro, assert with the `CHECK_*`
macros and register it with `RUN_TEST(name)` in the `Register*Tests` function at
the bottom of the file. New files only need a `Register*Tests` call in
`TestRunner.c`.

## What is not covered, and why

- `CPU.c` (`CentralProcess`) is an interactive loop that only ends when the
  global `Exit` flag is set, which nothing does yet, so it cannot be run from a
  test without hanging.
- `Hresize` / `Hrehash` are not exercised: `Hrehash` walks the *old* table using
  the *new* capacity, so the first resize (6 inserts on the default capacity of
  8) reads past the end of the old allocation. The table tests deliberately
  stay below that threshold.
- The growth path of `Tokenise` is not exercised for the same reason: its
  reallocations size the pointer array in `char`s instead of pointers
  (`ReallocPPtr(sz * sizeof(char), ...)` for `sz` pointers), so the final
  shrink to `i + 5` bytes leaves the array smaller than the `i + 1` pointers
  written into it.
- `Tokenise` also shrinks its first token to `strlen` bytes, dropping the
  terminator, which is why the tests compare that token byte by byte instead of
  with `strcmp`.
- The `UNKNOWN_ERROR` branch of `GetInput` needs a `stdin` that fails without
  setting either `feof` or `ferror`, which cannot be forced portably.
- `BA_Unit.c`, `LG_Unit.c`, `MLearning.c`, `M_Unit.c` and `SemanticP.c` are
  empty.
