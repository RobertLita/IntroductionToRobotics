#ifndef Menu
#define Menu

#include <LiquidCrystal.h>
#include <string.h>
#include "LCDSymbols.h"
#include "StorageManager.h"
#include "Globals.h"

unsigned long lastDebounceTime;

byte lastReading = HIGH;
byte buttonState = LOW;

String menuItems[] = { "NUME JOC", "Start Game", "Highscores", "Settings", "About", "How to play" };
String settingsItems[] = { "SETTINGS", "Your name", "Difficulty", "Contrast", "Brightness", "Matrix Light", "Sounds", "Reset HScores", "Back to Menu" };
String aboutItems[] = { "ABOUT", "Nume Joc", "Author:", "Lita Robert", "Github user:", "RobertLita", "Back to Menu" };
String htpItems[] = { "HOW TO PLAY", "1. Eat food", "2. Be fast", "3. Have fun", "Back to Menu" };
String hsItems[] = { "HIGHSCORES", "1.", "2.", "3.", "4.", "5.", "Back to Menu" };

byte currentMenuState = 1;
byte selectedSettingsItem = 1;
byte selectedHighscoresItem = 1;
byte selectedMenuItem = 1;
byte selectedHighscoreItem = 2;
byte selectedHTPItem = 1;
byte selectedAboutItem = 1;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

unsigned long long welcomeTime = 0;
unsigned long long hfTime = 0;

bool joyMoved = false;
bool shouldRefresh = true;
short yJoyState = 0;
short xJoyState = 0;

void buildHighscores();

void menuSetup() {
  Serial.begin(9600);
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(lcdBrightnessPin, OUTPUT);
  pinMode(lcdContrastPin, OUTPUT);

  lcd.createChar(0, rigthArrow);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
  lcd.begin(LCD_WIDTH, LCD_HEIGTH);

  loadAllFromStorage();
  analogWrite(lcdBrightnessPin, map(settings.lcdBrightness, 1, 10, MIN_LCD_BRIGHTNESS_VALUE, MAX_LCD_BRIGHTNESS_VALUE));
  analogWrite(lcdContrastPin, map(settings.lcdContrast, 1, 10, MIN_LCD_CONTRAST_VALUE, MAX_LCD_CONTRAST_VALUE));

  buildHighscores();
}

void printCentered(String message, byte row) {
  lcd.setCursor(LCD_WIDTH / LCD_HEIGTH - message.length() / LCD_HEIGTH, row);
  lcd.print(message);
}


void displayWelcome() {
  printCentered("Welcome!", 0);
  welcomeTime = millis();
  currentMenuState = 2;
}


void getjoyState() {
  short xValue = analogRead(pinX);
  short yValue = analogRead(pinY);

  xJoyState = STILL;
  yJoyState = STILL;

  if (stillMinThreshold <= xValue && xValue <= stillMaxThreshold && stillMinThreshold <= yValue && yValue <= stillMaxThreshold) {
    joyMoved = false;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    xJoyState = RIGHT;
    joyMoved = true;
  } else if (xValue < minThreshold && joyMoved == false) {
    xJoyState = LEFT;
    joyMoved = true;
  }

  if (yValue > maxThreshold && joyMoved == false) {
    yJoyState = UP;
    joyMoved = true;
  } else if (yValue < minThreshold && joyMoved == false) {
    yJoyState = DOWN;
    joyMoved = true;
  }
}


void getButtonState() {

  byte reading = digitalRead(pinSW);
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState != reading)
      buttonState = reading;
  }

  lastReading = reading;
}


