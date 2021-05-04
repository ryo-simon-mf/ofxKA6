#include "ofxKA6.h"

void ofxKA6::setup(int _inputChans, int _outputChans) {
	// Audio Device Initialize start
	ofSetVerticalSync(true);

	inputChans = _inputChans;
	outputChans = _outputChans;

	//check device list
	//soundStream.printDeviceList();

	soundSettings.setApi(ofSoundDevice::Api::MS_ASIO);

	//Select Specific Asio Driver
	std::vector<ofSoundDevice> devices_list = soundStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
	//for (int dl = 0; dl < devices_list.size(); dl++) {
	//	cout << dl << endl;
	//	cout << devices_list[dl] << endl;
	//}


    //cout << typeid(devices_list[4]).name() << endl;
	//cout << devices_list[4] << endl;
	//cout << "InputChans: " << devices_list[4].inputChannels << endl;
	//cout << "OutputChans: " << devices_list[4].outputChannels << endl;
	//cout << "ID: " << devices_list[4].deviceID << ' ' << devices_list[4].name  <<endl;


	//auto devices = soundStream.getMatchingDevices("Komplete Audio ASIO Driver");
	//if (!devices.empty()) {
	//	soundSettings.setInDevice(devices[0]);
	//	std::cout << deviceName << " is not exist\n" << std::endl;
	//}
	//else {
	//	std::cout << deviceName << " is exist\n" << std::endl;
	//}


	//soundSettings.setInDevice("[MS ASIO: 4] Komplete Audio ASIO Driver [in:6 out:6]");

	//In my alienware, KA6 is Komplete Audio 6 is in devices_list array of 4
	//

	soundSettings.setInDevice(devices_list[4]);
	soundSettings.setOutDevice(devices_list[4]);
	soundSettings.setInListener(this);
	soundSettings.sampleRate = SAMPLERATE;
	soundSettings.numInputChannels = inputChans;
	soundSettings.numOutputChannels = outputChans;
	soundSettings.bufferSize = BUFFERSIZE;

	soundStream.setup(soundSettings);

	//settedDeviceName = soundStreaam.get


	// Audio Device Initialize end

	int bufferSize = BUFFERSIZE;

	bufferCounter = 0;
	drawCounter = 0;

	// Audio Volume Setup start
	//volHisNum = 400;
	if (volHisNum == 0) {
		volHisNum = 120;
	}

	inputChansNum.resize(inputChans);
	inputChansVolHis.resize(inputChans);

	for (int icInit = 0; icInit < inputChans; icInit++) {
		inputChansNum[icInit].assign(bufferSize, 0.0);
		inputChansVolHis[icInit].assign(volHisNum, 0.0);
		inputChansSmoothVol[icInit] = 0.0;
		inputChansScaledVol[icInit] = 0.0;
	}

	//cout << inputChansSmoothVol.size() << endl;

}

void ofxKA6::update() {


	//Mapping sound value to type float(0.0~1.0) from audioIn
	//VARIABLE = ofMap(VARIABLE_FROM_AUDIOIN, 0.0, 0.17, 0.0, 1.0, true);



	for (int update_i = 0; update_i < inputChans; update_i++) {
		inputChansScaledVol[update_i] = ofMap(inputChansSmoothVol[update_i], 0.0, 0.17, 0.0, 1.0, true);
		inputChansVolHis[update_i].push_back(inputChansScaledVol[update_i]);

		if (inputChansVolHis[update_i].size() >= volHisNum) {
			inputChansVolHis[update_i].erase(inputChansVolHis[update_i].begin(), inputChansVolHis[update_i].begin() + 1);
		}

	}
	//cout << "1ch: " << inputChansScaledVol[0] << "      2ch: " << inputChansScaledVol[1] << endl;

}


