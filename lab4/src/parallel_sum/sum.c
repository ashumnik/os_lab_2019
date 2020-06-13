#include "sum.h"
#include <stdlib.h>

int Sum(const struct SumArgs *args)  {
	int sum = 0;	
	for (size_t i = args->begin; i < args->end; i++) {
		sum += args->array[i];		
	}
	return sum;
}
