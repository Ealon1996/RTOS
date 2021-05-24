/**
 * @file dsputil.c
 * @author edxian
 * @brief
 * @version 1.1
 * @date 2021-02-18
 *
 * @copyright Copyright (c) 2021
 *
 */


#include <dsputil.h>
#include <stdint.h>
#include <math.h>
#include <arrdef.h>
#include <stdio.h>
#include "spectrum.h"

float32_t sort_buffer[20] = {0.0f};

float32_t dsp_median_float32_t(float32_t data[], uint16_t n) {
	uint16_t ind = 0;
	arm_sort_instance_f32 sort_inst;
	sort_inst.alg = ARM_SORT_QUICK;
	sort_inst.dir = ARM_SORT_ASCENDING;
	arm_sort_f32(&sort_inst,data,sort_buffer,n);
	ind = (uint16_t)n/2;
	return (sort_buffer[ind] + sort_buffer[ind + 1]) / 2;   // to be revised
}

int16_t dsp_median_int16_t(int16_t data[], uint16_t n) {
  uint16_t ind = 0;
  arm_sort_instance_f32 sort_inst;
  sort_inst.alg = ARM_SORT_QUICK;
  sort_inst.dir = ARM_SORT_ASCENDING;
  arm_sort_f32(&sort_inst, (float32_t *) data, sort_buffer, n);  //to be revised 
  ind = (uint16_t)n / 2;
  return (sort_buffer[ind] + sort_buffer[ind + 1]) / 2;     //if the value is odd return the median otherwise return average
}


int16_t dsp_get_bpm(float32_t data[], int16_t lower_bound, int16_t upper_bound, int16_t fft_scale) {
	float32_t bpm = 0.0f;
	uint16_t bpm16 = 0;
	// int16_t max_index = 0;
	// int16_t i = 0;
	float32_t max_amp = 0.0f;
	uint32_t blocksize = 0;

	blocksize = upper_bound - lower_bound;
	arm_max_no_idx_f32(&data[lower_bound],blocksize,&max_amp);
	bpm = max_amp * 60 / ((float32_t)fft_scale*1.0f);
	bpm16 = round(bpm);
	return (int16_t)bpm16;
}


//
// window size
void dsp_hamming_window(float32_t data[], float32_t out[], uint32_t  window_size) {
	// the address of data and out should not be equal
	int i = 0;
	for (i=0;i<window_size;i++) {
		float32_t ind = 2 * PI * i / ((float32_t)window_size - 1);
		//float32_t ind =  PIx2 * i / ((float32_t)window_size - 1);
		out[i] = data[i]*(0.54347826f - 0.45652173f *arm_cos_f32(ind));   //25.0 / 46.0   21.0 / 46.0
		//( 0.54 - 0.46 * dsp.arm_cos_f32(2*np.pi * i / (n-1)))  python method
	}
	//the data is changed
}

float32_t dsp_dot_product(float32_t a[],float32_t b[],uint16_t n) {
	float32_t tmp = 0.0f;
	arm_dot_prod_f32(a,b,n,&tmp);
	return tmp;
}


float32_t dsp_mean(float32_t data[], uint16_t n) {
	float32_t meav_value = 0.0f;
	arm_mean_f32(data,n,&meav_value);
	return meav_value;
}


void dsp_clip_data(float32_t pSrc[],float32_t pDst[],float32_t low,float32_t high,uint16_t n) {
	for (uint32_t i = 0; i < n; i++) {
		if (pSrc[i] > high)
		pDst[i] = high;
		else if (pSrc[i] < low)
		pDst[i] = low;
		else
		pDst[i] = pSrc[i];
	}
}

void dsp_normalize(float32_t data[], uint16_t n) {
	float32_t min_val = 16777215.0f;//= PRESSURE_TYPE_MAX;
	float32_t max_val = -16777216.0f  ;//= PRESSURE_TYPE_MIN;
	float32_t scale;
	uint16_t ind;
	arm_max_f32(data,(uint32_t)n,&max_val,(uint32_t *)&ind);
	arm_min_f32(data,(uint32_t)n,&min_val,(uint32_t *)&ind);
	scale = 1/(max_val - min_val);
	arm_offset_f32(data,(-1.0*min_val),data,n);
	arm_scale_f32(data,scale,data,n);
	//the data will be normalized.
}


