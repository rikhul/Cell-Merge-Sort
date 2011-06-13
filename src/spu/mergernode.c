#include <stdio.h>
#include <spu_mfcio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <spu_intrinsics.h>
#include <malloc.h>
#include <assert.h>

#include "shareddefs.h"
#include "ctrlblock.h"
#include "mergernode.h"
#include "sort_vectors.h"

// Pointers to start of arrays
volatile spu_ctrlblock_t spu_ctrlblock aligned_128;

// Mergers ctrlblock
volatile merger_ctrlblock_t *mcb;
merger_data_t *md;

// Buffers
volatile vector signed int buffer[MAX_TOTAL_BUFFER_SIZE];

const vector signed int one = {1,1,1,1};
const vector signed int zero_v = {0,0,0,0};

int num_active_mergers = 0;
int ctrl_dma_tag;

void update_tail(int side);
void merge_buffers();
int num_in_buffer(int side);
int num_free_in_buffer(int side);
void check_pull_dma(int side);
void check_push_dma();
void setup_spu(unsigned int spu_ctrlblock_addr);
void push();
void notify_parent();
void notify_child(int side);
void pull(int side);
void get_next_buffer(int side, volatile vector signed int **buffer, int *idx);
void cp_buffer(int side);

// Index of active merger on this SPU, [0,num_mergers[
int am = 0;

#ifdef TRACE_TIME
unsigned int sort_v_ticks = 0;
unsigned int merge_ticks = 0;
unsigned int merge_loop_ticks = 0;

unsigned int dec_val = 0;
unsigned int dec_val2 = 0;
unsigned int dec_val3 = 0;
#endif

int main(unsigned long long spe_id,
	 unsigned long long argp,
	 unsigned long long envp)
{
  #ifdef TRACE_TIME
  spu_write_decrementer(0);
  #endif
  setup_spu(argp);

  //unsigned int prod_ticks = 0;

  // Merge
  num_active_mergers = spu_ctrlblock.num_mergers;

  while(num_active_mergers > 0){
    if(md[am].done){
      am = (am+1) % spu_ctrlblock.num_mergers;
      continue;
    }

    // Check DMA completion if leaf node or extern parent    
    if(mcb[am].leaf_node){
      check_pull_dma(LEFT);
      check_pull_dma(RIGHT);

      if(md[am].num_waiting[LEFT] + md[am].num_waiting[RIGHT]){
	am = (am+1) % spu_ctrlblock.num_mergers;
	continue;
      }
    }

    if(mcb[am].local[OUT] == 255){
      check_push_dma();
      if(md[am].held_tag[OUT] < 32){
	am = (am+1) % spu_ctrlblock.num_mergers;
	continue;
      }
    }

    if(md[am].done){
      am = (am+1) % spu_ctrlblock.num_mergers;
      continue;
    }

    // Produce    
    #ifdef TRACE_TIME
    dec_val = spu_read_decrementer();
    #endif
    if(md[am].depleted[LEFT] && !md[am].depleted[RIGHT]){      
      cp_buffer(RIGHT);
    } else if(md[am].depleted[RIGHT] && !md[am].depleted[LEFT]){
      cp_buffer(LEFT);
    } else {
      merge_buffers();
    }
    #ifdef TRACE_TIME
    merge_ticks += -(spu_read_decrementer() - dec_val);
    #endif

    // Push, if parent not local
    if(mcb[am].local[OUT] == 255)
      push();

    // Pull if leaf node
    if(mcb[am].leaf_node){
      pull(LEFT);
      pull(RIGHT);
    }

    am = (am+1) % spu_ctrlblock.num_mergers;
  }

  #ifdef TRACE_TIME
  float tot = -spu_read_decrementer() / (79.8*1000);
  float sort = sort_v_ticks / (79.8*1000);
  float merge = merge_ticks / (79.8*1000);
  float merge_loop = merge_loop_ticks / (79.8*1000);

  printf("SPU%d: Total %fms, merge %fms, inner loop %fms, sort %fms\n", spu_ctrlblock.spu_id,tot, merge, merge_loop, sort);
  #endif

  return 0;
}

