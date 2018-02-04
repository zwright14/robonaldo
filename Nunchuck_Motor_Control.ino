//START OF NUNCHUCK PREAMBLE - For more in depth information please visit the original source of this code http://www.windmeadow.com/node/42 
//-------------------------------------------------
/*
 * Wiring Details
 * white  - ground
 * red    - 3.3+v  - 5 volts seems to work
 * green  - data   - Analog 4
 * yellow - clock  - Analog 5
 */

#include <Wire.h>

uint8_t outbuf[6];		// array to store arduino output 
int ledPin = 13;  
int cnt = 0;                    // counter used for nunchuck comunication 
const int controlPin1 = 2;
const int controlPin2 = 3;
const int enablePin = 9;
int nunchuckValues[] = {0,0,0,0,0,0,0,0,0,0}; //An array to store the nuncheck values  
/*  * The index of each value within the nunchuckValues[] array  * ie. XSTICK value is stored at nunchuckValues[XSTICK] (0)  */ 
#define XSTICK 0      //The joystick values      
#define YSTICK 1 
#define XAXIS 2       //The three accelerometer values      
#define YAXIS 3         
#define ZAXIS 4  
#define ZBUTTON 5     //Front button values  (0 when pressed)   
#define CBUTTON 6   
#define XAXISDELTA 7  //Change in accelerometer data from last read; 
#define YAXISDELTA 8    
#define ZAXISDELTA 9   //Nunchuck G calculating Constants 
/*  
* For scaling the raw values from the nunchuck into G values  
* Zero Points        x_0 = (x_1 + x_2) / 2\,y_0 = (y_1 + y_3) / 2\,z_0 = (z_2 + z_3) / 2\,      One G points        x = \frac{x_{raw} - x_0}{x_3 - x_0}y = \frac{y_{raw} - y_0}{y_2 - y_0}z = \frac{z_{raw} - z_0}{z_1 - z_0} */
/* Not all of these are used and could be deleted (kept to make interpretting math's  
* Easier 0-Zero G Value 1-Value when laying on table 2-Value when resting on nose  
* 3-Value when resting on side (left side up) 
*/ 
#define X0 500  
#define X1 500 
#define X2 500 
#define X3 711  
#define Y0 465 
#define Y1 481 
#define Y2 621 
#define Y3 449  
#define Z0 578 
#define Z1 785 
#define Z2 575 
#define Z3 582  
//END OF NUNCHUCK PREAMBLE
//------------------------------------------------------  
//-------------------------------------------------------------------------- 

long lastPrint; //a long variable to store the time the wiimote state was last printed 
#define PRINTINTERVAL 1000  //the number of milliseconds between outputting the nunchuck state over the usb port 
#define DEADBAND 20         //A percentage away from center that is interpretted as still being zero   

void setup() {   
  Serial.begin(9600);   //Starts the serial port (used for debuging however makes servos jumpy)   
  nunchuck_init();     // send the nunchuck initilization handshake
  pinMode(controlPin1, OUTPUT);
  pinMode(controlPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);  
  lastPrint = millis(); 
}

void loop() {   
  readNunchuck();       //Reads the current state of the nunchucks buttons and accelerometers     
  moveWiiJoystick();    
  if((millis() - lastPrint) > PRINTINTERVAL) {  //If a second has passed since last printing nunchuck values print them     
    printData();          //print nunchuck values     
    Serial.println();     //add an enter     
    lastPrint = millis(); //store current time as lastPrint   
  }  
}  

void moveWiiJoystick() {  
  moveDifferential(map(getNunValue(YSTICK),30,220,-100,100));  
}  //Takes in a speed and a direction input (like a joystick) and translates it to speed commands  

void moveDifferential(int speed1) {   
  speed1 = deadBandFilter(speed1);
  setSpeedMotor(speed1);  
  //setSpeedLeft(speed1 + direction1);   
  //setSpeedRight(speed1 - direction1); 
}

int deadBandFilter(int value) {  
  if(value > -DEADBAND && value < DEADBAND) {
    value = 0;
  }  
  else {    
    if(value > 0) {
      value = value - DEADBAND * 100 / (100-DEADBAND);
    }    
    else {
      value = value + DEADBAND * 100 / (100-DEADBAND);
    }  
  }  
return value;  
}

void setSpeedMotor(int speedVal) {
  if (speedVal > 0) {
    digitalWrite(controlPin1, HIGH);
    digitalWrite(controlPin2, LOW);
  }
  
  else if (speedVal < 0) {
    digitalWrite(controlPin2, HIGH);
    digitalWrite(controlPin1, LOW);
  }
  
  int absSpeedVal = abs(speedVal);
  int mapSpeed = map(absSpeedVal, 0, 100, 0, 255);
  analogWrite(enablePin, mapSpeed);
}

//START OF NUNCHUCK Reading CODE
//--------------------------------------------------------------- 
void readNunchuck() {   
  Wire.requestFrom (0x52, 6);	// request data from nunchuck   
  while (Wire.available ()) {       
    outbuf[cnt] = nunchuk_decode_byte (Wire.read ());	// receive byte as an integer       
    digitalWrite (ledPin, HIGH);	// sets the LED on       
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
    if ((outbuf[5] >> 0) & 1) { nunchuckValues[ZBUTTON] = 1; } //checking if Z button is pressed (0=pressed 1=unpressed)
    if ((outbuf[5] >> 1) & 1) { nunchuckValues[CBUTTON] = 1; } //checking if C button is pressed (0=pressed 1=unpressed)
  }
  cnt = 0;
  send_zero (); // send the request for next bytes
}

int getNunValue(int valueIndex){
  return nunchuckValues[valueIndex];
}

void nunchuck_init() {
  Wire.begin();		// join i2c bus with address 0x52
  Wire.beginTransmission(0x52);	// transmit to device 0x52
  Wire.write(0x40);		// writes memory address
  Wire.write(0x00);		// sends sent a zero.  
  Wire.endTransmission();	// stop transmitting
}

void send_zero() {
  Wire.beginTransmission(0x52);	// transmit to device 0x52
  Wire.write(0x00);		// sends one byte
  Wire.endTransmission();	// stop transmitting
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char nunchuk_decode_byte(char x) {
  x = (x ^ 0x17) + 0x17;
  return x;
}

//END OF NUNCHUCK CODE
//--------------------------------------------------------------

//START  OF PRINT ROUTINES (can delete if not using)
//---------------------------------------------------------------

//Prints the Nunchucks last read data (must call NUN_readNunchuck(); before calling
void printData(){
  Serial.print("XJoy= ");Serial.print (getNunValue(XSTICK), DEC); Serial.print ("\t");
  Serial.print("YJoy= ");Serial.print (getNunValue(YSTICK), DEC); Serial.print ("\t");
  Serial.print("ZBut= ");Serial.print (getNunValue(ZBUTTON), DEC);  Serial.print ("\t");
  Serial.print("YBut= ");Serial.print (getNunValue(CBUTTON), DEC);  Serial.print ("\t");
}

//END OF PRINT ROUTINES
//--------------------------------------------------------------------
