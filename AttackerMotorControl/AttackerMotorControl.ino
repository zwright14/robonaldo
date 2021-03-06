// RF LINK RECEIVER CODE
#include <VirtualWire.h>

#define DEADBAND 20 
#define ymax 90
#define ymin -90
#define xmax 90
#define xmin -90

//Define Pins
const int enablePin1 =3;
const int motor1CP1 = 2;
const int motor1CP2 = 4;
const int enablePin2 = 6;
const int motor2CP1 = 5;
const int motor2CP2 = 7;
const int enablePin3 = 10;
const int motor3CP1 = 8;
const int motor3CP2 = 9;
const int rxPin = 11;

//Define States
int xval;
int yval;
int zbut;
int cbut;
int cnt = 0;

void setup()
{ 
  pinMode(enablePin1, OUTPUT);
  pinMode(motor1CP1, OUTPUT);
  pinMode(motor1CP2, OUTPUT);
  pinMode(enablePin2, OUTPUT);
  pinMode(motor2CP1, OUTPUT);
  pinMode(motor2CP2, OUTPUT);
  pinMode(enablePin3, OUTPUT);
  pinMode(motor3CP1, OUTPUT);
  pinMode(motor3CP2, OUTPUT);
  digitalWrite(enablePin1, LOW);
  digitalWrite(enablePin2, LOW);
  digitalWrite(enablePin3, LOW);

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
    cnt = 0;
    // Flash status LED to show received data
    xval = (buf[1]<<8)+buf[0];
    yval = (buf[3]<<8)+buf[2];
    zbut = (buf[5]<<8)+buf[4];
    cbut = (buf[7]<<8)+buf[6];
 
    moveDifferential(map(xval,30,220,-100,100),map(yval,30,220,-100,100));
    startSpinFeeder(zbut, cbut);
  }
  else {
    cnt++;
    if (cnt>100000) {
      moveDifferential(0,0);
      startSpinFeeder(1,1);
    }
  }
}  

void startSpinFeeder(int zbut, int cbut) {
  int spinFeederSpeed = 255;
  if (zbut==0) {
    digitalWrite(motor3CP1, LOW);
    digitalWrite(motor3CP2, HIGH);
    analogWrite(enablePin3, spinFeederSpeed);
  }
  else if (cbut==0) {
    digitalWrite(motor3CP1, HIGH);
    digitalWrite(motor3CP2, LOW);
    analogWrite(enablePin3, spinFeederSpeed);
  } 
  else {
    spinFeederSpeed = 0;
  }
    analogWrite(enablePin3, spinFeederSpeed);
}

void moveDifferential(int xdir, int ydir) {   
  int driveMotorSpeed = 200;
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
    driveMotorSpeed=0;
  }
  analogWrite(enablePin2, driveMotorSpeed);
  analogWrite(enablePin1, driveMotorSpeed); 
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

