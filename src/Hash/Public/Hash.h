#ifndef _HASH_
#define _HASH_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
  Error codes returned by the Hash methods
*/

#define H_SUCESS      0
#define H_INVALID_ARG   1
#define H_TABLE_FULL    2
#define H_ALLOC_FAILED    3
#define H_WRITE_FAILED    4

/* 
  Structs
*/

typedef enum 
{ 
  EMPTY, 
  OCCUPIED, 
  TOMBSTONE 
} EntryState;

typedef enum
{
  CHAR,
  INT,
  INT64,
} DisplayType;


typedef struct {
  void    *key;
  size_t    ksize;
  void    *value;
  size_t    vsize;
  int       PSL; // Probe Sequence Length
  EntryState  state;
} RHEntry;

typedef struct Hash Hash;
struct Hash
{
  size_t    CAPACITY;
  size_t    SIZE;
  size_t    LOAD_FACTOR;
  RHEntry   *TABLE;

  /* 
    You may ask, why not Global Functions? And the answer is...
    I'm soooo lazy and error-prone so having these functions pointers allow me to
    call any constructor to assign them to specific types, and these defines are
    easier to use.
  */

  int
  (*insert)
    (Hash *this, 
    const void *key,
    const size_t ksize,
    const void *value,
    const size_t vsize);
  
  void
  (*delete)
    (Hash *this,
    const void *key,
    const size_t size);
  
  const RHEntry
  *(*fetch)
    (Hash *this,
    const void *key,
    const size_t size);

  int
  (*display)
    (Hash *this,
    DisplayType type);
  
  int
  (*resize)
    (Hash *this);
  
  void
  (*destroy)
    (Hash *this);
  
};

#define INSERT(this, key, ksize, value, vsize) ((this)->insert((this), (key), (ksize), (value), (vsize)))
#define DELETE(this, key, size)        ((this)->delete((this), (key), (size)))
#define FETCH(this, key, size)         ((this)->fetch((this), (key), (size)))
#define DISPLAY(this, type)          ((this)->display((this), (type))) // Debugging purposes
#define RESIZE(this)               ((this)->resize((this)))
#define DESTROY(this)              ((this)->destroy((this)))

/*
  Binded methods
*/

int
Hinsert
  (Hash *this, 
  const void *key,
  const size_t ksize,
  const void *value,
  const size_t vsize);


void
Hdelete
  (Hash *this,
  const void *key,
  const size_t size);


const RHEntry
*Hfetch
  (Hash *this,
  const void *key,
  const size_t size);


int
Hdisplay
  (Hash *this,
  DisplayType type);


int 
Hresize
  (Hash *this);


void
Hdestroy
  (Hash *this);


Hash
*Hinit
  (void);

#endif

