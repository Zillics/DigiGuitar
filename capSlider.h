#ifndef ADD_H
#define ADD_H

#include "Arduino.h"
#include <EEPROM.h>

//EEPROM ADDRESSES
namespace addr {
	extern int pins[8]; //touchRead pins (8 bit)
	extern int mins[8]; //min cap values for touch detection of each pin (16 bit)
	extern int maxs[8]; //max cap values  of each pin (16 bit)
	extern int baseLineC[8]; //baseline capacitance for each pin (16 bit)
}
//GLOBAL VARIABLES - stored in volatile memory. Use these primarily (faster)
//ToDo: Define all needed variables and implement the updating of these variables during power on from eeprom
extern int pin[8];
extern int mini[8];
extern int maxi[8];
extern int baseC[8];
//getMax : Gets max value from touchPad over time t
int getMax(int pad, int t);

//calBaseLineC : calibrate baseline capacitance for n pads for time t
void calBaseLineC(int t);

//calPads : 
void calPad(int pad);

//readPadMean : perform touchRead() n times and calculate mean. After that exactly like readPad()
int readPadMean(int pad, int n);

//readPad : perform touchRead() and map the result according to that pads min and max values to 10 bit value
int readPad(int pad);

//readSlider2 : taking the mean of n samples(touchRead()) and calculating position on slider based on that
//Note on optimal value for n: 20 seems to be the sweet spot
double readSlider2(int pad1, int pad2, int n);

//readSlider3 : performing n readSlider1() and taking mean of that
//double readSlider3()

//readSlider1 : performing one calculation for slider value based on two triangular pads
double readSlider1(int pad1, int pad2);

//calSlider : Finds the average values for both pads, on both edges (max and min) of the slider
void calSlider(int pad1, int pad2);

void updatePins(byte b[8]);

void init();

#endif