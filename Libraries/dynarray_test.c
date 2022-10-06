#include <stdio.h>
#include "dynarray.h"

int main(void)
{
	dynarray_chunk_size = 10;
	//dynarray_chunk_size = 1000000;
	DYN_ARR test = init_array(sizeof(int));


	//expand_array(&test, 5000000);

	for (int i = 0; i < 10; i++) push_back(&test, &i);

	int counter = 0;
	FOREACH(int, k, test, 0)
	{
		// Dereference just how it works
		printf("%d\n", *k);
	}
	//printf("%d\n", counter);
	return 0;
}