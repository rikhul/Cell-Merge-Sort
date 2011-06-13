#ifndef MERGER_NODE_H
#define MERGER_NODE_H 1

#include <spu_intrinsics.h>
#include "shareddefs.h"
#include "ctrlblock.h"

typedef struct merger_data {

  unsigned char held_tag[3];
  unsigned int num_waiting[3];
  unsigned int num_pulled[2];
  unsigned int consumed[2];
  unsigned char depleted[2];
  unsigned char mm_depleted[2];
  unsigned char done;

  volatile vector signed int idx[4][2];
  volatile vector signed int *buffer[3];

} merger_data_t;

#endif 
