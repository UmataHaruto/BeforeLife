#pragma once
#include "Window.h"
class Setting : public Window
{
public:
	Setting();
	Setting(XMFLOAT2 Pos, XMFLOAT2 Size, XMFLOAT4 color, bool CancelKey, const char* TexturePath);
	void ClickEvent_00();

private:


};

