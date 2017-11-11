#ifndef ADD_midiGH
#define ADD_midiGH
#include "capSlider.h"
#include <MIDI.h>
#include <vector>
using std::vector;
class midiSlider
{
	private:
	//pin1 and pin2: ports used on Teensy, values: 0 - 10	
	int pin1;
	int pin2;
	//n: number of samples taken when using readSlider2
	int n = 10; 
	//r: range of values for this slider is 0 -> r
	int r = 255;
	//thresh: threshold value above baseLineC for triggering something
	int thresh = 20;
	public:
	//Constructor
	midiSlider(int p1,int p2);
	//Trivial functions
	void setPins(int p1,int p2);
	void setn(int num){n = num;}
	void setR(int maxi){r = maxi;}
	void setThr(int thr){thresh = thr;}
	//Function declarations
	double readRaw();
	int read();
	void print();
	//Midi functions
	void playNote();
	

};

class midiInstrument
{
	private:
	vector<midiSlider> sliders;
	public:
	//Constructor
	midiInstrument(vector<midiSlider> s);
	//Trivial functions
	midiSlider getSlider(int i){return sliders[i];}
	//Function declarations
	void printInfo();
	void addSlider(midiSlider s);
	void printAllRead();
	//StartString: starts playing string through MIDI with sliders number act and sens
	//act: Actuator (activates noteOn), sens: Sensor (changes pitch)
	void StartString(int act, int sens);
};









#endif