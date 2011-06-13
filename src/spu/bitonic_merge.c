#include <string.h>
#include "bitonic_merge.h"
#include "sort_vectors.h"

int K = 0;

volatile int *left;
volatile int *right;
volatile int *data;
volatile int *data2;

void btn_core_l8_1(int N, int k) {
  register vector unsigned char MASK1 = {0x00, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13, 
                                         0x08, 0x09, 0x0A, 0x0B, 0x18, 0x19, 0x1A, 0x1B};
  register vector unsigned char MASK2 = {0x04, 0x05, 0x06, 0x07, 0x14, 0x15, 0x16, 0x17, 
                                         0x0C, 0x0D, 0x0E, 0x0F, 0x1C, 0x1D, 0x1E, 0x1F};

  register int i;
  register int i4;
  register int i8;
  register int i12;

  register vector SORT_TYPE v1;
  register vector SORT_TYPE v2;
  register vector SORT_TYPE v3;
  register vector SORT_TYPE v4;
  register vector SORT_TYPE v5;
  register vector SORT_TYPE v6;
  register vector SORT_TYPE v7;
  register vector SORT_TYPE v8;

  register vector unsigned int t1; 
  register vector unsigned int t2; 

  register int u;
  register int v;
 
  if(!K) { 
    for(u=0; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;

        v1 = *(vector SORT_TYPE *) &data[i];
        v2 = *(vector SORT_TYPE *) &data[i4];
        v5 = *(vector SORT_TYPE *) &data[i8];
        v6 = *(vector SORT_TYPE *) &data[i12];
        v3 = spu_shuffle(v1, v2, MASK1);
        v4 = spu_shuffle(v1, v2, MASK2);
        v7 = spu_shuffle(v5, v6, MASK1);
        v8 = spu_shuffle(v5, v6, MASK2);
        t1 = spu_cmpgt(v3, v4);
        t2 = spu_cmpgt(v7, v8);
        v1 = spu_sel(v3, v4, t1);
        v2 = spu_sel(v4, v3, t1);
        v5 = spu_sel(v7, v8, t2);
        v6 = spu_sel(v8, v7, t2);
        *(vector SORT_TYPE *) &data[i]  = spu_shuffle(v1, v2, MASK1);
        *(vector SORT_TYPE *) &data[i4] = spu_shuffle(v1, v2, MASK2);
        *(vector SORT_TYPE *) &data[i8]  = spu_shuffle(v5, v6, MASK1);
        *(vector SORT_TYPE *) &data[i12] = spu_shuffle(v5, v6, MASK2);
      }
    }
    for(u=k; u<N; u+=2*k) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;

        v1 = *(vector SORT_TYPE *) &data[i];
        v2 = *(vector SORT_TYPE *) &data[i4];
        v5 = *(vector SORT_TYPE *) &data[i8];
        v6 = *(vector SORT_TYPE *) &data[i12];
        v3 = spu_shuffle(v1, v2, MASK1);
        v4 = spu_shuffle(v1, v2, MASK2);
        v7 = spu_shuffle(v5, v6, MASK1);
        v8 = spu_shuffle(v5, v6, MASK2);
        t1 = spu_cmpgt(v3, v4);
        t2 = spu_cmpgt(v7, v8);
        v1 = spu_sel(v4, v3, t1);
        v2 = spu_sel(v3, v4, t1);
        v5 = spu_sel(v8, v7, t2);
        v6 = spu_sel(v7, v8, t2);
        *(vector SORT_TYPE *) &data[i]  = spu_shuffle(v1, v2, MASK1);
        *(vector SORT_TYPE *) &data[i4] = spu_shuffle(v1, v2, MASK2);
        *(vector SORT_TYPE *) &data[i8]  = spu_shuffle(v5, v6, MASK1);
        *(vector SORT_TYPE *) &data[i12] = spu_shuffle(v5, v6, MASK2);
      }
    }
  } else {
    for(u=0; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;

        v1 = *(vector SORT_TYPE *) &data[i];
        v2 = *(vector SORT_TYPE *) &data[i4];
        v5 = *(vector SORT_TYPE *) &data[i8];
        v6 = *(vector SORT_TYPE *) &data[i12];
        v3 = spu_shuffle(v1, v2, MASK1);
        v4 = spu_shuffle(v1, v2, MASK2);
        v7 = spu_shuffle(v5, v6, MASK1);
        v8 = spu_shuffle(v5, v6, MASK2);
        t1  = spu_cmpgt(v3, v4);
        t2  = spu_cmpgt(v7, v8);
        v1 = spu_sel(v4, v3, t1);
        v2 = spu_sel(v3, v4, t1);
        v5 = spu_sel(v8, v7, t2);
        v6 = spu_sel(v7, v8, t2);
        *(vector SORT_TYPE *) &data[i]  = spu_shuffle(v1, v2, MASK1);
        *(vector SORT_TYPE *) &data[i4] = spu_shuffle(v1, v2, MASK2);
        *(vector SORT_TYPE *) &data[i8]  = spu_shuffle(v5, v6, MASK1);
        *(vector SORT_TYPE *) &data[i12] = spu_shuffle(v5, v6, MASK2);
      }
    }
    for(u=k; u<N; u+=2*k) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;

        v1 = *(vector SORT_TYPE *) &data[i];
        v2 = *(vector SORT_TYPE *) &data[i4];
        v5 = *(vector SORT_TYPE *) &data[i8];
        v6 = *(vector SORT_TYPE *) &data[i12];
        v3 = spu_shuffle(v1, v2, MASK1);
        v4 = spu_shuffle(v1, v2, MASK2);
        v7 = spu_shuffle(v5, v6, MASK1);
        v8 = spu_shuffle(v5, v6, MASK2);
        t1 = spu_cmpgt(v3, v4);
        t2 = spu_cmpgt(v7, v8);
        v1 = spu_sel(v3, v4, t1);
        v2 = spu_sel(v4, v3, t1);
        v5 = spu_sel(v7, v8, t2);
        v6 = spu_sel(v8, v7, t2);
        *(vector SORT_TYPE *) &data[i]  = spu_shuffle(v1, v2, MASK1);
        *(vector SORT_TYPE *) &data[i4] = spu_shuffle(v1, v2, MASK2);
        *(vector SORT_TYPE *) &data[i8]  = spu_shuffle(v5, v6, MASK1);
        *(vector SORT_TYPE *) &data[i12] = spu_shuffle(v5, v6, MASK2);
      }
    }
  }
}

