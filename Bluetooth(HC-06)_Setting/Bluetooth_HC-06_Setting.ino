#include <SoftwareSerial.h>

int Tx = 2;
int Rx = 3;
SoftwareSerial bluetooth(Tx, Rx);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  bluetooth.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(bluetooth.available()) {
    Serial.write(bluetooth.read());
  }
  if(Serial.available()) {
    bluetooth.write(Serial.read());
  }
}
