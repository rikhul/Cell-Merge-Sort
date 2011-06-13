#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "shareddefs.h"
#include "node_spu_map.h"
#include "optimal_maps.h"

node_map_t* create_map(int num_spus, int mem){
  // Assign nodes to SPUs
  node_map_t* map = DC_map(num_spus, mem, 1);

  // Allocate buffersizes
  int i,j;
  for(i = 0; i < num_spus; i++){

    // Find out how much is explicitly allocated
    int explicitly_allocated = 0;
    int num_buffers_exp_al = 0;
    for(j=0; j < map->map[i].num_nodes; j++){

      // Check if parent is local
      int id = map->map[i].nodes[j].id;
      int parent_id = (id+1)/2 -1;

      if(is_local(id,parent_id,map)){
	map->map[i].nodes[j].buffer_size[OUT] = 0;
	num_buffers_exp_al++;
      } else if(map->map[i].nodes[j].buffer_size[OUT]){
	explicitly_allocated += map->map[i].nodes[j].buffer_size[OUT];
	num_buffers_exp_al++;
      }

      if(map->map[i].nodes[j].buffer_size[LEFT]){
	explicitly_allocated += map->map[i].nodes[j].buffer_size[LEFT];
	num_buffers_exp_al++;
      }
	
      if(map->map[i].nodes[j].buffer_size[RIGHT]){
	explicitly_allocated += map->map[i].nodes[j].buffer_size[RIGHT];
	num_buffers_exp_al++;
      }
    }

    // Spread the rest out
    int mem_over = mem - explicitly_allocated;
    int bsize = mem_over / (map->map[i].num_nodes*3 - num_buffers_exp_al);

    for(j=0; j < map->map[i].num_nodes; j++){
      if(!map->map[i].nodes[j].buffer_size[LEFT])
	map->map[i].nodes[j].buffer_size[LEFT] = bsize;

      if(!map->map[i].nodes[j].buffer_size[RIGHT])
	map->map[i].nodes[j].buffer_size[RIGHT] = bsize;

      int id = map->map[i].nodes[j].id;
      int parent_id = (id+1)/2 -1;
      if(!is_local(id,parent_id,map) && !map->map[i].nodes[j].buffer_size[OUT]){
	map->map[i].nodes[j].buffer_size[OUT] = bsize;	
      } 
    }
  }

  return map;
}

node_map_t* DC_map(int num_spus, int mem, node_id_t root){
  assert(num_spus > 0);
  assert(root > 0);

  // Try read in a mapping for num_spus from file "[num_spus]map.txt"
  //  char filename_ending[] = "map.txt";
  char filename[] = "00map.txt";
  if(num_spus > 9){
    filename[0] = 49;
    filename[1] = (num_spus - 10) + 48;
  } else
    filename[1] = num_spus +48;

  //strcat(filename,filename_ending);
  
  FILE *f;
  f = fopen(&filename, "r");

  if(f != NULL){
    //printf("Found mapping file for %d SPUs\n", num_spus);
    return read_mapping(f, num_spus, mem, root);
  } 

  node_map_t* map = (node_map_t*) malloc(sizeof(node_map_t));
  map->num_spus = num_spus;
  map->map = (node_collection_t*) malloc(num_spus*sizeof(node_collection_t));
  map->map[0].num_nodes = 1;
  map->map[0].nodes = (node_t*) malloc(sizeof(node_t));
  map->map[0].nodes[0].id = root-1;
  map->map[0].nodes[0].buffer_size[LEFT] = mem/3;
  map->map[0].nodes[0].buffer_size[RIGHT] = mem/3;
  map->map[0].nodes[0].buffer_size[OUT] = mem/3;
    
  if(num_spus -1 > 0){
    node_map_t* left = DC_map(num_spus-1, mem/2, 2*root);
    node_map_t* right = DC_map(num_spus-1, mem/2, 2*root+1);

    sort_map_asc(left);
    sort_map_desc(right);

    node_map_t* subtree = merge_maps(left, right);
  
    free_map(left);
    free_map(right);
    
    insert_map(map, subtree);
    
    free_map(subtree);
  }

  return map;
}

int cmp_spus_asc(void* l, void* r){
  node_collection_t n1 = *(node_collection_t*)l;
  node_collection_t n2 = *(node_collection_t*)r;

  int temp = n1.num_nodes - n2.num_nodes;
  if(temp > 0)
    return 1;
  else if(temp < 0)
    return -1;
  else
    return 0;
}

