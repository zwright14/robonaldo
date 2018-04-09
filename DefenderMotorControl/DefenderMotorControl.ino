// RF LINK RECEIVER CODE
#include <VirtualWire.h>

#define DEADBAND 20 
#define ymax 90
#define ymin -90
#define xmax 90
#define xmin -90

//Define Pins
const int enablePin1 = 1;
const int motor1CP1 = 8;
const int motor1CP2 = 9;
const int enablePin2 = 4;
const int motor2CP1 = 12;
const int motor2CP2 = 10;
const int rxPin = 11;

//Define States
int xval;
int yval;

void setup()
{
  
  pinMode(enablePin1, OUTPUT);
  pinMode(motor1CP1, OUTPUT);
  pinMode(motor1CP2, OUTPUT);
  pinMode(enablePin2, OUTPUT);
  pinMode(motor2CP1, OUTPUT);
  pinMode(motor2CP2, OUTPUT);
  digitalWrite(enablePin1, LOW);
  digitalWrite(enablePin2, LOW);
  

  //Initialize the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	    // Bits per sec
  vw_set_rx_pin(rxPin);         //Pin 2 is connected to "Digital Output" of receiver
  vw_rx_start();           // Start the receiver PLL running

}

void loop()
{

  //Set buffer array based on max message length
  uint8_t buf[VW_MAX_MESSAGE_LEN];

  //Set variable to indicate buffer length
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    xval = (buf[1]<<8)+buf[0];;
    yval = (buf[3]<<8)+buf[2];
    
    moveDifferential(map(xval,30,220,-100,100),map(yval,30,220,-100,100));
  }
}  

void moveDifferential(int xdir, int ydir) {   
  int moveSpeed = 255;
  if (ydir>=ymax) {
    moveForward(); 
  }
  else if (ydir<=ymin) {
    moveBackward();
  }
  else if (xdir>=xmax) {
    moveRight();
  }
  else if (xdir<=xmin) {
    moveLeft();
  }
  else {
    moveSpeed=0;
  }
  analogWrite(enablePin2, moveSpeed);
  analogWrite(enablePin1, moveSpeed); 
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

void moveForward() {
  digitalWrite(motor1CP1, LOW);
  digitalWrite(motor1CP2, HIGH);
  digitalWrite(motor2CP1, LOW);
  digitalWrite(motor2CP2, HIGH);

}

void moveBackward() {
  digitalWrite(motor1CP1, HIGH);
  digitalWrite(motor1CP2, LOW);
  digitalWrite(motor2CP1, HIGH);
  digitalWrite(motor2CP2, LOW);

}

void moveLeft() {
  digitalWrite(motor1CP1, HIGH);
  digitalWrite(motor1CP2, LOW);
  digitalWrite(motor2CP1, LOW);
  digitalWrite(motor2CP2, HIGH);
}

void moveRight() {
  digitalWrite(motor1CP1, LOW);
  digitalWrite(motor1CP2, HIGH);
  digitalWrite(motor2CP1, HIGH);
  digitalWrite(motor2CP2, LOW);
}
