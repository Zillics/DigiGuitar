#define THRESH 40
#include "midiGuit.h"
//midiSlider
midiSlider::midiSlider(int pin1,int pin2){
	setPins(pin1,pin2);
}
void midiSlider::setPins(int p1,int p2){
	pin1 = p1;
	pin2 = p2;
}

double midiSlider::readRaw(){
		return readSlider2(pin1,pin2,n);
	}

int midiSlider::read(){
	double raw = readSlider2(pin1,pin2,n);
	raw = (0.5*raw)*r;
	return (int)raw;
}

void midiSlider::print(){
	Serial.println("-----------SLIDER-INFO-----------");
	Serial.print("Pins: ");
	Serial.print(pin1);
	Serial.print(", ");
	Serial.println(pin2);
	Serial.print("Number of samples n: ");
	Serial.println(n);
	Serial.print("Range: 0 -> ");
	Serial.println(r);
}
//midiInstrument
midiInstrument::midiInstrument(vector<midiSlider> s){
	for(midiSlider i : s){
		sliders.push_back(i);
	}
}

void midiInstrument::printInfo(){
	Serial.println("*********INSTRUMENT****INFO*********");
	Serial.println("All sliders: ");
	for(midiSlider i : sliders){
		i.print();
	}
}

void midiInstrument::addSlider(midiSlider s){
	sliders.push_back(s);
}

void midiInstrument::printAllRead(){
	for(midiSlider i : sliders){
		Serial.print(i.read());
		Serial.print(", ");
	}
	Serial.println();
}