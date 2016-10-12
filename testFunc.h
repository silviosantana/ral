#ifndef __TESTFUNC_H__
#define __TESTFUNC_H__

#include "frame.h"

using namespace std;

class testFunc : public sc_module {

public:
	explicit testFunc(sc_module_name name);
	Frame test;
	sc_fifo_out<Frame> pOut;
	void run();
};

#endif