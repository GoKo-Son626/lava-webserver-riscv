
/*
Use following commands to compile and run

arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old ./blink.ino
arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328old ./blink.ino
*/

void setup() {
  pinMode(A0, OUTPUT);        // D14 is A0
  digitalWrite(A0, LOW);
  delay(200);
  digitalWrite(A0, HIGH); 
}

void loop() {
}