void cp_buffer(int side){
  int avail_out = num_free_in_buffer(OUT);
  int avail_side = num_in_buffer(side);
  int max = avail_out < avail_side ? avail_out : avail_side;

  vector signed int *out_head;
  if(mcb[am].local[OUT] < 255)
    out_head = (vector signed int*) &md[ mcb[am].local[OUT] ].idx[ (mcb[am].id+1)&1 ][HEAD];
  else
    out_head = (vector signed int*) &md[am].idx[OUT][HEAD];

  vector unsigned int cmp_v;
  vector signed int from_size = spu_splats( mcb[am].buffer_size[side] );
  vector signed int out_size = spu_splats( mcb[ mcb[am].local[OUT] ].buffer_size[ (mcb[am].id+1)&1 ] );
  vector signed int ones = {1,1,1,1};
  vector signed int zeros = {0,0,0,0};

  int i;
  for(i = 0; i < max; i++){
    md[am].buffer[OUT][spu_extract( *out_head,0)] = md[am].buffer[side][spu_extract(md[am].idx[side][TAIL],0)];
    // update idx
    md[am].idx[side][TAIL] = spu_add(md[am].idx[side][TAIL], ones);
    cmp_v = spu_cmpeq(md[am].idx[side][TAIL],from_size);
    md[am].idx[side][TAIL] = spu_sel(md[am].idx[side][TAIL], zeros, cmp_v);

    *out_head = spu_add(*out_head,ones);
    cmp_v = spu_cmpeq(*out_head, out_size);
    *out_head = spu_sel(*out_head,zeros,cmp_v);
  }

  update_tail(side);

  md[am].consumed[side] += max;

  if(mcb[am].local[OUT] < 255 && md[am].consumed[side] == mcb[am].data_size[side]){
    md[am].depleted[side] = 1;
    md[am].done = 1;
    --num_active_mergers;
  }
}

