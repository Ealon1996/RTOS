#include "filter.h"


float32_t pre_filter_data[PRE_LOG_FILTER_SEC][SAMPLE_RATE];  // n*256
float32_t acc_filter_data[ACC_LOG_FILTER_SEC][SAMPLE_RATE];  // n*256

//uint32_t pre_dsfilter_data[PRE_LOG_FILTER_SEC][DOWN_SAMPLE_RATE];  //n*64
//uint32_t acc_dsfilter_data[ACC_LOG_FILTER_SEC][DOWN_SAMPLE_RATE];  //n*64

void p_fir_filter_init(void){
	arm_fir_init_f32(&p_fir_instance, P_FIR_TAPS, (float32_t *)&p_firCoeffs[0], &p_fir_State[0], P_FIR_LEN);
}

void p_fir_filter(float32_t intput[], float32_t output[],uint16_t len){
	p_fir_filter_init();  //should clean fir state
	arm_fir_f32(&p_fir_instance,intput ,output ,len);
}

void ax_fir_filter_init(void){
	arm_fir_init_f32(&a_fir_instance, A_FIR_TAPS, (float32_t *)&a_firCoeffs[0], &ax_fir_State[0], A_FIR_LEN);
}

void ax_fir_filter(float32_t intput[], float32_t output[],uint16_t len){
	//ax_fir_filter_init();
	arm_fir_f32(&a_fir_instance,intput ,output ,len);
}

void ay_fir_filter_init(void){
	arm_fir_init_f32(&a_fir_instance, A_FIR_TAPS, (float32_t *)&a_firCoeffs[0], &ay_fir_State[0], A_FIR_LEN);
}

void ay_fir_filter(float32_t intput[], float32_t output[],uint16_t len){
	//ax_fir_filter_init();
	arm_fir_f32(&a_fir_instance,intput ,output ,len);
}

void az_fir_filter_init(void){
	arm_fir_init_f32(&a_fir_instance, A_FIR_TAPS, (float32_t *)&a_firCoeffs[0], &az_fir_State[0], A_FIR_LEN);
}

void az_fir_filter(float32_t intput[], float32_t output[],uint16_t len){
	//ax_fir_filter_init();
	arm_fir_f32(&a_fir_instance,intput ,output ,len);
}



void Fir_Init(){
	p_fir_filter_init();
	ax_fir_filter_init();
	ay_fir_filter_init();
	az_fir_filter_init();
}


extern QueueHandle_t xQueue_accx_raw_data;
extern QueueHandle_t xQueue_accy_raw_data;
extern QueueHandle_t xQueue_accz_raw_data;
extern QueueHandle_t xQueue_pre_raw_data ;  //get queue handle from sensor task.


float32_t pre_sig_in_f32[32];
float32_t accx_sig_in_f32[32];
float32_t accy_sig_in_f32[32];
float32_t accz_sig_in_f32[32];
float32_t pre_sig_out[32];
float32_t accx_sig_out[32];
float32_t accy_sig_out[32];
float32_t accz_sig_out[32];

float32_t ds_pre_sig[8];
float32_t ds_accx_sig[8];
float32_t ds_accy_sig[8];
float32_t ds_accz_sig[8];
float32_t ds_acc_sig[8];

float32_t fds_pre_sig[8];
float32_t fds_accx_sig[8];
float32_t fds_accy_sig[8];
float32_t fds_accz_sig[8];
float32_t fds_acc_sig[8];

/*
stft_arry_list_t* p_stft_ptr;  //p sensor
stft_arry_list_t* a_stft_ptr;  //g sensor
stft_arry_list_t* f_stft_ptr;  //fusion sensor

stft_arry_list_t p_stft_arr[STFT_SEG];			 //for p senor
float32_t p_stft_matrix[STFT_SEG][FREQ_BIN_SIZE]={0};

stft_arry_list_t a_stft_arr[STFT_SEG];
float32_t a_stft_matrix[STFT_SEG][FREQ_BIN_SIZE]={0};  //for g senor

stft_arry_list_t fusion_stft_arr[STFT_SEG];
float32_t fusion_stft_matrix[STFT_SEG][FREQ_BIN_SIZE]={0};  //for fusion
*/


QueueHandle_t xQueue_pstft_cplt= NULL;
QueueHandle_t xQueue_astft_cplt= NULL;
QueueHandle_t xQueue_filtered_data= NULL;
QueueHandle_t xQueue_downsample_data= NULL;
SemaphoreHandle_t xStft_cplt_Semaphore = NULL;
filtered_dsdata_t ds_data;

