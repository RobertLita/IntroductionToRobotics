#ifndef LCDSymbols
#define LCDSymbols

const byte rigthArrow[] = {
  B00000,
  B10000,
  B01000,
  B00100,
  B00010,
  B00100,
  B01000,
  B10000
};

const byte upArrow[] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00000,
  B00000
};

const byte downArrow[] = {
  B00000,
  B00000,
  B00000,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100
};

const byte settingsIcon[] = { 
  B11000011,
  B11100111,
  B01111110,
  B00100100,
  B00100100,
  B01111110,
  B11100111,
  B11000011 
};

const byte highscoreIcon[] = {
  B00000000,
  B00011000,
  B00011000,
  B00011011,
  B00011011,
  B11011011,
  B11011011,
  B11011011
};

const byte howToPlayIcon[] = {
  B00011100,
  B00100010,
  B00100010,
  B00000100,
  B00001000,
  B00001000,
  B00000000,
  B00001000
};

const byte aboutIcon[] = {
  B00110000,
  B01001000,
  B10000100,
  B10000100,
  B01001000,
  B00110100,
  B00000010,
  B00000001
};

#endif