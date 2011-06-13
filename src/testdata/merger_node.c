#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

int comp_fn(int *a, int *b){
  int temp = *a - *b;
  if(temp > 0)
    return 1;
  else if(temp < 0)
    return -1;
  else
    return 0;
}

int comp_fn_rev(int *a, int *b){
  int temp = *b - *a;
  if(temp > 0)
    return 1;
  else if(temp < 0)
    return -1;
  else
    return 0;
}

int main(int argc, char **argv){

  if(argc != 5){
    printf("usage merger_node num_blocks num_ints out_blocks type\ntype: 1 random, 2 sorted\n");
    exit(0);
  }

  FILE *lr_random;
  lr_random = fopen("sorted_blocks.bin","w");
  if(lr_random == NULL){
    perror("Kunde inte skapa lr_random\n");
    exit(1);
  }

  int num_blocks = atoi(argv[1]);
  int block_size = atoi(argv[2]);
  int out_blocks = atoi(argv[3]);
  int nWritten;

  //printf("#blocks %d\n#ints per block %d\n#out blocks %d\n", num_blocks, block_size, out_blocks);

  // Write number of blocks
  nWritten = fwrite(&num_blocks,sizeof(int),1,lr_random);
  if(nWritten != 1){
    perror("Kunde inte skriva till lr_random\n");
    exit(1);
  }

  // Write block size
  nWritten = fwrite(&block_size,sizeof(int),1,lr_random);
  if(nWritten != 1){
    perror("Kunde inte skriva till lr_random\n");
    exit(1);
  }

  // Write blocks of sorted integers
  int i,k;
  int *out = (int*) malloc(num_blocks*block_size*sizeof(int));

  srand( time(NULL) );

  if(*argv[4] == 'r'){
    //printf("Generating random data...");
    for(k=0; k < num_blocks; k++){
      
      for(i = 0; i < block_size; i++){	
	out[k*block_size + i] = rand();
      }

      qsort(&out[k*block_size],block_size,sizeof(int),comp_fn);
    }


  } else if(*argv[4] == 's'){
    //printf("Generating presorted data...");
    for(k=0; k < num_blocks; k++){
      for(i = 0; i < block_size; i++){
	out[k*block_size + i] = k*block_size + i;
      }
      qsort(&out[k*block_size],block_size,sizeof(int),comp_fn);
    }
  }

  //printf("\nDone. Writing to file...\n");

  nWritten = fwrite(out,sizeof(int),num_blocks*block_size,lr_random);
  if(nWritten != num_blocks*block_size){
    perror("Kunde inte skriva till lr_random\n");
    exit(1);
  }

  fclose(lr_random);

  //printf("Done.\n");

  return 0;

  printf("Done. Sorting out blocks...\n");
  
  FILE *sorted;
  sorted = fopen("sorted.bin","w");
  if(sorted == NULL){
    perror("Kunde inte skapa lr_random\n");
    exit(1);
  }

  for(i=0;i<out_blocks;i++)
    qsort(&out[i * num_blocks*block_size/out_blocks],num_blocks*block_size/out_blocks,sizeof(int),comp_fn);

  printf("Done. Writing to file...\n");

  nWritten = fwrite(out,sizeof(int),num_blocks*block_size,sorted);
  if(nWritten != num_blocks*block_size){
    perror("Kunde inte skriva till sorted\n");
    exit(1);
  }

  printf("Done.\n");

  return 0;
}