void btn_core_l8_2(int N, int k) {
  register vector unsigned char MASK1 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
                                         0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  register vector unsigned char MASK2 = {0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
                                         0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

  register int i;
  register int i4;
  register int i8;
  register int i12;

  register vector SORT_TYPE v1;
  register vector SORT_TYPE v2;
  register vector SORT_TYPE v3;
  register vector SORT_TYPE v4;
  register vector SORT_TYPE v5;
  register vector SORT_TYPE v6;
  register vector SORT_TYPE v7;
  register vector SORT_TYPE v8;

  register vector unsigned int t1;
  register vector unsigned int t2;

  register int u;
  register int v;

  if(!K) { 
    for(u=0; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;

        v1 = *(vector SORT_TYPE *) &data[i];
        v2 = *(vector SORT_TYPE *) &data[i4];
        v5 = *(vector SORT_TYPE *) &data[i8];
        v6 = *(vector SORT_TYPE *) &data[i12];
        v3 = spu_shuffle(v1, v2, MASK1);
        v4 = spu_shuffle(v1, v2, MASK2);
        v7 = spu_shuffle(v5, v6, MASK1);
        v8 = spu_shuffle(v5, v6, MASK2);
        t1 = spu_cmpgt(v3, v4);
        t2 = spu_cmpgt(v7, v8);
        v1 = spu_sel(v3, v4, t1);
        v2 = spu_sel(v4, v3, t1);
        v5 = spu_sel(v7, v8, t2);
        v6 = spu_sel(v8, v7, t2);
        *(vector SORT_TYPE *) &data[i]   = spu_shuffle(v1, v2, MASK1);
        *(vector SORT_TYPE *) &data[i4]  = spu_shuffle(v1, v2, MASK2);
        *(vector SORT_TYPE *) &data[i8]  = spu_shuffle(v5, v6, MASK1);
        *(vector SORT_TYPE *) &data[i12] = spu_shuffle(v5, v6, MASK2);
      }
    }
    for(u=k; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;

        v1 = *(vector SORT_TYPE *) &data[i];
        v2 = *(vector SORT_TYPE *) &data[i4];
        v5 = *(vector SORT_TYPE *) &data[i8];
        v6 = *(vector SORT_TYPE *) &data[i12];
        v3 = spu_shuffle(v1, v2, MASK1);
        v4 = spu_shuffle(v1, v2, MASK2);
        v7 = spu_shuffle(v5, v6, MASK1);
        v8 = spu_shuffle(v5, v6, MASK2);
        t1 = spu_cmpgt(v3, v4);
        t2 = spu_cmpgt(v7, v8);
        v1 = spu_sel(v4, v3, t1);
        v2 = spu_sel(v3, v4, t1);
        v5 = spu_sel(v8, v7, t2);
        v6 = spu_sel(v7, v8, t2);
        *(vector SORT_TYPE *) &data[i]   = spu_shuffle(v1, v2, MASK1);
        *(vector SORT_TYPE *) &data[i4]  = spu_shuffle(v1, v2, MASK2);
        *(vector SORT_TYPE *) &data[i8]  = spu_shuffle(v5, v6, MASK1);
        *(vector SORT_TYPE *) &data[i12] = spu_shuffle(v5, v6, MASK2);
      }
    }
  } else {
    for(u=0; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;

        v1 = *(vector SORT_TYPE *) &data[i];
        v2 = *(vector SORT_TYPE *) &data[i4];
        v5 = *(vector SORT_TYPE *) &data[i8];
        v6 = *(vector SORT_TYPE *) &data[i12];
        v3 = spu_shuffle(v1, v2, MASK1);
        v4 = spu_shuffle(v1, v2, MASK2);
        v7 = spu_shuffle(v5, v6, MASK1);
        v8 = spu_shuffle(v5, v6, MASK2);
        t1 = spu_cmpgt(v3, v4);
        t2 = spu_cmpgt(v7, v8);
        v1 = spu_sel(v4, v3, t1);
        v2 = spu_sel(v3, v4, t1);
        v5 = spu_sel(v8, v7, t2);
        v6 = spu_sel(v7, v8, t2);
        *(vector SORT_TYPE *) &data[i]   = spu_shuffle(v1, v2, MASK1);
        *(vector SORT_TYPE *) &data[i4]  = spu_shuffle(v1, v2, MASK2);
        *(vector SORT_TYPE *) &data[i8]  = spu_shuffle(v5, v6, MASK1);
        *(vector SORT_TYPE *) &data[i12] = spu_shuffle(v5, v6, MASK2);
      }
    }
    for(u=k; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;
	
        v1 = *(vector SORT_TYPE *) &data[i];
        v2 = *(vector SORT_TYPE *) &data[i4];
        v5 = *(vector SORT_TYPE *) &data[i8];
        v6 = *(vector SORT_TYPE *) &data[i12];
        v3 = spu_shuffle(v1, v2, MASK1);
        v4 = spu_shuffle(v1, v2, MASK2);
        v7 = spu_shuffle(v5, v6, MASK1);
        v8 = spu_shuffle(v5, v6, MASK2);
        t1 = spu_cmpgt(v3, v4);
        t2 = spu_cmpgt(v7, v8);
        v1 = spu_sel(v3, v4, t1);
        v2 = spu_sel(v4, v3, t1);
        v5 = spu_sel(v7, v8, t2);
        v6 = spu_sel(v8, v7, t2);
        *(vector SORT_TYPE *) &data[i]   = spu_shuffle(v1, v2, MASK1);
        *(vector SORT_TYPE *) &data[i4]  = spu_shuffle(v1, v2, MASK2);
        *(vector SORT_TYPE *) &data[i8]  = spu_shuffle(v5, v6, MASK1);
        *(vector SORT_TYPE *) &data[i12] = spu_shuffle(v5, v6, MASK2);
      }
    }
  }
}

