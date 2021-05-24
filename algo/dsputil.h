/**
 * @file dsputil.h
 * @author edxian
 * @brief
 * @version 1.1
 * @date 2021-02-18
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __DSPUTIL_H_
#define __DSPUTIL_H_
#include <stdint.h>
#include "arm_math.h"


#include "pconfig.h"

#include "arrdef.h"
#define PIx2 6.28318530718


/**
 * @brief use dsp library to get median of a series
 * quick sort in recursive way
 * [in] @param data
 * [in] @param n
 * @return float32_t
 * @see arm_sort_f32()
 */
float32_t dsp_median_float32_t(float32_t data[], uint16_t n);

/**
 * @brief use dsp library to get median of a series
 * quick sort in recursive way
 * [in] @param a
 * [in] @param n
 * @return int16_t
 * @see arm_sort_f32()
 */
int16_t dsp_median_int16_t(int16_t data[], uint16_t n);


/**
 * @brief use dsp library to get bpm
 *
 * @param[in]  data
 * @param[in]  lower_bound
 * @param[in]  upper_bound
 * @param[in]  fft_scale
 * @return int16_t
 */
int16_t dsp_get_bpm(float32_t data[],int16_t lower_bound, int16_t upper_bound,int16_t fft_scale);


/**
 * @brief apply a window to a series
 *
 * @param[in]  data
 * @param[in]  out
 * @param[in]  window_size
 */
void dsp_hamming_window(float32_t data[],float32_t out[] ,uint32_t  window_size);


/**
 * @brief get inner product of two series
 *
 * @param[in]  a
 * @param[in]  b
 * @param[in]  n
 * @return float32_t
 */
float32_t dsp_dot_product(float32_t a[],float32_t b[],uint16_t n);

/**
 * @brief get mean of a series
 * @param[in]  data
 * @param[in]  n
 * @return float32_t
 */
float32_t dsp_mean(float32_t data[],uint16_t n);


/**
 * @brief filter data more than high and less than low
 * @param[in]  pSrc
 * @param[in]  pDst
 * @param[in]  low
 * @param[in]  high
 * @param[in]  n
 */
void dsp_clip_data(float32_t pSrc[],float32_t pDst[],float32_t low,float32_t high,uint16_t n);


/**
 * @brief normalize a series by maximum and minimum with DSP function
 *
 * @param[in,out]  data
 * @param[in]  n
 */
void dsp_normalize(float32_t data[],uint16_t n);


/**
 * @brief normalize a series by maximum and minimum
 *
 */
void dsp_normalize_peak();


/**
 * @brief normalize a series by area
 *
 */
//void dsp_nomralize_area();


/**
 * @brief remove magnitude of frequence more than upper_bound and less than lower_bound
 *
 */
void dsp_reserve_freq_band(float32_t src[],float32_t dst[], uint32_t size,float32_t low_index,float32_t high_index,float32_t reser_value);


/**
 * @brief get overlap frequency magnitude
 *
 * @param[in] out
 * @param[in] data
 * @param[in] windows_size
 * @param[in] overlap_weight
 */
void dsp_get_overlap(float32_t out[], float32_t data[], uint16_t window_size, int16_t overlap_weight[]);


/**
 * @brief get data after time fusion
 *
 * @param[in] fusion_data_time
 * @param[in] bcg_data
 * @param[in] acc_data
 * @param[in] window_size
 */
void dsp_time_fusion(float32_t fusion_data_time[], float32_t bcg_data[], float32_t acc_data[], int16_t window_size);


/**
 * @brief get final heart rate
 *
 * @param[in] data
 * @param[in] lower_bound
 * @param[in] upper_bound
 * @param[in] fft_scale
 * @param[in] sigma
 */
void dsp_get_bpm_final(float32_t data[], int16_t lower_bound,int16_t upper_bound, int16_t fft_scale,float32_t sigma);


/**
 * @brief get similarity of two series
 *
 * @param[in] x
 * @param[in] y
 * @param[in] windows_size
 * @return float
 */
float dsp_similarity(float32_t x[], float32_t y[], uint16_t windows_size);


