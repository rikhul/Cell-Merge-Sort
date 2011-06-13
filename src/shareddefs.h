#ifndef SHAREDDEFS_H
#define SHAREDDEFS_H 1

//#define PRINT_INFO
//#define SPU_PRINT_INFO
//#define TRACE_PRINTS
//#define PRINT_DATA
//#define SPU_PRINT_DATA
//#define TRACE_NOTIFY
//#define TRACE_WAIT
//#define TRACE_TIME

// Max total number of vectors in buffers per SPU
#define MAX_TOTAL_BUFFER_SIZE 13500

// Max number of vectors in one DMA transfer
#define MAX_DMA_SIZE 1024

#define aligned_128 __attribute__((aligned(128)))

#define LEFT 0
#define RIGHT 1
#define OUT 2
#define PARENT 3

#define HEAD 0
#define TAIL 1

#define STRLN 250
#endif
