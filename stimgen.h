SC_MODULE (stimgen){

	sc_port<sc_fifo_out_if<Frame> > a1;

	void stim_proc();

	SC_CTOR(stimgen){
		SC_THREAD(stim_proc);
	}
};