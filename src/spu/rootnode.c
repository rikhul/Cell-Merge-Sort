#include <stdio.h>
#include <spu_mfcio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <spu_intrinsics.h>

#include "ctrlblock.h"
#include "mergernode.h"
#include "bitonic_merge.h"
#include <malloc.h>
#include <assert.h>

#define CTRL_DMA_TAG 31 // for all DMA we don't wait for
#define BUFFER_SIZE_BYTES BUFFER_SIZE*4

// Please note that these must add up nicely
#define NUM_OUT_BUFFERS 2

// Pointers to start of arrays
volatile spu_ctrlblock_t *spu_ctrlblock_ptr;
volatile buffer_map_t *bm;

// Running mergers merger_data and ctrlblock
volatile merger_data_t *md;
volatile merger_ctrlblock_t *mcb;

const vector signed int one = {1,1,1,1};
const vector signed int zero_v = {0,0,0,0};

volatile vector signed int *out_buffer[NUM_OUT_BUFFERS];
int out_tag[NUM_OUT_BUFFERS];

void print_time_info();
void wait_on_child(int side, int count);
void wait_on_prev_out(int outcnt);
void push();
void notify_child(int side);
void setup_spu(unsigned int spu_ctrlblock_addr);
void get_next_buffer(int side, volatile vector signed int **buffer, int *idx);
unsigned int notify_dec_val = 0;
unsigned int notify_time = 0;
unsigned int notify_stall = 0;

#ifdef TRACE_TIME
unsigned int num_wait_dma = 0;
unsigned int dec_val;
unsigned int dec_val2;
unsigned int wait_dma = 0;
unsigned int wait_children = 0;
unsigned int merge_t = 0;
unsigned int sort_v_time = 0;
unsigned int rev_time = 0;
unsigned int cp_time = 0;
unsigned int push_time = 0;
int merged_cnt = 0;
#endif


