#include "testFunc.h"

testFunc::testFunc(sc_module_name name) : sc_module(name){
    SC_HAS_PROCESS(testFunc);
    SC_THREAD(run);
}

void testFunc::run()
{
	for(int i = 0; i < D_TAM; i++) test.data[i] = 1;
	test.last = true;
	test.size = 1;
	//cout << test << endl;
   	pOut->write(test);
   	//cout << "aqui_1" << endl;
   	//sc_stop();
}