#include "../include/Hash.h"

// STD
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h> // Satanic things
#include <math.h>
#include <assert.h>

// Methods

/*
    Hash Function
*/

const size_t
hashfn
    (const void *key,
    const size_t size)
{
    size_t hash = 0xcbf29ce484222325; // FNV-1a 64-bit offset basis
    for( size_t i = 0; i < size; ++i )
    {
        hash ^= (size_t) *((const char *)key + i);
        hash *= 0x100000001b3; // FNV-1a prime
    }
    return hash;
}


/*
    Binded methods
*/

bool 
Hcompare_key_entry
    (const void *key1,
    const size_t size,
    const RHEntry entry)
{
    if ( size != entry.ksize )
        return 0;
    const char *ckey1 = (const char *)key1;
    const char *ckey2 = (const char *)entry.key;

    for ( size_t i = 0; i < size; ++i )
    {
        if ( ckey1[i] != ckey2[i] )
            return 0;
    }
    return 1;
}


const size_t 
Hfind_slot
    (Hash *this, 
    const void *key,
    const  size_t size)
{
    jmp_buf err;
    if ( setjmp(err) )
    {
        Hresize(this);
    }   

    assert((this->CAPACITY & (this->CAPACITY - 1)) == 0 && "CAPACITY must be power of two, what did you do");
    const size_t o_idx = hashfn(key, size) & (this->CAPACITY-1); // Mod operation is really slow, and if I don't micro optimise I get stressed
    size_t idx = o_idx;
    size_t toret = SIZE_MAX;

    for ( size_t i = 0; i < this->CAPACITY; ++i )
    {
        idx %= this->CAPACITY;
        const EntryState state  = this->TABLE[idx].state;
        const RHEntry entry    = this->TABLE[idx];

        if ( TOMBSTONE == state || EMPTY == state )
        {
            toret = idx;
            break;
        }
        else if ( Hcompare_key_entry( key, size, entry ) )
        {
            toret = idx;
            break;
        }
        ++idx;
    }
    if (SIZE_MAX == toret)
        longjmp(err, 1);
    
    return toret;
}


const int
Hinsert
    (Hash *this,
    const void *key,
    const size_t ksize,
    const void *value,
    const size_t vsize)
{
    const size_t idx = Hfind_slot(this, key, ksize);
    if ( -1 == idx )
        return 1;

    if ( OCCUPIED != this->TABLE[idx].state )
        this->LOAD_FACTOR =  ++(this->SIZE)*1000/(this->CAPACITY);
    this->TABLE[idx] = (RHEntry){ (void *)key, ksize, (void *)value, vsize, (hashfn(key, ksize) & (this->CAPACITY-1)) - idx, OCCUPIED };
    

    if ( 700 < this->LOAD_FACTOR && Hresize(this) ); // && Will only execute if first condition is met, this is to avoid warnings from the compiler
    if ( 980 < this->LOAD_FACTOR )
        return Hresize(this);
    
    return 0;
}


void
Hdelete
    (Hash *this, 
    const void *key,
    const size_t size)
{
    /*
        When deleting a bucket or slot (making it available) only RHEntry.state and RHEntry.ksize
        state = TOMBSTONE; so it doesn't make inaccessible further keys
        ksize = 0; so in each comparison made with this key it is automatically ignored

        With this said, it is possible to conserve past values and access them manually.
    */
    const size_t idx = Hfind_slot(this, key, size);
    if ( -1 != idx && Hcompare_key_entry( key, size, this->TABLE[idx] ) )
    {
        this->TABLE[idx].state = TOMBSTONE;
        this->TABLE[idx].ksize = 0;
    }

    return;
}


const RHEntry
*Hfetch
    (Hash *this, 
    const void *key,
    const size_t size)
{
    const size_t idx = Hfind_slot(this, key, size);
    if ( -1 == idx || !Hcompare_key_entry( key, size, this->TABLE[idx] ) )
        return NULL;
    
    const RHEntry *toret = &(this->TABLE[idx]);

    return toret;
}

// Hdisplay
const int
Hdisplay_char
    (Hash *this);
const int
Hdisplay_int
    (Hash *this);
const int
Hdisplay_int64
    (Hash *this);

