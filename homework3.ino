#define BLINKING_TIME 400
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

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

int index = 0;

unsigned long lastBlinkTime = 0;

unsigned long lastDebounceTime;

int xJoyState, yJoyState = 0;

int currentLedPin = 1;
byte currentLedState = LOW;

byte buttonState = LOW;
byte buttonValue = HIGH;

byte reading = HIGH;
byte lastReading = HIGH;

int xValue, yValue = 500;

bool joyMoved = false;

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

int displayLedStates[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};

const int pinToGo[][4] = {
  { -1, 6, 5, 1 },
  { 0, 6, 5, -1 },
  { 5, 3, 4, 7 },
  { 5, -1, 4, 2 },
  { 5, 3, -1, 2 },
  { 0, 6, -1, 1 },
  { 0, 3, -1, -1 },
  { -1, -1, 2, -1 }
};

byte getButtonState() {

  reading = digitalRead(pinSW);
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState != reading)
      buttonState = reading;
      if (buttonState == LOW) {
        currentLedState = !currentLedState;
      }
  }

  lastReading = reading;

  return buttonState;
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
  }
  else if (xValue < minThreshold && joyMoved == false) {
    xJoyState = -1;
    yJoyState = 0;
    joyMoved = true;
  }    

  if (yValue > maxThreshold && joyMoved == false) {
    yJoyState = 1;
    joyMoved = true;
  }
  else if (yValue < minThreshold && joyMoved == false) {
    yJoyState = -1;
    joyMoved = true;
  }
}

void blinkLed(int currentLedPin) {

  if (millis() - lastBlinkTime > BLINKING_TIME) {
    lastBlinkTime = millis();
    currentLedState = !currentLedState;
  }

  digitalWrite(segments[currentLedPin], currentLedState);
}

void setDisplayLights() {
  for (int i = 0; i < segSize; i++) {
    digitalWrite(segments[i], displayLedStates[i]);
  }
}

void setup() {

  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(pinSW), buttonIsPressed, CHANGE);
  Serial.begin(9600);
}


void loop() {
  buttonState = getButtonState();
    
  getjoyState();
  if (xJoyState == -1 && pinToGo[currentLedPin][DOWN] != -1) currentLedPin = pinToGo[currentLedPin][DOWN];
  else if (xJoyState == 1 && pinToGo[currentLedPin][UP] != -1) currentLedPin = pinToGo[currentLedPin][UP];
  if (yJoyState == -1 && pinToGo[currentLedPin][RIGHT] != -1) currentLedPin = pinToGo[currentLedPin][RIGHT];
  else if (yJoyState == 1 && pinToGo[currentLedPin][LEFT] != -1) currentLedPin = pinToGo[currentLedPin][LEFT];
  setDisplayLights();
  blinkLed(currentLedPin);
  Serial.println(buttonValue);
}

void buttonIsPressed() {
  reading = !reading;
}