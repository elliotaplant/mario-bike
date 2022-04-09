/* Mario Bike
https://github.com/elliotaplant/mario-bike

Turn your bike into a Mario Kart controller
*/

const int numButtons = 12;
const int ANALOG_PIN = 0;

void setup()
{
  Serial.begin(9600);

  Serial.print("Beginning Mario Bike");
  // configure the joystick to manual send mode.  This gives precise
  // control over when the computer receives updates, but it does
  // require you to manually call Joystick.send_now().
  Joystick.useManualSend(true);
  for (int i = 0; i < numButtons; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  Serial.println("Begin Complete Joystick Test");
}

byte allButtons[numButtons];
byte prevButtons[numButtons];

void loop()
{
  Joystick.X(analogRead(ANALOG_PIN));
  
   // read digital pins and use them for the buttons
  for (int i = 0; i < numButtons; i++) {
    if (digitalRead(i)) {
      // when a pin reads high, the button is not pressed
      // the pullup resistor creates the "on" signal
      allButtons[i] = 0;
    } else {
      // when a pin reads low, the button is connecting to ground.
      allButtons[i] = 1;
    }
    Joystick.button(i + 1, allButtons[i]);
  }
  
  // Because setup configured the Joystick manual send,
  // the computer does not see any of the changes yet.
  // This send_now() transmits everything all at once.
  Joystick.send_now();

  // check to see if any button changed since last time
  boolean anyChange = false;
  for (int i=0; i<numButtons; i++) {
    if (allButtons[i] != prevButtons[i]) anyChange = true;
    prevButtons[i] = allButtons[i];
  }
  
  // if any button changed, print them to the serial monitor
  if (anyChange) {
    Serial.print("Buttons: ");
    for (int i=0; i<numButtons; i++) {
      Serial.print(allButtons[i], DEC);
    }
    Serial.println();
  }

  delay(5);
}