void
Hdisplay // Debug purposes
    (Hash *this, 
    DisplayType type)
{
    printf("Capacity: %zu\nSize: %zu\n", this->CAPACITY, this->SIZE);

    switch ( type )
    {
        case CHAR:
            Hdisplay_char(this);
            break;

        case INT:
            Hdisplay_int(this);
            break;

        case INT64:
            Hdisplay_int64(this);
            break;
    }

    return;
}

const int
Hdisplay_char
    (Hash *this)
{
    jmp_buf err;

    if ( setjmp(err) )
        return -2;

    for ( size_t i = 0; i < this->CAPACITY; ++i )
    {
        if ( OCCUPIED != this->TABLE[i].state)
            continue;
        const char *key     = this->TABLE[i].key;
        const size_t ksize  = this->TABLE[i].ksize;
        const char *value   = this->TABLE[i].value;
        const size_t vsize  = this->TABLE[i].vsize;
        if (fwrite(key, sizeof(char), ksize, stdout) != ksize)
            longjmp(err, 1);
        printf(" - ");
        if (fwrite(value, sizeof(char), vsize, stdout) != vsize)
            longjmp(err, 1); // In case of error writing
        puts(""); 
    }

    return 0;
}

const int
Hdisplay_int
    (Hash *this)
{
    jmp_buf err;

    if ( setjmp(err) )
        return -2;

    for ( size_t i = 0; i < this->CAPACITY; ++i )
    {
        if ( OCCUPIED != this->TABLE[i].state)
            continue;
        const int *key     = this->TABLE[i].key;
        const int *value   = this->TABLE[i].value;
        if (fwrite(key, sizeof(int), 1, stdout) != 1)
            longjmp(err, 1);
        printf(" - ");
        if (fwrite(value, sizeof(int), 1, stdout) != 1)
            longjmp(err, 1); // In case of error writing
        puts("");
    }
    
    return 0;
}


const int
Hdisplay_int64
    (Hash *this)
{
    jmp_buf err;

    if ( setjmp(err) )
        return -2;

    for ( size_t i = 0; i < this->CAPACITY; ++i )
    {
        if ( OCCUPIED != this->TABLE[i].state)
            continue;
        const int64_t *key     = this->TABLE[i].key;
        const int64_t *value   = this->TABLE[i].value;
        if (fwrite(key, sizeof(int64_t), 1, stdout) != 1)
            longjmp(err, 1);
        printf(" - ");
        if (fwrite(value, sizeof(int64_t), 1, stdout) != 1)
            longjmp(err, 1); // In case of error writing
        puts("");
    }
    
    return 0;
}

const int 
Hrehash
    (Hash *this,
    RHEntry *TABLE)
{
    int nsuccess = 0;
    for ( size_t i = 0; i < (this->CAPACITY); ++i )
    {
        if ( OCCUPIED != TABLE[i].state)
            continue;
        nsuccess = INSERT(this, TABLE[i].key, TABLE[i].ksize, TABLE[i].value, TABLE[i].vsize);
        if ( nsuccess )
            break;
    }
    return nsuccess;
}

const int 
Hresize
    (Hash *this)
{   
    size_t OLD_CAPACITY = this->CAPACITY;
    size_t NEW_CAPACITY = this->CAPACITY*2;
    
    void *tmp = calloc(NEW_CAPACITY, sizeof(RHEntry));;
    if ( tmp == NULL )
        return 1;

    RHEntry *TABLE = this->TABLE;
    this->TABLE = (RHEntry *)tmp;
    this->SIZE = 0;
    this->CAPACITY = NEW_CAPACITY;

    const int err_Hrehash = Hrehash(this, TABLE);
    if ( err_Hrehash )
    {
        this->TABLE = TABLE;
        this->CAPACITY = OLD_CAPACITY;
        return err_Hrehash;
    }

    this->LOAD_FACTOR /= 2;
    free(TABLE); // No memory leak
    
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

    size_t CAPACITY = 8;
    size_t SIZE = 0;

    ret->CAPACITY = CAPACITY;
    ret->SIZE = SIZE;
    ret->LOAD_FACTOR = SIZE*1000/CAPACITY;

    ret->TABLE = (RHEntry *)calloc( CAPACITY, sizeof(RHEntry) );

    ret->insert = Hinsert;
    ret->delete = Hdelete;
    ret->fetch = Hfetch;
    ret->display = Hdisplay;
    ret->resize = Hresize;
    ret->destroy = Hdestroy;

    return ret;
}

