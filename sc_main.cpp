#include "systemc.h"
#include "frame.h"
//#include "stimgen.cpp"
#include "testFunc.cpp"
#include "convertTimeFreq.cpp"
#include "MPF.cpp"


int sc_main(int argc, char *argv[]){

	//sc_fifo<Frame > s1(10);

	//stimgen stim("stim");
	//stim(s1);


	sc_fifo<Frame> vad_fft_channel(500);
	sc_fifo<Frame> fft_mpfo_channel(500);

	testFunc tst("test");
	convertTimeFreq cv ("convert");
	MPF mpf("mpf");

	tst.pOut(vad_fft_channel);
	cv.pIn(vad_fft_channel);
	cv.pOut(fft_mpfo_channel);
	mpf.in_f(fft_mpfo_channel);

	sc_start();

	return 0;
}