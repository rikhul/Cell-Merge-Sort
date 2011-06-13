#include <spu_intrinsics.h>

#include "sort_vectors.h"
#include "ctrlblock.h"
#include "mergernode.h"



// TODO: we could use the temp more effectivly
inline void sort_vectors(vector signed int *l, vector signed int *r){
  vector unsigned int mask;
  vector signed int temp;

  // 2-8 shuffle index
  const vector unsigned char si_2_8_l = {0,1,2,3,4,
					 5,6,7,16,
					 17,18,19,
					 20,21,22,23};

  const vector unsigned char si_2_8_r = {8,9,10,11,
					 12,13,14,15,
					 24,25,26,27,
					 28,29,30,31};
 
  // 1-8 shuffle index
  const vector unsigned char si_1_8_l = {0,1,2,3,
					 8,9,10,11,
					 16,17,18,19, 
					 24,25,26,27};

  const vector unsigned char si_1_8_r = {4,5,6,7,
					 12,13,14,15,
					 20,21,22,23,
					 28,29,30,31};

  // 1-8 shuffle back index
  const vector unsigned char si_1_8_l_rev = {0,1,2,3,
					     16,17,18,19, 
					     4,5,6,7,
					     20,21,22,23};

  const vector unsigned char si_1_8_r_rev = {8,9,10,11,
					     24,25,26,27,
					     12,13,14,15,
					     28,29,30,31};

  vector signed int left = *l;
  vector signed int right = *r;

  // "4-8 swap"
  // reverse right (howabout rotera istället för shuffle?)
  //  right = spu_shuffle(left,right,rev_right);
  // cmp & swap
  mask = spu_cmpgt(left, right);
  temp = spu_sel(left,right,mask);  
  right = spu_sel(right,left,mask);
  left = temp;
  
  // "2-8 swap"
  // shuffle
  temp = spu_shuffle(left,right,si_2_8_l);
  right = spu_shuffle(left,right,si_2_8_r);
  left = temp;
  // cmp & swap
  mask = spu_cmpgt(left,right);
  temp = spu_sel(left,right,mask);
  right = spu_sel(right,left,mask);
  left = temp;
  // shuffle back
  temp = spu_shuffle(left,right,si_2_8_l);
  right = spu_shuffle(left,right,si_2_8_r);
  left = temp;
  
  // "1-8 swap"
  temp = spu_shuffle(left,right,si_1_8_l);
  right = spu_shuffle(left,right,si_1_8_r);
  left = temp;
  // cmp & swap
  mask = spu_cmpgt(left,right);
  temp = spu_sel(left,right,mask);
  right = spu_sel(right,left,mask);
  left = temp;
  // shuffle back
  temp = spu_shuffle(left,right,si_1_8_l_rev);
  right = spu_shuffle(left,right,si_1_8_r_rev);
  left = temp;

  *l = left;
  *r = right;
}