void merge_buffers(){
  vector unsigned int cmp_v, cmp_v2;

  const vector signed int one_at_0 = {1,0,0,0};
  const vector signed int one_at_1 = {0,1,0,0};
  const vector signed int one_at_2 = {0,0,1,0};
  const vector signed int ones = {1,1,1,1};
  const vector signed int zeros = {0,0,0,0};

  const vector unsigned char cmp_v_shuffle_mask = {31,31,31,31,
						   31,31,31,31,
						   31,31,31,31,
						   31,31,31,31};
  vector unsigned char rev_mask;
  const vector unsigned char rev_left = {12,13,14,15,
					 8,9,10,11,
					 4,5,6,7,
					 0,1,2,3};

  const vector unsigned char rev_right = {28,29,30,31,
					  24,25,26,27,
					  20,21,22,23,
					  16,17,18,19};
  vector signed int *out_head_idx;
  if(mcb[am].local[OUT] < 255){
    int parent_idx = mcb[am].local[OUT];
    int side = (mcb[am].id+1)&1;
    out_head_idx = (vector signed int*) &md[parent_idx].idx[side][HEAD];
  } else {
    out_head_idx = (vector signed int*) &md[am].idx[OUT][HEAD];
  }

  vector signed int *left_tail_idx = (vector signed int*) &md[am].idx[LEFT][TAIL];
  vector signed int *right_tail_idx = (vector signed int*) &md[am].idx[RIGHT][TAIL];

  vector signed int size_v = {mcb[am].buffer_size[LEFT], mcb[am].buffer_size[RIGHT], mcb[am].buffer_size[OUT], 0};
  vector signed int avail_v = {num_in_buffer(LEFT), num_in_buffer(RIGHT), num_free_in_buffer(OUT), 1};
  vector signed int avail_before = { spu_extract(avail_v, 0), spu_extract(avail_v, 1), 0, 0 };
  vector unsigned int avail = spu_gather( spu_cmpgt(avail_v, zeros) ); // avail = 0x0F if all avail_v > zeros

  vector signed int *left, *right, *out;
  left = (vector signed int*) &md[am].buffer[LEFT][ spu_extract(*left_tail_idx,0) ];
  right = (vector signed int*) &md[am].buffer[RIGHT][ spu_extract(*right_tail_idx,0) ];
  out = (vector signed int*) &md[am].buffer[OUT][ spu_extract(*out_head_idx,0) ];

  #ifdef TRACE_TIME
    dec_val2 = spu_read_decrementer();
  #endif

  while(spu_extract(avail,0) == 0x0F){
    // cmp left and right to determine who gets eaten
    cmp_v = spu_cmpgt(*left,*right);
    cmp_v = spu_shuffle(cmp_v, cmp_v, cmp_v_shuffle_mask);
    // cmp_v = {FFFF,FFFF,FFFF,FFFF} if left[3] > right[3]

    *out = spu_sel(*left,*right,cmp_v);
    rev_mask = spu_sel(rev_right,rev_left,(vector unsigned char)cmp_v);
    *left = spu_shuffle(*left,*right,rev_mask);
    // data to be sorted is now in out and left, left in descending order

    sort_vectors(out,left);

    // update index of the used side
    if( spu_extract(cmp_v,0) ){
      // left[3] > right[3]
      *right_tail_idx = spu_add(*right_tail_idx,ones);
      avail_v = spu_sub(avail_v, one_at_1);
      right++;
      // modulus hack
      cmp_v2 = spu_cmpeq(*right_tail_idx, size_v);
      if( __builtin_expect( spu_extract(cmp_v2,0) ,0) ){
	*right_tail_idx = zeros;
	right = (vector signed int*) &md[am].buffer[RIGHT][0];
      }
    } else {
      *right = *left;
      *left_tail_idx = spu_add(*left_tail_idx,ones);
      avail_v = spu_sub(avail_v, one_at_0);
      left++;
      // modulus hack
      cmp_v2 = spu_cmpeq(*left_tail_idx, size_v);      
      if( __builtin_expect( spu_extract(cmp_v2,0) ,0) ){	
	*left_tail_idx = zeros;
	left = (vector signed int*) &md[am].buffer[LEFT][0];
      }
    }

    // update out head idx
    *out_head_idx = spu_add(*out_head_idx,ones);
    avail_v = spu_sub(avail_v, one_at_2);
    out++;
    // modulus hack
    cmp_v2 = spu_cmpeq(*out_head_idx, size_v);
    if( __builtin_expect(spu_extract(cmp_v2,0),0) ){
      out = (vector signed int*) &md[am].buffer[OUT][0];
      *out_head_idx = zeros;
    }

    // is there data still available?
    avail = spu_gather(spu_cmpgt(avail_v, zeros));
  }

  #ifdef TRACE_TIME
  merge_loop_ticks += -(spu_read_decrementer() - dec_val2);
  #endif

  // how much got produced?
  vector signed int consumed = spu_sub(avail_before, avail_v);
  int consumed_left = spu_extract(consumed, 0);
  int consumed_right = spu_extract(consumed, 1);

  if(consumed_left)
    update_tail(LEFT);

  if(consumed_right)
    update_tail(RIGHT);

  md[am].consumed[LEFT] += consumed_left;
  md[am].consumed[RIGHT] += consumed_right;
    
  if(md[am].consumed[LEFT] == mcb[am].data_size[LEFT])
    md[am].depleted[LEFT] = 1;
  
  if(md[am].consumed[RIGHT] == mcb[am].data_size[RIGHT])
    md[am].depleted[RIGHT] = 1;

  if(mcb[am].local[OUT] < 255 && md[am].depleted[LEFT] && md[am].depleted[RIGHT]){
    md[am].done = 1;
    --num_active_mergers;
  }
}

