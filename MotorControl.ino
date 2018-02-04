// Define pins that need to be used
const int controlPin1 = 2;
const int controlPin2 = 3;
const int enablePin = 9;
const int directionSwitchPin = 4;
const int onOffSwitchPin = 5;
const int potPin = A0;

// Define states
int onOffState = 0;
int previousOnOffState = 0;
int directionState = 0;
int previousDirectionState = 0;

// Define motor speecifications
int motorEnabled = 0;
int motorSpeed = 0;
int motorDirection = 0;


void setup() {
  pinMode(directionSwitchPin, INPUT);
  pinMode(onOffSwitchPin, INPUT);
  pinMode(controlPin1, OUTPUT);
  pinMode(controlPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
}

void loop() {
  onOffState = digitalRead(onOffSwitchPin);
  delay(1);
  directionState = digitalRead(directionSwitchPin);
  motorSpeed = analogRead(potPin)/4;
  
  //Enable or disable the motor when pressing onOffSwitchPin
  if(onOffState != previousOnOffState) {
    if(onOffState == HIGH) {
      motorEnabled = !motorEnabled;
    }
  }
  
  //Switch the direction of the motor when pressing directionSwitchPin
  if (directionState != previousDirectionState) {
    if (directionState == HIGH) {
      motorDirection = !motorDirection;
    }
  }
  
  if (motorDirection == 1) {
    digitalWrite(controlPin1, HIGH);
    digitalWrite(controlPin2, LOW);
  }
  
  else {
    digitalWrite(controlPin2, HIGH);
    digitalWrite(controlPin1, LOW);
  }
  
  if (motorEnabled == 1) {
    analogWrite(enablePin, motorSpeed);
  }
  
  else {
    analogWrite(enablePin, 0);
  }
  
  previousDirectionState = directionState;
  previousOnOffState = onOffState; 
}
