#include "Chat.h"
#include "NGram.h"
#include "Trainer.h"
#include "Vocab.h"

#include "check.h"

#include <string.h>


// Temperature 0 makes the reply the deterministic likeliest continuation
static ChatSettings
Deterministic
  (size_t max_reply)
{
  ChatSettings settings = ChatDefaults();
  settings.temperature = 0.0;
  settings.max_reply = max_reply;

  return settings;
}


static void
TestRepliesFromWhatItLearned
  (void)
{
  Vocab *vocab = VocabCreate();
  NGram *model = NGramCreate();
  CHECK(NULL != vocab && NULL != model);

  CHECK(0 == TrainOnText(vocab, model, "the cat sat on the mat"));

  char reply[256];
  CHECK(0 == ChatReply(vocab, model, Deterministic(4), "the", reply,
                       sizeof(reply)));
  CHECK(0 == strcmp("the cat sat on", reply));

  // The reply is seeded from the last word of the input the model knows
  CHECK(0 == ChatReply(vocab, model, Deterministic(2), "tell me about the cat",
                       reply, sizeof(reply)));
  CHECK(0 == strcmp("cat sat", reply));

  NGramDestroy(model);
  VocabDestroy(vocab);
}


// Generation stops when the last word was never followed by anything
static void
TestStopsAtDeadEnd
  (void)
{
  Vocab *vocab = VocabCreate();
  NGram *model = NGramCreate();

  CHECK(0 == TrainOnText(vocab, model, "alpha omega"));

  char reply[256];
  CHECK(0 == ChatReply(vocab, model, Deterministic(20), "alpha", reply,
                       sizeof(reply)));
  CHECK(0 == strcmp("alpha omega", reply));

  NGramDestroy(model);
  VocabDestroy(vocab);
}


static void
TestUnknownInputAndSmallBuffer
  (void)
{
  Vocab *vocab = VocabCreate();
  NGram *model = NGramCreate();

  CHECK(0 == TrainOnText(vocab, model, "the cat sat"));

  char reply[256];
  // Unknown words fall back to the most frequent word it knows
  CHECK(0 == ChatReply(vocab, model, Deterministic(3), "zzz qqq", reply,
                       sizeof(reply)));
  CHECK(0 == strcmp("the cat sat", reply));

  // A buffer too small truncates instead of overflowing
  char small[8];
  CHECK(0 == ChatReply(vocab, model, Deterministic(3), "the", small,
                       sizeof(small)));
  CHECK(0 == strcmp("the cat", small));

  NGramDestroy(model);
  VocabDestroy(vocab);
}


static void
TestNothingLearned
  (void)
{
  Vocab *vocab = VocabCreate();
  NGram *model = NGramCreate();
  char reply[64];

  CHECK(0 != ChatReply(vocab, model, Deterministic(3), "hello", reply,
                       sizeof(reply)));

  CHECK(0 != ChatReply(NULL, model, Deterministic(3), "hello", reply,
                       sizeof(reply)));
  CHECK(0 != ChatReply(vocab, model, Deterministic(3), NULL, reply,
                       sizeof(reply)));
  CHECK(0 != ChatReply(vocab, model, Deterministic(3), "hello", reply, 0));

  NGramDestroy(model);
  VocabDestroy(vocab);
}


int
main
  (void)
{
  TestRepliesFromWhatItLearned();
  TestStopsAtDeadEnd();
  TestUnknownInputAndSmallBuffer();
  TestNothingLearned();

  return 0;
}
