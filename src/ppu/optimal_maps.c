#include <stdlib.h>
#include <stdio.h>
#include "node_spu_map.h"
#include "optimal_maps.h"

#define LC(i) (2*i)
#define RC(i) (2*i+1)

node_map_t* optimal_2(node_id_t root){
 node_map_t* map = (node_map_t*) malloc(sizeof(node_map_t));
  map->num_spus = 2;
  map->map = (node_collection_t*) malloc(2*sizeof(node_collection_t));

  // Root
  map->map[0].num_nodes = 1;
  map->map[0].node_ids = (node_id_t*) malloc(sizeof(node_id_t));
  map->map[0].node_ids[0] = root-1;

  // Level 2
  map->map[1].num_nodes = 2;
  map->map[1].node_ids = (node_id_t*) malloc(2*sizeof(node_id_t));
  map->map[1].node_ids[0] = RC(root)-1;
  map->map[1].node_ids[1] = LC(root)-1;

  return map;
}

node_map_t* optimal_3(node_id_t root){
  node_map_t* map = (node_map_t*) malloc(sizeof(node_map_t));
  map->num_spus = 3;
  map->map = (node_collection_t*) malloc(3*sizeof(node_collection_t));

  // Root
  map->map[0].num_nodes = 1;
  map->map[0].node_ids = (node_id_t*) malloc(sizeof(node_id_t));
  map->map[0].node_ids[0] = root-1;

  // Level 2
  map->map[1].num_nodes = 2;
  map->map[1].node_ids = (node_id_t*) malloc(2*sizeof(node_id_t));
  map->map[1].node_ids[0] = RC(root)-1;
  map->map[1].node_ids[1] = LC(root)-1;

  // Level 3
  map->map[2].num_nodes = 4;
  map->map[2].node_ids = (node_id_t*) malloc(4*sizeof(node_id_t));
  map->map[2].node_ids[0] = LC(LC(root))-1;
  map->map[2].node_ids[1] = RC(LC(root))-1;
  map->map[2].node_ids[2] = LC(RC(root))-1;
  map->map[2].node_ids[3] = RC(RC(root))-1;

  return map;
}

node_map_t* optimal_4(node_id_t root){
  node_map_t* map = (node_map_t*) malloc(sizeof(node_map_t));
  map->num_spus = 4;
  map->map = (node_collection_t*) malloc(4*sizeof(node_collection_t));

  // Root
  map->map[0].num_nodes = 1;
  map->map[0].node_ids = (node_id_t*) malloc(sizeof(node_id_t));
  map->map[0].node_ids[0] = root-1;

  // Level 2
  map->map[1].num_nodes = 2;
  map->map[1].node_ids = (node_id_t*) malloc(2*sizeof(node_id_t));
  map->map[1].node_ids[0] = 2*root -1;
  map->map[1].node_ids[1] = 2*root;
  
  // Level 3
  map->map[2].num_nodes = 4;
  map->map[2].node_ids = (node_id_t*) malloc(4*sizeof(int));
  map->map[2].node_ids[0] = 2*2*root-1;
  map->map[2].node_ids[1] = 2*2*root;
  map->map[2].node_ids[2] = 2*(2*root+1)-1;
  map->map[2].node_ids[3] = 2*(2*root+1);
  //  map->map[2].node_ids[4] = 2*(2*(2*root+1)+1)-1;
  //  map->map[2].node_ids[5] = 2*(2*(2*root+1)+1);

  // Level 4
  map->map[3].num_nodes = 8;
  map->map[3].node_ids = (node_id_t*) malloc( 8*sizeof(node_id_t));
  map->map[3].node_ids[0] = 2*2*2*root -1;
  map->map[3].node_ids[1] = 2*2*2*root;
  map->map[3].node_ids[2] = 2*(2*2*root+1)-1;
  map->map[3].node_ids[3] = 2*(2*2*root+1);
  map->map[3].node_ids[4] = 2*2*(2*root+1)-1;
  map->map[3].node_ids[5] = 2*2*(2*root+1);
  map->map[3].node_ids[6] = 2*(2*(2*root+1)+1)-1;
  map->map[3].node_ids[7] = 2*(2*(2*root+1)+1);

  return map;
}

