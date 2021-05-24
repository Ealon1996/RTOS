#include "spectrum.h"

#define STFT_OVERLAP_MATRIX_ROW 10
#define STFT_OVERLAP_MATRIX_COL 10

void get_overlap_spectrum(float32_t out[],				//ssnoise
						  float32_t data[],				//output
						  uint16_t window_size,			//the size of ssnoise
						  int16_t overlap_weight[],		//overlap lwn
						  int16_t overlap_size) {
	int16_t i, j, k;
	float32_t max_amp = 0;
	for (i = 1; i <= overlap_size; i++) {
		for (j = 0; j < 120; j++) {
			max_amp = 0;
			for (k = i * j; k < (i * j + i); k++) {
				if (max_amp < data[k]) max_amp = data[k];
			}
			out[j] += max_amp * overlap_weight[i - 1];
		}
	}
}

static float32_t stft_overlap_matrix[STFT_OVERLAP_MATRIX_ROW][STFT_OVERLAP_MATRIX_COL];  //a predefined matrix

void compensate_overlap_spectrum( float32_t stft_overlap_in[], float32_t overlap_weight[],uint32_t stft_overlap_len, uint32_t overlap_weight_len){
	
	//initialize stft_overlap_matrix
	for (uint32_t i=0; i<STFT_OVERLAP_MATRIX_ROW; i++)
	{
		for(uint32_t j=0;j<STFT_OVERLAP_MATRIX_COL;j++){
			stft_overlap_matrix[i][j]=0;
		}
	}
	
	for (uint32_t i=0; i<60; i++)       //60? 
	{
		float32_t  reimburse = 0;
		uint32_t count = 0;
		for (uint32_t j=0; j<7; j++)
		{
			for (uint32_t k=0;k<j;k++)
			{
				uint32_t idx = (float)((float32_t)i*((float32_t)k+1)/((float32_t)j+1)+0.5f);
				
				if ( 
					 ((float32_t)k+1)/((float32_t)j+1)>0.4 &&
					 k!=j
				 )
				{
					
				  reimburse += stft_overlap_in[idx] * overlap_weight[k] * overlap_weight[j];
				  count += 1;	
				  
				}
				
				
			}
			
		}
		
		//sss2 = stft_overlap_in[i] + reimburse/count
		stft_overlap_in[i] = stft_overlap_in[i] + (float32_t)reimburse/(float32_t)count;
		
		//sss2[sss2 < 0] = 0
		for (uint32_t s=0;s<stft_overlap_len;s++)
		{
			stft_overlap_in[s] = 0;
		}
		//stft_overlap[i] = sss2
		arm_copy_f32(stft_overlap_in ,&stft_overlap_matrix[i][0] ,stft_overlap_len );
	}
}

//put nss here
// the size of arr should be accorded to FREQ_BIN_SIZE
float32_t overlap_weight[4] = {0,1,1,1};
float32_t spec_tmp[FREQ_BIN_SIZE];

void get_engine_noise(float32_t *nss, float32_t spec_data_overlap[], uint32_t size){
	float32_t moving_max = 0;
	uint32_t max_ind = 0;
	for(uint32_t i=150;i< 300;i++){
		for(uint32_t j=0;j<4;j++){
			
			if (i * (j + 1) < 960)
			{
				uint32_t start = i * (j + 1);
				uint32_t end= (i + 1) * (j + 1);
				memset(spec_tmp,0,512);
				memcpy(&spec_tmp[0], &nss[start],   (end-start)*sizeof(float32_t)   );
				arm_max_f32(nss,(uint32_t)spec_tmp,&moving_max,(uint32_t *)&max_ind);
				spec_data_overlap[i] += moving_max* overlap_weight[j];
			}
		}
	}	
}

/*

    def get_overlap_index(self,ssi, bpm_peak):
        peak_num = 0
        overlap_index = 0
        for mul in range(1, self.max_overlap + 1):
            overlap_ss = ssi[::mul]
            peaks, _ = find_peaks(overlap_ss, height=0.5)
            if not peaks.any():
                continue
            diff = np.abs(peaks - bpm_peak)
            min_index = np.argmin(diff)
            min_diff = diff[min_index]
            if min_diff < 2:
                overlap_index += 1 << (mul - 1)
                peak_num += 1
        return overlap_index, peak_num

*/

