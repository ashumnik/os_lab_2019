#include <limits.h>
#include "utils.h"


struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = begin; i != end; i++){
    min_max.min = min_max.min > array[i] ? array[i] : min_max.min;
    min_max.max = min_max.max <= array[i] ? array[i] : min_max.max;
  }

  return min_max;
}
