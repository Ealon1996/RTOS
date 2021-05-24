#include "stdio.h"
#include "dsputil.h"
#include "stdint.h"
#include "arrdef.h"
#include "pconfig.h"
#include "math.h"
#ifndef __TIMEFUSION_H_
#define __TIMEFUSION_H_




float32_t* time_fusion(float32_t fusion_data_time[],
						float32_t bcg_data[],
						float32_t acc_data[],
						float32_t filtered_bcg[],
						float32_t filtered_acc[],
						int16_t window_size
						,uint8_t flag);
				
				
void get_time_fusion_spectrum(float32_t ss_denoise[],
								float32_t p_sensor_data[],
								float32_t g_sensor_data[],
								float32_t output[]);

#endif


