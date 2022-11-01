#define CARS_CROSSING 1
#define WAITING_FOR_LIGHT_SWITCH 2
#define CAR_YELLOW_ON 3
#define PEDESTRIANS_CROSSING 4
#define GREEN_BLINKING 5

#define DURATION_UNTILL_SWITCH 8000
#define YELLOW_LIGHT_DURATION 3000
#define PEDESTRIANS_CROSSING_DURATION 8000
#define GREEN_LIGHT_BLINKING_DURATION 4000
#define GREEN_BEEP_DURATION 800
#define BLINKING_GREEN_BEEP_DURATION 500
#define BLINKING_GREEN_LED_INTERVAL 500

const int buttonPin = 2;
const int buzzerPin = 9;

const int redCarLedPin = 5;
const int yellowCarLedPin = 6;
const int greenCarLedPin = 7;

const int redPedestrianLedPin = 10;
const int greenPedestrianLedPin = 11;

byte redCarLedState = LOW;
byte yellowCarLedState = LOW;
byte greenCarLedState = LOW;

byte redPedestrianLedState = LOW;
byte greenPedestrianLedState = LOW;

byte buttonState = HIGH;
byte buttonValue = LOW;
byte buzzerState = LOW;

byte reading = HIGH;
byte lastReading = HIGH;

unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

unsigned long timeSincePressing = 0;
unsigned long timeSinceYellow = 0;
unsigned long timeSincePedestriansGreen = 0;
unsigned long timeSinceGreenBlinking = 0;
unsigned long timeSinceLastBlink = 0;
unsigned long timeSinceLastBeep = 0;

char currentState = 1;

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  pinMode(redCarLedPin, OUTPUT);
  pinMode(yellowCarLedPin, OUTPUT);
  pinMode(greenCarLedPin, OUTPUT);

  pinMode(redPedestrianLedPin, OUTPUT);
  pinMode(greenPedestrianLedPin, OUTPUT);
  Serial.begin(9600);
}

byte getButtonState() {

  byte reading = digitalRead(buttonPin);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState != reading)
      buttonState = reading;
  }

  lastReading = reading;

  return buttonState;
}

void setLights(byte carRedState, byte carYellowState, byte carGreenState, byte pedestrianRedState, byte pedestrianGreenState) {

  digitalWrite(redCarLedPin, carRedState);
  digitalWrite(yellowCarLedPin, carYellowState);
  digitalWrite(greenCarLedPin, carGreenState);

  digitalWrite(redPedestrianLedPin, pedestrianRedState);
  digitalWrite(greenPedestrianLedPin, pedestrianGreenState);

}

void setSounds(int beepDuration) {
  if ((millis() - timeSinceLastBeep) > beepDuration) {
    timeSinceLastBeep = millis();
    buzzerState = !buzzerState;
  }
  if (buzzerState == LOW)
    noTone(buzzerPin);
  else
    tone(buzzerPin, 1000);
}

void blinkPedestrianGreen() {

  if ((millis() - timeSinceLastBlink) > BLINKING_GREEN_LED_INTERVAL) {
    timeSinceLastBlink = millis();
    greenPedestrianLedState = !greenPedestrianLedState;
  }

  digitalWrite(greenPedestrianLedPin, greenPedestrianLedState);
}


void loop() {

  switch (currentState) {

    case CARS_CROSSING:
      setLights(LOW, LOW, HIGH, HIGH, LOW);
      buttonState = getButtonState();
      buttonValue = !buttonState;
      if (buttonValue == HIGH) {
        currentState = WAITING_FOR_LIGHT_SWITCH;
        timeSincePressing = millis();
      }
      break;

    case WAITING_FOR_LIGHT_SWITCH:
      setLights(LOW, LOW, HIGH, HIGH, LOW);
      if ((millis() - timeSincePressing) == DURATION_UNTILL_SWITCH) {
        currentState = CAR_YELLOW_ON;
        timeSinceYellow = millis();
      }
      break;

    case CAR_YELLOW_ON:
      setLights(LOW, HIGH, LOW, HIGH, LOW);
      if ((millis() - timeSinceYellow) == YELLOW_LIGHT_DURATION) {
        currentState = PEDESTRIANS_CROSSING;
        timeSincePedestriansGreen = millis();
      }
      break;

    case PEDESTRIANS_CROSSING:
      setLights(HIGH, LOW, LOW, LOW, HIGH);
      setSounds(GREEN_BEEP_DURATION);
      if ((millis() - timeSincePedestriansGreen) == PEDESTRIANS_CROSSING_DURATION) {
        currentState = GREEN_BLINKING;
        timeSinceGreenBlinking = millis();
      }
      break;

    case GREEN_BLINKING:
      setLights(HIGH, LOW, LOW, LOW, LOW);
      blinkPedestrianGreen();
      setSounds(BLINKING_GREEN_BEEP_DURATION);
      if ((millis() - timeSinceGreenBlinking) == GREEN_LIGHT_BLINKING_DURATION) {
        currentState = CARS_CROSSING;
        buttonState = HIGH;
        buttonValue = LOW;
      }
      break;
  }
}