node_map_t* optimal_5(node_id_t root){
  node_map_t* map = (node_map_t*) malloc(sizeof(node_map_t));
  map->num_spus = 5;
  map->map = (node_collection_t*) malloc(5*sizeof(node_collection_t));

  // Root
  map->map[0].num_nodes = 1;
  map->map[0].node_ids = (node_id_t*) malloc( sizeof(node_id_t));
  map->map[0].node_ids[0] = root-1;

  // Level 2
  map->map[1].num_nodes = 7;
  map->map[1].node_ids = (node_id_t*) malloc(7*sizeof(node_id_t));
  map->map[1].node_ids[0] = 2*root -1;               // 2
  map->map[1].node_ids[1] = 2*(2*2*root +1) -1;      // 10
  map->map[1].node_ids[2] = 2*(2*2*root +1);         // 11
  map->map[1].node_ids[3] = 2*2*(2*2*root+1) -1;     // 20
  map->map[1].node_ids[4] = 2*2*(2*2*root+1);        // 21
  map->map[1].node_ids[5] = 2*(2*(2*2*root+1)+1) -1; // 22
  map->map[1].node_ids[6] = 2*(2*(2*2*root+1)+1);    // 23
  
  // Level 3
  map->map[2].num_nodes = 7;
  map->map[2].node_ids = (node_id_t*) malloc(7*sizeof(node_id_t));
  map->map[2].node_ids[0] = 2*root;                    // 3
  map->map[2].node_ids[1] = 2*(2*(2*root+1)+1) -1;     // 14
  map->map[2].node_ids[2] = 2*(2*(2*root+1)+1);        // 15
  map->map[2].node_ids[3] = 2*2*(2*(2*root+1)+1)-1;    // 28
  map->map[2].node_ids[4] = 2*2*(2*(2*root+1)+1);      // 29
  map->map[2].node_ids[5] = 2*(2*(2*(2*root+1)+1)+1)-1;// 30
  map->map[2].node_ids[6] = 2*(2*(2*(2*root+1)+1)+1);  // 31

  // Level 4
  map->map[3].num_nodes = 8;
  map->map[3].node_ids = (node_id_t*) malloc(8*sizeof(node_id_t));
  map->map[3].node_ids[0] = 2*2*root-1;                // 4
  map->map[3].node_ids[1] = 2*2*root;                  // 5
  map->map[3].node_ids[2] = 2*2*2*root-1;              // 8
  map->map[3].node_ids[3] = 2*2*2*root;                // 9
  map->map[3].node_ids[4] = 2*2*2*2*root-1;            // 16
  map->map[3].node_ids[5] = 2*2*2*2*root;              // 17
  map->map[3].node_ids[6] = 2*(2*2*2*root+1)-1;        // 18
  map->map[3].node_ids[7] = 2*(2*2*2*root+1);          // 19

  // Level 5
  map->map[4].num_nodes = 8;
  map->map[4].node_ids = (node_id_t*) malloc(8*sizeof(node_id_t));
  map->map[4].node_ids[0] = 2*(2*root+1)-1;            // 6
  map->map[4].node_ids[1] = 2*(2*root+1);              // 7
  map->map[4].node_ids[2] = 2*2*(2*root+1)-1;          // 12
  map->map[4].node_ids[3] = 2*2*(2*root+1);            // 13
  map->map[4].node_ids[4] = 2*2*2*(2*root+1)-1;        // 24
  map->map[4].node_ids[5] = 2*2*2*(2*root+1);          // 25
  map->map[4].node_ids[6] = 2*(2*2*(2*root+1)+1)-1;    // 26
  map->map[4].node_ids[7] = 2*(2*2*(2*root+1)+1);      // 27

  return map;
}

