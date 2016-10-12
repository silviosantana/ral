#include "systemc.h"
#include "stimgen.h"

#define NUM_F 500


void random_data(float vec[], int v_size, int seed){
	srand(seed);
	for (int i = 0; i < v_size; i++){
		vec[i] = static_cast <float> (rand() % 99);
	}

}


void stimgen::stim_proc(){
	Frame data;

	for (int i = 0; i < NUM_F; i++){
		random_data(data.data, 128, i+1);
		
		if (i == (NUM_F -1)){
			data.last = true;
		}else{
			data.last = false;
		}
		data.size = NUM_F;
		a1->write(data);
	}

	//sc_stop();
}