void btn_core_l8_4(int N, int k) {
  register int i;
  register int i4;
  register int i8;
  register int i12;

  register vector SORT_TYPE v11;
  register vector SORT_TYPE v12;
  register vector SORT_TYPE v21;
  register vector SORT_TYPE v22;

  register vector unsigned int t1;  
  register vector unsigned int t2;  

  register int u;
  register int v;

  if(!K) { 
    for(u=0; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;


        v11 = *(vector SORT_TYPE *) &data[i];
        v12 = *(vector SORT_TYPE *) &data[i4];
        v21 = *(vector SORT_TYPE *) &data[i8];
        v22 = *(vector SORT_TYPE *) &data[i12];      
        t1  = spu_cmpgt(v11,v12);
        t2  = spu_cmpgt(v21,v22);
        *(vector SORT_TYPE *) &data[i]   = spu_sel(v11, v12, t1);	
        *(vector SORT_TYPE *) &data[i4]  = spu_sel(v12, v11, t1);
        *(vector SORT_TYPE *) &data[i8]  = spu_sel(v21, v22, t2);
	*(vector SORT_TYPE *) &data[i12] = spu_sel(v22, v21, t2);
      }
    } 
    for(u=k; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;


        v11 = *(vector SORT_TYPE *) &data[i];
        v12 = *(vector SORT_TYPE *) &data[i4];
        v21 = *(vector SORT_TYPE *) &data[i8];
        v22 = *(vector SORT_TYPE *) &data[i12];      
        t1  = spu_cmpgt(v11,v12);
        t2  = spu_cmpgt(v21,v22);
        *(vector SORT_TYPE *) &data[i4]  = spu_sel(v11, v12, t1);	
        *(vector SORT_TYPE *) &data[i]   = spu_sel(v12, v11, t1);
        *(vector SORT_TYPE *) &data[i12] = spu_sel(v21, v22, t2);
        *(vector SORT_TYPE *) &data[i8]  = spu_sel(v22, v21, t2);
      }
    } 
  } else {
    for(u=0; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;

        v11 = *(vector SORT_TYPE *) &data[i];
        v12 = *(vector SORT_TYPE *) &data[i4];
        v21 = *(vector SORT_TYPE *) &data[i8];
        v22 = *(vector SORT_TYPE *) &data[i12];      
        t1  = spu_cmpgt(v11,v12);
        t2  = spu_cmpgt(v21,v22);
        *(vector SORT_TYPE *) &data[i4]  = spu_sel(v11, v12, t1);	
        *(vector SORT_TYPE *) &data[i]   = spu_sel(v12, v11, t1);
        *(vector SORT_TYPE *) &data[i12] = spu_sel(v21, v22, t2);
        *(vector SORT_TYPE *) &data[i8]  = spu_sel(v22, v21, t2);
      }
    } 
    for(u=k; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i   = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4  = i+4;
        i8  = i+8;
        i12 = i+12;

        v11 = *(vector SORT_TYPE *) &data[i];
        v12 = *(vector SORT_TYPE *) &data[i4];
        v21 = *(vector SORT_TYPE *) &data[i8];
        v22 = *(vector SORT_TYPE *) &data[i12];      
        t1  = spu_cmpgt(v11,v12);
        t2  = spu_cmpgt(v21,v22);
        *(vector SORT_TYPE *) &data[i]   = spu_sel(v11, v12, t1);	
        *(vector SORT_TYPE *) &data[i4]  = spu_sel(v12, v11, t1);
        *(vector SORT_TYPE *) &data[i8]  = spu_sel(v21, v22, t2);
        *(vector SORT_TYPE *) &data[i12] = spu_sel(v22, v21, t2); 
      }
    } 
  }
}

