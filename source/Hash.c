#include "../include/Hash.h"

// STD
#include <stdlib.h>


// Methods

/*
    Hash Function
*/

size_t
hashfn
    (const void *key,
    const size_t size)
{
    
}


/*
    Binded methods
*/

bool 
Hcompare_keys
    (const void *key1, 
    const void *key2, 
    const size_t size)
{
    const char *ckey1 = (const char *)key1;
    const char *ckey2 = (const char *)key2;

    for ( size_t i = 0; i < size; ++i )
    {
        if ( ckey1[i] != ckey2[i] )
            return 0;
    }
    return 1;
}


size_t 
Hfind_slot
    (Hash *this, 
    const void *key,
    const  size_t size)
{
    size_t idx = hashfn(key, size) % this->CAPACITY;
    
    size_t toret = -1;

    for ( size_t i = idx; i < this->CAPACITY; ++i )
    {
        const EntryState state  = this->TABLE[i].state;
        const void *tkey        = this->TABLE[i].key; // This' key
        const size_t tsize      = this->TABLE[i].ksize;

        if ( TOMBSTONE == state || EMPTY == state )
            toret = i;
        else if ( tsize == size && Hcompare_keys( key, tkey, size ) )
        {
            toret = i;
            break;
        }
        else ( EMPTY == state )
        {
            toret = i;
            break;
        }
    }

    return toret;
}


void
Hinsert
    (Hash *this, 
    const void *key,
    const size_t size,
    const void *value)
{
    
}


void
Hdelete
    (Hash *this, 
    const void *key,
    const size_t size)
{
    
}


const RHEntry
*Hfetch
    (Hash *this, 
    const void *key,
    const size_t size)
{
    size_t idx = Hfind_slot(this, key, size);
    if ( -1 == idx || !Hcompare_keys(this->TABLE[idx].key, key, size) )
        return NULL;
    
    const RHEntry *toret = &(this->TABLE[idx].value);

    return toret;
}


void
Hdisplay
    (Hash *this)
{
    
}


int 
Hresize
    (Hash *this)
{
    void *tmp = realloc(this->TABLE, this->CAPACITY*2);
    if ( tmp == NULL )
        return 1;
    
    this->TABLE = tmp;
    this->CAPACITY *= 2;
    return 0;
}


void
Hdestroy
    (Hash *this)
{
    free(this->TABLE);
    free(this);
}


Hash
*Hinit
    (void)
{
    Hash *ret = (Hash *)malloc( sizeof(Hash) );

    size_t CAPACITY = 1;
    size_t SIZE = 0;

    ret->CAPACITY = CAPACITY;
    ret->SIZE = SIZE;
    ret->LOAD_FACTOR = SIZE*100/CAPACITY;

    ret->TABLE = (RHEntry *)malloc( CAPACITY * sizeof(RHEntry) );

    ret->insert = Hinsert;
    ret->delete = Hdelete;
    ret->fetch = Hfetch;
    ret->display = Hdisplay;
    ret->resize = Hresize;
    ret->destroy = Hdestroy;
}

