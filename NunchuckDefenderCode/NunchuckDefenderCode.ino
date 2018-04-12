#include "Nunchuck.h"
#include <Wire.h>
#include <VirtualWire.h>

const int txPin = 2;

void setup() {   
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	    // Bits per sec
  vw_set_tx_pin(txPin);         //Pin 3 is connected to "Digital Output" of transmitter

  Chuck.nunchuck_init();     // send the nunchuck initilization handshake
}

void loop() {   
  Chuck.readNunchuck();       //Reads the current state of the nunchucks buttons and accelerometers
  int *myVals = Chuck.getNunValues();
  int NunchuckValues[2] = {Chuck.getNunValue(XSTICK),Chuck.getNunValue(YSTICK)};
  vw_send((uint8_t *)NunchuckValues, 8);//send message
  vw_wait_tx(); // Wait until the whole message is gone
}


