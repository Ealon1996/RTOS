#include <math.h>
#include "bio_freq_algorithm.h"


extern QueueHandle_t xQueue_pstft_cplt;
extern QueueHandle_t xQueue_astft_cplt;
extern QueueHandle_t xQueue_downsample_data;

stft_arry_list_t* p_stft_ptr = NULL;  //p sensor
stft_arry_list_t* a_stft_ptr = NULL;  //g sensor
stft_arry_list_t* f_stft_ptr = NULL;  //fusion sensor

//warnning : STFT_SEG should be as low as possible otherwise cause the memory overflow.  
stft_arry_list_t p_stft_arr[STFT_SEG];			       //for p senor
float32_t p_stft_matrix[STFT_SEG][FREQ_BIN_SIZE]={0};

stft_arry_list_t a_stft_arr[STFT_SEG];
float32_t a_stft_matrix[STFT_SEG][FREQ_BIN_SIZE]={0};  //for g senor

stft_arry_list_t fusion_stft_arr[STFT_SEG];
float32_t fusion_stft_matrix[STFT_SEG][FREQ_BIN_SIZE]={0};  //for fusion

filtered_dsdata_t recv_dsdata;
stft_data_t recv_stft_data;
float32_t ds_acc_data[1024];  // downsample data
float32_t ds_pre_data[1024];  // 


void Algo_Init(){
	p_stft_ptr = (stft_arry_list_t*)p_stft_arr;
	a_stft_ptr = (stft_arry_list_t*)a_stft_arr;
	f_stft_ptr = (stft_arry_list_t*)fusion_stft_arr;
	p_array_list_init(p_stft_ptr,p_stft_matrix);
	p_array_list_init(a_stft_ptr,a_stft_matrix);
	p_array_list_init(f_stft_ptr,fusion_stft_matrix);

}


void move_stft_window(stft_arry_list_t* stft){
	stft = stft->nextwin;
}

