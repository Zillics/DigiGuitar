#include "capSlider.h"

//EEPROM ADDRESSES
//For 16 bit addresses: first byte: MSB, second byte: LSB

int addr::pins[8] = {1,2,3,4,5,6,7,8}; //touchRead pins (8 bit)
int addr::mins[8] = {12,14,16,18,20,22,24,26}; //min cap values for touch detection of each pin (16 bit)
int addr::maxs[8] = {32,34,36,38,40,42,44,46}; //max cap values  of each pin (16 bit)
int addr::baseLineC[8] = {42,44,46,48,50,52,54,56}; //baseline capacitance for each pin (16 bit)

	
#define THRESH 50

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
  while(millis() - t1 < t){
    temp = touchRead(EEPROM.read(addr::pins[pad]));
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


//calPads : 
void calPad(int pad){
  int temp;
  Serial.println("CALIBRATING MIN VALUE.....");
  Serial.println("WAITING FOR BUTTON PRESS.....");
  while(touchRead(pins[pad]) < baseLineC[pad] + THRESH){}
  while(touchRead(pins[pad]) >= baseLineC[pad] + THRESH){
    temp = getMax(pad,10);
    if(temp > mins[pad]){
      mins[pad] = temp; 
    }
  }
  Serial.println(mins[pad]);
  Serial.println("CALIBRATING MAX VALUE.....");
  //Serial.println(baseLineC[pad]);
  Serial.println("WAITING FOR BUTTON PRESS.....");
  while(touchRead(pins[pad]) < baseLineC[pad] + THRESH){}
  while(touchRead(pins[pad]) >= baseLineC[pad] + THRESH){
    temp = getMax(pad,10);
    if(temp > maxs[pad]){
      maxs[pad] = temp;  
    }
  }
  Serial.println(maxs[pad]);
}

//readPadMean : perform touchRead() n times and calculate mean. After that exactly like readPad()
int readPadMean(int pad, int n){
  double sum = 0;
  for(int i = 0;i < n;i++){
    sum += touchRead(pins[pad]);
  }
  double mean = sum/n;
  if(mean > maxs[pad]){mean = maxs[pad];}
  if(mean < mins[pad]){mean = mins[pad];}
  return map(mean,mins[pad],maxs[pad],0,1023);
}

//readPad : perform touchRead() and map the result according to that pads min and max values to 10 bit value
int readPad(int pad){
  int ret = touchRead(pins[pad]);
  if(ret > maxs[pad]){ret = maxs[pad];}
  if(ret < mins[pad]){ret = mins[pad];}
  return map(ret,mins[pad],maxs[pad],0,1023);
}

//readSlider1 : performing one calculation for slider value based on two triangular pads
double readSlider1(int pad1, int pad2){
  double a = (double)readPad(pad1);
  double b = (double)readPad(pad2);
  return (a-b)/(a+b) + 1;
}

//readSlider2 : taking the mean of n samples(touchRead()) and calculating position on slider based on that
//Note on optimal value for n: 20 seems to be the sweet spot
double readSlider2(int pad1, int pad2, int n){
  long sum = 0;  
  double a = (double)readPadMean(pad1,n);
  double b = (double)readPadMean(pad2,n);
  return (a-b)/(a+b) + 1;
}

//readSlider3 : performing n readSlider1() and taking mean of that
//double readSlider3()



//calSlider : Finds the average values for both pads, on both edges (max and min) of the slider
void calSlider(int pad1, int pad2){
  int pin1 = EEPROM.read(addr::pins[pad1]);
  int pin2 = EEPROM.read(addr::pins[pad2]);
  int baseC1 = ((EEPROM.read(addr::baseLineC[pad1]) << 8) | (EEPROM.read(addr::baseLineC[pad1]+1))) + THRESH;
  int baseC2 = ((EEPROM.read(addr::baseLineC[pad2]) << 8) | (EEPROM.read(addr::baseLineC[pad2]+1))) + THRESH;
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
  EEPROM.write(addr::maxs[pad1],mean1 >> 8);
  EEPROM.write(addr::maxs[pad1]+1,mean1);
  EEPROM.write(addr::mins[pad2], mean2 >> 8);
  EEPROM.write(addr::mins[pad2]+1, mean2 >> 8);
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
  EEPROM.write(addr::maxs[pad2], mean1 >> 8);
  EEPROM.write(addr::maxs[pad2]+1, mean1);
  EEPROM.write(addr::mins[pad1], mean2 >> 8);
  EEPROM.write(addr::mins[pad1]+1, mean2);
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
