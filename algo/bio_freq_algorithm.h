

#include "pconfig.h"
#include "arrdef.h"
#include "dsputil.h"
#include "timefusion.h"
#include "spectrum.h"
#include "filter.h"


#if USE_OS==1
	#include "FreeRTOS.h"
#endif


#ifndef __BIO_FREQ_ALGORITHM_H
#define __BIO_FREQ_ALGORITHM_H

//status for scenario

typedef enum status{
	NOBODY = 0,
	STILL = 1,
	STILL_BODY_MOVE = 2,
	CAR_IDLE = 3,
	CAR_IDLE_BODY_MOVE = 4,
	CAR_DRIVING = 5,
	CAR_DRIVING_BODY_MOVE = 6	
} sitting_status;

uint8_t get_scenario(float filter_bcg_signal[], float filter_acc_signal[], float algo_acc_signal[], uint8_t acc_org_peak_ratio, uint16_t len);
float32_t* get_trans_func(stft_arry_list_t* p_stft_ptr, stft_arry_list_t* a_stft_ptr, uint8_t scenario);

void Algo_Task(void *p);

#endif
