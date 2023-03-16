#include "stdafx.h"
#include "singletonBox.h"



float* singletonBox::getMatirix()
{
	return matirix;
}

bool* singletonBox::getKeyboardEvent()
{
	return KeyboardEvent;
}

void singletonBox::isMonsterOpen()
{
	Sleep(1000);
	BOX->isMonster = true;
}

