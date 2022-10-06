#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Char based memcpy arrays
#ifndef DYNARRAY_H_INCLUDED
	#define DYNARRAY_H_INCLUDED

	extern int dynarray_chunk_size;

	typedef struct 
	{
		char *items;
		unsigned int nelems;
		unsigned int capacity;
		size_t item_size;
	} DYN_ARR;

	DYN_ARR init_array(size_t item_size);

	int expand_array (DYN_ARR *array, size_t amount);

	void push_back (DYN_ARR *array, void *item); 

	void remove_item(DYN_ARR *array, int index);

	static inline void sort_array(DYN_ARR *array, int (*compare)(const void *, const void*)) 
	{
		qsort(array->items, array->nelems, array->item_size, compare);
	}

	static inline void free_array (DYN_ARR *array)
	{
		free(array->items);
	}

	#define GET_ITEM(array, index, type) ((type *)(array.items))[index]
	#define GET_ITEMP(array, index, type) ((type *)(array.items) + index)
#endif
