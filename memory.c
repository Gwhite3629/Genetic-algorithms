#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define WINDOWS 0
#define LOGGING 1

#include "utils.h"
#include "memory.h"

const char *stat_names[12] = {
    "UNALLOCATED",
    "RESERVED",
    "CULLED",
    "AVAILABLE"
};

heap_t *global_heap = NULL;
FILE *log_file = NULL;

inline void r_print(region_t *r)
{
    printf("%p : %d : %d : %d\n", r->base_addr, r->alloc_size, r->used_size, r->n_chunks);
    for (int i = 0; i < r->n_chunks; i++) {
        printf("\t%12s @ %p/%p : %d\n", 
        stat_names[r->chunks[i].flag], 
        r->chunks[i].base,
        r->chunks[i].addr, 
        r->chunks[i].size);
    }
}

inline region_t create_region(int alignment, int size)
{
    region_t r;

    r.chunks = (smart_ptr *)malloc(1*sizeof(smart_ptr));
    if (r.chunks == NULL) {
        fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
        fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
        goto exit;
    }
    memset(r.chunks, 0, 1*sizeof(smart_ptr));

    #if WINDOWS
        r.base_addr = (void *)_aligned_malloc(size, alignment);
        if (r.base_addr == NULL) {
            fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
            fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
            goto exit;
        }
        memset(r.base_addr, 0, size);
    #else
        r.base_addr = (void *)aligned_alloc(alignment, size);
        if (r.base_addr == NULL) {
            fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
            fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
            goto exit;
        }
        memset(r.base_addr, 0, size);
    #endif

    r.chunks[0].addr = r.base_addr;
    r.chunks[0].size = size;
    r.chunks[0].flag = UNALLOCATED;
    r.chunks[0].base = r.base_addr;
    r.n_chunks = 1;

    r.alignment = alignment;
    r.alloc_size = size;
    r.used_size = 0;

exit:
    return r;
}

inline void *alloc_region(region_t *r, int n)
{
    void *ptr = NULL;

    for (int i = 0; i < r->n_chunks; i++) {
        if (!((r->chunks[i].flag & 0b01) ^ \
              (r->chunks[i].flag & 0b10)) & \
              (r->chunks[i].size >= n)) {
            if (r->chunks[i].size == n) {
                r->chunks[i].flag = RESERVED;
                ptr = r->chunks[i].addr;
            } else {
                r->chunks = (smart_ptr *)realloc(r->chunks, (r->n_chunks+1)*sizeof(smart_ptr));
                if (r->chunks == NULL) {
                    fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
                    fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
                    goto exit;
                }
                for (int j = r->n_chunks; j > i; j--) {
                    r->chunks[j] = r->chunks[j-1];
                }
                r->chunks[i+1].size = r->chunks[i].size - n;
                r->chunks[i+1].flag = UNALLOCATED;
                r->chunks[i+1].addr = r->chunks[i].addr + n;
                r->chunks[i].size = n;
                r->chunks[i].flag = RESERVED;
                ptr = r->chunks[i].addr;
                r->n_chunks++;
                break;
            }
        }
    }

    r->used_size += n;

    return ptr;

exit:
    return NULL;
}

inline region_t *expand_region(region_t *r, int ptr_idx, int old_size, int addition)
{

    if (ptr_idx != (r->n_chunks - 1)) {
        if (!((r->chunks[ptr_idx+1].flag & 0b01) ^ \
              (r->chunks[ptr_idx+1].flag & 0b10)) & \
              (r->chunks[ptr_idx+1].size >= (addition))) {
            if (r->chunks[ptr_idx+1].size == addition) {
                for (int i = ptr_idx + 1; i < r->n_chunks-1; i++) {
                    r->chunks[i] = r->chunks[i+1];
                }
                r->chunks = (smart_ptr *)realloc(r->chunks, (r->n_chunks-1)*sizeof(smart_ptr));
                if (r->chunks == NULL) {
                    fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
                    fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
                    goto exit;
                }

                r->chunks[ptr_idx].size += addition;

                r->used_size += addition;

                r->n_chunks--;
            } else {
                r->chunks[ptr_idx+1].addr += addition;
                r->chunks[ptr_idx+1].size -= addition;
                r->chunks[ptr_idx].size += addition;
                r->used_size += addition;
            }
        }
        return r;
    }

    for (int i = 0; i < r->n_chunks; i++) {
        if (!((r->chunks[i].flag & 0b01) ^ \
              (r->chunks[i].flag & 0b10)) & \
              (r->chunks[i].size >= (old_size + addition))) {
            r->chunks = (smart_ptr *)realloc(r->chunks, (r->n_chunks+1)*sizeof(smart_ptr));
            if (r->chunks == NULL) {
                fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
                fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
                goto exit;
            }
            for (int j = r->n_chunks; j > i; j--) {
                r->chunks[j-1] = r->chunks[j];
            }
            r->chunks[i].flag = UNALLOCATED;
            r->chunks[i].size -= (old_size + addition);
            r->chunks[i].addr += (old_size + addition);
            r->chunks[ptr_idx].flag = RESERVED;
            r->chunks[ptr_idx].size = old_size + addition;
            r->n_chunks++;
            break;
        }
    }

    r->used_size += addition;

    return r;

exit:
    return NULL;
}

