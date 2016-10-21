#include "systemc.h"
//#include "stimgen.cpp"
#include "convertTimeFreq.h"
#include "MPF.h"
#include "vad.h"


int sc_main(int argc, char *argv[]){

	sc_fifo<Frame> vad_fft_channel(500);
	sc_fifo<Frame> fft_mpfo_channel(500);

	vad vad("vad");
	convertTimeFreq cv ("convert");
	MPF mpf("mpf");

	vad.vOut(vad_fft_channel);
	cv.pIn(vad_fft_channel);
	cv.pOut(fft_mpfo_channel);
	mpf.in_f(fft_mpfo_channel);

	sc_start();

	return 0;
}
