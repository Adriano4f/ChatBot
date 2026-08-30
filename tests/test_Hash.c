#include "TestFramework.h"

#include "Hash_internal.h" // Pulls Hash.h too

// STD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  The table stores the pointers it is given, it does not copy keys nor values,
  so every key/value used here is a string literal or a static buffer.
*/

#define KEY(literal) (literal), sizeof(literal)

/*
  Fills keys with n distinct keys that all hash to the same bucket of a table of
  the given capacity, so the probing, tombstone and collision paths can be
  driven without depending on a particular hash value.
*/
static void
CollidingKeys
  (char keys[][16],
  const size_t n,
  const size_t capacity)
{
  size_t found = 0;
  size_t target = SIZE_MAX;

  for ( size_t i = 0; found < n && i < 100000; ++i )
  {
    char candidate[16];
    snprintf(candidate, sizeof(candidate), "k%zu", i);
    const size_t bucket = hashfn(candidate, strlen(candidate) + 1) & (capacity - 1);

    if ( SIZE_MAX == target )
      target = bucket;
    if ( bucket != target )
      continue;

    strcpy(keys[found], candidate);
    ++found;
  }
}

/* == hashfn == */

TEST(hashfn_is_deterministic)
{
  CHECK_EQ_SIZE( hashfn(KEY("droga")), hashfn(KEY("droga")) );
}

TEST(hashfn_empty_key_is_the_fnv_offset_basis)
{
  CHECK_EQ_SIZE( hashfn("", 0), 0xcbf29ce484222325ULL );
}

TEST(hashfn_differs_for_different_keys)
{
  CHECK_TRUE( hashfn(KEY("droga")) != hashfn(KEY("drogb")) );
}

TEST(hashfn_is_order_sensitive)
{
  CHECK_TRUE( hashfn(KEY("ab")) != hashfn(KEY("ba")) );
}

TEST(hashfn_only_reads_size_bytes)
{
  CHECK_EQ_SIZE( hashfn("az", 1), hashfn("ax", 1) );
}

TEST(hashfn_reads_key_bytes_as_unsigned)
{
  /* Reading through a signed char would sign extend 0xff into the whole word. */
  const size_t expected = (0xcbf29ce484222325ULL ^ 0xffULL) * 0x100000001b3ULL;

  CHECK_EQ_SIZE( hashfn("\xff", 1), expected );
}

/* == Hcompare_key_entry == */

TEST(compare_key_entry_matches_equal_keys)
{
  char key[] = "droga";
  const RHEntry entry = { key, sizeof(key), NULL, 0, 0, OCCUPIED };

  CHECK_TRUE( Hcompare_key_entry(KEY("droga"), entry) );
}

TEST(compare_key_entry_rejects_different_content)
{
  char key[] = "droga";
  const RHEntry entry = { key, sizeof(key), NULL, 0, 0, OCCUPIED };

  CHECK_FALSE( Hcompare_key_entry(KEY("drogb"), entry) );
}

TEST(compare_key_entry_rejects_different_size)
{
  char key[] = "droga";
  const RHEntry entry = { key, sizeof(key), NULL, 0, 0, OCCUPIED };

  CHECK_FALSE( Hcompare_key_entry("droga", sizeof("droga") - 1, entry) );
}

TEST(compare_key_entry_ignores_deleted_entries)
{
  char key[] = "droga";
  const RHEntry entry = { key, 0, NULL, 0, 0, TOMBSTONE }; // Hdelete zeroes ksize

  CHECK_FALSE( Hcompare_key_entry(KEY("droga"), entry) );
}

/* == Hinit / Hdestroy == */

TEST(init_returns_an_empty_table)
{
  Hash *h = Hinit();
  CHECK_NOT_NULL( h );
  if ( h == NULL )
    return;

  CHECK_EQ_SIZE( h->CAPACITY, 8 );
  CHECK_EQ_SIZE( h->SIZE, 0 );
  CHECK_EQ_SIZE( h->LOAD_FACTOR, 0 );
  CHECK_NOT_NULL( h->TABLE );

  for ( size_t i = 0; i < h->CAPACITY; ++i )
    CHECK_EQ_INT( h->TABLE[i].state, EMPTY );

  Hdestroy(h);
}