void btn_core_8(int N, int k) {
  register int i;
  register int i4;
  register int ixj;
  register int ixj4;

  register vector SORT_TYPE v11;
  register vector SORT_TYPE v12;
  register vector SORT_TYPE v21;
  register vector SORT_TYPE v22;
  register vector unsigned int t1;  
  register vector unsigned int t2;  

  register int u;
  register int v;

  if(!K) { 
    for(u=0; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i    = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else
	  data = left;

        i4   = i+4;
        ixj  = i+8;
        ixj4 = i+12;

        v11 = *(vector SORT_TYPE *) &data[i];
        v12 = *(vector SORT_TYPE *) &data[ixj];
        v21 = *(vector SORT_TYPE *) &data[i4];
        v22 = *(vector SORT_TYPE *) &data[ixj4];
        t1 = spu_cmpgt(v11,v12);
        t2 = spu_cmpgt(v21,v22);
        *(vector SORT_TYPE *) &data[i]     = spu_sel(v11, v12, t1);
        *(vector SORT_TYPE *) &data[ixj]   = spu_sel(v12, v11, t1);
        *(vector SORT_TYPE *) &data[i4]    = spu_sel(v21, v22, t2);
        *(vector SORT_TYPE *) &data[ixj4]  = spu_sel(v22, v21, t2);
      }
    }
    for(u=k; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i    = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else {
	  data = left;
	}

	i4   = i+4;
	ixj  = i+8;
	ixj4 = i+12;

        v11 = *(vector SORT_TYPE *) &data[i];
        v12 = *(vector SORT_TYPE *) &data[ixj];
        v21 = *(vector SORT_TYPE *) &data[i4];
        v22 = *(vector SORT_TYPE *) &data[ixj4];
        t1 = spu_cmpgt(v11,v12);
        t2 = spu_cmpgt(v21,v22);
        *(vector SORT_TYPE *) &data[ixj]   = spu_sel(v11, v12, t1);
        *(vector SORT_TYPE *) &data[i]     = spu_sel(v12, v11, t1);
        *(vector SORT_TYPE *) &data[ixj4]  = spu_sel(v21, v22, t2);
        *(vector SORT_TYPE *) &data[i4]    = spu_sel(v22, v21, t2);
      }   
    }
  } else {
    for(u=0; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i    = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else {
	  data = left;
	}

        i4   = i+4;
        ixj  = i+8;
        ixj4 = i+12;
        v11 = *(vector SORT_TYPE *) &data[i];
        v12 = *(vector SORT_TYPE *) &data[ixj];
        v21 = *(vector SORT_TYPE *) &data[i4];
        v22 = *(vector SORT_TYPE *) &data[ixj4];
        t1 = spu_cmpgt(v11,v12);
        t2 = spu_cmpgt(v21,v22);
        *(vector SORT_TYPE *) &data[ixj]   = spu_sel(v11, v12, t1);
        *(vector SORT_TYPE *) &data[i]     = spu_sel(v12, v11, t1);
        *(vector SORT_TYPE *) &data[ixj4]  = spu_sel(v21, v22, t2);
        *(vector SORT_TYPE *) &data[i4]    = spu_sel(v22, v21, t2);
      }
    }
    for(u=k; u<N; u+=k<<1) {
      for(v=0; v<k; v+=16) {
        i    = u+v;

	if(i >= BUFFER_SIZE){
	  data = right;
	  i -= BUFFER_SIZE;
	} else {
	  data = left;
	}

        i4   = i+4;
        ixj  = i+8;
        ixj4 = i+12;
        v11 = *(vector SORT_TYPE *) &data[i];
        v12 = *(vector SORT_TYPE *) &data[ixj];
        v21 = *(vector SORT_TYPE *) &data[i4];
        v22 = *(vector SORT_TYPE *) &data[ixj4];
        t1 = spu_cmpgt(v11,v12);
        t2 = spu_cmpgt(v21,v22);
        *(vector SORT_TYPE *) &data[i]     = spu_sel(v11, v12, t1);
        *(vector SORT_TYPE *) &data[ixj]   = spu_sel(v12, v11, t1);
        *(vector SORT_TYPE *) &data[i4]    = spu_sel(v21, v22, t2);
        *(vector SORT_TYPE *) &data[ixj4]  = spu_sel(v22, v21, t2);
      }   
    }
  }
}