int cmp_spus_desc(void* l, void* r){
  return cmp_spus_asc(r,l);
}

void sort_map_asc(node_map_t* map){
  assert(map != NULL);

  qsort(map->map, map->num_spus, sizeof(node_collection_t), cmp_spus_asc);
}

void sort_map_desc(node_map_t* map){
  assert(map != NULL);

  qsort(map->map, map->num_spus, sizeof(node_collection_t), cmp_spus_desc);
}

node_map_t* merge_maps(node_map_t* left, node_map_t* right){
  assert(left->num_spus == right->num_spus);
  
  node_map_t* map = (node_map_t*) malloc(sizeof(node_map_t));
  map->num_spus = left->num_spus;
  map->map = (node_collection_t*)malloc(map->num_spus*sizeof(node_collection_t));
					
  int i;
  for(i=0; i < map->num_spus; i++){
    int num_nodes = left->map[i].num_nodes + right->map[i].num_nodes;
    map->map[i].num_nodes = num_nodes;
    map->map[i].nodes = (node_t*)malloc(num_nodes*sizeof(node_t));

    int k;
    for(k=0; k < left->map[i].num_nodes; k++){
      map->map[i].nodes[k].id = left->map[i].nodes[k].id;
      map->map[i].nodes[k].buffer_size[LEFT] = left->map[i].nodes[k].buffer_size[LEFT];
      map->map[i].nodes[k].buffer_size[RIGHT] = left->map[i].nodes[k].buffer_size[RIGHT];
      map->map[i].nodes[k].buffer_size[OUT] = left->map[i].nodes[k].buffer_size[OUT];
    }
    
    for(k=0; k < right->map[i].num_nodes; k++){
      map->map[i].nodes[k + left->map[i].num_nodes].id = right->map[i].nodes[k].id;
      map->map[i].nodes[k + left->map[i].num_nodes].buffer_size[LEFT] = right->map[i].nodes[k].buffer_size[LEFT];
      map->map[i].nodes[k + left->map[i].num_nodes].buffer_size[RIGHT] = right->map[i].nodes[k].buffer_size[RIGHT];
      map->map[i].nodes[k + left->map[i].num_nodes].buffer_size[OUT] = right->map[i].nodes[k].buffer_size[OUT];
    }
  }
					
  return map;
}

void insert_map(node_map_t* map, node_map_t* submap){

  int lvl = map->num_spus - submap->num_spus;

  int i;
  for(i=0; i < submap->num_spus; i++){
    map->map[i+lvl].num_nodes = submap->map[i].num_nodes;
    map->map[i+lvl].nodes = (node_t*) malloc(submap->map[i].num_nodes*sizeof(node_t));

    int k;
    for(k=0; k < submap->map[i].num_nodes; k++){
      map->map[i+lvl].nodes[k].id = submap->map[i].nodes[k].id;
      map->map[i+lvl].nodes[k].buffer_size[LEFT] = submap->map[i].nodes[k].buffer_size[LEFT];
      map->map[i+lvl].nodes[k].buffer_size[RIGHT] = submap->map[i].nodes[k].buffer_size[RIGHT];
      map->map[i+lvl].nodes[k].buffer_size[OUT] = submap->map[i].nodes[k].buffer_size[OUT];
    }
  }
}


void free_map(node_map_t* map){
  int i;
  for(i = 0; i < map->num_spus; i++){
    free(map->map[i].nodes);
  }

  free(map->map);
}


int is_local(node_id_t node_id_1, node_id_t node_id_2, node_map_t* map){
  assert(map != NULL);

  return get_spu(node_id_1, map) == get_spu(node_id_2, map);
}

unsigned char in_collection(node_id_t node_id, node_collection_t* nc){
  unsigned char i;
  for(i = 0; i < nc->num_nodes; i++)
    if(node_id == nc->nodes[i].id)
      return i;

  return 255;
}

int get_level(node_id_t node_id){
  ++node_id;
  int level = 0;

  while((1<<level) <= node_id){
    ++level;
  }

  return level-1;
}

int get_node_idx(int id, int spu, node_map_t* map){
  assert(map != NULL);

  int i;
  for(i=0; i < map->map[spu].num_nodes; i++)
    if(map->map[spu].nodes[i].id == id)
      return i;

  return -1;
}

int get_spu(unsigned short node_id, node_map_t* map){
  assert(map != NULL);

  int i,k;
  for(i = 0; i < map->num_spus; i++)
    for(k=0; k < map->map[i].num_nodes; k++)
      if(node_id == map->map[i].nodes[k].id)
	return i;
    
  return -1;
}