float32_t ssi_tmp[10];
void get_overlap_index(float32_t* ssi, float32_t *bpm_peak, float32_t *overlap_index, float32_t*peak_num){

	uint32_t oindex = 0;
	uint32_t len = 0; 
	
	for(uint32_t i=0; i<len; i++){
		//memcpy(ssi_tmp,ssi,,);
		//find_peaks(overlap_ss, height=0.5);
		
		//arm_abs_f32() 
		//min_index = np.argmin(diff)
		//min_diff = diff[min_index]
		
		//if min_diff < 2:
		//	overlap_index += 1 << (mul - 1)
		//	peak_num += 1
		
	}
}


void find_max(float* data,uint32_t size,float* max_value,uint32_t* result_id){
    float max =data[0];
	uint32_t max_id = 0;
	for(uint32_t i=0;i<size;i++){
		if(max< data[i]){
			max = data[i];
			max_id = i;
		}
	}
	*max_value = max;
	*result_id = max_id;
}

void find_min(float* data,uint32_t size,float* min_value,uint32_t* result_id){
    float min =data[0];
    uint32_t min_id = 0;
    for(uint32_t i=0;i<size;i++){
        if(min< data[i]){
            min = data[i];
            min_id = i;
        }
    }
    *min_value = min;
    *result_id = min_id;
}


//must initialize peak list before use list!
void peak_list_init(peak_list_t* list){
	list->head = NULL;
}

peak_list_t* create_peak(float32_t value, uint32_t ind){

#if USE_OS ==2
	peak_node_t* new_peak = (peak_node_t*)malloc(sizeof (peak_node_t));
#elif USE_OS ==1
	peak_node_t* new_peak =(peak_node_t*)pvPortMalloc(sizeof (peak_node_t));
#endif

	if (!new_peak) {
		return NULL;
	}
	new_peak->next =NULL;
	new_peak->ind = ind;
	new_peak->value = value;
	return new_peak;
}

void add_peak_node(peak_list_t* list,float32_t value,uint32_t ind){
	peak_node_t* current = NULL;

	if(list->head == NULL){
		list->head =  create_peak(value,ind);

	}else{
		
		current = list->head ;
		
		while(current->next!=NULL){
			current= (peak_list_t*)current->next;
		}
		current->next = create_peak(value,ind);
	}
    list->peak_len++;
}

//delete specific index from the list
void delete_peak_node_by_id(peak_list_t* list,uint32_t id){
    uint32_t count = 0;
    peak_node_t* current = list->head;
    peak_node_t* last;
    peak_node_t* denode;
    while(current!=NULL){

        if(current->ind == id){
           last->next =current->next;

           if(current == list->head){
                 list->head = current->next;
           }

           #if USE_OS ==2
               free(current);
           #elif USE_OS ==1
                vPortFree(current);
           #endif

           return;
        }
        last = current;
        current = current->next;
    }
    list->peak_len--;
}

void find_peaks(float32_t data[], uint32_t size, float32_t threshold,uint32_t width, float32_t height,peak_list_t* pl){
    peak_node_t* current;

    //trasverse the linked list and find the peak then add to the list.
    for (uint32_t i=0; i < size; i++) {
        if(data[i-1]<data[i] && data[i+1]<data[i]){
            add_peak_node(pl,data[i],i);
        }
    }

    for (peak_node_t* i= pl->head; i !=NULL; i=i->next) {
        uint32_t ind = i->ind;
        uint32_t value = i->value;
        float32_t max_value;
        uint32_t max_ind;
        float32_t min_value;
        uint32_t min_ind;

        //filter threshold
        if(i->value <threshold){
            delete_peak_node_by_id(pl,i->ind);
        }

        //filter height and width
        if((ind-width>0) && (ind+width)<size){
            int len = width/2;
            find_max(&data[ind-len],width,&max_value,&max_ind);
            find_min(&data[ind-len],width,&min_value,&min_ind);
            if(max_value>=data[ind]){
                if((data[ind] - min_value)<height){
                    delete_peak_node_by_id(pl,i->ind);
                }
            }


        }
    }
}


float32_t peaktmp[20];
float32_t plist[20];
uint32_t peakind[20];

//to be revised
void find_peak(float32_t* arr,uint32_t blocksize,uint32_t width, uint32_t *peak_list_index, float32_t* peak_list_value){

}

void get_bpm_final(){
	
	
	

}
