#pragma once
#include	<functional>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "dx11mathutil.h"

void imguiInit();
void imguiDraw(std::function<void(void)>);
void imguiExit();

//‘I‘ğ”ÍˆÍ“à‚©ŒŸØ
bool CheckMouseSelect(DirectX::XMFLOAT3 pos);
