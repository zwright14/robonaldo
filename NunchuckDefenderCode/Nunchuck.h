#ifndef Nunchuck_h
#define Nunchuck_h

#include "Arduino.h"
#include "Wire.h"

#define XSTICK 0      //The joystick values      
#define YSTICK 1 
#define ZBUTTON 2     //Front button values  (0 when pressed)   
#define CBUTTON 3 

class Nunchuck {
	public:
                Nunchuck();
		void readNunchuck();
		int getNunValue(int valueIndex);
                int* getNunValues();
		void nunchuck_init();
		void send_zero();
		char nunchuck_decode_byte(char x);
                void printData();
	private:
                int nunchuckValues[4] = {0,0,0,0};
                uint8_t outbuf[6];		// array to store arduino output 
                int cnt;
	
};

extern Nunchuck Chuck;

#endif
