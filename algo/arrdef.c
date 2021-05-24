#include "arrdef.h"



	
	
void arrf100_init(arrf100_t* arr){
	arm_fill_f32(0,arr->v,100);
	arr->len = 0;
	arr->max_len = 100;
}
