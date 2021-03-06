//Useful tools for testing and prototyping digital guitar
#ifndef ADD_testToolsH
#define ADD_testToolsH

#include "Arduino.h"
#include <EEPROM.h>
#include "capSlider.h"

void waitForButton();

void printVars();

void printEEPROM();

unsigned long timeF(double (*f) (int,int,int),int a, int b,int c, int samples);

unsigned long timeF(byte (*f) (byte), byte a, int samples);

#endif