TEST(init_binds_the_methods)
{
  Hash *h = Hinit();

  CHECK_TRUE( h->insert == Hinsert );
  CHECK_TRUE( h->delete == Hdelete );
  CHECK_TRUE( h->fetch == Hfetch );
  CHECK_TRUE( h->display == Hdisplay );
  CHECK_TRUE( h->resize == Hresize );
  CHECK_TRUE( h->destroy == Hdestroy );

  DESTROY(h);
}

/* == Hinsert / Hfetch == */

TEST(insert_then_fetch_returns_the_value)
{
  Hash *h = Hinit();

  CHECK_EQ_INT( Hinsert(h, KEY("droga"), KEY("Cocaina")), 0 );

  const RHEntry *entry = Hfetch(h, KEY("droga"));
  CHECK_NOT_NULL( entry );
  if ( entry != NULL )
  {
    CHECK_EQ_SIZE( entry->ksize, sizeof("droga") );
    CHECK_EQ_SIZE( entry->vsize, sizeof("Cocaina") );
    CHECK_EQ_STR( (const char *)entry->value, "Cocaina" );
    CHECK_EQ_INT( entry->state, OCCUPIED );
  }
  CHECK_EQ_SIZE( h->SIZE, 1 );

  Hdestroy(h);
}

TEST(insert_accounts_the_load_factor_per_thousand)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("a"), KEY("1"));
  CHECK_EQ_SIZE( h->LOAD_FACTOR, 125 ); // 1 * 1000 / 8

  Hinsert(h, KEY("b"), KEY("2"));
  CHECK_EQ_SIZE( h->LOAD_FACTOR, 250 );

  Hdestroy(h);
}

TEST(insert_of_an_existing_key_replaces_the_value)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));
  Hinsert(h, KEY("droga"), KEY("Metanfetamina"));

  const RHEntry *entry = Hfetch(h, KEY("droga"));
  CHECK_NOT_NULL( entry );
  if ( entry != NULL )
    CHECK_EQ_STR( (const char *)entry->value, "Metanfetamina" );

  CHECK_EQ_SIZE( h->SIZE, 1 ); // An update must not grow the table
  CHECK_EQ_SIZE( h->LOAD_FACTOR, 125 );

  Hdestroy(h);
}

TEST(fetch_of_a_missing_key_returns_null)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));

  CHECK_NULL( Hfetch(h, KEY("el caco de")) );

  Hdestroy(h);
}

TEST(fetch_on_an_empty_table_returns_null)
{
  Hash *h = Hinit();

  CHECK_NULL( Hfetch(h, KEY("droga")) );

  Hdestroy(h);
}

TEST(fetch_distinguishes_keys_sharing_a_prefix)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));
  Hinsert(h, KEY("drogas"), KEY("Varias"));

  const RHEntry *one = Hfetch(h, KEY("droga"));
  const RHEntry *many = Hfetch(h, KEY("drogas"));

  CHECK_NOT_NULL( one );
  CHECK_NOT_NULL( many );
  if ( one != NULL && many != NULL )
  {
    CHECK_EQ_STR( (const char *)one->value, "Cocaina" );
    CHECK_EQ_STR( (const char *)many->value, "Varias" );
  }

  Hdestroy(h);
}

TEST(insert_keeps_every_key_reachable)
{
  static const char *const keys[] = { "uno", "dos", "tres", "cuatro", "cinco" };
  static const char *const values[] = { "1", "2", "3", "4", "5" };
  Hash *h = Hinit();

  for ( size_t i = 0; i < 5; ++i )
    CHECK_EQ_INT( Hinsert(h, keys[i], strlen(keys[i]) + 1,
                          values[i], strlen(values[i]) + 1), 0 );

  CHECK_EQ_SIZE( h->SIZE, 5 );

  for ( size_t i = 0; i < 5; ++i )
  {
    const RHEntry *entry = Hfetch(h, keys[i], strlen(keys[i]) + 1);
    CHECK_NOT_NULL( entry );
    if ( entry != NULL )
      CHECK_EQ_STR( (const char *)entry->value, values[i] );
  }

  Hdestroy(h);
}

TEST(insert_handles_binary_keys_containing_zero_bytes)
{
  static const char key[] = { 'a', '\0', 'b' };
  static const int value = 42;
  Hash *h = Hinit();

  Hinsert(h, key, sizeof(key), &value, sizeof(value));

  const RHEntry *entry = Hfetch(h, key, sizeof(key));
  CHECK_NOT_NULL( entry );
  if ( entry != NULL )
    CHECK_EQ_INT( *(const int *)entry->value, 42 );

  CHECK_NULL( Hfetch(h, "a", 1) ); // Only the first byte matches

  Hdestroy(h);
}

