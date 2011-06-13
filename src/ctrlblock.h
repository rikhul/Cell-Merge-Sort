#ifndef _MERGERNODE_CTRLBLOCK
#define _MERGERNODE_CTRLBLOCK 1

#include "shareddefs.h"

// Structures must be 1,2,4,8 or n*16 bytes for DMA!
// Preferbly multiples of 16 bytes and 16 byte-aligned
// in memory

typedef struct merger_ctrlblock {
  // node id in tree in a breadth first order, starting at 0
  unsigned short id;
  // idx of merger on spu, for faster lookup
  unsigned short idx_on_spu;

  unsigned int block_addr[3];
  unsigned int data_size[2];

  // the address of left/right/out idx vector to update
  unsigned int idx_addr[3];

  // left/right/out/parent buffer size in #vectors
  int buffer_size[4];

  // >0 if node is leaf node
  unsigned char leaf_node;

  // local idx of left/right/parent or 255 if not local
  unsigned char local[3];
  
  unsigned char pad[5];
} merger_ctrlblock_t;

typedef struct spu_ctrlblock {
  // Merger control blocks
  unsigned int ctrlblocks_addr;
  unsigned char spu_id;
  unsigned short num_mergers;

  unsigned char pad[5];
} spu_ctrlblock_t;

#endif
