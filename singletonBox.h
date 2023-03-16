#pragma once
#include "singletonBase.h"

#include <string>
using namespace std;

class singletonBox : public singletonBase<singletonBox>
{

private:
	float matirix[16] =
	{ 1.f, 0.f, 0.f, 0.f,
	 0.f, 1.f, 0.f, 0.f,
	 0.f, 0.f, 1.f, 0.f,
	 0.f, 0.f, 0.f, 1.f };

	bool KeyboardEvent[5] = { false, false, false, false, false };

public:
	bool red = false;
	bool green = false;
	bool blue = false;
	bool black = false;

	bool clickRed = false;
	bool clickGreen = false;
	bool clickBlue = false;
	bool clickBlack = false;


	float* getMatirix();

	bool* getKeyboardEvent();

	bool isPicking = false;

	bool isWireFrame = false;

	bool isBox = false;

	bool startRed = false;
	bool startBlue = false;
	bool startGreen = false;

	bool clickStartRed = false;
	bool clickStartBlue = false;
	bool clickStartGreen = false;

	bool isAnimation = false;

	bool isOpen = false;

	bool isMonster = false;

	void isMonsterOpen();

	string fileNme = "";
	bool isLoad = false;

	bool fileSave = false;

	int xRatio = 100;
	int yRatio = 100;
	int zRatio = 100;

	singletonBox() {}
	~singletonBox () {}
};

