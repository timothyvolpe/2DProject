#pragma once

#include "interfacebase.h"

class CInterfacePanel : public CInterfaceParent
{
public:
	CInterfacePanel();
	~CInterfacePanel();

	bool onCreate();
	void onDestroy();

	bool onActivate();

	void onDraw();
};