/* == Hdelete == */

TEST(delete_makes_the_key_unreachable)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));
  Hdelete(h, KEY("droga"));

  CHECK_NULL( Hfetch(h, KEY("droga")) );

  Hdestroy(h);
}

TEST(delete_tombstones_the_slot_without_freeing_the_value)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));
  const size_t idx = Hfind_slot(h, KEY("droga"));
  Hdelete(h, KEY("droga"));

  CHECK_EQ_INT( h->TABLE[idx].state, TOMBSTONE );
  CHECK_EQ_SIZE( h->TABLE[idx].ksize, 0 );
  CHECK_NOT_NULL( h->TABLE[idx].value );
  if ( h->TABLE[idx].value != NULL )
    CHECK_EQ_STR( (const char *)h->TABLE[idx].value, "Cocaina" );

  Hdestroy(h);
}

TEST(delete_of_a_missing_key_changes_nothing)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));
  Hdelete(h, KEY("inexistente"));

  CHECK_EQ_SIZE( h->SIZE, 1 );
  CHECK_NOT_NULL( Hfetch(h, KEY("droga")) );

  Hdestroy(h);
}

TEST(delete_keeps_the_other_keys_reachable)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));
  Hinsert(h, KEY("el caco de"), KEY("Lauriano"));
  Hdelete(h, KEY("droga"));

  CHECK_NULL( Hfetch(h, KEY("droga")) );
  const RHEntry *entry = Hfetch(h, KEY("el caco de"));
  CHECK_NOT_NULL( entry );
  if ( entry != NULL )
    CHECK_EQ_STR( (const char *)entry->value, "Lauriano" );

  Hdestroy(h);
}

TEST(insert_reuses_a_tombstoned_slot)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));
  Hdelete(h, KEY("droga"));
  Hinsert(h, KEY("droga"), KEY("Metanfetamina"));

  const RHEntry *entry = Hfetch(h, KEY("droga"));
  CHECK_NOT_NULL( entry );
  if ( entry != NULL )
    CHECK_EQ_STR( (const char *)entry->value, "Metanfetamina" );

  Hdestroy(h);
}

TEST(delete_decrements_the_size_and_the_load_factor)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("a"), KEY("1"));
  Hinsert(h, KEY("b"), KEY("2"));
  Hdelete(h, KEY("a"));

  CHECK_EQ_SIZE( h->SIZE, 1 );
  CHECK_EQ_SIZE( h->LOAD_FACTOR, 125 ); // 1 * 1000 / 8

  /* Inserting and deleting the same key must not inflate the accounting. */
  for ( size_t i = 0; i < 20; ++i )
  {
    Hinsert(h, KEY("a"), KEY("1"));
    Hdelete(h, KEY("a"));
  }

  CHECK_EQ_SIZE( h->SIZE, 1 );
  CHECK_EQ_SIZE( h->CAPACITY, 8 );

  Hdestroy(h);
}

TEST(fetch_finds_a_key_stored_after_a_tombstone)
{
  static char keys[3][16];
  Hash *h = Hinit();

  CollidingKeys(keys, 3, h->CAPACITY);

  for ( size_t i = 0; i < 3; ++i )
    Hinsert(h, keys[i], strlen(keys[i]) + 1, keys[i], strlen(keys[i]) + 1);

  /* The hole left in the middle of the chain must not hide what follows it. */
  Hdelete(h, keys[1], strlen(keys[1]) + 1);

  CHECK_NULL( Hfetch(h, keys[1], strlen(keys[1]) + 1) );

  const RHEntry *last = Hfetch(h, keys[2], strlen(keys[2]) + 1);
  CHECK_NOT_NULL( last );
  if ( last != NULL )
    CHECK_EQ_STR( (const char *)last->value, keys[2] );

  Hdestroy(h);
}