int main(unsigned long long spe_id,
	 unsigned long long argp,
	 unsigned long long envp)
{
    // Get SPU control block
  volatile spu_ctrlblock_t spu_ctrlblock aligned_128;
  spu_ctrlblock_ptr = &spu_ctrlblock;

  mfc_get(&spu_ctrlblock,
	  argp,
	  sizeof(spu_ctrlblock),
	  CTRL_DMA_TAG,
	  0,0);

  mfc_write_tag_mask(1<<CTRL_DMA_TAG);
  mfc_read_tag_status_all();

  // Get merger control blocks
  volatile merger_ctrlblock_t ctrlblock[spu_ctrlblock.num_mergers] aligned_128;
  merger_data_t merger_data[spu_ctrlblock.num_mergers];

  mfc_get(ctrlblock,
	  spu_ctrlblock.merger_ctrlblocks_addr,
	  sizeof(ctrlblock),
	  CTRL_DMA_TAG,
	  0,0);
  
  mfc_read_tag_status_all();

  // Create buffer array
  volatile vector signed int buffers[(4 + NUM_OUT_BUFFERS)*VEC_BUFFER_SIZE];

  // Maps every mergers left and right buffer addresses
  volatile buffer_map_t buffer_map[spu_ctrlblock.num_mergers_global] aligned_128;
  bm = buffer_map;

  // Setup merger data buffers
  int i;

  merger_data[0].buffers[LEFT][LOW] = &buffers[0];
  merger_data[0].buffers[LEFT][HIGH] = &buffers[VEC_BUFFER_SIZE];
  merger_data[0].buffers[RIGHT][LOW] = &buffers[2*VEC_BUFFER_SIZE];
  merger_data[0].buffers[RIGHT][HIGH] = &buffers[3*VEC_BUFFER_SIZE];

  for(i=0; i < NUM_OUT_BUFFERS; i++){
    out_buffer[i] = &buffers[(4+i)*VEC_BUFFER_SIZE];
    out_tag[i] = i+5;
  }

  // Sync variables
  merger_data[0].left_child_done_pushing = zero_v;
  merger_data[0].right_child_done_pushing = zero_v;

  // Add to buffer_map
  // assignments might seem mighty strange, but the addresses are swapped
  // by PPE before distributed
  buffer_map[0].merger_id = ctrlblock[0].id;
  buffer_map[0].buffer_addr[LEFT] = 
    (unsigned int) merger_data[0].buffers[LEFT][LOW];
  buffer_map[0].buffer_addr[RIGHT] = 
    (unsigned int) merger_data[0].buffers[RIGHT][LOW];

  buffer_map[0].child_addr[LEFT] = 
    (unsigned int) &merger_data[0].left_child_done_pushing;
  buffer_map[0].child_addr[RIGHT] = 
    (unsigned int) &merger_data[0].right_child_done_pushing;
  buffer_map[0].done_pushing_out_addr = 
    (unsigned int) &merger_data[0].parent_ready;

  // Index into buffers
  merger_data[0].buffer_idx[LEFT] = 0;
  merger_data[0].buffer_idx[RIGHT] = 0;
  merger_data[0].buffer_idx_v[LEFT] = zero_v;
  merger_data[0].buffer_idx_v[RIGHT] = zero_v;
  merger_data[0].idx[LEFT] = 0;
  merger_data[0].idx[RIGHT] = 0;

  ctrlblock[0].tag_mask = 0;
  merger_data[0].out_cnt = 0;

  // Send this SPUs part of buffer_map to PPU
  // then wait for completion and get the entire map
  mfc_put(buffer_map,
	  spu_ctrlblock.buffer_map_addr,
	  spu_ctrlblock.num_mergers * sizeof(buffer_map_t),
	  CTRL_DMA_TAG,
	  0,0);

  mfc_write_tag_mask(1<<CTRL_DMA_TAG);
  mfc_read_tag_status_all();

  // send mail telling the PPU we have filled our part of
  // the buffer_map
  spu_write_out_mbox(1);

  // wait for answer
  spu_ctrlblock.buffer_map_addr = spu_read_in_mbox();

  mfc_get(buffer_map,
	  spu_ctrlblock.buffer_map_addr,
	  sizeof(buffer_map),
	  CTRL_DMA_TAG,
	  0,0);

  mfc_write_tag_mask(1<<CTRL_DMA_TAG);
  mfc_read_tag_status_all();

  md = &merger_data[0];
  mcb = &ctrlblock[0];


  #ifdef TRACE_TIME
  spu_write_decrementer(0);
  #endif

  // Merge
  int idx[3] = {0,0,0};
  volatile vector signed int *buffer[3];

  #ifdef TRACE_TIME
  dec_val = spu_read_decrementer();
  #endif

  // Wait for the first buffers
  volatile int left_cnt, right_cnt;
  do {
    left_cnt = spu_extract(md->left_child_done_pushing,0);
    right_cnt = spu_extract(md->right_child_done_pushing,0);
  } while( !(left_cnt && right_cnt) );

  #ifdef TRACE_TIME
  wait_children += -(spu_read_decrementer() - dec_val);
  #endif

  buffer[LEFT] = md->buffers[LEFT][LOW];
  buffer[RIGHT] = md->buffers[RIGHT][LOW];
  
  // Produce outbuffers
  //int i;
  for(i=0; i < 2*mcb->num_buffers; i++){
    // Wait on previus outbuffer
    #ifdef TRACE_TIME
    dec_val = spu_read_decrementer();
    #endif
    wait_on_prev_out(i);
    #ifdef TRACE_TIME
    wait_dma += -(spu_read_decrementer() - dec_val);
    #endif
    buffer[OUT] = out_buffer[i & 1];
    idx[OUT] = 0;

    // Merge together one out buffer
    while(idx[OUT] < VEC_BUFFER_SIZE){
      #ifdef TRACE_TIME
      dec_val = spu_read_decrementer();
      #endif
      if(md->buffer_idx[LEFT] == mcb->num_buffers && idx[LEFT] == 0){
	buffer[OUT][idx[OUT]++] = buffer[RIGHT][idx[RIGHT]++];
      } else if(md->buffer_idx[RIGHT] == mcb->num_buffers && idx[RIGHT] == 0){
	buffer[OUT][idx[OUT]++] = buffer[LEFT][idx[LEFT]++];
      } else {
	#ifdef TRACE_TIME
	dec_val2 = spu_read_decrementer();
	#endif
	merge_buffers_v(buffer, idx);
	#ifdef TRACE_TIME
	sort_v_time += -(spu_read_decrementer() - dec_val2);
	#endif
      }

      #ifdef TRACE_TIME
      merge_t += -(spu_read_decrementer() - dec_val);
      dec_val = spu_read_decrementer();
      #endif
      // Did we empty any buffers?
      if(idx[LEFT] == VEC_BUFFER_SIZE && md->buffer_idx[LEFT] < mcb->num_buffers){
	get_next_buffer(LEFT, buffer, idx);
      }
			
      if(idx[RIGHT] == VEC_BUFFER_SIZE && md->buffer_idx[RIGHT] < mcb->num_buffers){
	get_next_buffer(RIGHT, buffer, idx);
      }
      #ifdef TRACE_TIME
      wait_children += -(spu_read_decrementer() - dec_val);
      #endif
    }

    push(i);
  }

  #ifdef TRACE_TIME
  dec_val = spu_read_decrementer();
  #endif
  wait_on_prev_out(i);
  wait_on_prev_out(i+1);
  #ifdef TRACE_TIME
  wait_dma += -(spu_read_decrementer() - dec_val);
  #endif

  #ifdef TRACE_TIME
  print_time_info();
  #endif

  return 0;
}

