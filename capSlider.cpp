#include "capSlider.h"

//EEPROM ADDRESSES
//For 16 bit addresses: first byte: MSB, second byte: LSB

int addr::pins[11] = {1,2,3,4,5,6,7,8,9,10,11}; //touchRead pins (8 bit)
int addr::mins[11] = {12,14,16,18,20,22,24,26,28,30,32}; //min cap values for touch detection of each pin (16 bit)
int addr::maxs[11] = {34,36,38,40,42,44,46,48,50,52,54}; //max cap values  of each pin (16 bit)
int addr::baseLineC[11] = {56,58,60,62,64,68,70,72,74,78,80}; //baseline capacitance for each pin (16 bit)	

#define THRESH 100
//MACROS FOR READING EEPROM
#define COMB(msb,lsb) (msb << 8) | lsb
#define MINS(i) COMB(EEPROM.read(addr::mins[i]),EEPROM.read(addr::mins[i]+1)) 
#define MAXS(i) COMB(EEPROM.read(addr::maxs[i]),EEPROM.read(addr::maxs[i]+1))
#define BASEC(i) COMB(EEPROM.read(addr::baseLineC[i]),EEPROM.read(addr::baseLineC[i]+1))  
#define PINS(i) EEPROM.read(addr::pins[i])

//GLOBAL VARIABLES
//stored in volatile memory. Use these primarily (faster,unlimited writes).
//Initialized by loading from eeprom during power on
int pin[11] = {PINS(0),PINS(1),PINS(2),PINS(3),PINS(4),PINS(5),PINS(6),PINS(7),PINS(8),PINS(9),PINS(10)};
int mini[11] = {MINS(0),MINS(1),MINS(2),MINS(3),MINS(4),MINS(5),MINS(6),MINS(7),MINS(8),MINS(9),MINS(10)};
int maxi[11] = {MAXS(0), MAXS(1),MAXS(2),MAXS(3),MAXS(4),MAXS(5),MAXS(6),MAXS(7),MAXS(8),MAXS(9),MAXS(10)};
int baseC[11] = {BASEC(0),BASEC(1),BASEC(2),BASEC(3),BASEC(4),BASEC(5),BASEC(6),BASEC(7),BASEC(8),BASEC(9),BASEC(10)};

//getMax : Gets max value from touchPad over time t (ms)
int getMax(int pad, int t){
  int max = 0;
  int temp;
  int t1 = millis();
  int p = pin[pad];
  while(millis() - t1 < t){
    temp = touchRead(p);
    if(temp > max){
      max = temp;
    }  
  }
  return max;
}

//calBaseLineC : calculate baseline capacitance for all pads for time t (ms) 
//Store in eeprom and global variables
void calBaseLineC(int t){
	int v;
	byte msb;
	byte lsb;
	for(int i = 0; i < 11; i++){
		v = getMax(i,t);
		msb = v >> 8;
		lsb = v;
		EEPROM.write(addr::baseLineC[i],msb);
		EEPROM.write(addr::baseLineC[i] + 1,lsb);
		baseC[i] = v;
	}
}

//readSlider2 : taking the mean of n samples(touchRead()) and calculating position on slider based on that
//Return: value in range 0.0 - 2.0 , when untouched return -1
//Note on optimal value for n: 20 seems to be the sweet spot
double readSlider2(int pad1, int pad2, int n){
  double sum1 = 0;
  double sum2 = 0;
  for(int i = 0;i < n;i++){
    sum1 += touchRead(pin[pad1]);
	sum2 += touchRead(pin[pad2]);
  }
  double mean1 = sum1/n;
  double mean2 = sum2/n;
  if((int)mean1 > maxi[pad1]){mean1 = maxi[pad1];}
  if((int)mean1 < mini[pad1]){mean1 = mini[pad1];}
  if((int)mean2 > maxi[pad2]){mean2 = maxi[pad2];}
  if((int)mean2 < mini[pad2]){mean2 = mini[pad2];}

  double a = 1023*((mean1 - mini[pad1])/(maxi[pad1] - mini[pad2]));
  double b = 1023*((mean2 - mini[pad2])/(maxi[pad2] - mini[pad2]));
  if(a + b == 0){
	  return -1;
  } else{
	 return (a-b)/(a+b) + 1.0; 
  }
}

