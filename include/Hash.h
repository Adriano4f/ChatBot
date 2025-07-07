#include <stdint.h>
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

typedef struct {
    void        *key;
    size_t      ksize;
    void        *value;
    size_t      vsize;
    int         PSL;      // Probe Sequence Length
    EntryState  state;
} RHEntry;

typedef struct Hash
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

    #define INSERT(this, key, value)    ( (this)->insert( (this), (key), (value) ) )
    void
    (*insert)
        (Hash *this, 
        const void *key,
        const size_t size,
        const void *value);
    
    #define DELETE(this, key)           ( (this)->delete( (this), (key) ) )
    void
    (*delete)
        (Hash *this,
        const void *key,
        const size_t size);
    
    #define this(key)                   ( (this)->fetch( (this), (key) ) )
    const void
    *(*fetch)
        (Hash *this,
        const void *key,
        const size_t size);

    #define DISPLAY(this)               ( (this)->display( (this) ) ) // Debugging purposes
    void
    (*display)
        (Hash *this);
    
    #define RESIZE(this)                ( (this)->resize( (this) ) )
    void 
    (*resize)
        (Hash *this);
    
    #define DESTROY(this)               ( (this)->destroy( (this )) )
    void
    (*destroy)
        (Hash *this);
    
}	Hash;

// Methods

/*
    Hash Function
*/

size_t
hashfn
    (const void *key,
    const size_t size);


/*
    Binded methods
*/

size_t 
Hfind_slot
    (Hash *this, 
    const void *key,
    const size_t size);


void
Hinsert
    (Hash *this, 
    const void *key,
    const size_t size,
    const void *value);


void
Hdelete
    (Hash *this, 
    const void *key,
    const size_t size);


const void
*Hfetch
    (Hash *this, 
    const void *key,
    const size_t size);


void
Hdisplay
    (Hash *this);


int 
Hresize
    (Hash *this);


void
Hdestroy
    (Hash *this);


Hash
*Hinit
    (void);