void get_next_buffer(int side, volatile vector signed int **buffer, int *idx){
  ++md->buffer_idx[side];
  md->buffer_idx_v[side] = spu_add(md->buffer_idx_v[side], 1);
  buffer[side] = md->buffers[side][ md->buffer_idx[side]&1 ];
  idx[side] = 0;
  
  notify_child(side);  

  if(md->buffer_idx[side] < mcb->num_buffers)
    wait_on_child(side, md->buffer_idx[side]);
}

void wait_on_prev_out(int outcnt){
  #ifdef TRACE_PRINTS
  printf("Wait on ");
  if(outcnt&1)
    printf("high\n");
  else
    printf("low\n");
  #endif

  mfc_write_tag_mask( 1 << out_tag[outcnt&1] );
  mfc_read_tag_status_all();
}

void setup_spu(unsigned int spu_ctrlblock_addr){
  // Get SPU control block
  volatile spu_ctrlblock_t spu_ctrlblock aligned_128;
  spu_ctrlblock_ptr = &spu_ctrlblock;

  mfc_get(&spu_ctrlblock,
	  spu_ctrlblock_addr,
	  sizeof(spu_ctrlblock),
	  CTRL_DMA_TAG,
	  0,0);

  mfc_write_tag_mask(1<<CTRL_DMA_TAG);
  mfc_read_tag_status_all();

  // Get merger control blocks
  volatile merger_ctrlblock_t ctrlblock[spu_ctrlblock.num_mergers] aligned_128;
  merger_data_t merger_data[spu_ctrlblock.num_mergers];

  mfc_get(ctrlblock,
	  spu_ctrlblock.merger_ctrlblocks_addr,
	  sizeof(ctrlblock),
	  CTRL_DMA_TAG,
	  0,0);
  
  mfc_read_tag_status_all();

  // Create buffer array
  volatile vector signed int buffers[(4 + NUM_OUT_BUFFERS)*VEC_BUFFER_SIZE];

  // Maps every mergers left and right buffer addresses
  volatile buffer_map_t buffer_map[spu_ctrlblock.num_mergers_global] aligned_128;
  bm = buffer_map;

  // Setup merger data buffers
  int i;

  merger_data[0].buffers[LEFT][LOW] = &buffers[0];
  merger_data[0].buffers[LEFT][HIGH] = &buffers[VEC_BUFFER_SIZE];
  merger_data[0].buffers[RIGHT][LOW] = &buffers[2*VEC_BUFFER_SIZE];
  merger_data[0].buffers[RIGHT][HIGH] = &buffers[3*VEC_BUFFER_SIZE];

  for(i=0; i < NUM_OUT_BUFFERS; i++){
    out_buffer[i] = &buffers[(4+i)*VEC_BUFFER_SIZE];
    out_tag[i] = i+5;
  }

  // Sync variables
  merger_data[0].left_child_done_pushing = zero_v;
  merger_data[0].right_child_done_pushing = zero_v;

  // Add to buffer_map
  // assignments might seem mighty strange, but the addresses are swapped
  // by PPE before distributed
  buffer_map[0].merger_id = ctrlblock[0].id;
  buffer_map[0].buffer_addr[LEFT] = 
    (unsigned int) merger_data[0].buffers[LEFT][LOW];
  buffer_map[0].buffer_addr[RIGHT] = 
    (unsigned int) merger_data[0].buffers[RIGHT][LOW];

  buffer_map[0].child_addr[LEFT] = 
    (unsigned int) &merger_data[0].left_child_done_pushing;
  buffer_map[0].child_addr[RIGHT] = 
    (unsigned int) &merger_data[0].right_child_done_pushing;
  buffer_map[0].done_pushing_out_addr = 
    (unsigned int) &merger_data[0].parent_ready;

  // Index into buffers
  merger_data[0].buffer_idx[LEFT] = 0;
  merger_data[0].buffer_idx[RIGHT] = 0;
  merger_data[0].buffer_idx_v[LEFT] = one;
  merger_data[0].buffer_idx_v[RIGHT] = one;
  merger_data[0].idx[LEFT] = 0;
  merger_data[0].idx[RIGHT] = 0;

  ctrlblock[0].tag_mask = 0;
  merger_data[0].out_cnt = 0;

  // Send this SPUs part of buffer_map to PPU
  // then wait for completion and get the entire map
  mfc_put(buffer_map,
	  spu_ctrlblock.buffer_map_addr,
	  spu_ctrlblock.num_mergers * sizeof(buffer_map_t),
	  CTRL_DMA_TAG,
	  0,0);

  mfc_write_tag_mask(1<<CTRL_DMA_TAG);
  mfc_read_tag_status_all();

  // send mail telling the PPU we have filled our part of
  // the buffer_map
  spu_write_out_mbox(1);

  // wait for answer
  spu_ctrlblock.buffer_map_addr = spu_read_in_mbox();

  mfc_get(buffer_map,
	  spu_ctrlblock.buffer_map_addr,
	  sizeof(buffer_map),
	  CTRL_DMA_TAG,
	  0,0);

  mfc_write_tag_mask(1<<CTRL_DMA_TAG);
  mfc_read_tag_status_all();

  md = &merger_data[0];
  mcb = &ctrlblock[0];

  #ifdef SPU_PRINT_INFO
  printf("SPU%d: begin\n", spu_ctrlblock.spu_id);
  #endif
}

