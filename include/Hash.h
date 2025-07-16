#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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
    void        *key;
    size_t      ksize;
    void        *value;
    size_t      vsize;
    int         PSL;      // Probe Sequence Length
    EntryState  state;
} RHEntry;

typedef struct Hash Hash;

struct Hash 
{
	size_t          CAPACITY;
    size_t          SIZE;
    size_t          LOAD_FACTOR;
    RHEntry         *TABLE;

    /* 
        You may ask, why not Global Functions? And the answer is...
        I'm soooo lazy and error-prone so having these functions pointers allow me to
        call any constructor to assign them to specific types and these defines are
        easier to use.
    */

    const int
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

    void
    (*display)
        (Hash *this,
        DisplayType type);
    
    const int
    (*resize)
        (Hash *this);
    
    void
    (*destroy)
        (Hash *this);
    
};

#define INSERT(this, key, ksize, value, vsize) ((this)->insert((this), (key), (ksize), (value), (vsize)))
#define DELETE(this, key, size)                ((this)->delete((this), (key), (size)))
#define FETCH(this, key, size)                 ((this)->fetch((this), (key), (size)))
#define DISPLAY(this, type)                    ((this)->display((this), (type))) // Debugging purposes
#define RESIZE(this)                           ((this)->resize((this)))
#define DESTROY(this)                          ((this)->destroy((this)))

// Methods

/*
    Hash Function
*/

const size_t
hashfn
    (const void *key,
    const size_t size);


/*
    Binded methods
*/

const size_t 
Hfind_slot
    (Hash *this, 
    const void *key,
    const size_t size);


const int
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


void
Hdisplay
    (Hash *this,
    DisplayType type);


const int 
Hresize
    (Hash *this);


void
Hdestroy
    (Hash *this);


Hash
*Hinit
    (void);


