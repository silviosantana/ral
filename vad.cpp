#include "vad.h"

float hamming(int i, int N){
	return 0.54 - 0.46 * (float)cos((double)(2 * PI * i) / (N - 1));
}

float standardDeviation(float * data, int N){
	float mean, sd, sum = 0;

	for(int i=0; i < N; ++i)
	{
		sum += data[i];
	}

	mean = sum/10;

	for(int i=0; i<N; ++i)
		sd += pow(data[i] - mean, 2);

	return sqrt(sd/N);
}

void vad::run(void){

	//variaveis para leitura de arquivo de audio
	SNDFILE *sf;
	SF_INFO info;
	int num, num_items;
	float*   buf;
	int f,sr, c;
	FILE *out;
	Frame frame;
	frame.last = false;

	/* Open the WAV file. */
	info.format = 0;
	sf = sf_open("grav1.wav",SFM_READ,&info);
	if (sf == NULL)
		{
		printf("Failed to open the file.\n");
		exit(-1);
		}
	/* Print some of the info, and figure out how much data to read. */
	f = info.frames;
	sr = info.samplerate;
	c = info.channels;
	printf("frames=%d\n",f);
	printf("samplerate=%d\n",sr);
	printf("channels=%d\n",c);
	num_items = f*c;
	printf("num_items=%d\n",num_items);
	/* Allocate space for the data to be read, then read it. */
	buf = (float *) malloc(num_items*sizeof(float));
	num = sf_read_float(sf,buf,num_items);
	int size = num;

	sf_close(sf);

	printf("Read %d items\n",num);
	out = fopen("filedata.out","w");

	for (int i = 0; i < num; i += c)
		{
		for (int j = 0; j < c; ++j){
			fprintf(out,"%f ",buf[i+j]);
		}
		fprintf(out,"\n");
		}

	int counter1 = 0;
	int counter2 = 0;
	int counter3 = 0;
	int counter4 = 0;
	int ZCRCountf = 0;      						//Stores forward count of crossing rate > IZCT
	int ZCRCountb = 0;      						//As above, for backward count
	int w_sam = floor(FRAME_WIDTH*FREQ_RATIO);        	//No of Samples/window
	int o_sam = floor(FRAME_SHIFT*FREQ_RATIO);  	  	//No of samples/overlap
	int lengthAudio = size;
	int segs = floor((lengthAudio-w_sam)/o_sam)+1;  	//Number of segments in speech signal
	int sil = floor((SILENCE-FRAME_WIDTH)/FRAME_SHIFT)+1;  	//Number of segments in silent period

	int Limit = o_sam*(segs-1)+1;             //Start index of last segment


	int FrmIndex[segs];
	for (int i = 0; i < segs; i++){
		FrmIndex[i] = i*o_sam;
		if(i == segs - 1){
			FrmIndex[segs-1] = Limit;			//Vector containing starting index
		}
	}
											  //for each segment
	float ZCR_Vector[segs];          		//Vector to hold zero crossing rate
										//for all segments

	float IZC = 0;						//mean zero crossing rate for silence region



	//Below code computes and returns zero crossing rates for all segments in
	//speech sample
	for (int t = 0; t < segs; t++){
		int ZCRCounter = 0;
		int nextIndex = (t-1)*o_sam+1;
		for (int r = nextIndex+1; r < (nextIndex+w_sam-1); r++){
			if ((buf[r] >= 0) && (buf[r-1] < 0)){
				ZCRCounter = ZCRCounter + 1;
			}
			else if ((buf[r] < 0) && (buf[r-1] >= 0)){
				ZCRCounter = ZCRCounter + 1;
			}
		}
		ZCR_Vector[t] = ZCRCounter;
		if (t < sil){
			IZC += ZCR_Vector[t];
		}
	}

	IZC = IZC/sil;

	float stdev = standardDeviation(ZCR_Vector,sil); 	//standard deviation of crossing rate for
														//silence region
	float IZCT;
	if(ZERO_THR < (IZC+2*stdev)){
		IZCT = ZERO_THR;    					//Zero crossing rate threshold
	}
	else {
		IZCT = IZC+2*stdev;
	}


	//Below code computes and returns frame energy for all segments in speech
	//sample
	float Erg_Vector[segs];
	float IMX = 0;
	float IMN = 0;
	for (int u = 0; u < segs; u++){
		int nextIndex = (u-1)*o_sam+1;
		Erg_Vector[u] = 0;
		for (int k = nextIndex; k < nextIndex+w_sam-1; k++){
			Erg_Vector[u] += abs(buf[k]*hamming(k,w_sam));
		}
		if (Erg_Vector[u] > IMX){
			IMX = Erg_Vector[u];
		}
		if (u < sil){
			IMN += Erg_Vector[u];
		}
		//Energy = x(nextIndex:nextIndex+w_sam-1).*win;
		//Erg_Vector[u] = Energy;
	}
	IMN = IMN/sil;

	float I1 = 0.03 * (IMX-IMN) + IMN;    	//I1 & I2 are Initial thresholds
	float I2 = 4 * IMN;

	//	OK



	float ITL;
	if (I1 < I2){
		ITL = I1;			               	//Lower energy threshold
	}
	else {
		ITL = I2;
	}

	float ITU = 5 * ITL;                 	 //Upper energy threshold

	float indexi[lengthAudio];      		//Four single-row vectors are created

	//Search forward for frame with energy greater than ITU
	for (int i = 0; i < (int)sizeof(Erg_Vector); i++){
		if (Erg_Vector[i] > ITU){
			counter1 = counter1 + 1;
			indexi[counter1] = i;
		}
	}
	int ITUs = indexi[1];

	//Search further forward for frame with energy greater than ITL
	for (int j = ITUs; j >= 0 ; j--){
		if (Erg_Vector[j] < ITL){
			counter2 = counter2 + 1;
			indexi[counter2] = j;
		}
	}
	int start = indexi[1]+1;



	//float Erg_Vectorf[segs] = fliplr(Erg_Vector);		//Flips round the energy vector

	float Erg_Vectorf[segs];
	if (segs%2 == 0){
		for (int i = 0; i < segs/2; i++){
			Erg_Vectorf[i] = Erg_Vector[segs-1-i];
			Erg_Vectorf[segs-1-i] = Erg_Vector[i];
		}
	}
	else{
		for (int i = 0; i < (segs-1)/2; i++){
			Erg_Vectorf[i] = Erg_Vector[segs-1-i];
			Erg_Vectorf[segs-1-i] = Erg_Vector[i];
		}
		Erg_Vectorf[(segs-1)/2] = Erg_Vector[(segs-1)/2];
	}


	//Search forward for frame with energy greater than ITU
	//This is equivalent to searching backward from last sample for energy > ITU
	for (int k = 0; k < (int)sizeof(Erg_Vectorf); k++){
		if (Erg_Vectorf[k] > ITU){
			counter3 = counter3 + 1;
			indexi[counter3] = k;
		}
	}
	float ITUf = indexi[1];

	//Search further forward for frame with energy greater than ITL
	for (int l = ITUf; l >= 0; l--){
		if (Erg_Vectorf[l] < ITL){
			counter4 = counter4 + 1;
			indexi[counter4] = l;
		}
	}

	int finish = sizeof(Erg_Vector)-indexi[1]+1;//Tentative finish index

	//Search back from start index for crossing rates higher than IZCT

	//printf("__________________ teste ________________\n");

	int BackSearch = (start < 25) ? start : 25;
	float rate;
	int realstart;
	for (int m = start; m > start-BackSearch+1; m--){
		rate = ZCR_Vector[m];
		if (rate > IZCT){
			ZCRCountb = ZCRCountb + 1;
			realstart = m;
		}
	}
	if (ZCRCountb > 3){
		start = realstart;          //If IZCT is exceeded in more than 3 frames
									//set start to last index where IZCT is exceeded
	}

	//Search forward from finish index for crossing rates higher than IZCT
	int FwdSearch = (sizeof(Erg_Vector)-finish < 25) ? sizeof(Erg_Vector)-finish : 25;
	int realfinish;
	for (int n = finish+1; n < finish+FwdSearch; n++){
		rate = ZCR_Vector[n];
		if (rate > IZCT){
			ZCRCountf = ZCRCountf + 1;
			realfinish = n;
		}
	}
	if (ZCRCountf > 3){
		finish = realfinish;        //If IZCT is exceeded in more than 3 frames
									//set finish to last index where IZCT is
									//exceeded
	}

	int x_start = FrmIndex[start];      //actual sample index for frame 'start'
	printf("primeiro indice: %d\n", x_start);
	int x_finish = num-1;  //actual sample index for frame 'finish'
	printf("ultimo indice: %d\n", x_finish);
	//float * newBuf = (float *) malloc((x_finish-x_start+1)*sizeof(float));
	int k = 0;
	//printf("__________________ teste ________________\n");
	printf("tamanho segmento: %d\n", (int)floor(FRAME_WIDTH*FREQ_RATIO));
	frame.size = (int)ceil(num/(FRAME_WIDTH*FREQ_RATIO));
	printf("segmentos: %d\n", frame.size);
	for (int i = x_start; i <= x_finish; i++){
		//newBuf[i-x_start] = buf[i]; 	//Trim speech sample by start and finish indices
		if ( ((k % (int)floor(FRAME_WIDTH*FREQ_RATIO) == 0) && (k != 0)) || (i == x_finish)) {
			k = 0;
			vOut.write(frame);
			//cout << "frame: " << frame << endl;
		}
		frame.data[k] = buf[i];
		k++;
	}
	frame.last = true;
	vOut.write(frame);
	cout << "frame: " << frame << endl;

	printf("final");
	fclose(out);
}

vad::vad(sc_module_name name) : sc_module(name){
	SC_HAS_PROCESS(vad);
	SC_THREAD(run);
}