TEST(insert_does_not_duplicate_a_key_stored_after_a_tombstone)
{
  static char keys[3][16];
  Hash *h = Hinit();

  CollidingKeys(keys, 3, h->CAPACITY);

  for ( size_t i = 0; i < 3; ++i )
    Hinsert(h, keys[i], strlen(keys[i]) + 1, "viejo", sizeof("viejo"));

  Hdelete(h, keys[0], strlen(keys[0]) + 1); // Tombstone at the head of the chain
  Hinsert(h, keys[2], strlen(keys[2]) + 1, "nuevo", sizeof("nuevo"));

  CHECK_EQ_SIZE( h->SIZE, 2 ); // Updated in place, not written to the tombstone

  size_t copies = 0;
  for ( size_t i = 0; i < h->CAPACITY; ++i )
  {
    if ( OCCUPIED == h->TABLE[i].state
      && Hcompare_key_entry(keys[2], strlen(keys[2]) + 1, h->TABLE[i]) )
      ++copies;
  }
  CHECK_EQ_SIZE( copies, 1 );

  const RHEntry *entry = Hfetch(h, keys[2], strlen(keys[2]) + 1);
  CHECK_NOT_NULL( entry );
  if ( entry != NULL )
    CHECK_EQ_STR( (const char *)entry->value, "nuevo" );

  Hdestroy(h);
}

/* == Hresize / Hrehash == */

TEST(insert_grows_the_table_past_its_capacity)
{
  static char keys[64][16];
  Hash *h = Hinit();

  for ( size_t i = 0; i < 64; ++i )
  {
    snprintf(keys[i], sizeof(keys[i]), "clave%zu", i);
    CHECK_EQ_INT( Hinsert(h, keys[i], strlen(keys[i]) + 1,
                          keys[i], strlen(keys[i]) + 1), 0 );
  }

  CHECK_EQ_SIZE( h->SIZE, 64 );
  CHECK_TRUE( h->CAPACITY > 64 );
  CHECK_EQ_SIZE( h->LOAD_FACTOR, h->SIZE*1000/h->CAPACITY );
  CHECK_TRUE( h->LOAD_FACTOR <= LF_RESIZE_TRIGGER_VALUE );

  for ( size_t i = 0; i < 64; ++i )
  {
    const RHEntry *entry = Hfetch(h, keys[i], strlen(keys[i]) + 1);
    CHECK_NOT_NULL( entry );
    if ( entry != NULL )
      CHECK_EQ_STR( (const char *)entry->value, keys[i] );
  }

  Hdestroy(h);
}

TEST(resize_rehashes_every_entry_and_drops_the_tombstones)
{
  static char keys[6][16];
  Hash *h = Hinit();

  for ( size_t i = 0; i < 6; ++i )
  {
    snprintf(keys[i], sizeof(keys[i]), "clave%zu", i);
    Hinsert(h, keys[i], strlen(keys[i]) + 1, keys[i], strlen(keys[i]) + 1);
  }
  Hdelete(h, keys[0], strlen(keys[0]) + 1);

  const size_t OLD_CAPACITY = h->CAPACITY;
  CHECK_EQ_INT( Hresize(h), 0 );

  CHECK_EQ_SIZE( h->CAPACITY, OLD_CAPACITY*2 );
  CHECK_EQ_SIZE( h->SIZE, 5 );
  CHECK_EQ_SIZE( h->LOAD_FACTOR, 5*1000/h->CAPACITY );

  size_t tombstones = 0;
  for ( size_t i = 0; i < h->CAPACITY; ++i )
  {
    if ( TOMBSTONE == h->TABLE[i].state )
      ++tombstones;
  }
  CHECK_EQ_SIZE( tombstones, 0 );

  CHECK_NULL( Hfetch(h, keys[0], strlen(keys[0]) + 1) );
  for ( size_t i = 1; i < 6; ++i )
    CHECK_NOT_NULL( Hfetch(h, keys[i], strlen(keys[i]) + 1) );

  Hdestroy(h);
}

TEST(rehash_moves_the_entries_of_the_old_table)
{
  RHEntry OLD[2] =
  {
    { (void *)"droga", sizeof("droga"), (void *)"Cocaina", sizeof("Cocaina"), 0, OCCUPIED },
    { (void *)"borrada", 0, (void *)"Nada", sizeof("Nada"), 0, TOMBSTONE },
  };
  Hash *h = Hinit();

  CHECK_EQ_INT( Hrehash(h, OLD, 2), 0 );

  CHECK_EQ_SIZE( h->SIZE, 1 ); // Only the occupied slot is carried over
  CHECK_NOT_NULL( Hfetch(h, KEY("droga")) );
  CHECK_NULL( Hfetch(h, "borrada", sizeof("borrada")) );

  Hdestroy(h);
}

