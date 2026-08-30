#ifndef DANA_CHAT_H
#define DANA_CHAT_H

#include <stddef.h>

#include "NGram.h"
#include "Vocab.h"

#define CHAT_INPUT_SIZE 1024

typedef struct ChatSettings
{
  double temperature;   // 0 picks the most likely word, higher is more random
  size_t max_reply;     // Words to generate at most
} ChatSettings;

ChatSettings
ChatDefaults
  (void);

/*
  Generates a reply to `input` into `out` (a buffer of `size` bytes, always
  NUL-terminated on success). Returns DANA_EINVAL when the model has nothing
  to say about any word of the input.
*/
int
ChatReply
  (const Vocab *vocab,
  const NGram *model,
  ChatSettings settings,
  const char *input,
  char *out,
  size_t size);

// The read/reply loop, ends on end of input
int
ChatRun
  (const Vocab *vocab,
  const NGram *model,
  ChatSettings settings);

#endif