inline region_t *cull_region(region_t *r, void *ptr, int ptr_idx, int n)
{
    if (ptr == NULL) {
        goto exit;
    }

    r->chunks[ptr_idx].flag = CULLED;

    r->used_size -= n;

    return r;

exit:
    return NULL;
}

inline region_t *clean_region(region_t *r)
{
    int done = 1;
    while (done == 1) {
        done = 0;
        for (int i = 0; i < r->n_chunks-1; i++) {
            if (r->chunks[i].flag == CULLED) {
                memset(r->chunks[i].addr, 0, r->chunks[i].size);
                r->chunks[i].flag = AVAILABLE;
            }
            if (r->chunks[i].flag != RESERVED) {
                if (r->chunks[i + 1].flag != RESERVED) {
                    r->chunks[i].size += r->chunks[i+1].size;
                    for (int j = i + 1; j < r->n_chunks-1; j++) {
                        r->chunks[j] = r->chunks[j+1];
                    }
                    r->chunks = (smart_ptr *)realloc(r->chunks, (r->n_chunks-1)*sizeof(smart_ptr));
                    if (r->chunks == NULL) {
                        fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
                        fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
                        goto exit;
                    }
                    r->n_chunks--;
                    done = 1;
                }
            }
        }
    }

    return r;

exit:
    return NULL;
}

inline void destroy_region(region_t *r)
{
    if (r->chunks)
        free(r->chunks);
    r->chunks = NULL;
    #if WINDOWS
        if (r->base_addr)
            _aligned_free(r->base_addr);
        r->base_addr = NULL;
    #else
        if (r->base_addr)
            free(r->base_addr);
        r->base_addr = NULL;
    #endif
}

inline void h_print(heap_t *h)
{
    printf("Heap: %s\n", h->name);
    for (int i = 0; i < h->n_regions; i++) {
        r_print(h->regions + i);
    }
    printf("\n");
}

inline heap_t *create(int alignment, int size, const char *name)
{
    heap_t *h = NULL;
    h = (heap_t *)malloc(1*sizeof(heap_t));
    if (h == NULL) {
        fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
        fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
        goto exit;
    }
    memset(h, 0, 1*sizeof(heap_t));

    h->regions = (region_t *)malloc(1*sizeof(region_t));
    if (h->regions == NULL) {
        fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
        fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
        goto exit;
    }
    memset(h->regions, 0, 1*sizeof(region_t));

    strcpy(h->name, name);

    h->regions[0] = create_region(alignment, size);

    h->n_regions = 1;

    #if LOGGING
        log_file = fopen("mem_log.txt", "w+");
        if (log_file == NULL) {
        fprintf(stderr, "%s: %s\n", FILE_CODE, strerror(errno));
        fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
        goto exit;
    }
    #endif

    #if LOGGING
        log(h);
    #endif

    return h;

exit:
    return NULL;
}

inline heap_t *grow(heap_t *h, int n)
{
    h->regions = (region_t *)realloc(h->regions, (h->n_regions+1)*sizeof(region_t));
    if (h->regions == NULL) {
        fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
        fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
        goto exit;
    }
    memset(h->regions + h->n_regions, 0, 1*sizeof(region_t));

    h->regions[h->n_regions] = create_region(h->regions[0].alignment, n*h->regions[0].alignment);

    h->n_regions++;

    #if LOGGING
        log(h);
    #endif

    return h;

exit:
    return NULL;
}

