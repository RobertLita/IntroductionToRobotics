#ifndef Matrix
#define Matrix

#include "LedControl.h"
#include "LCDSymbols.h"
#include "Globals.h"

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

void matrixSetup() {
  lc.shutdown(0, false);
  lc.clearDisplay(0);
}

void setMatrixBrightness(byte value) {
  lc.setIntensity(0, value);
}

void displayIconOnMatrix(const byte icon[]) {
  lc.clearDisplay(0);
  for (byte row = 0; row <= MATRIX_SIZE; row++)
    lc.setRow(0, row, icon[row]);
}


void turnOffMatrix() {
  lc.clearDisplay(0);
}


#endif Matrix