void dsp_normalize_peak() {
	
	
	
}

void dsp_nomralize_area(float32_t src[],float32_t dst[],uint32_t size) {
	float32_t sum = 0;
	for (uint32_t i=0; i<size; i++)
	{
		sum += src[i];
	}
	for (uint32_t i=0; i<size; i++)
	{
		dst[i] = src[i]/sum; 
	}
	
}

void dsp_reserve_freq_band(float32_t src[],float32_t dst[], uint32_t size,float32_t low_index,float32_t high_index,float32_t reser_value) {
	float32_t max_val;
	uint32_t max_ind;
	arm_max_f32(src,size,&max_val,&max_ind);
	for (uint32_t i=0;i<size;i++)
	{
		dst[i] = src[i]/max_val;
	}
	
}

void dsp_get_overlap(float32_t out[], float32_t data[], uint16_t window_size, int16_t overlap_weight[]) {
	/*
	int16_t i, j, k;
	float32_t max_amp = 0;
	for (i = 1; i <= OVERLAP_LIMIT; i++) {
		for (j = 0; j < BPM_SEARCH_UPPER + 20; j++) {
			max_amp = 0;
			for (k = i * j; k < (i * j + i); k++) {
				if (max_amp < data[k]) max_amp = data[k];      //clip data
			}
			out[j] += max_amp * overlap_weight[i - 1];         //dot product
		}
	}
	*/
}

