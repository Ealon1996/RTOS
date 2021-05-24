#include "sample.h"

//int32_t pre_raw_data[PRE_LOG_RAW_SEC][SAMPLE_RATE];
//int32_t accx_raw_data[ACC_LOG_RAW_SEC][SAMPLE_RATE];
//int32_t accy_raw_data[ACC_LOG_RAW_SEC][SAMPLE_RATE];
//int32_t accz_raw_data[ACC_LOG_RAW_SEC][SAMPLE_RATE];  //reserved memory for acc





static SemaphoreHandle_t sensor_TimerSemaphore = NULL;
QueueHandle_t xQueue_pre_raw_data = NULL;


void Sensor_Task_Init(){
	xQueue_pre_raw_data= xQueueCreate(8,sizeof(sensor_data_t));
	
	sensor_TimerSemaphore = xSemaphoreCreateBinary();
	if (sensor_TimerSemaphore == NULL)
	{
		while (1)
		{
			;
		}
	}

}


uint32_t sensor_test_count = 0;  //for testting

int16_t tmpx,tmpy,tmpz;
void Sensor_Task(void *p)
{
	(void)p;
	DATA_32BIT adc_buff;
	uint8_t *asm330_6x;
	uint8_t sensor_data_buff[15] = {0};
	uint32_t len=0;
	sensor_data_t sd;

	while (1)
	{
		if( xSemaphoreTake(sensor_TimerSemaphore,portMAX_DELAY)) // 256 Hz
		{
			
			int32_t x,y,z;
#if PLATFORM == 0
			asm330_6x = ASM330_Data_Read();					//0.5~0.6ms
			
			//should sync data here

			adc_buff.data = MCP3918_SPI_24bit_ADC_Read();   //0.7ms

			//store data here
			sd.pressure = adc_buff.data;
			sd.accx = (int16_t)(asm330_6x[0] | asm330_6x[1]<<8);
			sd.accy = (int16_t)(asm330_6x[2] | asm330_6x[3]<<8);
			sd.accz = (int16_t)(asm330_6x[4] | asm330_6x[5]<<8);
#elif PLATFORM ==1
            //use simulation data here.
#endif



			xQueueSend(xQueue_pre_raw_data, &sd, ( TickType_t ) 0);
			sensor_test_count++;	
		}
	}
}

BaseType_t Sensor_Task_GiveSemaphore_FromISR(){
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(sensor_TimerSemaphore!=NULL){
		xSemaphoreGiveFromISR(sensor_TimerSemaphore,&xHigherPriorityTaskWoken);
	}
	return xHigherPriorityTaskWoken;
}


SemaphoreHandle_t get_sensor_Semaphore(){
	return sensor_TimerSemaphore;
}