/* == Hfind_slot == */

TEST(find_slot_stays_inside_the_table)
{
  Hash *h = Hinit();

  const size_t idx = Hfind_slot(h, KEY("droga"));

  CHECK_TRUE( idx < h->CAPACITY );
  CHECK_EQ_SIZE( idx, hashfn(KEY("droga")) & (h->CAPACITY - 1) );

  Hdestroy(h);
}

TEST(find_slot_returns_the_slot_holding_the_key)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));
  const size_t idx = Hfind_slot(h, KEY("droga"));

  CHECK_TRUE( idx < h->CAPACITY );
  CHECK_TRUE( &(h->TABLE[idx]) == Hfetch(h, KEY("droga")) );

  Hdestroy(h);
}

TEST(find_slot_probes_past_a_colliding_key)
{
  Hash *h = Hinit();

  Hinsert(h, KEY("droga"), KEY("Cocaina"));
  const size_t occupied = Hfind_slot(h, KEY("droga"));

  /* Forge a collision by claiming the same bucket for another key. */
  h->TABLE[occupied].key = (void *)"otra";
  h->TABLE[occupied].ksize = sizeof("otra");

  const size_t idx = Hfind_slot(h, KEY("droga"));

  CHECK_TRUE( idx != occupied );
  CHECK_EQ_INT( h->TABLE[idx].state, EMPTY );

  Hdestroy(h);
}

TEST(find_slot_reports_a_full_table)
{
  static char keys[8][16];
  Hash *h = Hinit();

  /* Occupy every slot behind the load factor's back. */
  for ( size_t i = 0; i < h->CAPACITY; ++i )
  {
    snprintf(keys[i], sizeof(keys[i]), "clave%zu", i);
    h->TABLE[i] = (RHEntry){ keys[i], strlen(keys[i]) + 1, keys[i], strlen(keys[i]) + 1, 0, OCCUPIED };
    ++(h->SIZE);
  }

  CHECK_EQ_SIZE( Hfind_slot(h, KEY("ninguna")), SIZE_MAX );

  /* Insert must recover from it by growing instead of failing. */
  CHECK_EQ_INT( Hinsert(h, KEY("ninguna"), KEY("cabe")), 0 );
  CHECK_TRUE( h->CAPACITY > 8 );
  CHECK_NOT_NULL( Hfetch(h, KEY("ninguna")) );

  Hdestroy(h);
}

TEST(find_slot_returns_the_first_tombstone_of_the_chain)
{
  static char keys[4][16];
  Hash *h = Hinit();

  CollidingKeys(keys, 4, h->CAPACITY);

  for ( size_t i = 0; i < 3; ++i ) // keys[3] shares the chain but is never stored
    Hinsert(h, keys[i], strlen(keys[i]) + 1, keys[i], strlen(keys[i]) + 1);

  const size_t freed = Hfind_slot(h, keys[1], strlen(keys[1]) + 1);
  Hdelete(h, keys[1], strlen(keys[1]) + 1);

  /* A missing key lands on the reusable hole, not on the empty slot after it. */
  CHECK_EQ_SIZE( Hfind_slot(h, keys[3], strlen(keys[3]) + 1), freed );

  Hdestroy(h);
}

/* == Bound method macros == */

TEST(macros_call_the_bound_methods)
{
  Hash *h = Hinit();

  CHECK_EQ_INT( INSERT(h, "droga", sizeof("droga"), "Cocaina", sizeof("Cocaina")), 0 );

  const RHEntry *entry = FETCH(h, "droga", sizeof("droga"));
  CHECK_NOT_NULL( entry );
  if ( entry != NULL )
    CHECK_EQ_STR( (const char *)entry->value, "Cocaina" );

  DELETE(h, "droga", sizeof("droga"));
  CHECK_NULL( FETCH(h, "droga", sizeof("droga")) );

  DESTROY(h);
}

/* == Hdisplay == */

