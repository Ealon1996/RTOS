#include "fatigue.h"



void dsp_fatigue_init(fatigue_instance_t* data){
	memset(data->HRS_Fatigue,0,5);
	data->counter = 0;
	data->fatigue = 0;
	data->len = 0;
}

uint8_t get_mean_u8(uint8_t *data,uint16_t len){
	uint8_t result;
	int16_t sumx = 0, i;
	// int16_t cnt = 0;
	for (i=0;i<len;i++)
	{
		sumx += data[i];
	}
	result = sumx/len;
	return result;
}




void dsp_fatigue_detect(fatigue_instance_t* ins, uint8_t hr){
	uint8_t mean = 0;  //int8_t  
	ins->fatigue = 0;
	ins->HRS_Fatigue[ins->len] = hr;
	ins->len++;
	if((ins->len+1)>average_window_fatigue)
	{
		mean = get_mean_u8(ins->HRS_Fatigue,5);
		ins->len = 0;
		if (mean <= fatigue_threshold)
		{
			ins->counter++;
			}else{
			ins->counter = 0;
		}
		if(ins->counter>= fatigue_counter){
			ins->fatigue = 1;
		}
		ins->len = 0;
	}
}
