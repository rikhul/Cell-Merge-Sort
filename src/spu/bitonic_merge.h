#include <spu_intrinsics.h>
#include "ctrlblock.h"

#define SORT_TYPE signed int

void merge_buffers(volatile vector signed int *left, volatile vector signed int *right);
void merge_buffers_v(volatile vector signed int **buffer, int *idx);
void cp_buffer(volatile vector signed int *to, volatile vector signed int *from, int num);
void rev_buffer(volatile vector signed int *b);
