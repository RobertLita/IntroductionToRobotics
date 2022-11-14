#define BLINKING_TIME 500
#define LONG_PRESS_DURATION 3000

#define NO_PRESS 0
#define LONG_PRESS 1
#define SHORT_PRESS 2

#define MOVING_THROUGH 1
#define DISPLAY_LOCKED_IN 2

#define RESET_VALUE 0

#define UP 1
#define DOWN -1
#define LEFT -1
#define RIGHT 1

const int debounceDelay = 50;

const int minThreshold = 200;
const int maxThreshold = 800;
const int stillMinThreshold = 400;
const int stillMaxThreshold = 600;

const int latchPin = 11; // STCP to 12 on Shift Register
const int clockPin = 10; // SHCP to 11 on Shift Register
const int dataPin = 12; // DS to 14 on Shift Register

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;

unsigned long lastBlinkTime = 0;
unsigned long pressingTime = 0;
unsigned long lastDebounceTime;

int xJoyState, yJoyState = 0;
int xValue, yValue = 500;
bool joyMoved = false;

int currentDisplayIndex = 0;
byte dpState[] = {0, 0, 0, 0};
int displayValue[] = {0, 0, 0, 0};

byte buttonState = HIGH;
byte reading = HIGH;
byte lastReading = HIGH;
int buttonAction = NO_PRESS;

int currentState = 1;

const byte regSize = 8; // 1 byte aka 8 bits
int displayDigits[] = {
  segD1, segD2, segD3, segD4
};

const int encodingsNumber = 16;
int byteEncodings[encodingsNumber] = {
//A B C D E F G DP 
  B11111100, // 0 
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110  // F
};

const int displayCount = 4;

void setup() {

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  pinMode(pinSW, INPUT_PULLUP);
  Serial.begin(9600);
}


void writeReg(int encoding) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}


void activateDisplay(int digit) {
  for(int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }

  digitalWrite(displayDigits[digit], LOW);
}


void writeAllDigits() {
  for(int i = 0; i < displayCount; i++) {
    writeReg(byteEncodings[displayValue[i]] ^ dpState[i]);
    activateDisplay(i);
    delay(5);
  }
  
  digitalWrite(displayDigits[displayCount - 1], HIGH);
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


int nextDisplayToMove() {

  getjoyState();
  int displayToGo = currentDisplayIndex;

  if (xJoyState == LEFT && currentDisplayIndex - 1 >= 0) displayToGo = currentDisplayIndex - 1;
  else if (xJoyState == RIGHT && currentDisplayIndex + 1 < displayCount) displayToGo = currentDisplayIndex + 1;

  if (displayToGo != currentDisplayIndex)
    dpState[currentDisplayIndex] = 0;
  return displayToGo;
}


void reset() {
  currentState = 1;
  for (int i = 0; i < displayCount; i++) {
    displayValue[i] = RESET_VALUE;
    dpState[i] = 0;
  }
  currentDisplayIndex = 0;
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


void blinkDP() {

  if (millis() - lastBlinkTime > BLINKING_TIME) {
    lastBlinkTime = millis();
    dpState[currentDisplayIndex] = !dpState[currentDisplayIndex];
  }
}


void changeDigit(int direction) {
  if (direction == UP) {
    if (displayValue[currentDisplayIndex] + 1 < encodingsNumber)
      displayValue[currentDisplayIndex]++;
    else 
      displayValue[currentDisplayIndex] = 0;
  }
  else if (direction == DOWN) {
    if (displayValue[currentDisplayIndex] - 1 >= 0)
      displayValue[currentDisplayIndex]--;
    else 
      displayValue[currentDisplayIndex] = encodingsNumber - 1;
  }
}

void lockIn() {
  dpState[currentDisplayIndex] = 1;
}


void loop() {
  
  writeAllDigits();

  switch (currentState) {
    case MOVING_THROUGH: 
      blinkDP();
      buttonAction = manageButtonPressing();
      if (buttonAction == LONG_PRESS)
        reset();

      else if (buttonAction == SHORT_PRESS) {
        currentState = 2;
      }

      else
        currentDisplayIndex = nextDisplayToMove();
      break;

    case DISPLAY_LOCKED_IN:
      lockIn();
      getjoyState();
      if (yJoyState != 0) changeDigit(yJoyState);
      buttonAction = manageButtonPressing();
      if (buttonAction == SHORT_PRESS)
        currentState = 1;
      break;
  }
}