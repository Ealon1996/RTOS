#include "dsputil.h"
#include "stdint.h"
#include "pconfig.h"
#ifndef __FATIGUE_H_
#define __FATIGUE_H_
 
 #define  average_window_fatigue 5
 #define  fatigue_counter 3
 #define  fatigue_threshold  98
 typedef struct algorithm_state{
	 int16_t bpm;
	 int16_t rpm;
	 int16_t bpm_pre;
	 uint8_t sitting_status;
 }algorithm_state_t;
 typedef struct fatigue_instance{
	 uint8_t len;
	 uint8_t counter;
	 int8_t HRS_Fatigue[10];
	 uint8_t fatigue;
 }fatigue_instance_t;

void dsp_fatigue_init(fatigue_instance_t* data);
uint8_t get_mean_u8(uint8_t *data,uint16_t len);
void dsp_fatigue_detect(fatigue_instance_t* ins, uint8_t hr);
 
#endif