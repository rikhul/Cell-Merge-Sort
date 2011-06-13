#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>

#include "shareddefs.h"
#include "ctrlblock.h"
#include "node_spu_map.h"

#define NUM_SPUS 5

extern spe_program_handle_t spu_mergernode;

typedef struct ppu_pthread_data {
  spe_context_ptr_t speid;
  pthread_t pthread;
  void* argp;
  unsigned int ls_start_addr;
} ppu_pthread_data_t;

void* ppu_pthread_function(void* arg){
  ppu_pthread_data_t* data = (ppu_pthread_data_t*) arg;
  int retval;
  unsigned int entry = SPE_DEFAULT_ENTRY;
  if((retval = spe_context_run(data->speid, &entry, 0, data->argp,
			       NULL,NULL)) < 0){
    perror("spe_context_run");
    exit(1);
  }
  pthread_exit(NULL);	       
};

void merge(int *in, int *out, int num_blocks, int block_size){

  node_map_t* map = create_map(NUM_SPUS,MAX_TOTAL_BUFFER_SIZE);
  int num_nodes = (1 << NUM_SPUS) -1;
  int i;

  #ifdef PRINT_INFO
  printf("NODE MAP\n");
  printf("num SPUS: %d\nnum nodes: %d\n", NUM_SPUS, num_nodes);
  print_map(map,1);

  printf("\nNUMBER OF NODES PER SPU\n");
  for(i=0; i < map->num_spus; i++)
    printf("SPU%d, num nodes: %d\n", i, map->map[i].num_nodes);
  #endif

  // Setup SPEs
  volatile merger_ctrlblock_t *merger_ctrlblock[NUM_SPUS];
  spu_ctrlblock_t *spu_ctrlblock[NUM_SPUS];

  for(i=0; i < NUM_SPUS; i++){
    posix_memalign((void**)&spu_ctrlblock[i],16,sizeof(spu_ctrlblock_t));
    spu_ctrlblock[i]->spu_id = i;
    spu_ctrlblock[i]->num_mergers = map->map[i].num_nodes;
       
    posix_memalign((void**)&merger_ctrlblock[i],16,map->map[i].num_nodes*sizeof(merger_ctrlblock_t));
    spu_ctrlblock[i]->ctrlblocks_addr = (unsigned int) merger_ctrlblock[i];
  }

  // Start SPEs
  int retval = 0;
  ppu_pthread_data_t data[NUM_SPUS];
  /* Create context */
  for(i=0;i<NUM_SPUS;i++){
    data[i].speid = spe_context_create(SPE_MAP_PS, NULL);
    if(!data[i].speid){
      perror("spe_context_create");
      exit(1);
    }
    data[i].ls_start_addr = (unsigned int) spe_ls_area_get(data[i].speid);
  }

  /*
  #ifdef PRINT_INFO
  printf("\nLS START ADDRESSES\n");
  for(i=0;i<NUM_SPUS;i++)
    printf("SPU%d ls start %x\n", i, data[i].ls_start_addr);
  printf("\n");
  #endif
  */

  /* Load the the program into the context */
  for(i=0;i<NUM_SPUS;i++){

    retval = spe_program_load(data[i].speid,&spu_mergernode);

    if(retval){
      perror("spe_program_load");
      return;
    }
  }

  /* Run the program inside the context */
  for(i=0;i<NUM_SPUS;i++){
    data[i].argp = spu_ctrlblock[i];

    if((retval = pthread_create(&data[i].pthread,NULL,&ppu_pthread_function,
				&data[i])) != 0){
      perror("pthread_create");
      exit(1);
    }
  }

  // wait for mail from SPUs telling us they are done with sending the merger ctrlblocks
  unsigned int msg[1];
  for(i=0; i < NUM_SPUS; i++){
    while(! spe_out_mbox_status(data[i].speid) );

    spe_out_mbox_read(data[i].speid,msg,1);
  }

  // init fields, wire addresses
  int k;
  int id,level, block_idx=0;

  for(i=0; i < NUM_SPUS; i++){
    unsigned int block_addr_offset = 0;
    for(k=0; k < map->map[i].num_nodes; k++){
      merger_ctrlblock[i][k].buffer_size[LEFT] = map->map[i].nodes[k].buffer_size[LEFT];
      merger_ctrlblock[i][k].buffer_size[RIGHT] = map->map[i].nodes[k].buffer_size[RIGHT];
      merger_ctrlblock[i][k].buffer_size[OUT] = map->map[i].nodes[k].buffer_size[OUT];

      int id = map->map[i].nodes[k].id;
      int parent_id = (id+1) /2 -1;
      int parent_spu = get_spu(parent_id,map);
      int parent_node_idx = get_node_idx(parent_id,parent_spu,map);

      if(id)
	merger_ctrlblock[i][k].buffer_size[PARENT] = map->map[parent_spu].nodes[parent_node_idx].buffer_size[(id+1)&1];
      else
	merger_ctrlblock[i][k].buffer_size[PARENT] = INT_MAX;

      merger_ctrlblock[i][k].block_addr[LEFT] = block_addr_offset;
      block_addr_offset += merger_ctrlblock[i][k].buffer_size[LEFT] * 16; // sizeof(vector) = 16...
      merger_ctrlblock[i][k].block_addr[RIGHT] = block_addr_offset;
      block_addr_offset += merger_ctrlblock[i][k].buffer_size[RIGHT] * 16;
      merger_ctrlblock[i][k].block_addr[OUT] = block_addr_offset;
      block_addr_offset += merger_ctrlblock[i][k].buffer_size[OUT] * 16;
    }
  }

  for(i=0; i < NUM_SPUS; i++){
    for(k=0; k < map->map[i].num_nodes; k++){
      id = map->map[i].nodes[k].id;
      level = get_level(id);
      merger_ctrlblock[i][k].id = id;
      merger_ctrlblock[i][k].leaf_node = 0;
     
      if(id == 0){
	// Root
	// Addr to MM out block
	merger_ctrlblock[i][k].block_addr[OUT] = (unsigned int) &out[0];

	merger_ctrlblock[i][k].local[LEFT] = 255;
	merger_ctrlblock[i][k].local[RIGHT] = 255;
	merger_ctrlblock[i][k].local[OUT] = 255;
      } else if(id >= num_nodes/2) {
	// Leaf
	merger_ctrlblock[i][k].leaf_node = 1;
	
	merger_ctrlblock[i][k].block_addr[LEFT] = (unsigned int) in + block_idx*block_size*sizeof(int);
	++block_idx;
	merger_ctrlblock[i][k].block_addr[RIGHT] = (unsigned int) in + block_idx*block_size*sizeof(int);
	++block_idx;

	merger_ctrlblock[i][k].local[LEFT] = 255;
	merger_ctrlblock[i][k].local[RIGHT] = 255;
      }

      merger_ctrlblock[i][k].data_size[LEFT] = (block_size / 4) << (NUM_SPUS-1-level);
      merger_ctrlblock[i][k].data_size[RIGHT] = (block_size / 4) << (NUM_SPUS-1-level);

      int parent_id = (id+1)/2 -1;
      int left_child_id = (id+1)*2 -1;
      int right_child_id = (id+1)*2;
      int parent_spu = get_spu(parent_id,map);
      int parent_spu_idx = get_node_idx(parent_id, parent_spu, map);

      if(id > 0 && id < num_nodes/2){
	merger_ctrlblock[i][k].local[LEFT] = in_collection(left_child_id, get_nodes(i, map));
	merger_ctrlblock[i][k].local[RIGHT] = in_collection(right_child_id, get_nodes(i, map));
      }

      if(id != 0){
	merger_ctrlblock[i][k].local[OUT] = in_collection(parent_id, get_nodes(i, map));

	// Swap addresses for buffers and index vectors
	unsigned int parent_head_addr = merger_ctrlblock[parent_spu][parent_spu_idx].idx_addr[(id+1)&1] + data[parent_spu].ls_start_addr;
	merger_ctrlblock[parent_spu][parent_spu_idx].idx_addr[(id+1)&1] = merger_ctrlblock[i][k].idx_addr[OUT] + data[i].ls_start_addr;
	merger_ctrlblock[i][k].idx_addr[OUT] = parent_head_addr;
      
	unsigned int parent_buffer_addr = merger_ctrlblock[parent_spu][parent_spu_idx].block_addr[(id+1)&1] + data[parent_spu].ls_start_addr;
	merger_ctrlblock[parent_spu][parent_spu_idx].block_addr[(id+1)&1] = merger_ctrlblock[i][k].block_addr[OUT] + data[i].ls_start_addr;
	merger_ctrlblock[i][k].block_addr[OUT] = parent_buffer_addr;
      }
    }
  }

  struct timeval tv1, tv2;
  // Start timer
  gettimeofday(&tv1, NULL);

  // Send go-ahead mail to SPEs
  for(i=0; i < NUM_SPUS; i++){
    spe_in_mbox_write(data[i].speid, msg, 1, SPE_MBOX_ALL_BLOCKING);
    //printf("PPU sent mail to SPU%d\n",i);
  }

  /* Wait for SPE to complete */
  for(i=0;i<NUM_SPUS;i++){
    retval = pthread_join(data[i].pthread,NULL);
    if(retval != 0){
      perror("pthread_join");
      exit(1);
    }
  }

  // Stop timer
  gettimeofday(&tv2, NULL);

   /* Deallocate the context */
  for(i=0;i<NUM_SPUS;i++){
    retval = spe_context_destroy(data[i].speid);
    if(retval){
      perror("spe_context_destroy");
      exit(1);
    }
  }

  double time;
  time = (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)/1000000.0;
  printf("%f\n", time);

}

