/* Pushbutton, Teensyduino Tutorial #3
http://www.pjrc.com/teensy/tutorial3.html

This example code is in the public domain.
*/

void setup() {
  Serial.begin(38400);
  pinMode(0, INPUT);
}

void loop()
{
  if (digitalRead(0) == HIGH) {
    Serial.println("Button is not pressed...");
  } else {
    Serial.println("Button pressed!!!");
  }
  delay(250);
}
