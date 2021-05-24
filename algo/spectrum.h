

#include "stdint.h"
#include "dsputil.h"
#include "pconfig.h"
#include "arrdef.h"

#ifndef __SPECTRUM_H_
#define __SPECTRUM_H_



void find_peak(float32_t* arr,uint32_t blocksize,uint32_t width,uint32_t *peak_list_index, float32_t* peak_list_value);
void peak_list_init(peak_list_t* list);
peak_list_t* create_peak(float32_t value, uint32_t ind);
void add_peak_node(peak_list_t* list,float32_t value,uint32_t ind);
void delete_peak_node(peak_list_t* list,uint32_t ind);
void delete_peak_node_by_id(peak_list_t* list,uint32_t id);
#endif