void displayItems(byte& index, byte listLen, String itemList[], byte step = 1, bool clickable = false) {
  if (shouldRefresh) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(itemList[index - 1]);
    if (index != 1) {
      lcd.setCursor(15, 0);
      lcd.write(byte(1));
    }
    if (index != listLen - 1) {
      lcd.setCursor(15, 1);
      lcd.write(byte(2));
    }
    lcd.setCursor(0, 1);
    if (clickable || itemList[index] == "Back to Menu")
      lcd.write(byte(0));
    lcd.print(itemList[index]);
    shouldRefresh = false;
  }

  getjoyState();

  if (joyMoved) {
    if (yJoyState == UP && index < listLen - step) {
      index += step;
      shouldRefresh = true;
    } else if (yJoyState == UP && listLen % 2 == 1 && index == listLen - 2) {
      index++;
      shouldRefresh = true;
    } else if (yJoyState == DOWN && listLen % 2 == 1 && index == listLen - 1) {
      index--;
      shouldRefresh = true;
    } else if (yJoyState == DOWN && index > step) {
      index -= step;
      shouldRefresh = true;
    }
  }
}


void goToState(byte& currentState, byte nextState) {
  currentState = nextState;
  shouldRefresh = true;
  buttonState = HIGH;
}


void checkButtonPressing(byte& currentState, byte nextState) {
  getButtonState();
  if (buttonState == BUTTON_PRESSED) {
    goToState(currentState, nextState);
  }
}


void displaySounds() {
  short letterIndex = 0;
  String onOff;
  String sounds = "SOUNDS ";

  if (settings.soundsMuted == false)
    onOff = " <ON>";
  else
    onOff = "<OFF>";
  lcd.clear();
  printCentered(sounds + onOff, 0);
  printCentered("Press to save!", 1);
  while (buttonState != BUTTON_PRESSED) {
    getjoyState();
    if (yJoyState != STILL) {
      if (onOff == " <ON>")
        onOff = "<OFF>";
      else
        onOff = " <ON>";

      lcd.setCursor(0, 0);
      lcd.print(" ");
      printCentered(sounds + onOff, 0);
    }

    getButtonState();
  }
  if (onOff == " <ON>")
    settings.soundsMuted = false;
  else
    settings.soundsMuted = true;
  saveSettingsInStorage();
  goToState(currentMenuState, 5);
}


void resetHighscores() {
  for (byte i = 0; i < 5; i++) {
    strcpy(highscores[i].name, "NoName");
    strcpy(highscores[i].score, "      0");
  }
  saveHighscoresInStorage();
  goToState(currentMenuState, 5);
}


void buildHighscores() {
  for (byte i = 1; i <= 5; i++)
    hsItems[i] = hsItems[i] + highscores[i - 1].name + highscores[i - 1].score;
}


void displaySlider(byte barsNr, byte& settingToUpdate, byte modifierPin = 0, byte minIntervalValue = 0, byte maxIntervalValue = 0) {

  char bar = 0xff;
  String content = "-";
  byte barIndex = settingToUpdate;
  for (byte i = 1; i <= barIndex; i++)
    content += bar;
  for (byte i = barIndex + 1; i <= barsNr; i++)
    content += ' ';
  content += '+';

  const short lcdCursor = 8 - barsNr / 2;

  lcd.clear();
  printCentered(content, 0);
  printCentered("Press to save!", 1);
  while (buttonState != BUTTON_PRESSED) {
    getjoyState();
    if (xJoyState != STILL) {
      if (xJoyState == RIGHT && barIndex < barsNr) {
        barIndex++;
        content[barIndex] = 0xff;
      } else if (xJoyState == LEFT && barIndex > 1) {
        content[barIndex] = ' ';
        barIndex--;
      }

      printCentered(content, 0);
      if (modifierPin != 0) {
        byte value = map(barIndex, 1, barsNr, minIntervalValue, maxIntervalValue);
        analogWrite(modifierPin, value);
      }
      delay(5);
    }
    getButtonState();
  }
  settingToUpdate = barIndex;
  saveSettingsInStorage();
  goToState(currentMenuState, 5);
}