typedef union _vindex {
  vector signed int v;
  int i[4];
} vindex;

void btn_core_l16(int N, int k, int j) {
  register int i;
  register int ixj;

  vindex vi;
  vindex vixj;

  register vector signed int vidx = {0, 4, 8, 12};

  register vector SORT_TYPE v11;
  register vector SORT_TYPE v12;
  register vector SORT_TYPE v21;
  register vector SORT_TYPE v22;
  register vector SORT_TYPE v31;
  register vector SORT_TYPE v32;
  register vector SORT_TYPE v41;
  register vector SORT_TYPE v42;
  register vector unsigned int t1;  
  register vector unsigned int t2;  
  register vector unsigned int t3;  
  register vector unsigned int t4;  


  if(!K) {
    for(i=0; i<N;) {
      ixj=i^j;
      if(ixj < i) {  
        i += j;
        continue;
      }  

      if(i >= BUFFER_SIZE){
	vi.v   = spu_splats(i-BUFFER_SIZE);
	vi.v   = spu_add(vi.v, vidx);
	data = right;
      } else {
	vi.v   = spu_splats(i);
	vi.v   = spu_add(vi.v, vidx);
	data = left;
      }

      if(ixj >= BUFFER_SIZE){
	vixj.v = spu_splats(ixj-BUFFER_SIZE);
	vixj.v = spu_add(vixj.v, vidx);
	data2 = right;
      } else {
	vixj.v = spu_splats(ixj);
	vixj.v = spu_add(vixj.v, vidx);
	data2 = left;
      }

      if(!(i&k)) {
          v11 = *(vector SORT_TYPE *) &data[vi.i[0]];                     v12 = *(vector SORT_TYPE *) &data2[vixj.i[0]];
          v21 = *(vector SORT_TYPE *) &data[vi.i[1]];                     v22 = *(vector SORT_TYPE *) &data2[vixj.i[1]];
          v31 = *(vector SORT_TYPE *) &data[vi.i[2]];                     v32 = *(vector SORT_TYPE *) &data2[vixj.i[2]];        
          v41 = *(vector SORT_TYPE *) &data[vi.i[3]];                     v42 = *(vector SORT_TYPE *) &data2[vixj.i[3]];
          t1 = spu_cmpgt(v11,v12);                                        t2 = spu_cmpgt(v21,v22);
          t3 = spu_cmpgt(v31,v32);                                        t4 = spu_cmpgt(v41,v42);
  	  *(vector SORT_TYPE *) &data[vi.i[0]] = spu_sel(v11, v12, t1);   *(vector SORT_TYPE *) &data2[vixj.i[0]] = spu_sel(v12, v11, t1);
	  *(vector SORT_TYPE *) &data[vi.i[1]] = spu_sel(v21, v22, t2);   *(vector SORT_TYPE *) &data2[vixj.i[1]] = spu_sel(v22, v21, t2);
	  *(vector SORT_TYPE *) &data[vi.i[2]] = spu_sel(v31, v32, t3);   *(vector SORT_TYPE *) &data2[vixj.i[2]] = spu_sel(v32, v31, t3);
	  *(vector SORT_TYPE *) &data[vi.i[3]] = spu_sel(v41, v42, t4);   *(vector SORT_TYPE *) &data2[vixj.i[3]] = spu_sel(v42, v41, t4);
	  i += 16;
      } else {
          v11 = *(vector SORT_TYPE *) &data[vi.i[0]];                     v12 = *(vector SORT_TYPE *) &data2[vixj.i[0]];
          v21 = *(vector SORT_TYPE *) &data[vi.i[1]];                     v22 = *(vector SORT_TYPE *) &data2[vixj.i[1]];
          v31 = *(vector SORT_TYPE *) &data[vi.i[2]];                     v32 = *(vector SORT_TYPE *) &data2[vixj.i[2]];
          v41 = *(vector SORT_TYPE *) &data[vi.i[3]];                     v42 = *(vector SORT_TYPE *) &data2[vixj.i[3]];
          t1 = spu_cmpgt(v11,v12);                                        t2 = spu_cmpgt(v21,v22);
          t3 = spu_cmpgt(v31,v32);                                        t4 = spu_cmpgt(v41,v42);
  	  *(vector SORT_TYPE *) &data2[vixj.i[0]] = spu_sel(v11, v12, t1); *(vector SORT_TYPE *) &data[vi.i[0]] = spu_sel(v12, v11, t1);
          *(vector SORT_TYPE *) &data2[vixj.i[1]] = spu_sel(v21, v22, t2); *(vector SORT_TYPE *) &data[vi.i[1]] = spu_sel(v22, v21, t2);
	  *(vector SORT_TYPE *) &data2[vixj.i[2]] = spu_sel(v31, v32, t3); *(vector SORT_TYPE *) &data[vi.i[2]] = spu_sel(v32, v31, t3);
	  *(vector SORT_TYPE *) &data2[vixj.i[3]] = spu_sel(v41, v42, t4); *(vector SORT_TYPE *) &data[vi.i[3]] = spu_sel(v42, v41, t4);
	  i += 16;
      }
    }
  } else {
    for(i=0; i<N;) {
      ixj=i^j;
      if(ixj < i) {  
        i += j;
        continue;
      }  

      if(i >= BUFFER_SIZE){
	vi.v   = spu_splats(i-BUFFER_SIZE);
	vi.v   = spu_add(vi.v, vidx);
	data = right;
      } else {
	vi.v   = spu_splats(i);
	vi.v   = spu_add(vi.v, vidx);
	data = left;
      }

      if(ixj >= BUFFER_SIZE){
	vixj.v = spu_splats(ixj-BUFFER_SIZE);
	vixj.v = spu_add(vixj.v, vidx);
	data2 = right;
      } else {
	vixj.v = spu_splats(ixj);
	vixj.v = spu_add(vixj.v, vidx);
	data2 = left;
      }

      if(!(i&k)) {
          v11 = *(vector SORT_TYPE *) &data[vi.i[0]];                     v12 = *(vector SORT_TYPE *) &data2[vixj.i[0]];
          v21 = *(vector SORT_TYPE *) &data[vi.i[1]];                     v22 = *(vector SORT_TYPE *) &data2[vixj.i[1]];
          v31 = *(vector SORT_TYPE *) &data[vi.i[2]];                     v32 = *(vector SORT_TYPE *) &data2[vixj.i[2]];
          v41 = *(vector SORT_TYPE *) &data[vi.i[3]];                     v42 = *(vector SORT_TYPE *) &data2[vixj.i[3]];
          t1 = spu_cmpgt(v11,v12);                                        t2 = spu_cmpgt(v21,v22);
          t3 = spu_cmpgt(v31,v32);                                        t4 = spu_cmpgt(v41,v42);
          *(vector SORT_TYPE *) &data2[vixj.i[0]] = spu_sel(v11, v12, t1); *(vector SORT_TYPE *) &data[vi.i[0]] = spu_sel(v12, v11, t1);
          *(vector SORT_TYPE *) &data2[vixj.i[1]] = spu_sel(v21, v22, t2); *(vector SORT_TYPE *) &data[vi.i[1]] = spu_sel(v22, v21, t2);
          *(vector SORT_TYPE *) &data2[vixj.i[2]] = spu_sel(v31, v32, t3); *(vector SORT_TYPE *) &data[vi.i[2]] = spu_sel(v32, v31, t3);
          *(vector SORT_TYPE *) &data2[vixj.i[3]] = spu_sel(v41, v42, t4); *(vector SORT_TYPE *) &data[vi.i[3]] = spu_sel(v42, v41, t4);
          i += 16;
      } else {
          v11 = *(vector SORT_TYPE *) &data[vi.i[0]];                     v12 = *(vector SORT_TYPE *) &data2[vixj.i[0]];
          v21 = *(vector SORT_TYPE *) &data[vi.i[1]];                     v22 = *(vector SORT_TYPE *) &data2[vixj.i[1]];
          v31 = *(vector SORT_TYPE *) &data[vi.i[2]];                     v32 = *(vector SORT_TYPE *) &data2[vixj.i[2]];        
          v41 = *(vector SORT_TYPE *) &data[vi.i[3]];                     v42 = *(vector SORT_TYPE *) &data2[vixj.i[3]];
          t1 = spu_cmpgt(v11,v12);                                        t2 = spu_cmpgt(v21,v22);
          t3 = spu_cmpgt(v31,v32);                                        t4 = spu_cmpgt(v41,v42);
          *(vector SORT_TYPE *) &data[vi.i[0]] = spu_sel(v11, v12, t1);   *(vector SORT_TYPE *) &data2[vixj.i[0]] = spu_sel(v12, v11, t1);
          *(vector SORT_TYPE *) &data[vi.i[1]] = spu_sel(v21, v22, t2);   *(vector SORT_TYPE *) &data2[vixj.i[1]] = spu_sel(v22, v21, t2);
          *(vector SORT_TYPE *) &data[vi.i[2]] = spu_sel(v31, v32, t3);   *(vector SORT_TYPE *) &data2[vixj.i[2]] = spu_sel(v32, v31, t3);
          *(vector SORT_TYPE *) &data[vi.i[3]] = spu_sel(v41, v42, t4);   *(vector SORT_TYPE *) &data2[vixj.i[3]] = spu_sel(v42, v41, t4);
          i += 16;
      }
    }
  } 
}