TEST(display_char_prints_the_header_and_every_pair)
{
  char out[512];
  Hash *h = Hinit();

  Hinsert(h, "droga", sizeof("droga") - 1, "Cocaina", sizeof("Cocaina") - 1);

  CaptureStdoutStart();
  Hdisplay(h, CHAR);
  CaptureStdoutStop(out, sizeof(out));

  CHECK_NOT_NULL( strstr(out, "Capacity: 8") );
  CHECK_NOT_NULL( strstr(out, "Size: 1") );
  CHECK_NOT_NULL( strstr(out, "droga - Cocaina") );

  Hdestroy(h);
}

TEST(display_char_skips_deleted_and_empty_slots)
{
  char out[512];
  Hash *h = Hinit();

  Hinsert(h, "droga", sizeof("droga") - 1, "Cocaina", sizeof("Cocaina") - 1);
  Hinsert(h, "caco", sizeof("caco") - 1, "Lauriano", sizeof("Lauriano") - 1);
  Hdelete(h, "droga", sizeof("droga") - 1);

  CaptureStdoutStart();
  const int err = Hdisplay_char(h);
  CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_INT( err, 0 );
  CHECK_NULL( strstr(out, "droga") );
  CHECK_NOT_NULL( strstr(out, "caco - Lauriano") );

  Hdestroy(h);
}

TEST(display_int_writes_the_raw_key_and_value)
{
  static const int key = 0x11223344;
  static const int value = 0x55667788;
  char out[512];
  Hash *h = Hinit();

  Hinsert(h, &key, sizeof(key), &value, sizeof(value));

  CaptureStdoutStart();
  const int err = Hdisplay_int(h);
  const size_t written = CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_INT( err, 0 );
  CHECK_EQ_SIZE( written, sizeof(int) + strlen(" - ") + sizeof(int) + 1 );
  CHECK_EQ_MEM( out, &key, sizeof(key) );
  CHECK_EQ_MEM( out + sizeof(int) + strlen(" - "), &value, sizeof(value) );

  Hdestroy(h);
}

TEST(display_int64_writes_the_raw_key_and_value)
{
  static const int64_t key = 0x1122334455667788;
  static const int64_t value = 0x8877665544332211;
  char out[512];
  Hash *h = Hinit();

  Hinsert(h, &key, sizeof(key), &value, sizeof(value));

  CaptureStdoutStart();
  const int err = Hdisplay_int64(h);
  const size_t written = CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_INT( err, 0 );
  CHECK_EQ_SIZE( written, sizeof(int64_t) + strlen(" - ") + sizeof(int64_t) + 1 );
  CHECK_EQ_MEM( out, &key, sizeof(key) );
  CHECK_EQ_MEM( out + sizeof(int64_t) + strlen(" - "), &value, sizeof(value) );

  Hdestroy(h);
}

TEST(display_of_an_empty_table_only_prints_the_header)
{
  char out[512];
  Hash *h = Hinit();

  CaptureStdoutStart();
  Hdisplay(h, INT);
  const size_t written = CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_SIZE( written, strlen("Capacity: 8\nSize: 0\n") );
  CHECK_EQ_STR( out, "Capacity: 8\nSize: 0\n" );

  Hdestroy(h);
}

TEST(display_char_reports_a_failed_write)
{
  Hash *h = Hinit();

  Hinsert(h, "droga", sizeof("droga") - 1, "Cocaina", sizeof("Cocaina") - 1);

  FailingStdoutStart();
  const int err = Hdisplay_char(h);
  FailingStdoutStop();

  CHECK_EQ_INT( err, -2 );

  Hdestroy(h);
}

TEST(display_int_reports_a_failed_write)
{
  static const int key = 1;
  static const int value = 2;
  Hash *h = Hinit();

  Hinsert(h, &key, sizeof(key), &value, sizeof(value));

  FailingStdoutStart();
  const int err = Hdisplay_int(h);
  FailingStdoutStop();

  CHECK_EQ_INT( err, -2 );

  Hdestroy(h);
}

TEST(display_int64_reports_a_failed_write)
{
  static const int64_t key = 1;
  static const int64_t value = 2;
  Hash *h = Hinit();

  Hinsert(h, &key, sizeof(key), &value, sizeof(value));

  FailingStdoutStart();
  const int err = Hdisplay_int64(h);
  FailingStdoutStop();

  CHECK_EQ_INT( err, -2 );

  Hdestroy(h);
}

TEST(display_char_reports_a_failed_value_write)
{
  Hash *h = Hinit();

  /* An empty key writes nothing, so the failure can only come from the value. */
  Hinsert(h, "", 0, "Cocaina", sizeof("Cocaina") - 1);

  FailingStdoutStart();
  const int err = Hdisplay_char(h);
  FailingStdoutStop();

  CHECK_EQ_INT( err, -2 );

  Hdestroy(h);
}

