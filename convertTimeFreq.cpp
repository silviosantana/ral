#include "convertTimeFreq.h"
#include <iostream>
#include <fstream>


#define PI 				3.14159265

convertTimeFreq::convertTimeFreq(sc_module_name name) : sc_module(name){
    SC_HAS_PROCESS(convertTimeFreq);
    SC_THREAD(run);
}

void convertTimeFreq::run()
{
    vector<double> inputReal(D_TAM);
    vector<double> inputImg(D_TAM,0);


    ofstream myfile;
    myfile.open("maria.txt");
    while(1){
        seg = pIn->read();
        // myfile << seg << endl;
    	//Aplicando o Janelamento de Hamming w[n] = input*(0.54 - 0.46*cos(2pin/M))
    	for(int i = 0; i < D_TAM; i++){ 
            inputReal[i] =  (0.54 - (0.46*cos((2*PI*i)/(D_TAM-1))))*seg.data[i];
            inputImg[i] = 0;
        }
        //Transformada de Fourier
        Fft::transform(inputReal, inputImg);

       // myfile << "Frame novo" << endl;
        for(int i = 0; i < D_TAM; i++)
        {

            freqFrame.data[i] = sqrt((inputReal[i]*inputReal[i]) + (inputImg[i]*inputImg[i]));
            myfile << freqFrame.data[i] << endl;
        }

        freqFrame.last = seg.last;
        freqFrame.size = seg.size;
        pOut->write(freqFrame);
        
    }
}
