#include "timefusion.h"


#define CORRELATION_BUFFER (10)
#define START_SHIFT     (-5)
#define MAX_PEAK_NUM (3)
#define TIMEFUSION_STFT_LEN 256

static float32_t bcg[TIMEFUSION_WINDOW_SEG]= {0}, acc[TIMEFUSION_WINDOW_SEG]= {0};
static float32_t peak_value[MAX_PEAK_NUM] = {0};
static int16_t peak_index[MAX_PEAK_NUM] = {0};

static float32_t tf_stft[TIMEFUSION_STFT_LEN];
static float32_t ssnoise[101];

typedef struct time_fusion_instance{	
	
}time_fusion_instance_t;

static uint32_t win_size;

float32_t* time_fusion(		float32_t fusion_data_time[], 
							float32_t bcg_data[],
							float32_t acc_data[],
							float32_t filtered_bcg[],
							float32_t filtered_acc[],
							int16_t window_size,
							uint8_t flag
)
				 
 {
	//clear buffer

	stft_arry_list_t  tf_stft_arr;
	//init stft
	tf_stft_arr.nextwin = NULL;
	tf_stft_arr.nextwin = NULL;
	tf_stft_arr.col = tf_stft;
	memset(bcg,0,TIMEFUSION_WINDOW_SEG);
	memset(acc,0,TIMEFUSION_WINDOW_SEG);
	memset(peak_value,0,MAX_PEAK_NUM);
	memset(peak_index,0,MAX_PEAK_NUM);

	int16_t i, j, start_shift = START_SHIFT;
	float32_t max_bcg = 0, max_acc = 0, sim_curr, sim_next;
	for (i = 0; i < window_size; i++) {
		if(max_bcg < filtered_bcg[i]) max_bcg = filtered_bcg[i];
		if(max_acc < filtered_acc[i]) max_acc = filtered_acc[i];
	}
	for (i = 0; i < window_size; i++) {
		bcg[i] = bcg_data[i] / max_bcg;
		acc[i] = acc_data[i] / max_acc;
	}
	sim_curr = dsp_similarity(bcg + CORRELATION_BUFFER,
							  acc + CORRELATION_BUFFER + start_shift,
							  (window_size - CORRELATION_BUFFER * 2) );

	sim_next = dsp_similarity(bcg + CORRELATION_BUFFER,
							  acc + CORRELATION_BUFFER + start_shift + 1,
							  window_size - CORRELATION_BUFFER * 2);

	while (start_shift < 9) {
		if (sim_next < sim_curr) break;
		start_shift++;
		sim_curr = sim_next;
		sim_next = dsp_similarity(bcg + CORRELATION_BUFFER,
								  acc + CORRELATION_BUFFER + start_shift + 1,
								  window_size - CORRELATION_BUFFER * 2);
	}
	if (start_shift > 0) {
		memmove(acc, acc + start_shift,
		(window_size - start_shift) * sizeof(float));
		memset(acc + window_size - start_shift, 0, start_shift * sizeof(float));
	} else {
		memmove(acc - start_shift, acc,
		(window_size + start_shift) * sizeof(float));
		memset(acc, 0, -start_shift * sizeof(float));
	}

	int peak_count = 0;
	//find peak
	for (i = 1; i < window_size - 1; i++) {
		if (bcg[i] > bcg[i - 1] && bcg[i] > bcg[i + 1]) {   //compare the sample points
			int insert_index = 0;
			if (peak_count < MAX_PEAK_NUM) peak_count++;
			for (j = 0; j < peak_count; j++) {
				if (bcg[i] > peak_value[j])
					break;
				else
					insert_index++;
			}
			if (insert_index < 3) {
				memmove(peak_value + insert_index + 1, peak_value + insert_index,
				(MAX_PEAK_NUM - insert_index - 1) * sizeof(float));
				memmove(peak_index + insert_index + 1, peak_index + insert_index,
				(MAX_PEAK_NUM - insert_index - 1) * sizeof(int16_t));
				peak_value[insert_index] = bcg[i];
				peak_index[insert_index] = i;
			}
		}
	}
	max_bcg = 0;
	max_acc = 0;
	for (i = 0; i < MAX_PEAK_NUM; i++) {
		max_bcg += peak_value[i];
		max_acc += acc[peak_index[i]];
	}
	max_bcg /= MAX_PEAK_NUM;
	max_acc /= MAX_PEAK_NUM;
	
	for (i = 0; i < window_size; i++) {
		fusion_data_time[i] = (bcg[i] / max_bcg) - (acc[i] / max_acc);
	}
	//win_size = window_size;
	
	dsp_dstft(  &tf_stft_arr , fusion_data_time  ,window_size);
	
	//abs
	for (uint32_t i=0;i<window_size;i++)
	{
#if PLATFORM == 1
		tf_stft[i] = fabs(tf_stft[i]);
#else
		tf_stft[i] = absf(tf_stft[i]);
#endif
	}
	
	dsp_normalize(tf_stft,window_size);
	for (uint32_t i=0;i<100;i++)
	{
		ssnoise[i] =  tf_stft[i] ;
	}
	ssnoise[100]  = flag;
	return ssnoise;
}