node_collection_t* get_nodes(spu_id_t spu, node_map_t* map){
  assert(map != NULL);
  assert(spu < map->num_spus);

  return map->map + spu;
}

void print_map(node_map_t* map, int style){
  assert(map != NULL);

  if(style == 0){
    int i,k;
    for(i = 0; i < map->num_spus; i++){
      printf("SPU%d:\n", i);
      for(k=0; k < map->map[i].num_nodes; k++)
	printf("%d (%d %d %d)\n", map->map[i].nodes[k].id, map->map[i].nodes[k].buffer_size[LEFT],
	       map->map[i].nodes[k].buffer_size[RIGHT], map->map[i].nodes[k].buffer_size[OUT]);
      printf("\n");
    }
  } else if(style == 1){
    int i,k;
    for(i = 0; i < map->num_spus; i++){
      printf("SPU%d: (%d nodes)", i, map->map[i].num_nodes);
      for(k=0; k < map->map[i].num_nodes; k++)
	printf(" %d", map->map[i].nodes[k].id);
      printf("\n");
    }
  }
}

void print_node_collection(node_collection_t* nc){
  int i;
  for(i=0; i < nc->num_nodes; i++){
    printf("%d ", nc->nodes[i].id);
  }
  printf("\n");
}

node_map_t* read_mapping(FILE *f, int num_spus, int mem, int root){
  int flag;
  char strb[STRLN];
  int num_mergers = (1 << num_spus) -1;


  // Skip until mapping begin
  flag = 0;
  while(!feof(f)){
    fgets(strb,STRLN,f);
    if(!strncmp(strb,"x [",3)){
      flag = 1;
      break;
    }
  }
  if(flag == 0){
    fprintf(stderr,"Error reading node mapping file\nCould not find line beginning with \"x [\"\n");
    return 0;
  }

  // Skip ": <spu> <spu>... :" line
  fgets(strb,STRLN,f);

  // Read actual mapping
  node_map_t *map = (node_map_t*) malloc( sizeof(node_map_t) );
  map->num_spus = num_spus;
  map->map = (node_collection_t*) malloc( num_spus * sizeof(node_collection_t) );

  int i;
  for(i=0; i < num_spus; i++){
    map->map[i].nodes = (node_t*) malloc( num_mergers * sizeof(node_t) );
    map->map[i].num_nodes = 0;
  }

  int j;
  int spu = 0;
  char *pch = NULL;
  for(i=0; i < num_mergers; i++){
    // Expect line like sequence of 0's and one 1, then 0,1 or 3 numbers representing total buffer size or buffer size for left,right and out-buffers
    fgets(strb,STRLN,f);
    pch = strtok(strb," ");
    // skip first token wich is just the merger id on each row
    pch = strtok(NULL," ");
    j = 0;
    while(pch != NULL){
      if( j < num_spus ){
	// SPU mapping
	if(atoi(pch) == 1){
	  //TODO: fix id
	  int level = get_level(i);
	  int level_id_begin = (root << level);
	  int id = level_id_begin + ( (i+1) % (1 << level));
	  map->map[j].nodes[ map->map[j].num_nodes ].id = id-1;
	  spu = j;
	}
      } else {
	// Buffer sizes
	// TODO: fix buffersize in relation to available mem
	int req_buffer_size = atoi(pch);
	float mem_factor = (float) mem / MAX_TOTAL_BUFFER_SIZE;
	int mem_given = mem_factor * req_buffer_size;
	map->map[spu].nodes[ map->map[spu].num_nodes ].buffer_size[j - num_spus] = mem_given;
      }
      j++;
      pch = strtok(NULL," ");
    }

    // No buffer sizes stated
    if(j == num_spus){
      map->map[spu].nodes[ map->map[spu].num_nodes ].buffer_size[LEFT] = 0;
      map->map[spu].nodes[ map->map[spu].num_nodes ].buffer_size[RIGHT] = 0;
      map->map[spu].nodes[ map->map[spu].num_nodes ].buffer_size[OUT] = 0;
    } else if(j == num_spus +1){
      map->map[spu].nodes[ map->map[spu].num_nodes ].buffer_size[RIGHT] = 0;
      map->map[spu].nodes[ map->map[spu].num_nodes ].buffer_size[OUT] = 0;
    } else if(j == num_spus +2){
      map->map[spu].nodes[ map->map[spu].num_nodes ].buffer_size[OUT] = 0;
    }

    map->map[spu].num_nodes++;
  }

  return map;
}
