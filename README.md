# Dana

A dependency-free C17 language model, written from scratch. It learns from a
text file by counting which word follows which, then talks by drawing the next
word from those counts — no rules are written by hand and nothing is downloaded.

```sh
cmake -S . -B build
cmake --build build

./build/Dana vocab corpus.txt   # what it found in the text
./build/Dana chat  corpus.txt   # learn the text, then talk
```

```
$ ./build/Dana vocab README.md
139 distinct words, 241 words read, 138 contexts

most frequent words
  the                  11
  a                    6
  module               6
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
tests/                  ctest executables, one per module
```

Modules, bottom up:

| Module        | Purpose                                                     |
|---------------|-------------------------------------------------------------|
| `Core`        | status codes, allocation wrappers, logging                  |
| `Collections` | `HashMap`: open-addressed, owns the keys and values it holds |
| `Text`        | `Tokenizer`: text in, lowercased tokens out                 |
| `Corpus`      | reading a corpus file and reading a line of input           |
| `Vocab`       | words to integer ids and back, with occurrence counts       |
| `NGram`       | the model: bigram counts, smoothed probability, sampling    |
| `Trainer`     | counting a corpus into a `Vocab` and an `NGram`             |
| `Chat`        | generating a reply, and the conversation loop               |
| `App`         | the executable: `vocab` and `chat` subcommands              |

Everything past `Text` works on `uint32_t` word ids rather than strings.

Adding a module means creating `src/<Name>/{Public,Private}` and one
`dana_module()` call:

```cmake
dana_module(MyModule
  SOURCES MyModule.c        # relative to src/MyModule/Private
  PUBLIC_DEPENDS NGram      # modules named in MyModule.h
  PRIVATE_DEPENDS Core      # modules used only by the implementation
)
```

Each module's `Private` directory is on its own include path only, so a module
cannot include another module's internal headers — the compiler enforces the
API boundary rather than a convention.

## The next rungs

The model here is the first one on the ladder: it looks one word back. What
follows, in order, is a trigram model with backoff, then learned embeddings and
a feed-forward network trained by hand-written backpropagation (`llm.c` is the
reference), then an RNN, then attention.
