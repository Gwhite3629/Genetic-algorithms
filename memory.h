#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#define UNALLOCATED 0
#define RESERVED 1
#define CULLED 2
#define AVAILABLE 3

extern const char *stat_names[12];

typedef struct smart_ptr {
    void *addr;
    void *base;
    int size;
    int flag;
} smart_ptr;

typedef struct region_t {
    void *base_addr;
    int alloc_size;
    int alignment;
    int used_size;
    smart_ptr *chunks;
    int n_chunks;
} region_t;

typedef struct heap_t {
    region_t *regions;
    int n_regions;
    char name[20];
} heap_t;

extern heap_t *global_heap;
extern FILE *log_file;

#define ALIGN 4096

#define init \
    global_heap = create(ALIGN, 1*ALIGN, "global"); \
    VALID(global_heap, MEM_CODE, ALLOCATION_ERROR);

#define new(ptr, size, type) \
    (type *)alloc(global_heap, size*sizeof(type)); \
    VALID(ptr, MEM_CODE, ALLOCATION_ERROR);

#define shorten(ptr, size, type) \
    (type *)shrink(global_heap, ptr, size*sizeof(type)); \
    VALID(ptr, MEM_CODE, ALLOCATION_ERROR);

#define widen(ptr, size, type) \
    (type *)expand(global_heap, ptr, size*sizeof(type)); \
    VALID(ptr, MEM_CODE, ALLOCATION_ERROR);

#define del(ptr) \
    global_heap = cull(global_heap, ptr); \
    VALID(global_heap, MEM_CODE, ALLOCATION_ERROR);

#define cleanup() \
    global_heap = clean(global_heap); \
    VALID(global_heap, MEM_CODE, ALLOCATION_ERROR);

#define end \
    destroy(global_heap);

#define log(h) \
    fprintf(log_file, "Heap: %s\n", h->name); \
    for (int iter = 0; iter < h->n_regions; iter++) { \
        fprintf(log_file, "%p : %d : %d : %d\n",\
        h->regions[iter].base_addr,\
        h->regions[iter].alloc_size,\
        h->regions[iter].used_size, h->regions[iter].n_chunks);\
        for (int it = 0; it < h->regions[iter].n_chunks; it++) {\
            fprintf(log_file, "\t%12s @ %p/%p : %d\n",\
            stat_names[h->regions[iter].chunks[it].flag],\
            h->regions[iter].chunks[it].base,\
            h->regions[iter].chunks[it].addr,\
            h->regions[iter].chunks[it].size);\
        } \
    } \
    fprintf(log_file, "\n\n");

 void r_print(region_t *r);

 region_t create_region(int alignment, int size);

 void *alloc_region(region_t *r, int n);

 region_t *expand_region(region_t *r, int ptr_idx, int old_size, int addition);

 region_t *cull_region(region_t *r, void *ptr, int ptr_idx, int n);

 region_t *clean_region(region_t *r);

 void destroy_region(region_t *r);

 void h_print(heap_t *h);

 heap_t *create(int alignment, int size, const char *name);

 heap_t *grow(heap_t *h, int n);

 void *alloc(heap_t *h, int n);

 void *expand(heap_t *h, void *ptr, int size);

 void *shrink(heap_t *h, void *ptr, int size);

 heap_t *cull(heap_t *h, void *ptr);

 heap_t *clean(heap_t *h);

 void destroy(heap_t *h);

#endif // _MEMORY_H_