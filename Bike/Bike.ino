/* Mario Bike
https://github.com/elliotaplant/mario-bike

Turn your bike into a Mario Kart controller
*/

const int MIN_BUTTON_PIN = 1;
const int MAX_BUTTON_PIN = 12;
const int ANALOG_PIN = 0;

void setup()
{
  Serial.begin(38400);
  Serial.print("Beginning Mario Bike");

  // Mark all button pins as pullup inputs
  for (int p = MIN_BUTTON_PIN; p <= MAX_BUTTON_PIN; p++) {
    pinMode(p, INPUT_PULLUP);
  }
}

int val;

void loop()
{
  // Check button pins to see if any are pressed
  for (int p = MIN_BUTTON_PIN; p <= MAX_BUTTON_PIN; p++) {
    // Pass allong the pressed value through the USB
    Joystick.button(p, digitalRead(p));
  }

  Joystick.X(analogRead(ANALOG_PIN));

  delay(50);
}