// b1 and b2 must point to two buffers sorted in opposite directions of size BUFFER_SIZE
void merge_buffers(volatile vector signed int *b1, volatile vector signed int *b2) {
  left = (volatile int*) b1;
  right = (volatile int*) b2;
  int k = 2*BUFFER_SIZE, j;

  for(j=k>>1; j>0; j=j>>1) {
    if    (j>=16) btn_core_l16(2*BUFFER_SIZE, k, j);
    else if(j==8) btn_core_8(2*BUFFER_SIZE, k);
    else if(j==4) btn_core_l8_4(2*BUFFER_SIZE, k);
    else if(j==2) btn_core_l8_2(2*BUFFER_SIZE, k);       
    else          btn_core_l8_1(2*BUFFER_SIZE, k);
  }
}

void merge_buffers_v(volatile vector signed int **buffer, int *idx) {
   vector unsigned int t;
   vector unsigned int tmask = {0,0,0,0xFFFFFFFF};

  for(; idx[OUT] < VEC_BUFFER_SIZE; idx[OUT]++){

    t = spu_cmpgt(buffer[LEFT][ idx[LEFT] ], buffer[RIGHT][ idx[RIGHT] ]);

    sort_vectors(&buffer[LEFT][idx[LEFT]], &buffer[RIGHT][idx[RIGHT]]);
    buffer[OUT][ idx[OUT] ] = buffer[LEFT][idx[LEFT]];

    t = spu_and(t,tmask);
    if(spu_extract(t,3)){
      // Lmax > Rmax
      buffer[LEFT][idx[LEFT]] = buffer[RIGHT][idx[RIGHT]];
      ++idx[RIGHT];
      if(idx[RIGHT] == VEC_BUFFER_SIZE){
	++idx[OUT];
	return;
      }
    } else {
      ++idx[LEFT];
      if(idx[LEFT] == VEC_BUFFER_SIZE){
	++idx[OUT];
	return;
      }
    }
  }
}


