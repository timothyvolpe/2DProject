#pragma once

#include "interfacebase.h"

class CInterfaceScreen : public CInterfaceParent
{
public:
	CInterfaceScreen();
	~CInterfaceScreen();

	bool onCreate();
	void onDestroy();

	void onDraw();
};