node_map_t* optimal_6(node_id_t root){
  node_map_t* map = (node_map_t*) malloc(sizeof(node_map_t));
  map->num_spus = 6;
  map->map = (node_collection_t*) malloc(6*sizeof(node_collection_t));

  // Root
  map->map[0].num_nodes = 1;
  map->map[0].node_ids = (node_id_t*) malloc(sizeof(node_id_t));
  map->map[0].node_ids[0] = root-1;

  // Level 2
  map->map[1].num_nodes = 2;
  map->map[1].node_ids = (node_id_t*) malloc(2*sizeof(node_id_t));
  map->map[1].node_ids[0] = 2*root -1;
  map->map[1].node_ids[1] = 2*root;
  
  // Level 3
  map->map[2].num_nodes = 4;
  map->map[2].node_ids = (node_id_t*) malloc(4*sizeof(node_id_t));
  map->map[2].node_ids[0] = 2*2*root-1;
  map->map[2].node_ids[1] = 2*2*root;
  map->map[2].node_ids[2] = 2*(2*root+1)-1;
  map->map[2].node_ids[3] = 2*(2*root+1);

  // Level 4
  map->map[3].num_nodes = 8;
  map->map[3].node_ids = (node_id_t*) malloc(8*sizeof(node_id_t));
  map->map[3].node_ids[0] = 2*2*2*root -1;
  map->map[3].node_ids[1] = 2*2*2*root;
  map->map[3].node_ids[2] = 2*(2*2*root+1)-1;
  map->map[3].node_ids[3] = 2*(2*2*root+1);
  map->map[3].node_ids[4] = 2*2*(2*root+1)-1;
  map->map[3].node_ids[5] = 2*2*(2*root+1);
  map->map[3].node_ids[6] = 2*(2*(2*root+1)+1)-1;
  map->map[3].node_ids[7] = 2*(2*(2*root+1)+1);

  // Level 5
  map->map[4].num_nodes = 16;
  map->map[4].node_ids = (node_id_t*) malloc(16*sizeof(node_id_t));
  map->map[4].node_ids[0] = 2*2*2*root -1;
  map->map[4].node_ids[1] = 2*2*2*root;
  map->map[4].node_ids[2] = 2*(2*2*root+1)-1;
  map->map[4].node_ids[3] = 2*(2*2*root+1);
  map->map[4].node_ids[4] = 2*2*(2*root+1)-1;
  map->map[4].node_ids[5] = 2*2*(2*root+1);
  map->map[4].node_ids[6] = 2*(2*(2*root+1)+1)-1;
  map->map[4].node_ids[7] = 2*(2*(2*root+1)+1);
  map->map[4].node_ids[8] = 2*2*2*root -1;
  map->map[4].node_ids[9] = 2*2*2*root;
  map->map[4].node_ids[10] = 2*(2*2*root+1)-1;
  map->map[4].node_ids[11] = 2*(2*2*root+1);
  map->map[4].node_ids[12] = 2*2*(2*root+1)-1;
  map->map[4].node_ids[13] = 2*2*(2*root+1);
  map->map[4].node_ids[14] = 2*(2*(2*root+1)+1)-1;
  map->map[4].node_ids[15] = 2*(2*(2*root+1)+1);

  // Level 6
  map->map[5].num_nodes = 32;
  map->map[5].node_ids = (node_id_t*) malloc(32*sizeof(node_id_t));
  map->map[5].node_ids[0] = 2*2*2*root -1;
  map->map[5].node_ids[1] = 2*2*2*root;
  map->map[5].node_ids[2] = 2*(2*2*root+1)-1;
  map->map[5].node_ids[3] = 2*(2*2*root+1);
  map->map[5].node_ids[4] = 2*2*(2*root+1)-1;
  map->map[5].node_ids[5] = 2*2*(2*root+1);
  map->map[5].node_ids[6] = 2*(2*(2*root+1)+1)-1;
  map->map[5].node_ids[7] = 2*(2*(2*root+1)+1);
  map->map[5].node_ids[8] = 2*2*2*root -1;
  map->map[5].node_ids[9] = 2*2*2*root;
  map->map[5].node_ids[10] = 2*(2*2*root+1)-1;
  map->map[5].node_ids[11] = 2*(2*2*root+1);
  map->map[5].node_ids[12] = 2*2*(2*root+1)-1;
  map->map[5].node_ids[13] = 2*2*(2*root+1);
  map->map[5].node_ids[14] = 2*(2*(2*root+1)+1)-1;
  map->map[5].node_ids[15] = 2*(2*(2*root+1)+1);
  map->map[5].node_ids[16] = 2*2*2*root -1;
  map->map[5].node_ids[17] = 2*2*2*root;
  map->map[5].node_ids[18] = 2*(2*2*root+1)-1;
  map->map[5].node_ids[19] = 2*(2*2*root+1);
  map->map[5].node_ids[20] = 2*2*(2*root+1)-1;
  map->map[5].node_ids[21] = 2*2*(2*root+1);
  map->map[5].node_ids[22] = 2*(2*(2*root+1)+1)-1;
  map->map[5].node_ids[23] = 2*(2*(2*root+1)+1);
  map->map[5].node_ids[24] = 2*2*2*root -1;
  map->map[5].node_ids[25] = 2*2*2*root;
  map->map[5].node_ids[26] = 2*(2*2*root+1)-1;
  map->map[5].node_ids[27] = 2*(2*2*root+1);
  map->map[5].node_ids[28] = 2*2*(2*root+1)-1;
  map->map[5].node_ids[29] = 2*2*(2*root+1);
  map->map[5].node_ids[30] = 2*(2*(2*root+1)+1)-1;
  map->map[5].node_ids[31] = 2*(2*(2*root+1)+1);

  return map;
}