void dsp_time_fusion(float32_t fusion_data_time[], float32_t bcg_data[], float32_t acc_data[], int16_t window_size) {
	int16_t i, j, start_shift = -5;
	float32_t max_bcg = 0, max_acc = 0, sim_curr, sim_next;
  float32_t bcg[320], acc[320];
  float32_t peak_value[3] = {0};
	int16_t peak_index[3] = {0};
	float32_t filtered_bcg[320];
	float32_t filtered_acc[320];
	uint8_t CORRELATION_BUFFER = 10;
	uint8_t MAX_PEAK_NUM = 3;
  int peak_count = 0;
	//fing maximum

	arm_max_no_idx_f32(filtered_bcg,window_size,&max_bcg);
	arm_max_no_idx_f32(filtered_acc,window_size,&max_acc);

	//scale
	arm_scale_f32(bcg_data,max_bcg,bcg,window_size);
	arm_scale_f32(acc_data,max_acc,acc,window_size);

	sim_curr = dsp_similarity(bcg+CORRELATION_BUFFER, acc+ CORRELATION_BUFFER+ start_shift, window_size - CORRELATION_BUFFER * 2);
	sim_next = dsp_similarity(bcg + CORRELATION_BUFFER, acc + CORRELATION_BUFFER + start_shift + 1, window_size - CORRELATION_BUFFER * 2);

	while (start_shift < 9) {
		if (sim_next < sim_curr) break;
		start_shift++;
		sim_curr = sim_next;
		sim_next = dsp_similarity(bcg + CORRELATION_BUFFER,
		acc + CORRELATION_BUFFER + start_shift + 1,
		window_size - CORRELATION_BUFFER * 2);
	}

	if (start_shift > 0) {
		memmove(acc, acc + start_shift, (window_size - start_shift) * sizeof(float));
		memset(acc + window_size - start_shift, 0, start_shift * sizeof(float));
	} else {
		memmove(acc - start_shift, acc, (window_size + start_shift) * sizeof(float));
		memset(acc, 0, -start_shift * sizeof(float));
	}

	for (i = 1; i < window_size - 1; i++) {
  	if (bcg[i] > bcg[i - 1] && bcg[i] > bcg[i + 1]) {
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
}

void dsp_get_bpm_final(float32_t data[], int16_t lower_bound,int16_t upper_bound, int16_t fft_scale,float32_t sigma) {
	/*
	if (stable_counter == 0) {
  	return -1;
	}
	int16_t i = 0;
	int16_t truth_peak_index = 0;
	int16_t last_peak_index = 0;
	pressure_type max_prob_val = 0;
	pressure_type peak_amp = 0;
	int16_t temp_list[STABLE_LIST_NUM] ={0};

	memcpy(temp_list, stable_list, sizeof(temp_list));
	last_peak_index = get_median(temp_list, stable_counter);
	last_peak_index = last_peak_index * fft_scale / 60;
	for (i = lower_bound; i < upper_bound; i++) {
  	peak_amp = pow(data[i], BDT_POWER) *
  	exp((-2 * pow(abs(i - last_peak_index) / sigma, 2)));
  	if (peak_amp > max_prob_val) {
    	max_prob_val = peak_amp;
    	truth_peak_index = i;
  	}
	}

	return truth_peak_index * 60 / fft_scale;
	*/
}

float32_t dsp_similarity(float32_t x[], float32_t y[], uint16_t windows_size) {
	volatile float32_t A = 0.0f;
	static volatile float32_t B = 0.0f;
	volatile float32_t C = 0.0f,D = 0.0f;

	arm_dot_prod_f32(x,y,windows_size,&A);     //SUM(x*y)
	arm_power_f32(x,(uint32_t)windows_size,&C);          //SUM(x*x)
	arm_power_f32(y,(uint32_t)windows_size,&D);		   //SUM(y*y)
	C = C*D;								   //SUM(x*x)*SUM(y*y)
	arm_sqrt_f32(C,&D);						   //sqrt(SUM(x*x)*SUM(y*y))
	//check D, if D is D is 0.
	B= A/D;
	return (float32_t)B;
}

/**
 * @brief Pearsonˇs Correlation
 *
 *
*/

float32_t dsp_correlation(float32_t x[], float32_t y[], uint16_t windows_size) {
	float32_t xmean = 0.0f, ymean = 0.0f;
	float32_t sumA = 0.0f;
	float32_t sumB = 0.0f;
	float32_t sumC = 0.0f;
	float32_t x_bar = 0.0f, y_bar = 0.0f;

	arm_mean_f32(x,windows_size,&xmean);
	arm_mean_f32(y,windows_size,&ymean);

	for (uint16_t i=0;i<windows_size;i++) {
		x_bar = x[i] - xmean;
		y_bar = y[i] - ymean;
		sumA += x_bar*y_bar;
		sumB += x_bar*x_bar;
		sumC += y_bar*y_bar;
	}
	//check sumB*sumC
	//arm_offset_f32(x,(-1.0*xmean),x,windows_size); // x-x_bar
	//arm_offset_f32(y,(-1.0*ymean),y,windows_size); // y-y_bar
	//bug this will modify the origin x y vector
	//B = dsp_similarity(x,y,windows_size);
	return sumA/sqrtf(sumB*sumC);
}

float32_t dsp_variance(float32_t data[],uint16_t n) {
	float32_t result = 0.0f;
	arm_var_f32(data,n,&result);
	return result;
}

float32_t dsp_std(float32_t data[], uint16_t n) {
  float32_t result = 0.0f;
  arm_std_f32(data,n,&result);
  return result;
}

float32_t dsp_rms(float32_t *data, uint16_t len) {
  float32_t result = 0.0f;
  arm_rms_f32 (data, len, &result);
  return result;
}

float32_t dsp_exp10(float32_t data) {
	//return exp10(data);
	//return 0.0;
}

float32_t dsp_exp2(float32_t data) {
	//return exp2(data);
	//return NULL;
}

float32_t dsp_expe(float32_t data) {
	//return expe(data);
	//return NULL;
}

void dsp_copy_f32(float32_t *a,float32_t *b, uint16_t len) {
	arm_copy_f32(a,b,len);
}


void dsp_mat_row(arm_matrix_instance_f32* data, float32_t* out, uint32_t row) {
	//arm_copy_f32
	uint32_t ind = row*data->numCols;
	arm_copy_f32((data->pData+ind),out,data->numCols);

}

void dsp_mat_col(arm_matrix_instance_f32* data, float32_t* out, uint32_t col) {
	uint32_t offset = data->pData + col;
	for (uint16_t i=0;i<data->numRows;i++) {
		out[i] = *(float32_t*)(offset + i*data->numCols*sizeof(float32_t));
	}
}

//static float32_t filter_sig[HAMM_WINDOW_SIZE] = {0.0f};
//static float32_t cfft_buffer[2048] = {0.0f};
static arm_cfft_radix2_instance_f32 fft_s;

uint32_t stft_window_size = 0;

void Dsp_Stft_Init(void){

	stft_window_size = STFT_SAMPLES;

//	arm_cfft_radix2_init_f32(&fft_s, STFT_SAMPLES, 0, 1);

}

uint32_t addr1 = 0;
uint32_t addr2 = 0;
void dsp_dstft(stft_arry_list_t* arr,float32_t* _signal, uint32_t size){
		float32_t* filter_sig;
		float32_t* cfft_buffer;
		
#if USE_OS == 1
		cfft_buffer = (float32_t*)pvPortMalloc(sizeof(float32_t)*size*4); //2048
		filter_sig = (float32_t*)pvPortMalloc(sizeof(float32_t)*size);  //512
#elif USE_OS ==2
		cfft_buffer = (float32_t*)malloc(sizeof(float32_t)*size*4); //2048
		filter_sig = (float32_t*)malloc(sizeof(float32_t)*size);  //512
#endif
		
		
		
#if PLATFORM == 0
		arm_cfft_radix2_instance_f32 fft;
		arm_cfft_radix2_init_f32(&fft,size,0,1);
#else
		arm_cfft_instance_f32 fft;
		arm_cfft_init_f32(&fft,size);
#endif
	
	
		dsp_hamming_window(_signal,filter_sig,size);  //if no overlap->  windowsize // if overlap windowsize-overlap
		//overlap should not be greater than window size
		for(uint32_t i = 0; i < stft_window_size; i++){
			cfft_buffer[i * 2] = filter_sig[i];
			cfft_buffer[i * 2 + 1] = 0;
		}
		
#if PLATFORM == 0		
		arm_cfft_radix2_f32(&fft,cfft_buffer);  //cffbuffer = 1024
#else		
		arm_cfft_f32(&fft,cfft_buffer,0,1);
#endif		

		arm_cmplx_mag_f32(cfft_buffer,&cfft_buffer[sizeof(float32_t)*size*2],size);
		arm_copy_f32(&cfft_buffer[sizeof(float32_t)*size*2],arr->col,(int32_t)(size/2));  // cfft is symmetric
		
		
#if USE_OS == 1
		vPortFree(filter_sig);
		vPortFree(cfft_buffer);
#elif USE_OS ==2
		free(filter_sig);
		free(cfft_buffer);
#endif
}

float32_t c_buffer[2048];
float32_t f_sig[1024];

void dsp_stft(float32_t* sig,float32_t* result,uint32_t size){


#if PLATFORM == 0
	arm_cfft_radix2_instance_f32 fft;
	arm_cfft_radix2_init_f32(&fft,size,0,1);
#else
	arm_cfft_instance_f32 fft;
	arm_cfft_init_f32(&fft,size);
#endif
	//memset(c_buffer,0,);
	arm_fill_f32(0,c_buffer,2048);
	arm_fill_f32(0,f_sig,1024);
	dsp_hamming_window(sig,f_sig,size);  //if no overlap->  windowsize // if overlap windowsize-overlap
	//overlap should not be greater than window size
	for(uint32_t i = 0; i < size; i++){
		c_buffer[i * 2] = f_sig[i];
		c_buffer[i * 2 + 1] = 0;
	}
	
	#if PLATFORM == 0
		arm_cfft_radix2_f32(&fft,c_buffer);  //cffbuffer = 1024
	#else
		arm_cfft_f32(&fft,c_buffer,0,1);
	#endif

	arm_cmplx_mag_f32(c_buffer,&c_buffer[sizeof(float32_t)*size*2],size);
	arm_copy_f32(&c_buffer[sizeof(float32_t)*size*2],result,(int32_t)(size/2));  // cfft is symmetric
}




void dsp_exclude_abnormal_peak(float32_t acc_sig[], uint16_t len) {
  float32_t mean_value = 0.0f, mean_peak_value = 0.0f;
  uint32_t peak_width = 1, peak_list_len = 0;
  uint32_t *peak_list_index;
  float32_t *peak_list_value;
  peak_node_t *over_peak_list_head = 0, *over_peak_list_tail = 0;
  peak_node_t *content;
  float32_t max_value = 0.0f, value = 0.0f;
  int32_t upper = 0, lower = 0;

  arm_mean_f32(acc_sig,len,&mean_value);
  arm_offset_f32(acc_sig, -mean_value, acc_sig, len);
  arm_abs_f32(acc_sig, acc_sig, len);
  find_peak(acc_sig, len, peak_width, peak_list_index, peak_list_value);
  arm_mean_f32(peak_list_value,sizeof(peak_list_value) / sizeof(peak_list_value[0]),&mean_peak_value);

  for (uint16_t i = 0; i < len; i ++) {
    // Need to revise
    if (acc_sig[i] > mean_peak_value * 2.0f) {
		
#if USE_OS == 1
      content = pvPortMalloc(sizeof(peak_list_t));
#elif USE_OS == 2
	  content = malloc(sizeof(peak_list_t));
#endif  
	  
      content->ind = i;
      content->value = acc_sig[i];
      content->pre = 0;
      content->next = 0;
      if (over_peak_list_head == 0) {
        over_peak_list_head = content;
        over_peak_list_tail = content;
      } else {
        over_peak_list_tail->next = content;
        content->pre = over_peak_list_tail;
        over_peak_list_tail = content;
      }
    }
  }

  while(over_peak_list_head != 0) {
    max_value = over_peak_list_head->value;
    upper = over_peak_list_head->ind + 1;
    if ((over_peak_list_head->ind - 1) > 0) {
      lower = over_peak_list_head->ind - 1;
    }
    else {
      lower = 0;
    }
    value = max_value;
    while (acc_sig[upper] < value) {
      value = acc_sig[upper];
      upper += 1;
      if (upper >= len) {
        break;
      }
    }
    value = max_value;
    while (acc_sig[lower] < value) {
      value = acc_sig[lower];
      lower -= 1;
      if (lower <= 0) {
        break;
      }
    }
    for (uint32_t i = lower; i < upper; i ++) {
      acc_sig[i] *= (mean_peak_value / max_value);
    }
    over_peak_list_head = over_peak_list_head->next;
  }
}

/*
float32_t dsp_cosine_similarity(float32_t bcg_data[], float32_t acc_data[], uint16_t window_size) {
	float32_t denom_A = 0.0f, denom_B = 0.0f, num = 0.0f;

  for (int i = 0; i < window_size; i++) {
		float32_t var_bcg = bcg_data[i] - 0.5;
		float32_t var_acc = acc_data[i] - 0.5;
		denom_A += var_bcg * var_bcg;
		denom_B += var_acc * var_acc;
		num += var_bcg * var_acc;
	}

	if (denom_A == 0.0f || denom_B == 0.0f) {  //denominator should not be 0.
		num = -1;
	} else {
		num /= sqrt(denom_A * denom_B);
		//num = num/dsp_invsqrt(denom_A*denom_B);
	}
	return (float)(0.5 * num + 0.5);
}
*/

/*
int16_t dsp_get_median(float32_t* data, uint16_t N) {
	 int16_t i = 0;

   //bubble sort
	 for (i = 0; i <= (N / 2); i++) {
		 int16_t j = 0;
		 for (j = i; j < N; j++) {
			 if (a[j] < a[i]) {
				 int16_t temp = a[j];
				 a[j] = a[i];
				 a[i] = temp;
			 }
		 }
	 }

   if (N % 2 == 0) {
		 return (a[(N / 2) - 1] + a[N / 2]) / 2;
	 } else {
		 return a[N / 2];
	 }
	return 0;
}
*/

// This method would be a little worse, now use ARM DSP library to get and remove all references.
/*float32_t dsp_invsqrt(float32_t number) {
	long i = 0;
	float x2 = 0.0f, y = 0.0f;
	const float threehalfs = 1.5f;
	x2 = number * 0.5f;
	y  = number;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );		// 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	return y;
}*/

/*
void stft(float amplitude[], float fft_in[], int window_size) {
	int i = 0;
	for(i = 0; i < window_size; i++) {
		fft_buffer[i * 2] = fft_in[i];
		fft_buffer[i * 2 + 1] = 0;
	}
	arm_cfft_radix2_f32(&fft_s, fft_buffer);
	arm_cmplx_mag_f32(fft_buffer + 1, amplitude, window_size / 2);
}
*/