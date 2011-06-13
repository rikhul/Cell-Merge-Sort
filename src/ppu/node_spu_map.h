#ifndef NODE_SPU_MAP_H
#define NODE_SPU_MAP_H 1

#include <stdio.h>
#include "shareddefs.h"

typedef unsigned short node_id_t;
typedef unsigned short spu_id_t;
typedef unsigned short num_t;

typedef struct node_struct {
  node_id_t id;
  unsigned int buffer_size[3];
} node_t;

typedef struct node_collection_struct {
  num_t num_nodes;
  node_t* nodes;
} node_collection_t;

typedef struct node_map_struct {
  num_t num_spus;
  node_collection_t* map;
} node_map_t;

int is_local(node_id_t node_id_1, node_id_t node_id_2, node_map_t* map);
unsigned char in_collection(node_id_t node_id, node_collection_t* nc);

int get_spu(node_id_t node_id, node_map_t* map);
int get_node_idx(int id, int spu, node_map_t* map);
node_collection_t* get_nodes(spu_id_t spu, node_map_t* map);
int get_level(node_id_t node_id);

void print_map(node_map_t* map, int style);
void print_node_collection(node_collection_t* nc);

node_map_t* create_map(int num_spus, int mem);
node_map_t* DC_map(int num_spus, int mem, node_id_t root);
node_map_t* read_mapping(FILE *f, int num_spus, int mem, int root);
int cmp_spus_asc(void*, void*);
int cmp_spus_desc(void*, void*);
void sort_map_asc(node_map_t* map);
void sort_map_desc(node_map_t* map);
node_map_t* merge_maps(node_map_t* left, node_map_t* right);
void insert_map(node_map_t* map, node_map_t* submap);
void free_map(node_map_t* map);

#endif
