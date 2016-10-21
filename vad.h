#include "systemc.h"
#include "frame.h"
#include <stdio.h>
#include <math.h>
#include <sndfile.h>

#define PI 				3.14159265
#define FRAME_WIDTH		0.016			//duration of audio segments
#define FRAME_SHIFT		0.008			//shift in audio segments / overlap factor
#define FREQ_RATIO		8000			//sample rate of audio file
#define ZERO_THR		40				//zero crossing comparison rate for threshold
#define SILENCE			0.16			//initial silence duration in seconds


class vad : public sc_module{
public:
	explicit vad(sc_module_name name);
	sc_fifo_out<Frame> vOut;
	void run();
};
