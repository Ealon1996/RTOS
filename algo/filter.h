#include "stdint.h"
#include "dsputil.h"
#include "arm_math.h"
#include "pconfig.h"
#include "sample.h"

#ifndef __FILTER_H_
#define __FILTER_H_

typedef struct filtered_dsdata{
	sensor_data_t seg[4];
}filtered_dsdata_t;

typedef struct downsample_data{
	sensor_data_t seg[4];
}downsample_data_t;


typedef struct stft_data{
	float32_t bin[512];
}stft_data_t;

static arm_fir_instance_f32 a_fir_instance;
static arm_fir_instance_f32 p_fir_instance;


static float32_t p_fir_State[P_FIR_LEN + P_FIR_TAPS - 1];


static float32_t ax_fir_State[A_FIR_LEN + A_FIR_TAPS - 1];
static float32_t ay_fir_State[A_FIR_LEN + A_FIR_TAPS - 1];
static float32_t az_fir_State[A_FIR_LEN + A_FIR_TAPS - 1];


/**
 * @brief taps for pressure filter
 * from scipy import signal
 * numtaps = 100
 * f1, f2, fs= 0.5, 30, 64
 * signal.firwin(100,[0.5, 30],pass_zero=False,fs=64)
 * # CMSIS DSP VS PYTHON SCIPY
 * signalx = np.cos(2 * np.pi * np.arange(nb) / nb)   \n
 * a = signal.firwin(100,[0.5, 30],pass_zero=False,fs=64) \n
 * b = np.array([1]) \n
 * filtered_x = signal.lfilter(a,b, signalx) \n
 * firf32 = dsp.arm_fir_instance_f32() \n
 * firstate = np.zeros(419) \n
 * dsp.arm_fir_init_f32(firf32,100,a,firstate) \n
 * firx = dsp.arm_fir_f32(firf32,signalx) \n
 * plot(filtered_x - firx) \n
 * show() \n
 */

static float32_t p_firCoeffs[]={
	    1.56141352e-04, -8.94062683e-04,  1.51843822e-04, -1.03540716e-03,
        6.19711282e-05, -1.17055221e-03, -1.73804838e-04, -1.25139330e-03,
       -6.37806327e-04, -1.21638898e-03, -1.40773587e-03, -1.02177456e-03,
       -2.52062273e-03, -6.77740671e-04, -3.93730746e-03, -2.78421692e-04,
       -5.51918872e-03, -1.42577924e-05, -7.02766634e-03, -1.58136091e-04,
       -8.15241144e-03, -1.02226120e-03, -8.56808204e-03, -2.88979784e-03,
       -8.01182998e-03, -5.93230832e-03, -6.36769194e-03, -1.01290755e-02,
       -3.74044116e-03, -1.52060223e-02, -5.01967248e-04, -2.06091699e-02,
        2.70166334e-03, -2.55201715e-02,  4.98466422e-03, -2.89095695e-02,
        5.28545159e-03, -2.96063144e-02,  2.42680245e-03, -2.63341215e-02,
       -4.90140717e-03, -1.76004353e-02, -1.84253985e-02, -1.10064794e-03,
       -4.14984378e-02,  2.88047623e-02, -8.48099097e-02,  9.60658424e-02,
       -2.18044487e-01,  6.17278294e-01,  6.17278294e-01, -2.18044487e-01,
        9.60658424e-02, -8.48099097e-02,  2.88047623e-02, -4.14984378e-02,
       -1.10064794e-03, -1.84253985e-02, -1.76004353e-02, -4.90140717e-03,
       -2.63341215e-02,  2.42680245e-03, -2.96063144e-02,  5.28545159e-03,
       -2.89095695e-02,  4.98466422e-03, -2.55201715e-02,  2.70166334e-03,
       -2.06091699e-02, -5.01967248e-04, -1.52060223e-02, -3.74044116e-03,
       -1.01290755e-02, -6.36769194e-03, -5.93230832e-03, -8.01182998e-03,
       -2.88979784e-03, -8.56808204e-03, -1.02226120e-03, -8.15241144e-03,
       -1.58136091e-04, -7.02766634e-03, -1.42577924e-05, -5.51918872e-03,
       -2.78421692e-04, -3.93730746e-03, -6.77740671e-04, -2.52062273e-03,
       -1.02177456e-03, -1.40773587e-03, -1.21638898e-03, -6.37806327e-04,
       -1.25139330e-03, -1.73804838e-04, -1.17055221e-03,  6.19711282e-05,
       -1.03540716e-03,  1.51843822e-04, -8.94062683e-04,  1.56141352e-04            
};

