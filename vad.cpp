#include "vad.h"

float hamming(int i, int N){
	return 0.54 - 0.46 * cosf((float)(2 * PI * i) / (N - 1));
}

float standardDeviation(float * data, int N){
	float mean = 0, sd = 0, sum = 0;

	for(int i=0; i < N; ++i)
	{
		sum += data[i];
	}

	mean = sum/N;

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
	sf = sf_open("teste.wav",SFM_READ,&info);
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

	sf_close(sf);

	printf("Read %d items\n",num);
	out = fopen("filedata.out","w");

	/*for (int i = 0; i < num; i++)
	{
		//for (int j = 0; j < c; j++){
			fprintf(out,"%f ",buf[i]);
		//}
		fprintf(out,"\n");
	}*/

	int ZCRCountf = 0;      	//Stores forward count of crossing rate > IZCT
	int ZCRCountb = 0;      	//As above, for backward count
	int w_sam = floor(FRAME_WIDTH*FREQ_RATIO);        	//No of Samples/window
	int o_sam = floor(FRAME_SHIFT*FREQ_RATIO);  	  	//No of samples/overlap
	int lengthAudio = num;
	int segs = floor((lengthAudio-w_sam)/o_sam)+1;  		//Number of segments in speech signal
	int sil = floor((SILENCE-FRAME_WIDTH)/FRAME_SHIFT)+1;  	//Number of segments in silent period

	int Limit = o_sam*segs;             //Start index of last segment

	printf("%d %d %d %d %d %d\n", w_sam, o_sam, segs, Limit, num, sil);

	//Vector containing starting index for each segment
	int FrmIndex[segs];
	for (int i = 0; i < segs; i++){
		FrmIndex[i] = i*o_sam;
	}

	float ZCR_Vector[segs];          	//Vector to hold zero crossing rate for all segments
	float IZC = 0;						//Mean zero crossing rate for silence region

	int nextIndex;

	int ZCRCounter;
	//Below code computes and returns zero crossing rates for all segments in
	//speech sample
	for (int t = 0; t < segs; t++){
		ZCRCounter = 0;
		nextIndex = t*o_sam;
		for (int r = nextIndex+1; r < (nextIndex+w_sam); r++){
			//if(r >= 0){
				if ((buf[r] >= 0) && (buf[r-1] < 0)){
					ZCRCounter = ZCRCounter + 1;
				}
				else if ((buf[r] < 0) && (buf[r-1] >= 0)){
					ZCRCounter = ZCRCounter + 1;
				//}
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
	//Zero crossing rate threshold
	float IZCT;
	if(ZERO_THR < (IZC+2*stdev)){
		IZCT = ZERO_THR;
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
		nextIndex = u*o_sam;
		Erg_Vector[u] = 0;
		for (int k = nextIndex; k < nextIndex+w_sam; k++){
			Erg_Vector[u] += fabsf(buf[k]*hamming(k%w_sam,w_sam));
		}
		if (Erg_Vector[u] > IMX){
			IMX = Erg_Vector[u];
		}
		if (u < sil){
			IMN += Erg_Vector[u];
		}
	}
	IMN = IMN/sil;

	//I1 & I2 are Initial thresholds
	float I1 = 0.03 * (IMX-IMN) + IMN;
	float I2 = 4 * IMN;

	float ITL;					//Lower energy threshold
	if (I1 < I2){
		ITL = I1;
	}
	else {
		ITL = I2;
	}

	float ITU = 5 * ITL;        //Upper energy threshold

	int ITUs = 0;

	printf("%f %f %f %f\n", I1, I2, ITL, ITU);

	//Search forward for frame with energy greater than ITU
	for (int i = 0; i < segs; i++){
		if (Erg_Vector[i] > ITU){
			ITUs = i;
			break;
		}
	}
	printf("ITUs: %d\n", ITUs);

	int start = 0;
	//Search further forward for frame with energy greater than ITL
	for (int j = ITUs; j >= 0 ; j--){
		if (Erg_Vector[j] < ITL){
			start = j + 1;
			break;
		}
	}

	printf("start: %d\n", start);

	float Erg_Vectorf[segs];
	//Flips round the energy vector
	if (segs%2 == 0){
		for (int i = 0; i < segs/2; i++){
			Erg_Vectorf[i] = Erg_Vector[segs-1-i];
			Erg_Vectorf[segs-1-i] = Erg_Vector[i];
			//printf("Erg %d: %f\n", i, Erg_Vectorf[i]);
		}
	}
	else{
		for (int i = 0; i < (segs-1)/2; i++){
			Erg_Vectorf[i] = Erg_Vector[segs-1-i];
			Erg_Vectorf[segs-1-i] = Erg_Vector[i];
			//printf("Erg %d: %f\n", i, Erg_Vectorf[i]);
		}
		Erg_Vectorf[(segs-1)/2] = Erg_Vector[(segs-1)/2];
	}


	int ITUf = 0;
	//Search forward for frame with energy greater than ITU
	//This is equivalent to searching backward from last sample for energy > ITU
	for (int k = 0; k < (int)sizeof(Erg_Vectorf); k++){
		if (Erg_Vectorf[k] > ITU){
			ITUf = k;
			break;
		}
	}

	printf("ITUf: %d\n", ITUf);

	int finish = segs;
	int l;
	//Search further forward for frame with energy greater than ITL
	for (l = ITUf; l >= 0; l--){
		if (Erg_Vectorf[l] < ITL){
			finish = segs - l;
			break;
		}
	}

	printf("finish: %d %d\n", finish, l);

	//Search back from start index for crossing rates higher than IZCT
	int BackSearch = (start < 25) ? start : 25;
	int realstart;
	for (int m = start; m > start-BackSearch+1; m--){
		if (ZCR_Vector[m] > IZCT){
			ZCRCountb = ZCRCountb + 1;
			realstart = m;
		}
	}
	//If IZCT is exceeded in more than 3 frames set start to last index where IZCT is exceeded
	if (ZCRCountb > 3){
		start = realstart;
	}

	printf("start: %d\n", start);

	//Search forward from finish index for crossing rates higher than IZCT
	int FwdSearch = (segs-finish < 25) ? segs-finish : 25;
	int realfinish;
	for (int n = finish+1; n < finish+FwdSearch; n++){
		if (ZCR_Vector[n] > IZCT){
			ZCRCountf = ZCRCountf + 1;
			realfinish = n;
		}
	}

	//If IZCT is exceeded in more than 3 frames set finish to last index where IZCT is exceeded
	if (ZCRCountf > 3){
		finish = realfinish;
	}

	printf("finish: %d\n", finish);

	int x_start = FrmIndex[start+1];      		//actual sample index for frame 'start'
	printf("primeiro indice: %d\n", x_start);
	int x_finish = FrmIndex[finish-1];  						//actual sample index for frame 'finish'
	printf("ultimo indice: %d\n", x_finish);
	int k = 0;
	printf("tamanho segmento: %d\n", (int)floor(FRAME_WIDTH*FREQ_RATIO));
	frame.size = (int)floor((x_finish-x_start)/(FRAME_WIDTH*FREQ_RATIO) + 0.5);
	printf("segmentos: %d\n", frame.size);
	for (int i = x_start; i <= x_finish; i++){
		//newBuf[i-x_start] = buf[i]; 	//Trim speech sample by start and finish indices
		if ( ((k % (int)floor(FRAME_WIDTH*FREQ_RATIO) == 0) && (k != 0)) || (i == x_finish)) {
			frame.last = false;
			
			if(i == x_finish)
			{
				frame.last = true;
				frame.data[k] = buf[i];
				k++;
				for(int j = k; j < (int)floor(FRAME_WIDTH*FREQ_RATIO); j++){
					frame.data[j] = 0;
				}
			} 
			vOut.write(frame);
			k = 0;
		}
		frame.data[k] = buf[i];
		k++;
	}
	// cout << "frame: " << frame << endl;

	// printf("final");

	// fprintf(out, "\n\n__________ VAD _________\n\n");
	// for (int i = x_start; i <= x_finish; i++)
	// 	{
	// 		//for (int j = 0; j < c; j++){
	// 			fprintf(out,"%f ",buf[i]);
	// 		//}
	// 		fprintf(out,"\n");
	// 	}

	fclose(out);
}

vad::vad(sc_module_name name) : sc_module(name){
	SC_HAS_PROCESS(vad);
	SC_THREAD(run);
}