inline void *alloc(heap_t *h, int n)
{
    int found = 0;
    void *ptr = NULL;

    for (int i = 0; i < h->n_regions; i++) {
        if (n < (h->regions[i].alloc_size - h->regions[i].used_size)) {
            for (int j = 0; j < h->regions[i].n_chunks; j++) {
                if (!((h->regions[i].chunks[j].flag & 0b01) ^ \
                      (h->regions[i].chunks[j].flag & 0b10)) & \
                      (h->regions[i].chunks[j].size >= (n))) {
                    ptr = alloc_region(h->regions + i, n);
                    found = 1;
                    break;
                }
            }
        }
        if (found == 1)
            break;
    }

    if (found == 0) {
        h = grow(h, (n / h->regions[0].alignment) + 1);
        ptr = alloc_region(h->regions + (h->n_regions-1), n);
    }

    #if LOGGING
        log(h);
    #endif

    return ptr;

}

inline void *expand(heap_t *h, void *ptr, int size)
{
    int ptr_idx = -1;
    int ptr_reg = -1;
    int new_idx = -1;
    int found = 0;
    int i, j, k = 0;

    for (i = 0; i < h->n_regions; i++) {
        for (j = 0; j < h->regions[i].n_chunks; j++) {
            if (h->regions[i].chunks[j].addr == ptr) {
                ptr_idx = j;
                break;
            }
        }
        if (ptr_idx >= 0)
            break;
    }

    ptr_reg = i;

    if (!((h->regions[i].chunks[ptr_idx+1].flag & 0b01) ^ \
          (h->regions[i].chunks[ptr_idx+1].flag & 0b10)) & \
          (h->regions[i].chunks[ptr_idx+1].size >= (size - h->regions[i].chunks[ptr_idx].size))) {
            found = 1;
    } else if (!((h->regions[i].chunks[h->regions[i].n_chunks-1].flag & 0b01) ^ \
                 (h->regions[i].chunks[h->regions[i].n_chunks-1].flag & 0b10)) & \
                 (h->regions[i].chunks[h->regions[i].n_chunks-1].size >= (size))) {
        found = 1;
    }

    if (found == 1) {
        expand_region(h->regions + i, ptr_idx, h->regions[i].chunks[ptr_idx].size, size - h->regions[i].chunks[ptr_idx].size);
        return ptr;
    } else {
        for (i = 0; i < h->n_regions; i++) {
            for (j = 0; j < h->regions[i].n_chunks; j++) {
                if (!((h->regions[i].chunks[j].flag & 0b01) ^ \
                      (h->regions[i].chunks[j].flag & 0b10)) & \
                      (h->regions[i].chunks[j].size >= (size))) {
                            found = 1;
                            break;
                }
            }
            if (found == 1)
                break;
        }
    }

    if (found == 1) {
        ptr = alloc(h, size);
        memcpy(ptr, h->regions[ptr_reg].chunks[ptr_idx].addr, h->regions[ptr_reg].chunks[ptr_idx].size);
        for (k = 0; k < h->n_regions; k++) {
            for (j = 0; j < h->regions[k].n_chunks; j++) {
                if (h->regions[k].chunks[j].addr == ptr) {
                    new_idx = j;
                    break;
                }
            }
            if (new_idx >= 0)
                break;
        }
        h->regions[k].chunks[new_idx].base = h->regions[k].base_addr;
        h->regions[k].chunks[new_idx].size = size;
        cull_region(h->regions + ptr_reg, h->regions[ptr_reg].chunks[ptr_idx].addr, ptr_idx, h->regions[ptr_reg].chunks[ptr_idx].size);
    } else {
        h = grow(h, size);
        ptr = alloc(h, size);
        memcpy(ptr, h->regions[i].chunks[ptr_idx].addr, h->regions[i].chunks[ptr_idx].size);
        h->regions[i].chunks[h->n_regions-1].base = h->regions[i].base_addr;
        h->regions[i].chunks[h->n_regions-1].size = size;
        cull_region(h->regions + i, h->regions[i].chunks[ptr_idx].addr, ptr_idx, h->regions[i].chunks[ptr_idx].size);
    }

    #if LOGGING
        log(h);
    #endif

    return ptr;
}