TEST(display_dispatches_on_the_requested_type)
{
  static const int64_t key = 0x1122334455667788;
  static const int64_t value = 0x8877665544332211;
  char out[512];
  Hash *h = Hinit();

  Hinsert(h, &key, sizeof(key), &value, sizeof(value));

  CaptureStdoutStart();
  Hdisplay(h, INT64);
  const size_t written = CaptureStdoutStop(out, sizeof(out));

  const size_t header = strlen("Capacity: 8\nSize: 1\n");
  CHECK_EQ_SIZE( written, header + sizeof(int64_t) + strlen(" - ") + sizeof(int64_t) + 1 );
  CHECK_EQ_MEM( out + header, &key, sizeof(key) );

  Hdestroy(h);
}

void
RegisterHashTests
  (void)
{
  printf("Hash\n");

  RUN_TEST(hashfn_is_deterministic);
  RUN_TEST(hashfn_empty_key_is_the_fnv_offset_basis);
  RUN_TEST(hashfn_differs_for_different_keys);
  RUN_TEST(hashfn_is_order_sensitive);
  RUN_TEST(hashfn_only_reads_size_bytes);
  RUN_TEST(hashfn_reads_key_bytes_as_unsigned);

  RUN_TEST(compare_key_entry_matches_equal_keys);
  RUN_TEST(compare_key_entry_rejects_different_content);
  RUN_TEST(compare_key_entry_rejects_different_size);
  RUN_TEST(compare_key_entry_ignores_deleted_entries);

  RUN_TEST(init_returns_an_empty_table);
  RUN_TEST(init_binds_the_methods);

  RUN_TEST(insert_then_fetch_returns_the_value);
  RUN_TEST(insert_accounts_the_load_factor_per_thousand);
  RUN_TEST(insert_of_an_existing_key_replaces_the_value);
  RUN_TEST(fetch_of_a_missing_key_returns_null);
  RUN_TEST(fetch_on_an_empty_table_returns_null);
  RUN_TEST(fetch_distinguishes_keys_sharing_a_prefix);
  RUN_TEST(insert_keeps_every_key_reachable);
  RUN_TEST(insert_handles_binary_keys_containing_zero_bytes);

  RUN_TEST(delete_makes_the_key_unreachable);
  RUN_TEST(delete_tombstones_the_slot_without_freeing_the_value);
  RUN_TEST(delete_of_a_missing_key_changes_nothing);
  RUN_TEST(delete_keeps_the_other_keys_reachable);
  RUN_TEST(insert_reuses_a_tombstoned_slot);
  RUN_TEST(delete_decrements_the_size_and_the_load_factor);
  RUN_TEST(fetch_finds_a_key_stored_after_a_tombstone);
  RUN_TEST(insert_does_not_duplicate_a_key_stored_after_a_tombstone);

  RUN_TEST(insert_grows_the_table_past_its_capacity);
  RUN_TEST(resize_rehashes_every_entry_and_drops_the_tombstones);
  RUN_TEST(rehash_moves_the_entries_of_the_old_table);

  RUN_TEST(find_slot_stays_inside_the_table);
  RUN_TEST(find_slot_returns_the_slot_holding_the_key);
  RUN_TEST(find_slot_probes_past_a_colliding_key);
  RUN_TEST(find_slot_reports_a_full_table);
  RUN_TEST(find_slot_returns_the_first_tombstone_of_the_chain);

  RUN_TEST(macros_call_the_bound_methods);

  RUN_TEST(display_char_prints_the_header_and_every_pair);
  RUN_TEST(display_char_skips_deleted_and_empty_slots);
  RUN_TEST(display_int_writes_the_raw_key_and_value);
  RUN_TEST(display_int64_writes_the_raw_key_and_value);
  RUN_TEST(display_of_an_empty_table_only_prints_the_header);
  RUN_TEST(display_char_reports_a_failed_write);
  RUN_TEST(display_int_reports_a_failed_write);
  RUN_TEST(display_int64_reports_a_failed_write);
  RUN_TEST(display_char_reports_a_failed_value_write);
  RUN_TEST(display_dispatches_on_the_requested_type);

  return;
}