uint8_t get_scenario(float filter_bcg_signal[], float filter_acc_signal[], float algo_acc_signal[], uint8_t acc_org_peak_ratio, uint16_t len) {
  uint8_t result;
  static uint8_t result_list[HALF_RANGE * 2 + 1] = {0};
  static uint8_t result_list_num = 0, result_list_index = 0;
  float32_t down_acc_power = 0.0f, algo_acc_signal_power = 0.0f, bcg_mean_power = 0.0f, bcg_mean_power_temp = 0.0f;
  static float32_t down_acc_power_list[STFT_SEG] = {0.0f}, down_acc_power_list_temp[STFT_SEG] = {0.0f}, bcg_mean_power_list[STFT_SEG] = {0.0f}, power_diff_list[STFT_SEG] = {0.0f};
  static uint8_t down_acc_power_list_num = 0, down_acc_power_list_index = 0;
  uint8_t down_acc_power_max_index = 0;
  float32_t down_acc_power_max_value = 0.0f;
  float32_t motion_threshold[SCENARIO_NUM / 2] = {0.0f};
  uint8_t scenario_counter[SCENARIO_NUM / 2] = {0};
  float32_t scenario_power_diff[SCENARIO_NUM / 2] = {0.0f, 0.0f, 0.0f};
  dsp_exclude_abnormal_peak(filter_acc_signal, len);
  dsp_exclude_abnormal_peak(algo_acc_signal, len);

  arm_mean_f32(filter_acc_signal, len, &down_acc_power);
  arm_mean_f32(algo_acc_signal, len, &algo_acc_signal_power);
  arm_mean_f32(filter_bcg_signal, len, &bcg_mean_power);
  arm_abs_f32(&bcg_mean_power, &bcg_mean_power, 1);
  arm_abs_f32(filter_bcg_signal, filter_bcg_signal, len);
  arm_mean_f32(filter_bcg_signal, len, &bcg_mean_power_temp);
  bcg_mean_power = bcg_mean_power_temp - bcg_mean_power;

  down_acc_power_list[down_acc_power_list_index] = down_acc_power;
  down_acc_power_list_temp[down_acc_power_list_index] = down_acc_power;
  bcg_mean_power_list[down_acc_power_list_index] = bcg_mean_power;


  if (down_acc_power > DRIVING_THRESHOLD) {
    if (algo_acc_signal_power < IDLE_THRESHOLD) {
      result = 1;
    } else {
      result = 5;
    }
  } else if (down_acc_power > IDLE_THRESHOLD) {
    if (acc_org_peak_ratio == 1) {
      result = 3;
    } else {
      result = 1;
    }
  } else {
    result = 1;
  }
  result_list[result_list_index] = result;
  result_list_index = (result_list_index + 1) % (HALF_RANGE * 2 + 1);
  if (result_list_num != HALF_RANGE * 2 + 1) {
    result_list_num += 1;
  }
  result = dsp_median_int16_t(result_list, result_list_num);
  arm_max_f32(down_acc_power_list, down_acc_power_list_num, &down_acc_power_max_value, &down_acc_power_max_index);
  while (down_acc_power_list[down_acc_power_max_index] != 0 && bcg_mean_power_list[down_acc_power_max_index] == 0) {
    down_acc_power_list[down_acc_power_max_index] = 0;
    arm_max_f32(down_acc_power_list, down_acc_power_list_num, &down_acc_power_max_value, &down_acc_power_max_index);
  }

  if (bcg_mean_power_list[down_acc_power_max_index] == 0 || down_acc_power_list[down_acc_power_max_index] == 0) {
    power_diff_list[down_acc_power_max_index] = 0;
  } else {
    power_diff_list[down_acc_power_max_index] = bcg_mean_power_list[(uint8_t)((STFT_SEG + 1) / 2) - 1] / bcg_mean_power_list[down_acc_power_max_index] - down_acc_power_list[(uint8_t)((STFT_SEG + 1) / 2) - 1] / down_acc_power_list[down_acc_power_max_index];
  }

  for (uint8_t i = 0; i < STFT_SEG; i ++) {
    if (power_diff_list[i] < 0) {
      power_diff_list[i] = 0;
    }
  }

  for (uint8_t i = 0; i < STFT_SEG; i ++) {
    switch (result_list[i]) {
      case 1:
        scenario_counter[0] += 1;
        scenario_power_diff[0] += power_diff_list[0];
        break;
      case 3:
        scenario_counter[1] += 1;
        scenario_power_diff[1] += power_diff_list[1];
        break;
      case 5:
        scenario_counter[2] += 1;
        scenario_power_diff[2] += power_diff_list[2];
        break;
    }
  }

  for (uint8_t i = 0; i < SCENARIO_NUM / 2; i ++) {
    if (scenario_counter[i] == 0) {
      motion_threshold[i] = 1;
    } else {
      motion_threshold[i] = scenario_power_diff[i] / scenario_counter[i];
      if (motion_threshold[i] < 0.025) {
        motion_threshold[i] = 0.025;
      }
    }
  }

  switch (result_list[(uint8_t)((STFT_SEG + 1) / 2) - 1]) {
    case 1:
      if (power_diff_list[(uint8_t)((STFT_SEG + 1) / 2) - 1] > motion_threshold[0]) {
        result_list[(uint8_t)((STFT_SEG + 1) / 2) - 1] += 1;
      }
      break;
    case 3:
      if (power_diff_list[(uint8_t)((STFT_SEG + 1) / 2) - 1] > motion_threshold[1]) {
        result_list[(uint8_t)((STFT_SEG + 1) / 2) - 1] += 1;
      }
      break;
    case 5:
      if (power_diff_list[(uint8_t)((STFT_SEG + 1) / 2) - 1] > motion_threshold[2]) {
        result_list[(uint8_t)((STFT_SEG + 1) / 2) - 1] += 1;
      }
      break;
  }
  down_acc_power_list_index = (down_acc_power_list_index + 1) % STFT_SEG;
  if (down_acc_power_list_num != STFT_SEG) {
    down_acc_power_list_num += 1;
  }
  memcpy(down_acc_power_list, down_acc_power_list_temp, sizeof(down_acc_power_list) / sizeof(down_acc_power_list[0]));
  result = result_list[(uint8_t)((STFT_SEG + 1) / 2) - 1];
  return result;

}

