#include "dynarray.h"

int dynarray_chunk_size = 10;

DYN_ARR init_array(size_t item_size)
{
	DYN_ARR new_arr;
	new_arr.items = malloc(dynarray_chunk_size * item_size);
	new_arr.capacity = dynarray_chunk_size;
	new_arr.nelems = 0;
	new_arr.item_size = item_size;
	return new_arr;
}

int expand_array (DYN_ARR *array, size_t amount)
{
	char *temp = realloc(array->items, (array->capacity + amount) * array->item_size);
	if (temp) 
	{
		array->items = temp;
		array->capacity += amount;
		return 1;
	}
	return 0;
}

void push_back (DYN_ARR *array, void *item)
{
	if (array->nelems == array->capacity)
	{
		if (!expand_array(array, dynarray_chunk_size))
		{
			printf("Unable to add item to array.\n");
			return;
		}
	}
	memcpy(array->items + array->nelems * array->item_size, item, array->item_size);
	array->nelems++;
}

void remove_item(DYN_ARR *array, int index)
{
	char *cpy_dest = array->items + (index * array->item_size);
	char *cpy_source = cpy_dest + array->item_size;
	size_t cpy_size = (array->nelems - index - 1) * array->item_size;
	array->nelems--;

	memcpy(cpy_dest, cpy_source, cpy_size);
}

