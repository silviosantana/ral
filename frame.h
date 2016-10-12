#ifndef __FRAME_H__
#define __FRAME_H__

#include <cstdio>
#include <iostream>
#include <cmath>
#include <vector>
#include <systemc.h>

#define FREQ_RATIO 8000
#define FRAME_WIDTH 0.016
#define D_TAM 128
#define PI 3.14159 //Definição de Pi

typedef struct{
	float data [D_TAM];
	bool last;
	int size;
}Frame;

inline ostream & operator <<(ostream & os, const Frame & dt)
{
  os << "data[ ";
  for (unsigned int i = 0; i < D_TAM; i++){
   os << dt.data[i] << " ";
  }
  os << "] last: " << dt.last << std::endl;
  return os;
}

/*	inline bool operator ==(const Frame & dt) const {
	  unsigned int check_int;
	  check_int = 1;
	  for (unsigned int i = 0; i < D_TAM; i++){
	   if(dt.data[i] == data[i])
		check_int++;
	  }
	  return (check_int == D_TAM) && (dt.last == last);
	}

	inline friend void sc_trace(sc_trace_file * &tf, const Frame & dt, std::string & name)
	{
	  for (unsigned int i = 0; i < D_TAM; i++){
		std::stringstream str;
		str << name << ".data_" << i;
		sc_trace(tf, dt.data[i], str.str());
	  }
	  sc_trace(tf, dt.last, name + ".flag");
	}*/

#endif
