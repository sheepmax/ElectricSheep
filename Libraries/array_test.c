#include "dynarray.h"
#include <stdio.h>

int compreints (const void *a, const void *b)
{
	int ia = *((int *)a);
	int ib = *((int *)b);
	
	return (ia < ib) ? -1 : (ia > ib);
}

int main (int argc, char **argv) 
{
	DYN_ARR test_array = init_array(sizeof(int));

	for (int i = 10; i > 0; i--)
	{
		push_back(&test_array, &i);
	}

	printf("%d, %d\n", test_array.nelems, test_array.item_size);

	for (int i = 0; i < 11; i++)
	{
		printf("%d\n", GET_ITEM(test_array, i, int));
	}

	remove_item(&test_array, 0);

	sort_array(&test_array, compreints);

	printf("Sorted:\n");

	for (int i = 0; i < 10; i++)
	{
		printf("%d\n", GET_ITEM(test_array, i, int));
	}

	return 0; 
}