float32_t* get_trans_func(stft_arry_list_t* p_stft_ptr, stft_arry_list_t* a_stft_ptr, uint8_t scenario) {
  float32_t driving_sum[FREQ_BIN_SIZE] = {0.0f}, driving_acc_sum[FREQ_BIN_SIZE] = {0.0f};

  memset(driving_sum, 0, sizeof(driving_sum));
  memset(driving_acc_sum, 0, sizeof(driving_sum));

  for (uint8_t i = 0; i < TRANS_FUNC_TIME_LEN; i ++) {
    if (p_stft_ptr != NULL) {
      if (p_stft_ptr->scenario == 5) {
        arm_add_f32(p_stft_ptr->col, driving_sum, driving_sum, FREQ_BIN_SIZE);
        arm_add_f32(a_stft_ptr->col, driving_acc_sum, driving_acc_sum, FREQ_BIN_SIZE);
      }
    }
    p_stft_ptr = p_stft_ptr->prevwin;
    a_stft_ptr = a_stft_ptr->prevwin;
  }

  for (uint8_t i = 0; i < FREQ_BIN_SIZE; i ++) {
    if (driving_sum[i] == 0) {
      driving_sum[i] = 1.0f;
    }
    if (driving_acc_sum[i] == 0) {
      driving_acc_sum[i] = 1.0f;
    }
    driving_sum[i] = driving_sum[i] / driving_acc_sum[i];
  }
  return driving_sum;
}

 void Algo_Task(void *p)
 {
	 (void)p;
	 stft_arry_list_t* stft_;   //a double pointer to point to the stft result
	 uint32_t address;
	 uint32_t recv_len = 0;
	 float32_t ds_pressure[4]={0};
	 float32_t ds_acc[4]={0};
	 uint8_t first_conversion = 0;
	 uint8_t test=0;
	 uint8_t downsample_ready =0;
	 uint8_t astft_ready = 0;
	 uint8_t pstft_ready = 0;
	 
	 while (1)
	 {
		if (xQueueReceive(xQueue_downsample_data,&recv_dsdata,1))
		{
			for (uint8_t i=0;i<4;i++)
			{
				ds_pressure[i] = recv_dsdata.seg[i].pressure*1.0f;
				ds_acc[i] = recv_dsdata.seg[i].accx*1.0f;
			}

			arm_copy_f32(ds_pressure,&ds_pre_data[recv_len*4],4);
			arm_copy_f32(ds_acc,&ds_acc_data[recv_len*4],4);

			if (recv_len == 15)
			{
				downsample_ready = 1;
				recv_len = 0;

			}else{
				recv_len++;
			}
		}    //for get scenario
																	   //19.553 ms
		if(xQueueReceive(xQueue_pstft_cplt,&recv_stft_data,1)){        //27.541 ms
			test++;
			
			arm_copy_f32((&recv_stft_data)->bin, p_stft_ptr->col ,FREQ_BIN_SIZE);
			
			recv_len++;
			
			pstft_ready = 1;
			p_stft_ptr = p_stft_ptr->nextwin;
		}

		if(xQueueReceive(xQueue_astft_cplt,&recv_stft_data,1)){
			test++;
			arm_copy_f32((&recv_stft_data)->bin, a_stft_ptr->col  ,FREQ_BIN_SIZE);
			
			recv_len++;
			
			astft_ready = 1;
			a_stft_ptr = a_stft_ptr->nextwin;
            //SEGGER_SYSVIEW_PrintfHost("Algo Start%d\n",test);
		}
		if (recv_len == 30)
		{
			astft_ready = 1;
			recv_len = 0;
		}
		
		
		//1.      get_scenario();
		//2.	  reserve_freq_band();  arm_abs_f32();  //ok
		//3.	  get_engine_noise();
		//4.	  get_transfer_func();
		//5.	  get_ss();
		//6.	  get_overlap_spectrum();
		//7.	  compensate_overlap_spectrum();
		//8.	  get_confidence_level()
		//9.	  get_overlap_list()
		//10.	  get_bpm_final()
		
	 }
 }