uint32_t dsp_test_count = 0;

 void dsp_task(void *p){
	(void)p;
	DATA_32BIT adc_buff;
	uint32_t len = 0 ;
	int32_t x,y,z;
	uint8_t acc_data_ready = 0;
	uint8_t pre_data_ready = 0;
	sensor_data_t sd;
	while(1){
		
		if (
			xQueueReceive(xQueue_pre_raw_data,&sd,portMAX_DELAY)
		)
		{
			//run filter
			//sig_in[len] = adc_buff.data;    //4 bytes
			pre_sig_in_f32[len] =  sd.pressure  *1.0f;
			accx_sig_in_f32[len] = sd.accx*1.0f;
			accy_sig_in_f32[len] = sd.accy*1.0f;
			accz_sig_in_f32[len] = sd.accz*1.0f;
			
			if (len == 15)
			{
				//1.store data
				
				//2.filter
				p_fir_filter(pre_sig_in_f32,pre_sig_out,16);    //0.36ms
				ax_fir_filter(accx_sig_in_f32, accx_sig_out,16);
				ay_fir_filter(accy_sig_in_f32, accy_sig_out,16);
				az_fir_filter(accz_sig_in_f32, accz_sig_out,16);
				
				//3.1 raw data &down sampling
				for (uint8_t i=0;i<4;i++)
				{
					ds_pre_sig[i] = (pre_sig_in_f32[i*4+0]+pre_sig_in_f32[i*4+1]+pre_sig_in_f32[i*4+2]+pre_sig_in_f32[i*4+3])/4.0f;
					ds_accx_sig[i] = (accx_sig_in_f32[i*4+0]+ accx_sig_in_f32[i*4+1]+accx_sig_in_f32[i*4+2]+accx_sig_in_f32[i*4+3])/4.0f;
					ds_accy_sig[i] = (accy_sig_in_f32[i*4+0]+ accy_sig_in_f32[i*4+1]+ accy_sig_in_f32[i*4+2]+accy_sig_in_f32[i*4+3])/4.0f;
					ds_accz_sig[i] = (accy_sig_in_f32[i*4+0]+ accy_sig_in_f32[i*4+1] + accy_sig_in_f32[i*4+2]+ accy_sig_in_f32[i*4+3])/4.0f;
				}
				for (uint8_t i=0; i<4; i++)
				{
					ds_acc_sig[i] = ds_accx_sig[i]*ds_accx_sig[i] + ds_accy_sig[i]*ds_accy_sig[i] + ds_accz_sig[i]*ds_accz_sig[i];  //take square
					ds_acc_sig[i] = sqrtf(ds_acc_sig[i]);//dsp
				}
				for(uint8_t i=0;i<4;i++){
					ds_data.seg[i].pressure = ds_pre_sig[i];
					ds_data.seg[i].accx = ds_acc_sig[i];
					ds_data.seg[i].accy = 0;
					ds_data.seg[i].accz = 0;
				}
				//send down sample data
				xQueueSend(xQueue_downsample_data, &ds_data, ( TickType_t ) 0);
				
				
				//3.2 filtered & down sampling..
				for (uint32_t i=0;i<4;i++)
				{
					fds_pre_sig[i] = (pre_sig_out[i*4+0]+pre_sig_out[i*4+1]+pre_sig_out[i*4+2]+pre_sig_out[i*4+3])/4.0f;
					fds_accx_sig[i] = (accx_sig_out[i*4+0]+ accx_sig_out[i*4+1]+accx_sig_out[i*4+2]+accx_sig_out[i*4+3])/4.0f;
					fds_accy_sig[i] = (accy_sig_out[i*4+0]+ accy_sig_out[i*4+1]+ accy_sig_out[i*4+2]+accy_sig_out[i*4+3])/4.0f;
					fds_accz_sig[i] = (accz_sig_out[i*4+0]+ accz_sig_out[i*4+1] + accz_sig_out[i*4+2]+ accz_sig_out[i*4+3])/4.0f;
				}
				
				for (uint32_t i=0; i<4; i++)
				{
					fds_acc_sig[i] = fds_accx_sig[i]*fds_accx_sig[i] + fds_accy_sig[i]*fds_accy_sig[i] + fds_accz_sig[i]*fds_accz_sig[i];  //take square
					fds_acc_sig[i] = sqrtf(fds_acc_sig[i]);
				}
				for(uint8_t i=0;i<4;i++){
					ds_data.seg[i].pressure = fds_pre_sig[i];
					ds_data.seg[i].accx = fds_acc_sig[i];
					ds_data.seg[i].accy =0;
					ds_data.seg[i].accz =0;
				}
				//4.send raw_data(down sample)   filtered data(down sample)
				
				xQueueSend(xQueue_filtered_data, &ds_data, ( TickType_t ) 0);
				
				dsp_test_count++;
				len =0;
			}else{
				len++;
			}
			
			//send a window
		}

	}
}