node_map_t* optimal_7(node_id_t root){
  node_map_t* map = (node_map_t*) malloc(sizeof(node_map_t));
  map->num_spus = 7;
  map->map = (node_collection_t*) malloc(7*sizeof(node_collection_t));

  // Root
  map->map[0].num_nodes = 1;
  map->map[0].node_ids = (node_id_t*) malloc(sizeof(node_id_t));
  map->map[0].node_ids[0] = root-1; // 1

  // Level 2
  map->map[1].num_nodes = 3;
  map->map[1].node_ids = (node_id_t*) malloc(3*sizeof(node_id_t));
  map->map[1].node_ids[0] = LC(root)-1;     // 2
  map->map[1].node_ids[1] = LC(LC(root))-1; // 4
  map->map[1].node_ids[2] = RC(LC(root))-1; // 5

  // Level 3
  map->map[2].num_nodes = 3;
  map->map[2].node_ids = (node_id_t*) malloc(3*sizeof(node_id_t));
  map->map[2].node_ids[0] = RC(root)-1;     // 3
  map->map[2].node_ids[1] = LC(RC(root))-1; // 6
  map->map[2].node_ids[2] = RC(RC(root))-1; // 7

  // Level 4
  map->map[3].num_nodes = 30;
  map->map[3].node_ids = (node_id_t*) malloc(30*sizeof(node_id_t));
  int r = map->map[1].node_ids[1]+1;         // 4

  map->map[3].node_ids[0] = LC(r) -1;      // 8
  map->map[3].node_ids[1] = RC(r) -1;      // 9

  r = map->map[3].node_ids[0]+1; // 8
  map->map[3].node_ids[2] = LC(r)-1;       // 16
  map->map[3].node_ids[3] = RC(r)-1;       // 17

  r = map->map[3].node_ids[1]+1; // 9
  map->map[3].node_ids[4] = LC(r)-1;   // 18
  map->map[3].node_ids[5] = RC(r)-1;   // 19

  r = map->map[3].node_ids[2]+1; // 16
  map->map[3].node_ids[6] = LC(r)-1;       // 32
  map->map[3].node_ids[7] = RC(r)-1;       // 33
  map->map[3].node_ids[8] = LC(LC(r))-1;   // 64
  map->map[3].node_ids[9] = RC(LC(r))-1;   // 65
  map->map[3].node_ids[10] = LC(RC(r))-1;  // 66
  map->map[3].node_ids[11] = RC(RC(r))-1;  // 67 

  r = map->map[3].node_ids[3]+1; // 17
  map->map[3].node_ids[12] = LC(r)-1;       // 34
  map->map[3].node_ids[13] = RC(r)-1;       // 35
  map->map[3].node_ids[14] = LC(LC(r))-1;   // 68
  map->map[3].node_ids[15] = RC(LC(r))-1;   // 69
  map->map[3].node_ids[16] = LC(RC(r))-1;   // 70
  map->map[3].node_ids[17] = RC(RC(r))-1;   // 71 

  r = map->map[3].node_ids[4]+1; // 18
  map->map[3].node_ids[18] = LC(r)-1;       // 36
  map->map[3].node_ids[19] = RC(r)-1;       // 37
  map->map[3].node_ids[20] = LC(LC(r))-1;   // 72
  map->map[3].node_ids[21] = RC(LC(r))-1;   // 73
  map->map[3].node_ids[22] = LC(RC(r))-1;   // 74
  map->map[3].node_ids[23] = RC(RC(r))-1;   // 75 

  r = map->map[3].node_ids[5]+1; // 19
  map->map[3].node_ids[24] = LC(r)-1;       // 38
  map->map[3].node_ids[25] = RC(r)-1;       // 39
  map->map[3].node_ids[26] = LC(LC(r))-1;   // 76
  map->map[3].node_ids[27] = RC(LC(r))-1;   // 77
  map->map[3].node_ids[28] = LC(RC(r))-1;   // 78
  map->map[3].node_ids[29] = RC(RC(r))-1;   // 79 

  // Level 5
  map->map[4].num_nodes = 30;
  map->map[4].node_ids = (node_id_t*) malloc(30*sizeof(node_id_t));
  r = map->map[1].node_ids[2]+1;         // 5

  map->map[4].node_ids[0] = LC(r) -1;      // 10
  map->map[4].node_ids[1] = RC(r) -1;      // 11

  r = map->map[4].node_ids[0]+1; // 10
  map->map[4].node_ids[2] = LC(r)-1;       // 20
  map->map[4].node_ids[3] = RC(r)-1;       // 21

  r = map->map[4].node_ids[1]+1; // 11
  map->map[4].node_ids[4] = LC(r)-1;   // 22
  map->map[4].node_ids[5] = RC(r)-1;   // 23

  r = map->map[4].node_ids[2]+1; // 20
  map->map[4].node_ids[6] = LC(r)-1;       // 40
  map->map[4].node_ids[7] = RC(r)-1;       // 41
  map->map[4].node_ids[8] = LC(LC(r))-1;   // 80
  map->map[4].node_ids[9] = RC(LC(r))-1;   // 81
  map->map[4].node_ids[10] = LC(RC(r))-1;  // 82
  map->map[4].node_ids[11] = RC(RC(r))-1;  // 83 

  r = map->map[4].node_ids[3]+1; // 21
  map->map[4].node_ids[12] = LC(r)-1;       // 42
  map->map[4].node_ids[13] = RC(r)-1;       // 43
  map->map[4].node_ids[14] = LC(LC(r))-1;   // 84
  map->map[4].node_ids[15] = RC(LC(r))-1;   // 85
  map->map[4].node_ids[16] = LC(RC(r))-1;   // 86
  map->map[4].node_ids[17] = RC(RC(r))-1;   // 87 

  r = map->map[4].node_ids[4]+1; // 22
  map->map[4].node_ids[18] = LC(r)-1;       // 44
  map->map[4].node_ids[19] = RC(r)-1;       // 45
  map->map[4].node_ids[20] = LC(LC(r))-1;   // 88
  map->map[4].node_ids[21] = RC(LC(r))-1;   // 89
  map->map[4].node_ids[22] = LC(RC(r))-1;   // 90
  map->map[4].node_ids[23] = RC(RC(r))-1;   // 91 

  r = map->map[4].node_ids[5]+1; // 23
  map->map[4].node_ids[24] = LC(r)-1;       // 46
  map->map[4].node_ids[25] = RC(r)-1;       // 47
  map->map[4].node_ids[26] = LC(LC(r))-1;   // 92
  map->map[4].node_ids[27] = RC(LC(r))-1;   // 93
  map->map[4].node_ids[28] = LC(RC(r))-1;   // 94
  map->map[4].node_ids[29] = RC(RC(r))-1;   // 95

  // Level 6
  map->map[5].num_nodes = 30;
  map->map[5].node_ids = (node_id_t*) malloc(30*sizeof(node_id_t));
  r = map->map[2].node_ids[1]+1;         // 6

  map->map[5].node_ids[0] = LC(r) -1;      // 12
  map->map[5].node_ids[1] = RC(r) -1;      // 13

  r = map->map[5].node_ids[0]+1; // 12
  map->map[5].node_ids[2] = LC(r)-1;       // 24
  map->map[5].node_ids[3] = RC(r)-1;       // 25

  r = map->map[5].node_ids[1]+1; // 13
  map->map[5].node_ids[4] = LC(r)-1;   // 26
  map->map[5].node_ids[5] = RC(r)-1;   // 27

  r = map->map[5].node_ids[2]+1; // 24
  map->map[5].node_ids[6] = LC(r)-1;       // 48
  map->map[5].node_ids[7] = RC(r)-1;       // 49
  map->map[5].node_ids[8] = LC(LC(r))-1;   // 96
  map->map[5].node_ids[9] = RC(LC(r))-1;   // 97
  map->map[5].node_ids[10] = LC(RC(r))-1;  // 98
  map->map[5].node_ids[11] = RC(RC(r))-1;  // 99 

  r = map->map[5].node_ids[3]+1; // 25
  map->map[5].node_ids[12] = LC(r)-1;       // 50
  map->map[5].node_ids[13] = RC(r)-1;       // 51
  map->map[5].node_ids[14] = LC(LC(r))-1;   // 100
  map->map[5].node_ids[15] = RC(LC(r))-1;   // 101
  map->map[5].node_ids[16] = LC(RC(r))-1;   // 102
  map->map[5].node_ids[17] = RC(RC(r))-1;   // 103

  r = map->map[5].node_ids[4]+1; // 26
  map->map[5].node_ids[18] = LC(r)-1;       // 52
  map->map[5].node_ids[19] = RC(r)-1;       // 53
  map->map[5].node_ids[20] = LC(LC(r))-1;   // 104
  map->map[5].node_ids[21] = RC(LC(r))-1;   // 105
  map->map[5].node_ids[22] = LC(RC(r))-1;   // 106
  map->map[5].node_ids[23] = RC(RC(r))-1;   // 107 

  r = map->map[5].node_ids[5]+1; // 27
  map->map[5].node_ids[24] = LC(r)-1;       // 54
  map->map[5].node_ids[25] = RC(r)-1;       // 55
  map->map[5].node_ids[26] = LC(LC(r))-1;   // 108
  map->map[5].node_ids[27] = RC(LC(r))-1;   // 109
  map->map[5].node_ids[28] = LC(RC(r))-1;   // 110
  map->map[5].node_ids[29] = RC(RC(r))-1;   // 111

  // Level 7
  map->map[6].num_nodes = 30;
  map->map[6].node_ids = (node_id_t*) malloc(30*sizeof(node_id_t));
  r = map->map[2].node_ids[2]+1;         // 7

  map->map[6].node_ids[0] = LC(r) -1;      // 14
  map->map[6].node_ids[1] = RC(r) -1;      // 15

  r = map->map[6].node_ids[0]+1; // 14
  map->map[6].node_ids[2] = LC(r)-1;       // 28
  map->map[6].node_ids[3] = RC(r)-1;       // 29

  r = map->map[6].node_ids[1]+1; // 15
  map->map[6].node_ids[4] = LC(r)-1;   // 30
  map->map[6].node_ids[5] = RC(r)-1;   // 31

  r = map->map[6].node_ids[2]+1; // 28
  map->map[6].node_ids[6] = LC(r)-1;       // 56
  map->map[6].node_ids[7] = RC(r)-1;       // 57
  map->map[6].node_ids[8] = LC(LC(r))-1;   // 112
  map->map[6].node_ids[9] = RC(LC(r))-1;   // 113
  map->map[6].node_ids[10] = LC(RC(r))-1;  // 114
  map->map[6].node_ids[11] = RC(RC(r))-1;  // 115

  r = map->map[6].node_ids[3]+1; // 29
  map->map[6].node_ids[12] = LC(r)-1;       // 58
  map->map[6].node_ids[13] = RC(r)-1;       // 59
  map->map[6].node_ids[14] = LC(LC(r))-1;   // 116
  map->map[6].node_ids[15] = RC(LC(r))-1;   // 117
  map->map[6].node_ids[16] = LC(RC(r))-1;   // 118
  map->map[6].node_ids[17] = RC(RC(r))-1;   // 119

  r = map->map[6].node_ids[4]+1; // 30
  map->map[6].node_ids[18] = LC(r)-1;       // 60
  map->map[6].node_ids[19] = RC(r)-1;       // 61
  map->map[6].node_ids[20] = LC(LC(r))-1;   // 120
  map->map[6].node_ids[21] = RC(LC(r))-1;   // 121
  map->map[6].node_ids[22] = LC(RC(r))-1;   // 122
  map->map[6].node_ids[23] = RC(RC(r))-1;   // 123 

  r = map->map[6].node_ids[5]+1; // 31
  map->map[6].node_ids[24] = LC(r)-1;       // 62
  map->map[6].node_ids[25] = RC(r)-1;       // 63
  map->map[6].node_ids[26] = LC(LC(r))-1;   // 124
  map->map[6].node_ids[27] = RC(LC(r))-1;   // 125
  map->map[6].node_ids[28] = LC(RC(r))-1;   // 126
  map->map[6].node_ids[29] = RC(RC(r))-1;   // 127
  
  return map;
}