int num_in_buffer(int side){

  volatile vector signed int *head_idx, *tail_idx;
  int buffer_size;

  if(side == OUT && mcb[am].local[OUT] < 255){
    int parent_idx = mcb[am].local[OUT];
    int side = (mcb[am].id+1)&1;
    head_idx = &md[parent_idx].idx[side][HEAD];
    tail_idx = &md[parent_idx].idx[side][TAIL];
    buffer_size = mcb[parent_idx].buffer_size[side];
  } else {
    head_idx = &md[am].idx[side][HEAD];
    tail_idx = &md[am].idx[side][TAIL];
    buffer_size = mcb[am].buffer_size[side];
  }

  vector signed int diff = spu_sub(*head_idx,*tail_idx);
  int num = spu_extract(diff,0);

  if(num < 0)
    num = num + buffer_size;

  return num;
}

int num_free_in_buffer(int side){
  int taken = num_in_buffer(side);
  int buffer_size;

  if(side == OUT && mcb[am].local[OUT] < 255){
    int side = (mcb[am].id+1)&1;
    int parent_idx = mcb[am].local[OUT];
    buffer_size = mcb[parent_idx].buffer_size[side];
  } else
    buffer_size = mcb[am].buffer_size[side];

  return  buffer_size - taken - 1;
}

void check_pull_dma(int side){
  // Check left
  if(md[am].held_tag[side] < 32){
    mfc_write_tag_mask( 1 << md[am].held_tag[side] );
    int status = mfc_read_tag_status_immediate();

    if(status){
      // Update idx
      md[am].idx[side][HEAD] = spu_add(md[am].idx[side][HEAD], md[am].num_waiting[side]);

      vector signed int buffer_size = spu_splats(mcb[am].buffer_size[side] -1);
      vector unsigned int cmp_v = spu_cmpgt(md[am].idx[side][HEAD], buffer_size);
      vector signed int zeros = {0,0,0,0};
      buffer_size = spu_add(buffer_size,1);
      zeros = spu_sel(zeros,buffer_size,cmp_v);
      md[am].idx[side][HEAD] = spu_sub(md[am].idx[side][HEAD],zeros);

      md[am].num_pulled[side] += md[am].num_waiting[side];      
      md[am].num_waiting[side] = 0;
      if(md[am].num_pulled[side] == mcb[am].data_size[side]){
	md[am].mm_depleted[side] = 1;
      }
      // Release tag
      mfc_tag_release( md[am].held_tag[side] );
      md[am].held_tag[side] = 32;      
    }
  }
}

void check_push_dma(){
  if(md[am].held_tag[OUT] < 32){

    mfc_write_tag_mask( 1 << md[am].held_tag[OUT] );
    int status = mfc_read_tag_status_immediate();

    if(status){
      // Release tag
      mfc_tag_release( md[am].held_tag[OUT] );
      md[am].held_tag[OUT] = 32;

      if(md[am].consumed[LEFT] == mcb[am].data_size[LEFT] && md[am].consumed[RIGHT] ==  mcb[am].data_size[RIGHT]){
	if(num_in_buffer(OUT) == 0){
	  md[am].done = 1;
	  --num_active_mergers;
	}
      }
    }
  }
}

void update_tail(int side){
  if(mcb[am].leaf_node || mcb[am].local[side] < 255)
    return;

  mfc_putf(&md[am].idx[side][TAIL],
	   mcb[am].idx_addr[side],
	   sizeof(vector signed int),
	   ctrl_dma_tag,
	   0,0);
}