/**
 * @brief taps for acc filter
 * cost 100*4 bytes
*/ 
static float32_t a_firCoeffs[]={
	1.56141352e-04, -8.94062683e-04,  1.51843822e-04, -1.03540716e-03,
	6.19711282e-05, -1.17055221e-03, -1.73804838e-04, -1.25139330e-03,
	-6.37806327e-04, -1.21638898e-03, -1.40773587e-03, -1.02177456e-03,
	-2.52062273e-03, -6.77740671e-04, -3.93730746e-03, -2.78421692e-04,
	-5.51918872e-03, -1.42577924e-05, -7.02766634e-03, -1.58136091e-04,
	-8.15241144e-03, -1.02226120e-03, -8.56808204e-03, -2.88979784e-03,
	-8.01182998e-03, -5.93230832e-03, -6.36769194e-03, -1.01290755e-02,
	-3.74044116e-03, -1.52060223e-02, -5.01967248e-04, -2.06091699e-02,
	2.70166334e-03, -2.55201715e-02,  4.98466422e-03, -2.89095695e-02,
	5.28545159e-03, -2.96063144e-02,  2.42680245e-03, -2.63341215e-02,
	-4.90140717e-03, -1.76004353e-02, -1.84253985e-02, -1.10064794e-03,
	-4.14984378e-02,  2.88047623e-02, -8.48099097e-02,  9.60658424e-02,
	-2.18044487e-01,  6.17278294e-01,  6.17278294e-01, -2.18044487e-01,
	9.60658424e-02, -8.48099097e-02,  2.88047623e-02, -4.14984378e-02,
	-1.10064794e-03, -1.84253985e-02, -1.76004353e-02, -4.90140717e-03,
	-2.63341215e-02,  2.42680245e-03, -2.96063144e-02,  5.28545159e-03,
	-2.89095695e-02,  4.98466422e-03, -2.55201715e-02,  2.70166334e-03,
	-2.06091699e-02, -5.01967248e-04, -1.52060223e-02, -3.74044116e-03,
	-1.01290755e-02, -6.36769194e-03, -5.93230832e-03, -8.01182998e-03,
	-2.88979784e-03, -8.56808204e-03, -1.02226120e-03, -8.15241144e-03,
	-1.58136091e-04, -7.02766634e-03, -1.42577924e-05, -5.51918872e-03,
	-2.78421692e-04, -3.93730746e-03, -6.77740671e-04, -2.52062273e-03,
	-1.02177456e-03, -1.40773587e-03, -1.21638898e-03, -6.37806327e-04,
	-1.25139330e-03, -1.73804838e-04, -1.17055221e-03,  6.19711282e-05,
	-1.03540716e-03,  1.51843822e-04, -8.94062683e-04,  1.56141352e-04
};

void p_fir_filter_init(void);
void p_fir_filter(float32_t intput[], float32_t output[],uint16_t len);
void ax_fir_filter_init(void);
void ax_fir_filter(float32_t intput[], float32_t output[],uint16_t len);
void ay_fir_filter_init(void);
void ay_fir_filter(float32_t intput[], float32_t output[],uint16_t len);
void az_fir_filter_init(void);
void az_fir_filter(float32_t intput[], float32_t output[],uint16_t len);
// void a_fir_filter_init(void);
// void a_fir_filter(float32_t intput[], float32_t output[],uint16_t len);

void dsp_task(void *p);
void p_array_list_init(stft_arry_list_t* arr,float32_t matrix[][FREQ_BIN_SIZE]);
void stft_task_init();
void stft_task(void *p);


#endif