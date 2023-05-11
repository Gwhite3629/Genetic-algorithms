#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "utils.h"
#include "memory.h"

typedef struct hashmap_t {
    char *string;
    unsigned int hash;
	void *data;
} hashmap_t;

static inline unsigned int
hash(char *string)
{
	unsigned long val = 5381;
	int c;
	while ((c = *string++)) {
		val = ((val << 5) + val) + c;
	}
	return (unsigned int)(val % (__UINT32_MAX__));
}


static inline int
cmp(const void *va, const void *vb)
{
	const hashmap_t *a = (hashmap_t *)va;
	const hashmap_t *b = (hashmap_t *)vb;
	return (unsigned int)(a->hash - b->hash);
}

static inline hashmap_t
lookup(char *string, hashmap_t *user_table, int size)
{
	hashmap_t zero = {
		"None",
		0,
		NULL
	};
	unsigned int key = hash(string);
	hashmap_t table = {"None",key,NULL};
	hashmap_t *r = (hashmap_t *)bsearch(&table, user_table, size, sizeof(hashmap_t), &cmp);
	return r ? *r : zero;
}

static inline hashmap_t
direct_lookup(unsigned int key, hashmap_t *user_table, int size)
{
	hashmap_t zero = {
		"None",
		0,
		NULL
	};
	hashmap_t table = {"None",key,NULL};
	hashmap_t *r = (hashmap_t *)bsearch(&table, user_table, size, sizeof(hashmap_t), &cmp);
	return r ? *r : zero;
}

static inline int
map_insert(hashmap_t **map, int *size, char *string, void *data)
{
	int ret = SUCCESS;
    hashmap_t temp = lookup(string, (*map), (*size));
    hashmap_t *scan = (hashmap_t *)bsearch(&temp, (*map), (*size), sizeof(hashmap_t), &cmp);
    if (scan != NULL) {
        printf("String already in map\n");
        return 0;
    }

    (*size)++;
	(*map) = widen((*map), (*size), hashmap_t);
	(*map)[(*size)-1].string = new((*map)[(*size)-1].string, strlen(string)+1, char);
	strcpy((*map)[(*size)-1].string, string);
	(*map)[(*size)-1].data = (void *)data;
	(*map)[(*size)-1].hash = hash(string);
	qsort((*map), (*size), sizeof(hashmap_t), &cmp);

exit:
    return ret;
}

static inline int
map_remove(hashmap_t **map, int *size, char *string)
{
	int ret = SUCCESS;

	hashmap_t temp = lookup(string, (*map), (*size));
	hashmap_t *scan = (hashmap_t *)bsearch(&temp, (*map), (*size), sizeof(hashmap_t), &cmp);
	if (scan != NULL) {
		temp = (*map)[(*size)-1];
		(*map)[(*size)-1] = (*scan);
		(*scan) = temp;
		(*size)--;
		(*map) = shorten((*map), (*size), hashmap_t);
	} else {
		printf("String not in map\n");
	}

exit:
	return ret;
}

static inline int
direct_map_remove(hashmap_t **map, int *size, unsigned int key)
{
	int ret = SUCCESS;

	hashmap_t temp = direct_lookup(key, (*map), (*size));
	hashmap_t *scan = (hashmap_t *)bsearch(&temp, (*map), (*size), sizeof(hashmap_t), &cmp);
	if (scan != NULL) {
		temp = (*map)[(*size)-1];
		(*map)[(*size)-1] = (*scan);
		(*scan) = temp;
		(*size)--;
		shorten((*map), (*size), hashmap_t);
	} else {
		printf("Key not in map\n");
	}

exit:
	return ret;
}

#endif // _HASHMAP_H_