void push(){
  int avail_out = num_in_buffer(OUT);
  if(!avail_out)
    return;  

  int avail_parent = num_free_in_buffer(PARENT);
  if(mcb[am].id == 0)
    avail_parent = mcb[am].data_size[LEFT] + mcb[am].data_size[RIGHT];

  int num_send = avail_out < avail_parent ? avail_out : avail_parent;
  num_send = num_send < MAX_DMA_SIZE ? num_send : MAX_DMA_SIZE;
  if(!num_send)
    return;

  int tag = mfc_tag_reserve();
  if(tag == MFC_TAG_INVALID){
    return;
  } else
    md[am].held_tag[OUT] = tag;

  // send num_send vectors, in up to three DMA-put's
  while(num_send > 0){
    int parent_head = spu_extract(md[am].idx[PARENT][HEAD],0);
    int free_from_head = mcb[am].buffer_size[PARENT] - parent_head;

    int tail = spu_extract(md[am].idx[OUT][TAIL],0);
    int avail_from_tail = mcb[am].buffer_size[OUT] - tail;

    int part_send = num_send < free_from_head ? num_send : free_from_head;
    part_send = part_send < avail_from_tail ? part_send : avail_from_tail;

    unsigned int to = mcb[am].block_addr[OUT] + parent_head*sizeof(vector signed int);

    mfc_put(&md[am].buffer[OUT][tail],
	    to,	    
	    part_send * sizeof(vector signed int),
	    md[am].held_tag[OUT],
	    0,0);
    
    md[am].idx[PARENT][HEAD] = spu_add(md[am].idx[PARENT][HEAD], part_send);
    parent_head = spu_extract(md[am].idx[PARENT][HEAD],0);

    if(parent_head == mcb[am].buffer_size[PARENT])
      md[am].idx[PARENT][HEAD] = spu_splats(0);

    md[am].idx[OUT][TAIL] = spu_add(md[am].idx[OUT][TAIL], part_send);
    tail = spu_extract(md[am].idx[OUT][TAIL],0);

    if(tail == mcb[am].buffer_size[OUT])
      md[am].idx[OUT][TAIL] = spu_splats(0);

    num_send -= part_send;
  }

  // Inner nodes updates parent in buffer head idx
  if(mcb[am].id)
    mfc_putf(&md[am].idx[PARENT][HEAD],
	     mcb[am].idx_addr[OUT],	     
	     sizeof(vector signed int),
	     md[am].held_tag[OUT],
	     0,0);
}

void pull(int side){
  int avail_in = num_free_in_buffer(side);
  int avail_mm = mcb[am].data_size[side] - md[am].num_pulled[side];
  int num_pull = avail_in < avail_mm ? avail_in : avail_mm;
  num_pull = num_pull < MAX_DMA_SIZE ? num_pull : MAX_DMA_SIZE;
  int head = spu_extract(md[am].idx[side][HEAD],0);
  int avail_from_head = mcb[am].buffer_size[side] - head;
  int first_pull = num_pull < avail_from_head ? num_pull : avail_from_head;
        
  if(!first_pull)
    return;

  // pull #first_pull
  unsigned int to_ea = (unsigned int) &md[am].buffer[side][head];
  int tag = mfc_tag_reserve();
  if(tag == MFC_TAG_INVALID){
    return;
  } else {
    md[am].held_tag[side] = tag;
  }

  mfc_get((void*)to_ea,
	  mcb[am].block_addr[side],
	  first_pull * sizeof(vector signed int),
	  md[am].held_tag[side],
	  0,0);
  mcb[am].block_addr[side] += first_pull * sizeof(vector signed int);

  if(first_pull < num_pull){
    to_ea = (unsigned int) &md[am].buffer[side][0];
    int second_pull = num_pull - first_pull;

    mfc_get((void*)to_ea,
	    mcb[am].block_addr[side],
	    second_pull * sizeof(vector signed int),
	    md[am].held_tag[side],
	    0,0);
    mcb[am].block_addr[side] += second_pull * sizeof(vector signed int);
  }

  md[am].num_waiting[side] = num_pull;
}