/**
 * @brief get correlation of two series
 *
 * @param[in]  x
 * @param[in]  y
 * @param[in]  windows_size
 * @return float32_t
 */
float32_t dsp_correlation(float32_t x[], float32_t y[], uint16_t windows_size);


/**
 * @brief get the variance of a series
 *
 * @param[in]  data
 * @param[in]  n
 * @return float32_t
 */
float32_t dsp_variance(float32_t data[],uint16_t n);


/**
 * @brief get the standard deviation of a series
 *
 * @param[in]  data
 * @param[in]  n
 * @return float32_t
 */
float32_t dsp_std(float32_t data[], uint16_t n);


/**
 * @brief get the root mean square of a series
 * a.k.a sqrt( (data[0]^data[0]) + ... (data[len-1]^data[len-1]) )/len)
 * @param[in]  data
 * @param[in]  len
 * @return float32_t
 */
float32_t dsp_rms(float32_t *data, uint16_t len);


/**
 * @brief get the root mean square of a series
 * aka 10^data
 * @param[in]  data
 * @return float32_t
 */
float32_t dsp_exp10(float32_t data);


/**
 * @brief get the root mean square of a series
 * aka 2^data
 * @param[in]  data
 * @return float32_t
 */
float32_t dsp_exp2(float32_t data);


/**
 * @brief get the exponential value with base e.
 * aka e^data
 * @param[in]  data
 * @return float32_t
 */
float32_t dsp_expe(float32_t data);


/**
 * @brief copy data from series a to series b
 *
 * @param[in]  a
 * @param[in]  b
 * @param[in]  len
 */
void dsp_copy_f32(float32_t *a,float32_t *b, uint16_t len);


/**
 * @brief copy one row into series out
 *
 * @param[in]  data
 * @param[in]  out
 * @param[in]  row
 */
void dsp_mat_row(arm_matrix_instance_f32* data, float32_t* out, uint32_t row);


/**
 * @brief copy one column into series out
 *
 * @param[in]  data
 * @param[in]  out
 * @param[in]  col
 */
void dsp_mat_col(arm_matrix_instance_f32* data, float32_t* out, uint32_t col);


/**
 * @brief initialize parameters before STFT
 *
 */
void dsp_stft_init(void);


/**
 * @brief implement STFT by DSP function with dynamic malloc
 *
 * @param[in]  arr
 * @param[in]  _signal
 * @param[in]  size
 */
void dsp_dstft(stft_arry_list_t* arr,float32_t* _signal, uint32_t size);

/**
 * @brief implement STFT by DSP function with static memory.
 *
 * @param[in]  arr
 * @param[in]  _signal
 * @param[in]  size
 */
void dsp_stft(float32_t* sig,float32_t* result,uint32_t size);


/**
 * @brief implement STFT by DSP function with norm
 *
 */
void dsp_stft_norm();


/**
 * @brief exclude abnormal peaks which have large magnitudes by DSP function
 * @param[in] bcg_data
 * @param[in] acc_data
 * @param[in] window_size
 * @return float32_t
 */
void dsp_exclude_abnormal_peak(float32_t acc_sig[], uint16_t len);


/**
 * @brief get cosine similarity of two series
 * @param[in] bcg_data
 * @param[in] acc_data
 * @param[in] window_size
 * @return float32_t
 */
// float32_t dsp_cosine_similarity(float32_t bcg_data[],float32_t acc_data[],uint16_t window_size);


/**
 * @brief get median
 * @param[in] data
 * @param[in] N
 * @return int16_t
 */
// int16_t dsp_get_median(float32_t* data, uint16_t N);


/**
 * @brief fast inverse suqare root a.k.a 1/sqrt(x)
 *
 * @param[in] number
 * @return float32_t
 */
// float32_t dsp_invsqrt(float32_t number);


/**
 * @brief get STFT
 * @param[in] amplitude
 * @param[in] fft_in
 * @param[in] window_size
 * @return int16_t
 */
// void stft(float amplitude[], float fft_in[], int window_size);
#endif