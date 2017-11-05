#include "testTools.h"


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