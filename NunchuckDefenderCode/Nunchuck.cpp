#include "Nunchuck.h"

Nunchuck::Nunchuck() {
   cnt = 0;
}

void Nunchuck::readNunchuck() {
	Wire.requestFrom(0x52, 6);	// request data from nunchuck   
	while (Wire.available()) {       
		outbuf[cnt] = nunchuck_decode_byte(Wire.read());	// receive byte as an integer
		cnt++;
	}    // If we recieved the 6 bytes, then go print them   
	if (cnt >= 5) {
		nunchuckValues[XSTICK] = outbuf[0];
		nunchuckValues[YSTICK] = outbuf[1];
		nunchuckValues[ZBUTTON] = 0;
		nunchuckValues[CBUTTON] = 0;
		// byte outbuf[5] contains bits for z and c buttons
		// it also contains the least significant bits for the accelerometer data
		// so we have to check each bit of byte outbuf[5]
		if((outbuf[5] >> 0) & 1) {nunchuckValues[ZBUTTON] = 1;} //checking if Z button is pressed (0=pressed 1=unpressed)
		if((outbuf[5] >> 1) & 1) {nunchuckValues[CBUTTON] = 1;} //checking if C button is pressed (0=pressed 1=unpressed)
        }
        cnt = 0;
	send_zero(); // send the request for next bytes
}

int Nunchuck::getNunValue(int valueIndex) {
	return nunchuckValues[valueIndex];
}

int* Nunchuck::getNunValues() {
        return nunchuckValues;
}

void Nunchuck::nunchuck_init() {
	Wire.begin();		// join i2c bus with address 0x52
	Wire.beginTransmission(0x52);	// transmit to device 0x52
	Wire.write(0x40);		// writes memory address
	Wire.write(0x00);		// sends sent a zero.  
	Wire.endTransmission();	// stop transmitting
}

void Nunchuck::send_zero() {
	Wire.beginTransmission(0x52);	// transmit to device 0x52
	Wire.write(0x00);		// sends one byte
	Wire.endTransmission();	// stop transmitting
}

char Nunchuck::nunchuck_decode_byte(char x) {
	x = (x ^ 0x17) + 0x17;
	return x;
}


//Prints the Nunchucks last read data (must call NUN_readNunchuck(); before calling
void Nunchuck::printData() {
  Serial.print("XJoy= ");Serial.print (getNunValue(XSTICK), DEC); Serial.print ("\t");
  Serial.print("YJoy= ");Serial.print (getNunValue(YSTICK), DEC); Serial.print ("\t");
  Serial.print("ZBut= ");Serial.print (getNunValue(ZBUTTON), DEC);  Serial.print ("\t");
  Serial.print("YBut= ");Serial.print (getNunValue(CBUTTON), DEC);  Serial.print ("\t");
}

Nunchuck Chuck = Nunchuck();
