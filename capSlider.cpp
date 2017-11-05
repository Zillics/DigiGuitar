#include "capSlider.h"

//EEPROM ADDRESSES
//For 16 bit addresses: first byte: MSB, second byte: LSB

int addr::pins[8] = {1,2,3,4,5,6,7,8}; //touchRead pins (8 bit)
int addr::mins[8] = {12,14,16,18,20,22,24,26}; //min cap values for touch detection of each pin (16 bit)
int addr::maxs[8] = {32,34,36,38,40,42,44,46}; //max cap values  of each pin (16 bit)
int addr::baseLineC[8] = {42,44,46,48,50,52,54,56}; //baseline capacitance for each pin (16 bit)

//GLOBAL VARIABLES
//stored in volatile memory. Use these primarily (faster)
//ToDo: Define all needed variables and implement the updating of these variables during power on from eeprom

	
#define THRESH 50
#define COMB(msb,lsb) (msb << 8) | lsb
#define MINS(i) COMB(EEPROM.read(addr::mins[i]),EEPROM.read(addr::mins[i]+1)) 
#define MAXS(i) COMB(EEPROM.read(addr::maxs[i]),EEPROM.read(addr::maxs[i]+1))
#define BASEC(i) COMB(EEPROM.read(addr::baseLineC[i]),EEPROM.read(addr::baseLineC[i]+1))  
#define PIN(i) EEPROM.read(addr::pins[i])

//TEMPORARY VARIABLES
int pins[2] = {30, 29};
static int mins[2] = {2684, 2466};
static int maxs[2] = {8660, 7611};
static int baseLineC[2] = {1356, 1365};

//getMax : Gets max value from touchPad over time t (ms)
int getMax(int pad, int t){
  int max = 0;
  int temp;
  int t1 = millis();
  int pin = EEPROM.read(addr::pins[pad]);
  while(millis() - t1 < t){
    temp = touchRead(pin);
    if(temp > max){
      max = temp;
    }  
  }
  return max;
}

//calBaseLineC : calculate baseline capacitance for all pads for time t (ms) and store in eeprom
void calBaseLineC(int t){
	int v;
	byte msb;
	byte lsb;
	for(int i = 0; i < 8; i++){
		v = getMax(i,t);
		msb = v >> 8;
		lsb = v;
		EEPROM.write(addr::baseLineC[i],msb);
		EEPROM.write(addr::baseLineC[i] + 1,lsb);
	}
}

//readSlider2 : taking the mean of n samples(touchRead()) and calculating position on slider based on that
//Return: value in range 0.0 - 2.0 , when untouched return -1
//Note on optimal value for n: 20 seems to be the sweet spot
double readSlider2(int pad1, int pad2, int n){
  int max1 = MAXS(pad1);
  int max2 = MAXS(pad2);
  int min1 = MINS(pad1);
  int min2 = MINS(pad2);
  int pin1 = PIN(pad1);
  int pin2 = PIN(pad2);
  double sum1 = 0;
  double sum2 = 0;
  for(int i = 0;i < n;i++){
    sum1 += touchRead(pin1);
	sum2 += touchRead(pin2);
  }
  double mean1 = sum1/n;
  double mean2 = sum2/n;
  if((int)mean1 > max1){mean1 = max1;}
  if((int)mean1 < min1){mean1 = min1;}
  if((int)mean2 > max2){mean2 = max2;}
  if((int)mean2 < min2){mean2 = min2;}

  double a = 1023*((mean1 - min1)/(max1 - min1));
  double b = 1023*((mean2 - min2)/(max2 - min2));
  if(a + b == 0){
	  return -1;
  } else{
	 return (a-b)/(a+b) + 1.0; 
  }
}

//readSlider3 : performing n readSlider1() and taking mean of that
//double readSlider3()



//calSlider : Finds the average values for both pads, on both edges (max and min) of the slider
void calSlider(int pad1, int pad2){
  int pin1 = PIN(pad1);
  int pin2 = PIN(pad2);
  int baseC1 = BASEC(pad1) + THRESH;
  int baseC2 = BASEC(pad2) + THRESH;
  int mean1;
  int mean2;
  unsigned long sum1 = 0;
  unsigned long sum2 = 0;
  int count = 0;
  bool sampled1 = false;
  bool sampled2 = false;
  Serial.println("CALIBRATING MIN.....");
  while(!sampled1){
    while((touchRead(pin1) > baseC1) || (touchRead(pin2) > baseC2)){
      sum1 += touchRead(pin1);
      sum2 += touchRead(pin2);
      count += 1;
      sampled1 = true;
    }
  }
  mean1 = sum1/count;
  mean2 = sum2/count;
  if(mean1 > mean2){
	EEPROM.write(addr::maxs[pad1],mean1 >> 8);
	EEPROM.write(addr::maxs[pad1]+1,mean1);
	EEPROM.write(addr::mins[pad2], mean2 >> 8);
	EEPROM.write(addr::mins[pad2]+1, mean2 >> 8);  
  } else{
	EEPROM.write(addr::mins[pad1],mean1 >> 8);
	EEPROM.write(addr::mins[pad1]+1,mean1);
	EEPROM.write(addr::maxs[pad2], mean2 >> 8);
	EEPROM.write(addr::maxs[pad2]+1, mean2 >> 8);  
  }
  sum1 = 0;
  sum2 = 0;
  count = 0;
  Serial.println("CALIBRATING MAX.....");
  while(!sampled2){
      while((touchRead(pin1) > baseC1) || (touchRead(pin2) > baseC2)){
      sum1 += touchRead(pin1);
      sum2 += touchRead(pin2);
      count += 1;
      sampled2 = true;
    }
  }
  mean1 = sum1/count;
  mean2 = sum2/count;
  if(mean1 > mean2){
	EEPROM.write(addr::maxs[pad1],mean1 >> 8);
	EEPROM.write(addr::maxs[pad1]+1,mean1);
	EEPROM.write(addr::mins[pad2], mean2 >> 8);
	EEPROM.write(addr::mins[pad2]+1, mean2 >> 8);  
  } else{
	EEPROM.write(addr::mins[pad1],mean1 >> 8);
	EEPROM.write(addr::mins[pad1]+1,mean1);
	EEPROM.write(addr::maxs[pad2], mean2 >> 8);
	EEPROM.write(addr::maxs[pad2]+1, mean2 >> 8);  
  }
}

//updatePins : writes pin numbers to eeprom memory
//LATEST UPDATE (4.11): {30, 29, 15, 16, 17, 18, 19, 22}
void updatePins(byte b[8]){
	for(int i = 0;i < 8;i++){
		EEPROM.write(addr::pins[i],b[i]);
	}
}
//stores relevant info in 
void init(){
	calSlider(0,1);
}
