#include "systemc.h"
#include "frame.h"

SC_MODULE (MPF){
	
	sc_fifo_in<Frame>  in_f;

	void mpf_main();

	SC_CTOR(MPF){
		SC_THREAD(mpf_main);
	}
};