void displaySetName() {
  short letterIndex = 0;
  String name = settings.name;

  lcd.clear();
  printCentered('<' + name + '>', 0);
  printCentered("Press to save!", 1);
  lcd.setCursor(5 + letterIndex, 0);
  lcd.cursor();
  while (buttonState != BUTTON_PRESSED) {
    getjoyState();
    if (xJoyState != STILL) {
      letterIndex += xJoyState;
      if (letterIndex < 0)
        letterIndex = 5;
      else if (letterIndex == 6)
        letterIndex = 0;

      lcd.setCursor(5 + letterIndex, 0);
    }
    if (yJoyState != STILL) {
      name[letterIndex] -= yJoyState;
      switch (name[letterIndex]) {
        case (' ' - 1):
          name[letterIndex] = 'Z';
          break;

        case (' ' + 1):
          name[letterIndex] = 'A';
          break;

        case ('A' - 1):
          name[letterIndex] = ' ';
          break;

        case ('Z' + 1):
          name[letterIndex] = ' ';
          break;
      }
      printCentered('<' + name + '>', 0);
      lcd.setCursor(5 + letterIndex, 0);
    }
    getButtonState();
  }
  lcd.noCursor();
  strcpy(settings.name, name.c_str());
  saveSettingsInStorage();
  goToState(currentMenuState, 5);
}


void menuLoop() {

  getButtonState();

  // Serial.println(highscores[4].name);
  if (currentMenuState == 1)
    displayWelcome();

  else if (currentMenuState == 2) {
    if ((millis() - welcomeTime) >= WELCOME_TIME) {
      printCentered("Have Fun", 1);
      hfTime = millis();
      currentMenuState = 3;
    }
  }

  else if (currentMenuState == 3) {
    if ((millis() - hfTime) >= HAVEFUN_TIME) {
      lcd.clear();
      currentMenuState = 4;
    }
  } else if (currentMenuState == 4 && buttonState != BUTTON_PRESSED) {
    displayItems(selectedMenuItem, 6, menuItems, 1, true);
    checkButtonPressing(currentMenuState, 5);
  } else if (currentMenuState == 5 && buttonState != BUTTON_PRESSED)
    switch (selectedMenuItem) {

      case START_GAME:
        //startGame()
        break;

      case HIGHSCORES:
        displayItems(selectedHighscoresItem, 7, hsItems);
        if (selectedHighscoresItem == 6)
          checkButtonPressing(currentMenuState, 8);
        break;

      case SETTINGS:
        displayItems(selectedSettingsItem, 9, settingsItems, 1, true);
        checkButtonPressing(currentMenuState, 7);
        break;

      case ABOUT:
        displayItems(selectedAboutItem, 7, aboutItems, 2);
        if (selectedAboutItem == 6)
          checkButtonPressing(currentMenuState, 8);
        break;

      case HOW_TO_PLAY:
        displayItems(selectedHTPItem, 5, htpItems, 1);
        if (selectedHTPItem == 4)
          checkButtonPressing(currentMenuState, 8);
        break;
    }
  else if (currentMenuState == 7 && buttonState != BUTTON_PRESSED)
    switch (selectedSettingsItem) {
      case YOUR_NAME:
        displaySetName();
        break;

      case DIFFICULTY:
        displaySlider(3, settings.difficulty);
        break;

      case CONTRAST:
        displaySlider(10, settings.lcdContrast, lcdContrastPin, MIN_LCD_CONTRAST_VALUE, MAX_LCD_CONTRAST_VALUE);
        break;

      case BRIGHTNESS:
        displaySlider(10, settings.lcdBrightness, lcdBrightnessPin, MIN_LCD_BRIGHTNESS_VALUE, MAX_LCD_BRIGHTNESS_VALUE);
        break;

      case MATRIX_LIGHT:
        displaySlider(10, settings.matrixBrightness);
        break;

      case SOUNDS:
        displaySounds();
        break;

      case RESET_HIGHSCORES:
        resetHighscores();
        break;

      case BACK_TO_MENU:
        goToState(currentMenuState, 4);
        selectedSettingsItem = 1;
        break;
    }
  else if (currentMenuState == 8) {
    goToState(currentMenuState, 4);
    selectedAboutItem = 1;
    selectedHighscoresItem = 1;
    selectedHTPItem = 1;
  }
}

#endif