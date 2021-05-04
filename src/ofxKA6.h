#pragma once
#include "ofMain.h"
#include "ofxFft.h"

#define ofxKA6 "Komplete Audio ASIO Driver"
#define BUFFERSIZE 512
#define SAMPLERATE 48000



class ofxKA6 {
public:
	void setup(int _inputChans, int _outputChans);
	void update();
	void audioIn(ofSoundBuffer & input);
	void audioLevelView(int _levelViewPosX, int _levelViewPosY, int _levelViewX, int _levelViewY, int _levelViewChannel);
	void audioHistoryView(int _historyViewPosX, int _historyViewPosY, int _historyViewHeight, int _historyViewChannel);
	float passSoundVolume(int _chans) const;



private:
	string settedDeviceName;


	int inputChans, outputChans;
	int bufferCounter, drawCounter, volHisNum;

	ofSoundDevice deviceName;
	ofSoundStream soundStream;
	ofSoundStreamSettings soundSettings;
	ofxFft* fft;


	vector<vector<float>> inputChansNum;
	vector<vector<float>> inputChansVolHis;

	float* inputChansSmoothVol = new float[inputChans];
	float* inputChansScaledVol = new float[inputChans];

};