int main(int argc, char **argv){
  int i;
  //char *test;
  //posix_memalign((void**)&test,128,1024);

  struct timeval tv1, tv2;
  double time = 0;

#ifdef PRINT_INFO
  printf("STRUCTURE SIZE\n");
  printf("spu_ctrlblock_t: %u\n",sizeof(spu_ctrlblock_t));
  printf("merger_ctrlblock_t: %u\n",sizeof(merger_ctrlblock_t));
#endif

  assert( sizeof(spu_ctrlblock_t) % 16 == 0 );
  assert( sizeof(merger_ctrlblock_t) % 16 == 0 );

  // Read testdata
  if(argc != 2){
    printf("usage sorter in_datafile\n");
    exit(0);
  }

  gettimeofday(&tv1, NULL);

  FILE *f;
  int nRead;
    
  f = fopen(argv[1],"rb");
  if(f == NULL){
    printf("Error opening file\n");
    exit(0);
  }
  
  int num_blocks;
  nRead = fread(&num_blocks,sizeof(int),1,f);
  if(nRead != 1){
    perror("Error reading file");
    exit(1);
  }

  assert( num_blocks == 1<<NUM_SPUS );

  int block_size;
  nRead = fread(&block_size,sizeof(int),1,f);
  if(nRead != 1){
    perror("Error reading file");
    exit(1);
  }

#ifdef PRINT_INFO
  printf("\nPROBLEM SIZE\n");
  printf("#integers: %d (%dkb)\n", block_size*num_blocks, block_size*num_blocks*sizeof(int)/1000);
  printf("#blocks: %d\nblock size: %d (%dkb)\n", num_blocks, block_size, block_size*sizeof(int)/1000);
#endif

  int *in; //[num_blocks*block_size];
  posix_memalign((void**)&in,16,num_blocks*block_size*sizeof(int));
  int *out; //[num_blocks*block_size] aligned_128;
  posix_memalign((void**)&out,16,num_blocks*block_size*sizeof(int));

  assert( *in % 16 == 0 );
  assert( *out % 16 == 0 );

  #ifdef PRINT_INFO
  printf("OUT addr %x - %x\n", (unsigned int) out, (unsigned int) out + num_blocks*block_size*sizeof(int));
  #endif

  for(i=0; i < num_blocks*block_size; i++)
    out[i] = 0;

  nRead = fread(in,sizeof(int),num_blocks*block_size,f);
  if(nRead != num_blocks*block_size){
    perror("Error reading file");
    exit(1);
  }

  gettimeofday(&tv2, NULL);

  #ifdef PRINT_INFO
  time = (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)/1000000.0;
  printf("Time to read test data: %f\n\n", time);
  #endif

  #ifdef PRINT_DATA
  printf("IN DATA\n");
  for(i=0; i < num_blocks*block_size; i += 4){
    printf("%d:\t%d\t%d\t%d\t%d\n",
	   i,
	   in[i],
	   in[i+1],
	   in[i+2],
	   in[i+3]);
  }

  printf("\n\n");
  #endif

  gettimeofday(&tv1, NULL);
  merge(in, out, num_blocks, block_size);
  gettimeofday(&tv2, NULL);

  time = (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)/1000000.0;
  //printf("\nRESULT Run 1\nTime to setup SPUs and merge data: %f\n\n", time);

  /*
  gettimeofday(&tv1, NULL);
  merge(in, out, num_blocks, block_size);
  gettimeofday(&tv2, NULL);

  time = (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)/1000000.0;
  printf("\nRESULT Run 2\nTime to setup SPUs and merge data: %f\n\n", time);
  */

  #ifdef PRINT_DATA
  printf("\nOUT DATA\n");
  for(i=0; i < num_blocks*block_size; i += 4){
    printf("%d:\t%d\t%d\t%d\t%d\n",
	   i,
	   out[i],
	   out[i+1],
	   out[i+2],
	   out[i+3]);
  }
  #endif

  gettimeofday(&tv1, NULL);

  return 0;

  // Validate out data
  FILE *sorted;
  sorted = fopen(argv[2],"rb");
  if(f == NULL){
    printf("Error opening file.\n");
    exit(0);
  }

  nRead = fread(in,sizeof(int),num_blocks*block_size,sorted);
  if(nRead != num_blocks*block_size){
    perror("Error reading file.\n");
    exit(1);
  }

  fclose(sorted);

  for(i=0; i < num_blocks*block_size; i++){
    if(in[i] != out[i]){
      printf("Incorrectly sorted!\ni: %d\n", i);

      int d1 = i < 10 ? 0 : i-10;
      int d2 = i > num_blocks*block_size-10 ? num_blocks*block_size : i + 10;

      printf("IDX\tOUT\t\tCORRECT\n");
      for(; d1 < d2; d1++){
	printf("%d:\t%d\t%d\n", d1, out[d1], in[d1]);
      }
           
      break;
    }
    
  }

  gettimeofday(&tv2, NULL);

  #ifdef PRINT_INFO
  time = (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)/1000000.0;
  printf("Time to validate data: %f\n\n", time);
  #endif  


  return 0;
}
