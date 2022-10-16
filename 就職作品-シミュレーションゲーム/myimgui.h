#pragma once
#include	<functional>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

void imguiInit();
void imguiDraw(std::function<void(void)>);
void imguiExit();
