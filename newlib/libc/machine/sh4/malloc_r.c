#include <reent.h>
#include <stdint.h>
#include <string.h>

extern void *malloc(size_t size);
extern void free(void *ptr);

void *
_malloc_r (struct _reent *reent, 
    size_t size) 
{
    return malloc(size);
}

void 
_free_r (struct _reent *reent, 
    void *ptr) 
{
    free(ptr);
}

void *
_calloc_r (struct _reent *reent, 
    size_t num,
    size_t size) 
{
    void *block = _malloc_r(reent, num * size);
    return memset(block, 0, num * size);
}

void *
calloc (size_t num,
    size_t size) 
{
    return _calloc_r(_REENT, num, size);
}

void *
_realloc_r (struct _reent *reent, 
    void *ptr, 
    size_t new_size) 
{
    void *block = NULL;
    
    if (new_size > 0) 
    {
        block = _malloc_r(reent, new_size);

        if (!block) 
        {
            return NULL;
        }
    }

    if (!ptr)
    {
        return block;
    }

    /* FIXME: This is wrong and will break, however for now it's good enough. */
    memcpy(block, ptr, new_size);
    free(ptr);
    
    return block;
}

void *
realloc (void *ptr, 
    size_t size) 
{
    return _realloc_r(_REENT, ptr, size);
}
