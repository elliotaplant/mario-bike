/* Mario Bike
https://github.com/elliotaplant/mario-bike

Turn your bike into a Mario Kart controller
*/

const int numButtons = 12;
const int ANALOG_PIN = 0;
const int scaleFactor = 2;
const int limit = 1024;
const int ledPin = 13;


void setup()
{
  Serial.begin(9600);

//  Serial.println("Beginning Mario Bike");
  Serial.print("hallValue");
  Serial.print("\t");
  Serial.print("trailingAverageValue");
  Serial.print("\t");
  Serial.println("difference");
  // configure the joystick to manual send mode.  This gives precise
  // control over when the computer receives updates, but it does
  // require you to manually call Joystick.send_now().
  Joystick.useManualSend(true);
  for (int i = 0; i < numButtons; i++) {
    pinMode(i, INPUT_PULLUP);
  }

  // Prepare the LED pin for output
  pinMode(ledPin, OUTPUT);

  setupSpeed();
}

byte allButtons[numButtons];
byte prevButtons[numButtons];

int leftJoystick = 1023;
int centerJoystick = 512;
int rightJoystick = 0;

void loop()
{
  int analogValue = analogRead(ANALOG_PIN);
  int interpolatedValue = interpolateJoystick(analogValue);
  
  Joystick.X(interpolatedValue);

  bool movin = moving();
  Serial.println(movin ? "Moving" : "Not Moving");
  
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
    if (allButtons[i] != prevButtons[i]) {
      anyChange = true;
    }
    prevButtons[i] = allButtons[i];
  }
  
  // if any button changed, print them to the serial monitor
  if (anyChange) {
    stateChangeButtons(analogValue); 
  }

  delay(5);
}

int interpolateJoystick(int analogValue) {
  if (analogValue > leftJoystick) {
    return 1024;
  } else if (analogValue > centerJoystick) {
    return ((512 * (analogValue - centerJoystick)) / (1024 - centerJoystick)) + 512;
  } else if (analogValue > rightJoystick) {
    return (512 * (analogValue - rightJoystick)) / (centerJoystick - rightJoystick);
  }
  return 0;
}

void stateChangeButtons(int joystickReading) {
  if (allButtons[4]) {
    if (allButtons[5]) {
      leftJoystick = joystickReading;
      Serial.print(joystickReading, DEC);
      Serial.println(" Lefting");
      blink();
    } else if (allButtons[2]) {
      Serial.print(joystickReading, DEC);
      Serial.println(" Centering");
      centerJoystick = joystickReading;
      blink();
    } else if (allButtons[3]) {
      Serial.print(joystickReading, DEC);
      Serial.println(" Righting");
      rightJoystick = joystickReading;
      blink();
    }
  }
}

void blink() {
  digitalWrite(ledPin, HIGH);   // set the LED on
  delay(1000);                  // wait for a second
  digitalWrite(ledPin, LOW);    // set the LED off
}
