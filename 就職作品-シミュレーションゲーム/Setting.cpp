#include "Setting.h"
#include "CDirectInput.h"
#include "2dsystem.h"

Setting::Setting(XMFLOAT2 Pos, XMFLOAT2 Size, XMFLOAT4 color, bool CancelKey, const char* TexturePath) : Window(Pos,Size,color,CancelKey,TexturePath)
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

void Setting::ClickEvent_00()
{
	m_IsDestroy = true;
}