// TODO: restrict qualifyer here somehow
void cp_buffer(volatile vector signed int *to, volatile vector signed int *from, int num){
  
  int i;
  for(i=0; i < num/4; i++){
    to[4*i] = from[4*i];
    to[4*i+1] = from[4*i+1];
    to[4*i+2] = from[4*i+2];
    to[4*i+3] = from[4*i+3];
  }
  
  //memcpy(to,from, BUFFER_SIZE_BYTES);
}

void rev_buffer(vector signed int volatile *b){
  int i;
  register vector signed int l1,l2;

  register vector unsigned char rev_mask1 = {12,13,14,15,
                                             8,9,10,11,
                                             4,5,6,7,
                                             0,1,2,3};

  register vector unsigned char rev_mask2 = {28,29,30,31,
                                             24,25,26,27,
                                             20,21,22,23,
                                             16,17,18,19};

  for(i=0; i < VEC_BUFFER_SIZE/2; i++){
    // swap i och VEC_BUFFER_SIZE-i mha spu_shuffle
    l1 = spu_shuffle(b[i],b[i], rev_mask1);
    l2 = spu_shuffle(b[i],b[VEC_BUFFER_SIZE-1-i], rev_mask2);

    b[i] = l2;
    b[VEC_BUFFER_SIZE-1-i] = l1;
  }
}
