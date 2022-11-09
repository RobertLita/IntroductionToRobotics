#define BLINKING_TIME 500
#define LONG_PRESS_DURATION 3000

#define NO_PRESS 0
#define LONG_PRESS 1
#define SHORT_PRESS 2

#define LED_MOVING 1
#define LED_LOCKED_IN 2

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define NA -1

const int debounceDelay = 50;

const int minThreshold = 200;
const int maxThreshold = 800;
const int stillMinThreshold = 400;
const int stillMaxThreshold = 600;

const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;
const int segSize = 8;
const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;

unsigned long lastBlinkTime = 0;
unsigned long pressingTime = 0;
unsigned long lastDebounceTime;

int xJoyState, yJoyState = 0;
int xValue, yValue = 500;
bool joyMoved = false;

int currentLedPin = pinDP;
byte currentLedState = LOW;

byte buttonState = HIGH;
byte reading = HIGH;
byte lastReading = HIGH;
int buttonAction = NO_PRESS;

int currentState = 1;


int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

byte displayLedStates[] = { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW };

const int pinToGo[][4] = {
  {},
  {},
  {},
  {},
  { NA, pinG, pinF, pinB },
  { pinA, pinG, pinF, NA },
  { pinG, pinD, pinE, pinDP },
  { pinG, NA, pinE, pinC },
  { pinG, pinD, NA, pinC },
  { pinA, pinG, NA, pinB },
  { pinA, pinD, NA, NA },
  { NA, NA, pinC, NA }
};


void setup() {

  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
  Serial.begin(9600);
}


void getjoyState() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  xJoyState = 0;
  yJoyState = 0;

  if (stillMinThreshold <= xValue && xValue <= stillMaxThreshold && stillMinThreshold <= yValue && yValue <= stillMaxThreshold) {
    joyMoved = false;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    xJoyState = 1;
    joyMoved = true;
  } else if (xValue < minThreshold && joyMoved == false) {
    xJoyState = -1;
    joyMoved = true;
  }

  if (yValue > maxThreshold && joyMoved == false) {
    yJoyState = 1;
    joyMoved = true;
  } else if (yValue < minThreshold && joyMoved == false) {
    yJoyState = -1;
    joyMoved = true;
  }
}


void toggleLed() {
  displayLedStates[currentLedPin] = !displayLedStates[currentLedPin];
  
  digitalWrite(currentLedPin, displayLedStates[currentLedPin]);
}


int nextPinToMove() {

  getjoyState();
  int ledToGo = currentLedPin;

  if (xJoyState == -1 && pinToGo[currentLedPin][DOWN] != NA) ledToGo = pinToGo[currentLedPin][DOWN];
  else if (xJoyState == 1 && pinToGo[currentLedPin][UP] != NA) ledToGo = pinToGo[currentLedPin][UP];
  if (yJoyState == -1 && pinToGo[currentLedPin][RIGHT] != NA) ledToGo = pinToGo[currentLedPin][RIGHT];
  else if (yJoyState == 1 && pinToGo[currentLedPin][LEFT] != NA) ledToGo = pinToGo[currentLedPin][LEFT];

  return ledToGo;
}


void reset() {
  currentState = 1;
  for (int i = pinA; i <= pinDP; i++) {
    displayLedStates[i] = LOW;
  }
  currentLedPin = pinDP;
  buttonAction = NO_PRESS;
}


int manageButtonPressing() {

  reading = digitalRead(pinSW);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState != reading) {
      buttonState = reading;
      if (buttonState == LOW)
        pressingTime = millis();
      else if (millis() - pressingTime < LONG_PRESS_DURATION)
        return SHORT_PRESS;

    } else if (buttonState == LOW && millis() - pressingTime >= LONG_PRESS_DURATION)
      return LONG_PRESS;
  }

  lastReading = reading;

  return NO_PRESS;
}


void blinkCurrentLed() {

  if (millis() - lastBlinkTime > BLINKING_TIME) {
    lastBlinkTime = millis();
    currentLedState = !currentLedState;
  }

  digitalWrite(currentLedPin, currentLedState);
}


void setDisplayLights() {
  for (int i = 0; i < segSize; i++) {
    if (segments[i] != currentLedPin)
      digitalWrite(segments[i], displayLedStates[segments[i]]);
    else
     digitalWrite(segments[i], LOW);
  }
}


void loop() {
  switch (currentState) {

    case LED_MOVING:
      setDisplayLights();
      blinkCurrentLed();

      buttonAction = manageButtonPressing();

      if (buttonAction == LONG_PRESS)
        reset();

      else if (buttonAction == SHORT_PRESS) {
        digitalWrite(currentLedPin, displayLedStates[currentLedPin]);
        currentState = 2;
      }
      currentLedPin = nextPinToMove();
      break;

    case LED_LOCKED_IN:
      getjoyState();
      if (yJoyState == -1) toggleLed();
      buttonAction = manageButtonPressing();
      if (buttonAction == SHORT_PRESS)
        currentState = 1;
      break;
  }
}