#ifdef TRACE_TIME
void print_time_info(){
  float wait_dma_ms = wait_dma / (79.8*1000);
  float wait_children_ms = wait_children / (79.8*1000);
  float sort_v_ms = sort_v_time / (79.8*1000);
  float merge_ms = merge_t / (79.8*1000);
  

  printf("SPU%d: merge %fms (%fms), Wait for DMA %fms, Wait on children %fms\n", 
	 spu_ctrlblock_ptr->spu_id, 
	 merge_ms,
	 sort_v_ms,
	 wait_dma_ms,
	 wait_children_ms);

}
#endif

void wait_on_child(int side, int cnt){
  
  #ifdef TRACE_PRINTS
  printf("SPU0: 0, waiting on child %d to become greater than %d\n", side, cnt);
  #endif
  

  volatile vector unsigned int t_gt;

  if(side){
    do {
      t_gt = spu_cmpgt(md->right_child_done_pushing, cnt);
    } while( !spu_extract(t_gt,3) );
  } else {
    do {
      t_gt = spu_cmpgt(md->left_child_done_pushing, cnt);
    } while( !spu_extract(t_gt,3) );
  }
}

void push(int outcnt){
  #ifdef TRACE_PRINTS
  printf("SPU%d: %d push %d", spu_ctrlblock_ptr->spu_id, mcb->id, outcnt+1);

  if(outcnt&1)
    printf(" high\n");
  else
    printf(" low\n");
  #endif

  mfc_put(out_buffer[outcnt&1],
	  mcb->out_addr,
	  BUFFER_SIZE_BYTES,
	  out_tag[outcnt&1],
	  0,0);

  mcb->out_addr += BUFFER_SIZE_BYTES;
}

void notify_child(int side){
  #ifdef TRACE_PRINTS
  printf("SPU0: 0, notify child %d %d\n", side+1, md->buffer_idx[side]);
  #endif

  #ifdef TRACE_TIME
  notify_dec_val = spu_read_decrementer();
  #endif
  
  unsigned int addr = bm[ mcb->id ].child_addr[side];

  #ifdef TRACE_TIME
  if(!mfc_stat_cmd_queue())
    ++notify_stall;
  #endif

  // putf b/c we don't want a lower value to arrive after a higher value
  mfc_putf((void*) &md->buffer_idx_v[side],
	   addr,
	   sizeof(vector signed int),
	   CTRL_DMA_TAG,
	   0,0);
  
  #ifdef TRACE_TIME
  notify_time += -(spu_read_decrementer() - notify_dec_val);
  #endif
}