void p_array_list_init(stft_arry_list_t* arr,float32_t matrix[][FREQ_BIN_SIZE]){
	
	for (uint8_t i=0;i<STFT_SEG;i++){
		arr[i].ind = i;
		if (i == 0){
			arr[i].nextwin = &arr[i+1];
			arr[i].prevwin = &arr[STFT_SEG-1];
		}else if(i==(STFT_SEG-1)){
			arr[i].nextwin = &arr[0];
			arr[i].prevwin = &arr[i-1];
		}else{
			arr[i].nextwin = &arr[i+1];
			arr[i].prevwin = &arr[i-1];
		}
	}
	
	for(uint8_t i=0;i<STFT_SEG;i++){
		arr[i].col = &matrix[i][0];
	}
}

void Stft_Task_Init(){
	
	xQueue_filtered_data = xQueueCreate(4,sizeof(filtered_dsdata_t));  
	xQueue_downsample_data = xQueueCreate(16,sizeof(filtered_dsdata_t));  
	xQueue_pstft_cplt = xQueueCreate(4,sizeof(stft_data_t));
	xQueue_astft_cplt =  xQueueCreate(4,sizeof(stft_data_t));

}



float32_t fil_sig[4];
float32_t pstft_sig_in[1024];
float32_t astft_sig_in[1024];
float32_t result[1024];
stft_data_t st_data;

void stft_task(void *p){
	uint32_t len = 0 ;
	uint32_t buffer_size;
	uint8_t first_conversion = 0;
	filtered_dsdata_t fds_data;
	float32_t pressure[4]={0}; 
	float32_t acc[4]={0};

	while (1)
	{
		
		if (
			xQueueReceive(xQueue_filtered_data,&fds_data,portMAX_DELAY)  //delay until receive data.
		)
		{
			//fill data
			for (uint8_t i=0;i<4;i++)
			{
				pressure[i] = fds_data.seg[i].pressure;
				acc[i] = fds_data.seg[i].accx;
			}

			//use ping-pong mode or moving memory to handle overlap problem
			if (first_conversion ==0)
			{
				memcpy(&pstft_sig_in[len*4],pressure ,4*sizeof(float32_t));  //input 4 data
				memcpy(&astft_sig_in[len*4],acc ,4*sizeof(float32_t));  //input 4 data
				
				if (len == STFT_BLOCK_SAMPLES)      // block size = 4*256 =1024
				{
					
					//do fft here
				
                    //SEGGER_SYSVIEW_PrintfHost("stft start 1\n");    //for debug event
					dsp_stft(pstft_sig_in,&st_data.bin[0],STFT_SAMPLES);
					//send  queue to algo
					xQueueSend(xQueue_pstft_cplt, &st_data, ( TickType_t ) 0);
					
					//do fft here
					dsp_stft(astft_sig_in,&st_data.bin[0],STFT_SAMPLES);
					
					
					
					//send  queue to algo
					xQueueSend(xQueue_astft_cplt, &st_data, ( TickType_t ) 0);
					
					//1651 920 1685
					//shifting: copy 512~1024 to 0~512
					memcpy(&pstft_sig_in[0],&pstft_sig_in[SHIFT],(STFT_SAMPLES-SHIFT)* sizeof(float32_t));
					memcpy(&astft_sig_in[0],&astft_sig_in[SHIFT],(STFT_SAMPLES-SHIFT)* sizeof(float32_t));
					
					first_conversion = 1;
					len = 0;
					
				}else{
					
					len++;	
					
				}
				
			}else{  //for overlapping 50%
				
				memcpy(&pstft_sig_in[len*4 + (STFT_SAMPLES-SHIFT)],pressure ,4*  sizeof(float32_t));  //start from 255
				memcpy(&astft_sig_in[len*4 + (STFT_SAMPLES-SHIFT)],acc ,4*   sizeof(float32_t));  //input 4 data
				if (len == NON_OVERLAP_BLOCK)     
				{

                    //SEGGER_SYSVIEW_PrintfHost("stft start 2\n");				//for debug event
					dsp_stft(pstft_sig_in,&st_data.bin[0],STFT_SAMPLES);
										
					xQueueSend(xQueue_pstft_cplt, &st_data, ( TickType_t ) 0);

					//do fft here
					dsp_stft(astft_sig_in,&st_data.bin[0],STFT_SAMPLES);
					
					xQueueSend(xQueue_astft_cplt, &st_data, ( TickType_t ) 0);
					
					//shifting copy SHIFT~1024 to 0~SHIFT-1
					//void *memcpy(void *dest, const void * src, size_t n)
					memcpy(&astft_sig_in[0],&astft_sig_in[SHIFT],(STFT_SAMPLES-SHIFT)* sizeof(float32_t));

					memcpy(&pstft_sig_in[0],&pstft_sig_in[SHIFT],(STFT_SAMPLES-SHIFT)* sizeof(float32_t));

					//send  queue to algo
					
					
					len = 0;
				}else{
					len++;
				}
				
			}
			
		}
	}
}
