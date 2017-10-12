void setup() {
  //Start serial communication at 9600 bits per second
  Serial.begin(9600);

}

void loop() {
  //Read the sensor input on analog pin 0
  int sensorReading = analogRead(A0);
  
  //Convert the analog input to a voltage reading
  float voltageReading = sensorReading * (5.0 / 1023.0);
  
  //Account for the voltage divider used to step down the voltage
  float actualVoltage = voltageReading * 11.0;
  
  Serial.println(actualVoltage);
}
