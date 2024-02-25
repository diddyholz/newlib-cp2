#include <stdint.h>
#include <reent.h>

extern void *malloc(size_t size);
extern void free(void *ptr);

inline void *_malloc_r(struct _reent *reent, size_t size) {
    return malloc(size);
}

inline void _free_r(struct _reent *reent, void *ptr) {
    free(ptr);
}