inline void *shrink(heap_t *h, void *ptr, int size)
{
    int ptr_idx = -1;
    int i = 0, j = 0;
    int old_size = 0;

    if (size <= 0) {
        return NULL;
    }

    for (i = 0; i < h->n_regions; i++) {
        for (j = 0; j < h->regions[i].n_chunks; j++) {
            if (h->regions[i].chunks[j].addr == ptr) {
                ptr_idx = j;
                break;
            }
        }
        if (ptr_idx >= 0)
            break;
    }

    old_size = h->regions[i].chunks[ptr_idx].size;

    if ((h->regions[i].n_chunks - 1) == ptr_idx) {
        h->regions[i].chunks = (smart_ptr *)realloc(h->regions[i].chunks, (h->regions[i].n_chunks+1)*sizeof(smart_ptr));
        if (h->regions[i].chunks == NULL) {
            fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
            fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
            goto exit;
        }
        h->regions[i].chunks[h->regions[i].n_chunks].size = h->regions[i].chunks[ptr_idx].size - size;
        h->regions[i].chunks[h->regions[i].n_chunks].flag = CULLED;
        h->regions[i].chunks[h->regions[i].n_chunks].addr = ptr + size;
        h->regions[i].chunks[ptr_idx].size = size;
        h->regions[i].n_chunks++;
    } else {
        if (!((h->regions[i].chunks[ptr_idx+1].flag & 0b01) ^ \
              (h->regions[i].chunks[ptr_idx+1].flag & 0b10))) {
            h->regions[i].chunks[ptr_idx+1].addr -= size;
            h->regions[i].chunks[ptr_idx+1].size += size;
            h->regions[i].chunks[ptr_idx].size -= size;
        } else {
            h->regions[i].chunks = (smart_ptr *)realloc(h->regions[i].chunks, (h->regions[i].n_chunks+1)*sizeof(smart_ptr));
            if (h->regions[i].chunks == NULL) {
                fprintf(stderr, "%s: %s\n", MEM_CODE, strerror(errno));
                fprintf(stderr, "%s: %s: %d\n", __FILE__, __func__, __LINE__);
                goto exit;
            }
            for (int j = h->regions[i].n_chunks; j > ptr_idx; j--) {
                h->regions[i].chunks[j] = h->regions[i].chunks[j-1];
            }
            h->regions[i].chunks[ptr_idx+1].size = h->regions[i].chunks[ptr_idx].size - size;
            h->regions[i].chunks[ptr_idx+1].flag = CULLED;
            h->regions[i].chunks[ptr_idx+1].addr = ptr + size;
            h->regions[i].chunks[ptr_idx].size = size;
            h->regions[i].n_chunks++;
        }
    }

    h->regions[i].used_size -= (old_size - size);

    #if LOGGING
        log(h);
    #endif

    return ptr;

exit:
    return NULL;
}

inline heap_t *cull(heap_t *h, void *ptr)
{
    int found = 0;
    int ptr_idx = -1;
    int i, j = 0;

    for (i = 0; i < h->n_regions; i++) {
        for (j = 0; j < h->regions[i].n_chunks; j++) {
            if (h->regions[i].chunks[j].addr == ptr) {
                found = 1;
                ptr_idx = j;
                break;
            }
        }
        if (found == 1)
            break;
    }

    if (found == 1)
        cull_region(h->regions + i, ptr, ptr_idx, h->regions[i].chunks[ptr_idx].size);

    #if LOGGING
        log(h);
    #endif

    return h;
}

inline heap_t *clean(heap_t *h)
{
    for (int i = 0; i < h->n_regions; i++) {
        clean_region(h->regions + i);
    }

    #if LOGGING
        log(h);
    #endif

    return h;
}

inline void destroy(heap_t *h)
{
    if (h) {
        for (int i = 0; i < h->n_regions; i++) {
            destroy_region(h->regions + i);
        }
        if (h->regions)
            free(h->regions);
        h->regions = NULL;
        free(h);
        h = NULL;
    }

    #if LOGGING
        fclose(log_file);
    #endif
}
