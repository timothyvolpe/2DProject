#pragma once

#include "interfacebase.h"

class CFont;

class CInterfaceLabel : public CInterfaceBase
{
private:
	CFont *m_pFontHandle;

	std::wstring m_text;
public:
	CInterfaceLabel();
	~CInterfaceLabel();

	bool onCreate();
	void onDestroy();

	bool onActivate();

	void onDraw();
};