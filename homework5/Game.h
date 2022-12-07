#ifndef Game
#define Game

#include "Globals.h"
#include "StorageManager.h"

const byte moveInterval = 100;
unsigned long long lastMoved = 0;

bool matrixChanged = true;

byte matrix[][MATRIX_SIZE] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

byte foodPosX;
byte foodPosY;

short foodEated;

void updateMatrix();

void gameSetup() {
  matrix[xPos][yPos] = 1;
  foodPosX = rand() % 8;
  foodPosY = rand() % 8;
  matrix[foodPosX][foodPosY] = 1;
  foodEated = 0;
  updateMatrix();
}

void updateMatrix() {
  // TODO: update matrix using byte matrix (refresh function)
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      lc.setLed(0, i, j, matrix[i][j]);
    }
  }
}


void generateFood() {
  byte lastFoodPosX = foodPosX;
  byte lastFoodPosY = foodPosY;
  foodPosX = rand() % 8;
  foodPosY = rand() % 8;
  // matrix[lastFoodPosX][lastFoodPosY] = 0;
  matrix[foodPosX][foodPosY] = 1;
  updateMatrix();
}


void updatePositions() {
  // TODO: joystick control
  int xValue = analogRead(pinY);
  int yValue = analogRead(pinX);

  xLastPos = xPos;
  yLastPos = yPos;
  if (xValue > minThreshold) {
    if (xPos < MATRIX_SIZE - 1) {
      xPos++;
    } 
    else {
      xPos = 0;
    }
  }

  if (xValue < maxThreshold) {
    if (xPos > 0) {
      xPos--;
    }
    else {
      xPos = MATRIX_SIZE - 1;
    }
  }


  if (yValue > maxThreshold) {
    if (yPos < MATRIX_SIZE - 1) {
      yPos++;
    } 
    else {
      yPos = 0;
    }
  }

  if (yValue < minThreshold) {
    if (yPos > 0) {
      yPos--;
    }
    else {
      yPos = MATRIX_SIZE - 1;
    }
  }

  if (xLastPos != xPos || yLastPos != yPos) {
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = 1;
    matrixChanged = true;
    lastMoved = millis();
  }
}

void updateHighscore() {
  for (byte i = 0 ; i < 5; i++)
    if (foodEated > highscores[i].score) {
      for (byte j = 4; j > i; j--) {
        highscores[j].score = highscores[j - 1].score;
        strcpy(highscores[j].name, highscores[j - 1].name);
      }
    highscores[i].score = foodEated;
    strcpy(highscores[i].name, settings.name);
    break;
  }
}

void gameEnded() {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      matrix[i][j] = 0;
    }
  }
  updateHighscore();
  foodPosX = 0;
  foodPosY = 0;
  foodEated = 0;
}

void gameLoop(bool &updateLCD) {
  // TODO: update positions in an interval

  if (millis() - lastMoved > moveInterval) {
    updatePositions();  // calculare stare
  }

  if (matrixChanged == true) {
    updateMatrix();
    matrixChanged = false;
  }

  if (xPos == foodPosX && yPos == foodPosY) {
    generateFood();
    foodEated++;
    updateLCD = true;
  }
  
}
// theoretical example

short getPoints() {
  return foodEated;
}

#endif Game