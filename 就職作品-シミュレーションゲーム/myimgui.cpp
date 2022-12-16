#include <functional>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "Application.h"
#include "dx11util.h"
#include "myimgui.h"
#include "dx11mathutil.h"

// 初期化
void imguiInit() {

	// im gui 初期化
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("assets\\font\\Zenfont.ttf", 70.0f, nullptr,glyphRangesJapanese);

	ImGui_ImplWin32_Init(Application::Instance()->GetHWnd());
	ImGui_ImplDX11_Init(GetDX11Device(), GetDX11DeviceContext());
	// スタイルをクラシックに
	ImGui::StyleColorsClassic();

}

void imguiDraw(std::function<void(void)> func){

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	func();

	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void imguiExit() {
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
}

//マウス選択範囲内にオブジェクトが存在するかを返す
bool CheckMouseSelect(DirectX::XMFLOAT3 pos)
{

	//選択範囲
	static ImVec2 DragStartPos = ImVec2(0, 0);

	//選択範囲スタート地点
	if (ImGui::IsMouseClicked(0)) {
		DragStartPos = ImVec2(ImGui::GetMousePos().x / Application::CLIENT_WIDTH, ImGui::GetMousePos().y / Application::CLIENT_HEIGHT);
	}
	if (ImGui::IsMouseDown(0))
	{
		ImVec2 SelectMin;
		ImVec2 SelectMax;
		if (DragStartPos.x >= ImGui::GetMousePos().x / Application::CLIENT_WIDTH)
		{
			SelectMin.x = ImGui::GetMousePos().x / Application::CLIENT_WIDTH;
			SelectMax.x = DragStartPos.x;
		}
		else
		{
			SelectMin.x = DragStartPos.x;
			SelectMax.x = ImGui::GetMousePos().x / Application::CLIENT_WIDTH;
		}

		if (DragStartPos.y >= ImGui::GetMousePos().y / Application::CLIENT_HEIGHT)
		{
			SelectMin.y = ImGui::GetMousePos().y / Application::CLIENT_HEIGHT;
			SelectMax.y = DragStartPos.y;
		}
		else
		{
			SelectMin.y = DragStartPos.y;
			SelectMax.y = ImGui::GetMousePos().y / Application::CLIENT_HEIGHT;
		}

		//スクリーン上での座標
		DirectX::XMFLOAT3 ScreenPos = DX11WorldtoScreen(DirectX::XMFLOAT3(pos.x,pos.y,pos.z));
		//選択されているか
		if (ScreenPos.x > SelectMin.x && ScreenPos.x < SelectMax.x)
		{
			if (ScreenPos.y > SelectMin.y && ScreenPos.y < SelectMax.y)
			{
				return true;
			}
		}
	}
	return false;
}
