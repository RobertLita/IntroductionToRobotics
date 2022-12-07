#ifndef StorageManager
#define StorageManager
#include <EEPROM.h>

struct PlayerScore {
  char name[7];
  short score;
};


struct Settings {
  char name[7];
  byte difficulty;
  byte lcdContrast;
  byte lcdBrightness;
  byte matrixBrightness;
  bool soundsMuted;
} settings;

PlayerScore highscores[5];


void loadAllFromStorage() {
  short eePos = 0;
  EEPROM.get(eePos, settings);
  eePos += sizeof(settings);
  EEPROM.get(eePos, highscores);
}

void saveSettingsInStorage() {
  EEPROM.put(0, settings);
}

void saveHighscoresInStorage() {
  short eePos = sizeof(settings); 
  EEPROM.put(eePos, highscores);
}

#endif StorageManager