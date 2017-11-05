#include "testTools.h"

//waitForButton : waits for that something is sent on the Serial channel and then terminates
void waitForButton() {
  Serial.read(); //Empty Serial buffer
  while (Serial.available() == 0) {}
}

void printAll(){
	Serial.println("------------ALL VALUES STORED IN EEPROM------------");
	Serial.println("******Pins******");
	for(int i = 0; i < 8; i++){
		Serial.print("Pin");
		Serial.print(i+1);
		Serial.print(": ");
		Serial.println(EEPROM.read(addr::pins[i]));
	}
	Serial.println("******Min values******");
		for(int i = 0; i < 8; i++){
		Serial.print("Pin");
		Serial.print(i+1);
		Serial.print(": ");
		Serial.println((EEPROM.read(addr::mins[i]) << 8) | (EEPROM.read(addr::mins[i]+1)));
	}
	Serial.println("******Max values******");
		for(int i = 0; i < 8; i++){
		Serial.print("Pin");
		Serial.print(i+1);
		Serial.print(": ");
		Serial.println((EEPROM.read(addr::maxs[i]) << 8) | (EEPROM.read(addr::maxs[i]+1)));
	}
	Serial.println("******Baseline capacitance values******");
		for(int i = 0; i < 8; i++){
		Serial.print("Pin");
		Serial.print(i+1);
		Serial.print(": ");
		Serial.println((EEPROM.read(addr::baseLineC[i]) << 8) | (EEPROM.read(addr::baseLineC[i]+1)));
	}

}

unsigned long timeF(double (*f) (int,int,int),int a, int b, int c, int samples){
	unsigned long t;
	unsigned long sum = 0;
	for(int i = 0;i < samples;i++){
		t = micros();
		f(a,b,c);
		t = micros() - t;
		sum += t;
	}
	return sum/samples;
}

unsigned long timeF(byte (*f) (byte), byte a, int samples){
	unsigned long t;
	unsigned long sum = 0;
	for(int i = 0;i < samples;i++){
		t = micros();
		f(a);
		t = micros() - t;
		sum += t;
	}
	return sum/samples;
}