//readSlider3 : performing n readSlider1() and taking mean of that
//double readSlider3()



//calSlider : Finds the average values for both pads, on both edges (max and min) of the slider
//Updates new values for both eeprom and global variables
void calSlider(int pad1, int pad2){
  int pin1 = pin[pad1];
  int pin2 = pin[pad2];
  int baseC1 = baseC[pad1] + THRESH;
  int baseC2 = baseC[pad2] + THRESH;
  int mean1;
  int mean2;
  unsigned long sum1 = 0;
  unsigned long sum2 = 0;
  int count = 0;
  bool sampled1 = false;
  bool sampled2 = false;
  Serial.print("SLIDER ");
  Serial.print(pad1/2 + 1);
  Serial.println(": CALIBRATING MIN.....");
  while(!sampled1){
	mean1 = touchRead(pin1);
	mean2 = touchRead(pin2);
    while((mean1 > baseC1) || (mean2 > baseC2)){
	  sum1 += mean1;
      sum2 += mean2;
      count += 1;
      sampled1 = true;
	  mean1 = touchRead(pin1);
      mean2 = touchRead(pin2);
	  delay(200);
    }

  }
  Serial.print("FOUND IT! (min), SAMPLES: ");
  Serial.println(count);
  mean1 = sum1/count;
  mean2 = sum2/count;
  if(mean1 > mean2){
	EEPROM.write(addr::maxs[pad1],mean1 >> 8);
	EEPROM.write(addr::maxs[pad1]+1,mean1);
	EEPROM.write(addr::mins[pad2], mean2 >> 8);
	EEPROM.write(addr::mins[pad2]+1, mean2 >> 8);
	maxi[pad1] = mean1;
	mini[pad2] = mean2;
  } else{
	EEPROM.write(addr::mins[pad1],mean1 >> 8);
	EEPROM.write(addr::mins[pad1]+1,mean1);
	EEPROM.write(addr::maxs[pad2], mean2 >> 8);
	EEPROM.write(addr::maxs[pad2]+1, mean2 >> 8);
	maxi[pad2] = mean2;
	mini[pad1] = mean1;
  }
  sum1 = 0;
  sum2 = 0;
  count = 0;
  delay(2000);
  Serial.print("SLIDER ");
  Serial.print(pad1/2 + 1);
  Serial.println(": CALIBRATING MAX.....");
  while(!sampled2){
	  mean1 = touchRead(pin1);
	  mean2 = touchRead(pin2);
      while((mean1 > baseC1) || (mean2 > baseC2)){
      sum1 += mean1;
      sum2 += mean2;
      count += 1;
      sampled2 = true;
	  mean1 = touchRead(pin1);
	  mean2 = touchRead(pin2);
	  delay(200);
    }
  }
  mean1 = sum1/count;
  mean2 = sum2/count;
  if(mean1 > mean2){
	EEPROM.write(addr::maxs[pad1],mean1 >> 8);
	EEPROM.write(addr::maxs[pad1]+1,mean1);
	EEPROM.write(addr::mins[pad2], mean2 >> 8);
	EEPROM.write(addr::mins[pad2]+1, mean2 >> 8);  
	maxi[pad1] = mean1;
	mini[pad2] = mean2;
  } else{
	EEPROM.write(addr::mins[pad1],mean1 >> 8);
	EEPROM.write(addr::mins[pad1]+1,mean1);
	EEPROM.write(addr::maxs[pad2], mean2 >> 8);
	EEPROM.write(addr::maxs[pad2]+1, mean2 >> 8);
	maxi[pad2] = mean2;
	mini[pad1] = mean1;
  }
  Serial.println("CALIBRATED! ");
}

//updatePins : writes pin numbers to eeprom memory and global variables
//LATEST UPDATE (5.11): {30, 29, 23, 22, 17, 18, 19, 22}
void updatePins(byte b[11]){
	for(int i = 0;i < 11;i++){
		EEPROM.write(addr::pins[i],b[i]);
		pin[i] = b[i];
	}
}
//stores relevant info in 
void init(){
	calSlider(0,1);
}
