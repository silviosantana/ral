SC_MODULE (MPF){
	
	sc_fifo_in<Frame >  in_f;

	//sc_port<sc_fifo_in_if<vector<float> > > in_f;

	//sc_port<sc_fifo_out_if<T_ADD> > out1;
	//sc_out<int> out_f;


	void mpf_main();

	SC_CTOR(MPF){
		SC_THREAD(mpf_main);
	}
};