void setup_spu(unsigned int spu_ctrlblock_addr){
  ctrl_dma_tag = mfc_tag_reserve();

  // Get SPU control block
  mfc_get(&spu_ctrlblock,
	  spu_ctrlblock_addr,
	  sizeof(spu_ctrlblock),
	  ctrl_dma_tag,
	  0,0);

  mfc_write_tag_mask(1<<ctrl_dma_tag);
  mfc_read_tag_status_all();

  mcb = (merger_ctrlblock_t*)memalign(128,spu_ctrlblock.num_mergers * sizeof(merger_ctrlblock_t) );
  md = (merger_data_t*)malloc(spu_ctrlblock.num_mergers * sizeof(merger_data_t));

  // Set addresses
  int i;
  for(i = 0; i < spu_ctrlblock.num_mergers; i++){
    // Set head/tail vector addresses
    mcb[i].idx_addr[LEFT] = (unsigned int) &md[i].idx[LEFT][HEAD];
    mcb[i].idx_addr[RIGHT] = (unsigned int) &md[i].idx[RIGHT][HEAD];
    mcb[i].idx_addr[OUT] = (unsigned int) &md[i].idx[PARENT][TAIL];
  }

  // Send merger control blocks
  mfc_put(mcb,
	  spu_ctrlblock.ctrlblocks_addr,
	  spu_ctrlblock.num_mergers * sizeof(merger_ctrlblock_t),
	  ctrl_dma_tag,
	  0,0);

  mfc_read_tag_status_all();

  // Mail PPU telling it we've set the addresses
  spu_write_out_mbox(1);

  // Wait for go-ahead mail
  spu_read_in_mbox();

  // Get merger blocks
  mfc_get(mcb,
	  spu_ctrlblock.ctrlblocks_addr,
	  spu_ctrlblock.num_mergers * sizeof(merger_ctrlblock_t),
	  ctrl_dma_tag,
	  0,0);

  mfc_read_tag_status_all();

  int buffer_idx = 0;
  for(i = 0; i < spu_ctrlblock.num_mergers; i++){
    // Add start address of buffer array to all block addresses
    if(mcb[i].id != 0)
      mcb[i].block_addr[OUT] += (unsigned int) &buffer[0];

    if(!mcb[i].leaf_node){
      mcb[i].block_addr[LEFT] += (unsigned int) &buffer[0];
      mcb[i].block_addr[RIGHT] += (unsigned int) &buffer[0];
    }

    // Setup merger data
    md[i].held_tag[LEFT] = 32;
    md[i].held_tag[RIGHT] = 32;
    md[i].held_tag[OUT] = 32;
    md[i].num_pulled[LEFT] = 0;
    md[i].num_pulled[RIGHT] = 0;
    md[i].mm_depleted[LEFT] = 0;
    md[i].mm_depleted[RIGHT] = 0;
    md[i].depleted[LEFT] = 0;
    md[i].depleted[RIGHT] = 0;
    md[i].done = 0;
    md[i].consumed[LEFT] = 0;
    md[i].consumed[RIGHT] = 0;

    md[i].idx[LEFT][HEAD] = spu_splats(0);
    md[i].idx[LEFT][TAIL] = spu_splats(0);
    md[i].idx[RIGHT][HEAD] = spu_splats(0);
    md[i].idx[RIGHT][TAIL] = spu_splats(0);
    md[i].idx[OUT][HEAD] = spu_splats(0);
    md[i].idx[OUT][TAIL] = spu_splats(0);
    md[i].idx[PARENT][HEAD] = spu_splats(0);
    md[i].idx[PARENT][TAIL] = spu_splats(0);

    md[i].buffer[LEFT] = &buffer[buffer_idx];
    buffer_idx += mcb[i].buffer_size[LEFT];
    md[i].buffer[RIGHT] = &buffer[buffer_idx];
    buffer_idx += mcb[i].buffer_size[RIGHT];
    md[i].buffer[OUT] = &buffer[buffer_idx];
    buffer_idx += mcb[i].buffer_size[OUT];
  }

  // Setup internal nodes
  for(i = 0; i < spu_ctrlblock.num_mergers; i++){
    if(mcb[i].local[OUT] < 255){
      int parent_idx = mcb[i].local[OUT];
      int side = (mcb[i].id+1)&1;
      md[i].buffer[OUT] = md[parent_idx].buffer[side];
      mcb[i].buffer_size[OUT] = mcb[parent_idx].buffer_size[side];
    }
  }
}
