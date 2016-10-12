#ifndef __CONVERTTIMEFREQ_H__
#define __CONVERTTIMEFREQ_H__

#include "frame.h"
#include "fft.cpp"

using namespace std;

class convertTimeFreq : public sc_module {

public:
	explicit convertTimeFreq(sc_module_name name);
	Frame seg;
	Frame freqFrame;
	sc_fifo_in<Frame> pIn;
	sc_fifo_out<Frame> pOut;
	void run();
};

#endif