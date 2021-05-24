#include "stdint.h"
#include "arm_math.h"
#include "pconfig.h"
#ifndef _ADDDEF_H_
#define _ADDDEF_H_



// common array type
typedef struct  arrayf10{
	float32_t v[10];	
	uint32_t max_len;
	uint32_t len;
}arrf10_t;


typedef struct  arrayf100{
	float32_t v[100];
	uint32_t max_len;
	uint32_t len;


}arrf100_t;

// circular list for stft data array.
typedef struct  stft_arry_list{
	float32_t* col;
	uint32_t ind;
	uint32_t converted;
	uint8_t scenario;			
	struct stft_arry_list_t* prevwin;
	struct stft_arry_list_t* nextwin;
}stft_arry_list_t;

typedef struct peak_node {
  uint32_t ind;
  float32_t value;
  struct peak_node_t* pre;
  struct peak_node_t* next;
} peak_node_t;

typedef struct peak_list{
	peak_node_t* head;   //the pointer to the first element of list
	//peak_node_t* end;	 //the pointer to the end element of list
	uint32_t peak_len;
						//can add more members into structure if needed
}peak_list_t;

void arrf100_init(arrf100_t* arr);
void array_list_init(stft_arry_list_t* arr);

#endif