

#include "dsputil.h"
#include "stdint.h"

#ifndef __PCONFIG_H_
#define __PCONFIG_H_

#define PLATFORM 1    //0 MCU 1 simulation
#define USE_OS 1	  //0 NONOS 1 if use FreeRTOS 2. POSIX OS

#if PLATFORM == 0
	#include "atmel_start.h"   // only use if PLATFROM is MCU
#endif

//define sampling config
#define SAMPLE_RATE						(256)
#define DOWN_SAMPLE_RATE				(64)
#define ACC_LOG_RAW_SEC					(3)
#define PRE_LOG_RAW_SEC					(3)
#define ACC_LOG_FILTER_SEC					(3)
#define PRE_LOG_FILTER_SEC					(3)
#define STFT_LOG_SEC					(5)

//for FIR && IIR
#define P_FIR_TAPS  100
#define P_FIR_LEN  32
#define A_FIR_TAPS  100
#define A_FIR_LEN  32

//STFT & FFT
#define STFT_SAMPLES					(1024)  //1024
#define STFT_BLOCK_SAMPLES				(255)   //(1025/4-1)
#define FREQ_BIN_SIZE                   (512)   //should be half of SAMPLES LENGTH
#define STFT_SEG						(6)

#define NON_OVERLAP_BLOCK				(50)   
#define SHIFT							(204)   // shift = 
/*
SHIFT = 1024*(1-overlape_rate)  Blcok = (SHIFT)/4 - 1

RATE			SHIFT			Blcok	accurate

90%				100				25		98%	approximate
80%				204				50		99% approximate
75%				256				63		100% 
50%				512				127		100%
25%				768				191		100%

*/



#define TIMEFUSION_WINDOW_SEG 320

//#define TAPS   100               ///< TAPS num of fir filter
//#define FFT_WINDOW_SIZE 2048     

#define MAX_OVERLAP  7
#define ITERATION_TIMES 1
#define BDT_WEIGHT 0.4



#define p_stft_bin 512
#define p_stft_window 6
#define p_stft_time_seg 6

#define a_stft_bin 512
#define a_stft_window 6
#define a_stft_time_seg 6

//Threshold
#define ALS_THRESHOLD  4
#define ENGINE_THRESHOLD 2

#define RELIABLE_THRESHOLD 0.75

#define RESERVED_FREQ_LOWER_BOUND 1
#define RESERVED_FREQ_UPPER_BOUND 1

//The follwing definition is for get_scenario()
#define HALF_RANGE        2
#define SCENARIO_NUM      6
#define TRANS_FUNC_TIME_LEN   6                                                 // Unit: STFT time segment
#define NOBODY_THRESHOLD 5000
#define SITTING_THRESHOLD 3000000
#define DRIVING_THRESHOLD 10
#define IDLE_THRESHOLD  0.8f




#endif
