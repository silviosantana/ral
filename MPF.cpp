#include "MPF.h"

#define OT1_S 1
#define OT2_S 1
#define OT3_S 2
#define OT4_S 4
#define OT5_S 8
#define OT6_S 16
#define OT7_S 31

#define TOTAL 63

struct frame {
	float ot1 [OT1_S];
	float ot2 [OT2_S];
	float ot3 [OT3_S];
	float ot4 [OT4_S];
	float ot5 [OT5_S];
	float ot6 [OT6_S];
	float ot7 [OT7_S];
};


void print_vector(float vec[], int v_size){
	for (int i = 0; i < v_size; i++){
		cout << vec[i] << ' ';
	}
	cout << endl;
}

void print_frame(frame *f){
	print_vector(f->ot1, OT1_S);
	print_vector(f->ot2, OT2_S);
	print_vector(f->ot3, OT3_S);
	print_vector(f->ot4, OT4_S);
	print_vector(f->ot5, OT5_S);
	print_vector(f->ot6, OT6_S);
	print_vector(f->ot7, OT7_S);
}

void masking(float vec[], int v_size){
	int i_max = 0;

	if (v_size > 1){
		for (int i = 1; i < v_size; i++){
			if (vec[i] > vec[i_max]){
				vec[i_max] = 0;
				i_max = i;
			}else{
				vec[i] = 0;
			}
		}
	}
}

void create_mean_frame (frame  v_frame[], frame *m_frame, int num_f){
	float sum_ot1[OT1_S];
	float sum_ot2[OT2_S];
	float sum_ot3[OT3_S];
	float sum_ot4[OT4_S];
	float sum_ot5[OT5_S];
	float sum_ot6[OT6_S];
	float sum_ot7[OT7_S];

	for (int j = 0; j < OT7_S; j++){
		if (j < OT1_S){ sum_ot1[j] = 0; }
		if (j < OT2_S){ sum_ot2[j] = 0; }
		if (j < OT3_S){ sum_ot3[j] = 0; }
		if (j < OT4_S){ sum_ot4[j] = 0; }
		if (j < OT5_S){ sum_ot5[j] = 0; }
		if (j < OT6_S){ sum_ot6[j] = 0; }
		if (j < OT7_S){ sum_ot7[j] = 0; }
	}

	for (int i = 0; i < num_f; i ++){
		for (int j = 0; j < OT7_S; j++){
			if (j < OT1_S){ sum_ot1[j] += v_frame[i].ot1[j]; }
			if (j < OT2_S){ sum_ot2[j] += v_frame[i].ot2[j]; }
			if (j < OT3_S){ sum_ot3[j] += v_frame[i].ot3[j]; }
			if (j < OT4_S){ sum_ot4[j] += v_frame[i].ot4[j]; }
			if (j < OT5_S){ sum_ot5[j] += v_frame[i].ot5[j]; }
			if (j < OT6_S){ sum_ot6[j] += v_frame[i].ot6[j]; }
			if (j < OT7_S){ sum_ot7[j] += v_frame[i].ot7[j]; }
		}
	}

	for (int j = 0; j < OT7_S; j++){
		if (j < OT1_S){ m_frame->ot1[j] = sum_ot1[j]/num_f; }
		if (j < OT2_S){ m_frame->ot2[j] = sum_ot2[j]/num_f; }
		if (j < OT3_S){ m_frame->ot3[j] = sum_ot3[j]/num_f; }
		if (j < OT4_S){ m_frame->ot4[j] = sum_ot4[j]/num_f; }
		if (j < OT5_S){ m_frame->ot5[j] = sum_ot5[j]/num_f; }
		if (j < OT6_S){ m_frame->ot6[j] = sum_ot6[j]/num_f; }
		if (j < OT7_S){ m_frame->ot7[j] = sum_ot7[j]/num_f; }
	}
}

void create_characteristic_vector(frame *m_frame, float v_charac[]){
	int i;
	float sum = 0;

	for (i = 0; i < 7; i ++){
		v_charac[i] = 0;
	}

	for (i = 0; i < OT7_S; i++){
		if (i < OT1_S){ v_charac[0] += m_frame->ot1[i]; }
		if (i < OT2_S){ v_charac[1] += m_frame->ot2[i]; }
		if (i < OT3_S){ v_charac[2] += m_frame->ot3[i]; }
		if (i < OT4_S){ v_charac[3] += m_frame->ot4[i]; }
		if (i < OT5_S){ v_charac[4] += m_frame->ot5[i]; }
		if (i < OT6_S){ v_charac[5] += m_frame->ot6[i]; }
		if (i < OT7_S){ v_charac[6] += m_frame->ot7[i]; }
	}

	//print_vector(v_charac, 7);

	sum = v_charac[0] + v_charac[1] + v_charac[2] + v_charac[3] + v_charac[4] + v_charac[5] + v_charac[6];
	for (i = 0; i < 7; i ++){
		v_charac[i] = v_charac[i]/sum;
	}

}


void mask_frame(frame *f){
	masking(f->ot1,OT1_S);
	masking(f->ot2,OT2_S);
	masking(f->ot3,OT3_S);
	masking(f->ot4,OT4_S);
	masking(f->ot5,OT5_S);
	masking(f->ot6,OT6_S);
	masking(f->ot7,OT7_S);

}

void create_octave_frame(Frame *in, frame *f){
 	int i, j;

	for (i = 1; i <= OT1_S ; i ++){
		f->ot1[i-1] = in->data[i];
	}

	for (j = 0; j < OT2_S; j++, i++){
		f->ot2[j] = in->data[i];
	}

	for (j = 0; j < OT3_S; j++, i++){
		f->ot3[j] = in->data[i];
	}

	for (j = 0; j < OT4_S; j++, i++){
		f->ot4[j] = in->data[i];
	}

	for (j = 0; j < OT5_S; j++, i++){
		f->ot5[j] = in->data[i];
	}

	for (j = 0; j < OT6_S; j++, i++){
		f->ot6[j] = in->data[i];
	}

	for (j = 0; j < OT7_S; j++, i++){
		f->ot7[j] = in->data[i];
	}


}

void MPF::mpf_main(){
	//frame v_frame[10];
	frame mean_f;
	float v_charac [7];
	bool flag = false;
	int count = 0;

	Frame input;
	//frame v_octave; 

	input = in_f->read();

	frame v_frame[input.size];
	//cout << "count: "<< input.last << endl;

	while(!flag || (count == 0)){
		
		if (count != 0){
			input = in_f->read();
		}
		
		//cout << "input: " << input << endl;

		create_octave_frame(&input, &v_frame[count]);
		//print_frame(&v_frame[count]);
		//cout << endl;

		mask_frame(&v_frame[count]);
		//print_frame(&v_frame[count]);
		//cout << endl;
		//cout << "count: "<< input.last << endl;

		count++;
		flag = input.last;
	}

	//cout << "count: "<< input.size << endl;

	for (int h = 0; h < input.size ; h++){
		// << v_frame[h].ot1 << endl;
	}


	create_mean_frame(v_frame, &mean_f, count);
	//print_frame(&mean_f);

	create_characteristic_vector(&mean_f, v_charac);
	cout << "\n\n\n\n\n_________VETOR CARACTERISTICO__________:\n\n" << endl;
	print_vector(v_charac, 7);


//	sc_stop();
}
