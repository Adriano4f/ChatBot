#include "HashMap.h"

#include "check.h"

#include <stdio.h>
#include <string.h>

#define KEY(literal) (literal), (sizeof(literal))


static void
TestPutGet
  (void)
{
  HashMap *map = HashMapCreate(sizeof(int));
  CHECK(NULL != map);

  int value = 7;
  CHECK(NULL != HashMapPut(map, KEY("droga"), &value));
  CHECK(1 == HashMapCount(map));

  int *stored = (int *)HashMapGet(map, KEY("droga"));
  CHECK(NULL != stored && 7 == *stored);
  CHECK(NULL == HashMapGet(map, KEY("missing")));

  // Overwriting replaces the value and does not grow the map
  value = 9;
  CHECK(NULL != HashMapPut(map, KEY("droga"), &value));
  CHECK(1 == HashMapCount(map));
  CHECK(9 == *(int *)HashMapGet(map, KEY("droga")));

  HashMapDestroy(map);
}


// The map copies what it is given, so callers may reuse their buffers
static void
TestOwnsKeysAndValues
  (void)
{
  HashMap *map = HashMapCreate(sizeof(int));
  CHECK(NULL != map);

  char key[8];
  int value = 42;
  memcpy(key, "stable", 7);
  CHECK(NULL != HashMapPut(map, key, 7, &value));

  memcpy(key, "junkju", 7);
  value = 0;

  int *stored = (int *)HashMapGet(map, "stable", 7);
  CHECK(NULL != stored && 42 == *stored);

  HashMapDestroy(map);
}


static void
TestRemoveAndTombstones
  (void)
{
  HashMap *map = HashMapCreate(sizeof(int));
  CHECK(NULL != map);

  for ( int i = 0; i < 200; ++i )
  {
    char key[32];
    snprintf(key, sizeof(key), "key-%d", i);
    CHECK(NULL != HashMapPut(map, key, strlen(key) + 1, &i));
  }
  CHECK(200 == HashMapCount(map));

  // Every key must stay reachable after deletions probed past a tombstone
  for ( int i = 0; i < 200; i += 2 )
  {
    char key[32];
    snprintf(key, sizeof(key), "key-%d", i);
    CHECK(HashMapRemove(map, key, strlen(key) + 1));
  }
  CHECK(100 == HashMapCount(map));

  for ( int i = 0; i < 200; ++i )
  {
    char key[32];
    snprintf(key, sizeof(key), "key-%d", i);
    int *stored = (int *)HashMapGet(map, key, strlen(key) + 1);

    if ( 0 == i % 2 )
      CHECK(NULL == stored);
    else
      CHECK(NULL != stored && i == *stored);
  }

  CHECK(!HashMapRemove(map, KEY("key-0")));

  // A reused tombstone must not double-count
  int value = -1;
  CHECK(NULL != HashMapPut(map, KEY("key-0"), &value));
  CHECK(101 == HashMapCount(map));

  HashMapDestroy(map);
}


static void
TestIteration
  (void)
{
  HashMap *map = HashMapCreate(sizeof(int));
  CHECK(NULL != map);

  int sum = 0;
  for ( int i = 1; i <= 50; ++i )
  {
    char key[32];
    snprintf(key, sizeof(key), "%d", i);
    CHECK(NULL != HashMapPut(map, key, strlen(key) + 1, &i));
    sum += i;
  }

  int seen = 0;
  size_t visited = 0;
  size_t cursor = 0;
  HashMapEntry entry;
  while ( HashMapNext(map, &cursor, &entry) )
  {
    seen += *(int *)entry.value;
    ++visited;
  }

  CHECK(50 == visited);
  CHECK(sum == seen);

  HashMapDestroy(map);
}


static void
TestInvalidArguments
  (void)
{
  CHECK(NULL == HashMapCreate(0));

  HashMap *map = HashMapCreate(sizeof(int));
  CHECK(NULL != map);
  CHECK(NULL == HashMapPut(map, NULL, 4, NULL));
  CHECK(NULL == HashMapPut(map, "word", 0, NULL));
  CHECK(0 == HashMapCount(map));

  // A NULL value leaves a new entry zeroed
  int *stored = (int *)HashMapPut(map, KEY("word"), NULL);
  CHECK(NULL != stored && 0 == *stored);
  HashMapDestroy(map);

  CHECK(NULL == HashMapGet(NULL, KEY("word")));
  CHECK(!HashMapRemove(NULL, KEY("word")));
  CHECK(0 == HashMapCount(NULL));
  HashMapDestroy(NULL);
}


int
main
  (void)
{
  TestPutGet();
  TestOwnsKeysAndValues();
  TestRemoveAndTombstones();
  TestIteration();
  TestInvalidArguments();

  return 0;
}
