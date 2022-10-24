#define potUpperBound 1023
#define potLowerBound 0
#define ledUpperBound 255
#define ledLowerBound 0

const int redLedPin = 11;
const int greenLedPin = 10;
const int blueLedPin = 9;

const int redPotPin = A0;
const int greenPotPin = A1;
const int bluePotPin = A2;

unsigned short int redPotValue;
unsigned short int greenPotValue;
unsigned short int bluePotValue;

unsigned char redLedValue;
unsigned char greenLedValue;
unsigned char blueLedValue;


void setup() {
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
}

void loop() {

  redPotValue = analogRead(redPotPin);
  greenPotValue = analogRead(greenPotPin);
  bluePotValue = analogRead(bluePotPin);

  redLedValue = map(redPotValue, potLowerBound, potUpperBound, ledLowerBound, ledUpperBound);
  greenLedValue = map(greenPotValue, potLowerBound, potUpperBound, ledLowerBound, ledUpperBound);
  blueLedValue = map(bluePotValue, potLowerBound, potUpperBound, ledLowerBound, ledUpperBound);

  analogWrite(redLedPin, redLedValue);
  analogWrite(greenLedPin, greenLedValue);
  analogWrite(blueLedPin, blueLedValue);
}