void ofxKA6::audioIn(ofSoundBuffer & input) {

	//float* curVol = new float[inputChans];

	float CurVol = 0.0;
	float* inputChansCurVol = new float[inputChans];

	for (int cvInit = 0; cvInit < inputChans; cvInit++) {
		inputChansCurVol[cvInit] = 0.0;
	}

	int numCounted = 0;

	for (size_t i = 0; i < input.getNumFrames(); i++) {

		for (int audioInTmp = 0; audioInTmp < inputChans; audioInTmp++) {
			inputChansNum[audioInTmp][i] = input[i * inputChans + audioInTmp] * 0.5;
			inputChansCurVol[audioInTmp] += std::pow(inputChansNum[audioInTmp][i], 2.0) * 4.0;
			//CurVol += std::pow(inputChansNum[audioInTmp][i], 2.0);
		}

		numCounted += inputChans;
	}

	//CurVol /= (float)numCounted;
	//CurVol = sqrt(CurVol);

	for (int rms_i = 0; rms_i < inputChans; rms_i++) {
		inputChansCurVol[rms_i] /= (float)numCounted;
		inputChansCurVol[rms_i] = sqrt(inputChansCurVol[rms_i]);
		inputChansSmoothVol[rms_i] *= 0.93;
		inputChansSmoothVol[rms_i] += 0.07 * inputChansCurVol[rms_i];
	}

	bufferCounter++;

}


void ofxKA6::audioLevelView(int _levelViewPosX, int _levelViewPosY, int _levelViewX, int _levelViewY, int _levelViewChannel) {
	float tmpScaledVol = 0.0;
	int tmpChannel = _levelViewChannel - 1;


	tmpScaledVol = inputChansScaledVol[tmpChannel];

	int rectVol = tmpScaledVol * 100 / 10;

	ofPushMatrix();
	for (int test_i = 0; test_i < rectVol; test_i++) {
		ofFill();
		if (test_i < 6) {
			ofSetColor(ofColor::green);
		}
		else if (test_i >= 6 && test_i < 9) {
			ofSetColor(ofColor::yellow);
		}
		else {
			ofSetColor(ofColor::red);
		}
		ofDrawRectangle(_levelViewPosX, _levelViewPosY + _levelViewY - (test_i + 1) * _levelViewY / 10, _levelViewX, _levelViewY / 10);
		//ofDrawRectangle(ofGetWidth() / 2, ofGetHeight() / 2 - test_i * 30, 20, 20);
	}
	ofPopMatrix();

	ofSetColor(ofColor::black);
	ofPushMatrix();
	for (int ol_i = 0; ol_i < 10; ol_i++) {
		ofNoFill();
		ofDrawRectangle(_levelViewPosX, _levelViewPosY + ol_i * _levelViewY / 10, _levelViewX, _levelViewY / 10);
	}
	ofPopMatrix();

}

void ofxKA6::audioHistoryView(int _historyViewPosX, int _historyViewPosY, int _historyViewHeight, int _historyViewChannel) {
	int tmpPosNum = _historyViewHeight;
	int tmpChannel = _historyViewChannel - 1;

	ofPushStyle();
	ofPushMatrix();
	ofTranslate(_historyViewPosX, _historyViewPosY);
	ofSetColor(225);
	//ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(leftVolHis * 100.0, 0), 4, 18);
	ofFill();

	ofBeginShape();
	ofSetColor(125, 125, 125);

	for (unsigned int i = 0; i < inputChansVolHis[tmpChannel].size(); i++) {
		if (i == 0) ofVertex(i, tmpPosNum);
		//ofVertex(i, 400 - volHistory[i] * 70);
		ofVertex(i, tmpPosNum - inputChansVolHis[tmpChannel][i] * 100);

		if (i == inputChansVolHis[tmpChannel].size() - 1) ofVertex(i, tmpPosNum);
	}
	ofEndShape(false);
	ofPopMatrix();
	ofPopStyle();

	drawCounter++;

}

float ofxKA6::passSoundVolume(int _chans) const {
	return inputChansScaledVol[_chans - 1];
}