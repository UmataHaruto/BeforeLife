#include "Window.h"
#include "CDirectInput.h"
#include "2dsystem.h"

Window::Window(XMFLOAT2 Pos, XMFLOAT2 Size,XMFLOAT4 color, bool CancelKey, const char* TexturePath)
{
	//初期処理
	Init();

	Init2D();

	//Windowの生成
	m_sprite.LoadTexture(TexturePath);
	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 1.0f;
	uv[3].x = 1.0f;
	uv[3].y = 1.0f;

	//初期処理
	m_sprite.Init(Size.x, Size.y, color, uv);

	//色設定
	m_sprite.SetColor(color);

	//位置設定
	m_sprite.SetPosition(Pos.x, Pos.y, 0);
	//ステータス設定
	m_sprite.SetStatus(UISTATUS::LIVE);

	m_sprite.SetAnimation(false, false);
	m_sprite.SetCutUV(1, 1);

	//頂点更新
	m_sprite.updateVbuffer();

	Button board(ButtonType::TOGGLE, XMFLOAT2(660, 126), XMFLOAT2(25, 25), "assets/sprite/UI/CansselButton.png");
	if (CancelKey) {
		//バツボタンアリの場合
		m_Buttons.push_back(board);
	}
}

void Window::Init()
{
}

void Window::Update()
{
	//ウィンドウのスプライトを更新
	m_sprite.Update();

	//ボタンの更新
	for (int i = 0; i < m_Buttons.size(); i++) {
		if (m_Buttons[i].CheckHit())
		{
			//マウスがボタン上にある
			m_Buttons[i].ChangeMouseOn();
			//ボタンがクリックされた
			if (CDirectInput::GetInstance().GetMouseLButtonTrigger()) {
				m_Buttons[i].ChangeClick();
			}
		}
		
	}
	

}

void Window::Draw()
{
	//ウィンドウの表示
	m_sprite.Draw();
	//ボタンの描画
	for (int i = 0; i < m_Buttons.size(); i++) {
		m_Buttons[i].Draw();
	}
}

void Window::UnInit()
{
	//ウィンドウの解放
	m_sprite.UnInit();

	//ボタンの解放
	for (int i = 0; i < m_Buttons.size(); i++) {
		m_Buttons[i].UnInit();
	}
	m_Buttons.clear();
}

bool Window::GetDestroy()
{
	return m_IsDestroy;
}

void Window::ClickEvent_00()
{
}

void Window::ClickEvent_01()
{
}

void Window::ClickEvent_02()
{
}

void Window::ClickEvent_03()
{
}

void Window::ClickEvent_04()
{
}

void Window::ClickEvent_05()
{
}

void Window::ClickEvent_06()
{
}

void Window::ClickEvent_07()
{
}

void Window::ClickEvent_08()
{
}

void Window::ClickEvent_09()
{
}
