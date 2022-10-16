#include "Button.h"
#include "2dsystem.h"

//ボタン生成
Button::Button(ButtonType type, XMFLOAT2 Pos,XMFLOAT2 Size, const char* TexturePath)
{
	Init2D();

    //初期化処理
	Init();

	//タイプ設定
	m_type = type;

	//ボタンUIの生成
	m_sprite.LoadTexture(TexturePath);
	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 0.5f;
	uv[3].x = 1.0f;
	uv[3].y = 0.5f;

	//初期処理
	m_sprite.Init(Size.x, Size.y, XMFLOAT4(1, 1, 1, 1), uv);

	//色設定
	m_sprite.SetColor(XMFLOAT4(1,1,1,1));
	//サイズ設定
	//m_sprite.SetScale(Size.x, Size.y, 0);
	//位置設定
	m_sprite.SetPosition(Pos.x, Pos.y, 0);
	//ステータス設定
	m_sprite.SetStatus(UISTATUS::LIVE);

	m_sprite.SetAnimation(false, false);
	m_sprite.SetCutUV(1, 1);

	//頂点更新
	m_sprite.updateVbuffer();
}

//初期化処理
void Button::Init()
{
	m_sts = false;
}

void Button::Update()
{
	m_sprite.Update();

	if (m_type == ButtonType::TOGGLE)
	{
	}

	//ステータスに応じて画像を変更
	XMFLOAT2 uv[4];
	//マウスがボタンの上にある
	if (m_OnMouse)
	{
		//クリックされている
		if (m_sts)
		{
			uv[0].x = 0.0f;
			uv[0].y = 0.75f;
			uv[1].x = 1.0f;
			uv[1].y = 0.75f;
			uv[2].x = 0.0f;
			uv[2].y = 1.0f;
			uv[3].x = 1.0f;
			uv[3].y = 1.0f;
		}
		//クリックされていない
		else
		{
			uv[0].x = 0.0f;
			uv[0].y = 0.5f;
			uv[1].x = 1.0f;
			uv[1].y = 0.5f;
			uv[2].x = 0.0f;
			uv[2].y = 0.75f;
			uv[3].x = 1.0f;
			uv[3].y = 0.75f;
		}
	}
	//Mouseがボタンの上にない
	else
	{
		//クリックされている
		if (m_sts)
		{
			uv[0].x = 0.0f;
			uv[0].y = 0.25f;
			uv[1].x = 1.0f;
			uv[1].y = 0.25f;
			uv[2].x = 0.0f;
			uv[2].y = 0.5f;
			uv[3].x = 1.0f;
			uv[3].y = 0.5f;
		}
		//クリックされていない
		else
		{
			uv[0].x = 0.0f;
			uv[0].y = 0.0f;
			uv[1].x = 1.0f;
			uv[1].y = 0.0f;
			uv[2].x = 0.0f;
			uv[2].y = 0.25f;
			uv[3].x = 1.0f;
			uv[3].y = 0.25f;
		}
	}
	m_sprite.SetUV(uv);
	m_sprite.updateVbuffer();
}

//当たり判定
bool Button::CheckHit()
{
	XMFLOAT3 Pos = m_sprite.GetWorldPos();

	float width = m_sprite.GetSize().x;
	float height = m_sprite.GetSize().y;
	//あたり判定
	if (Pos.x - width / 2 < CDirectInput::GetInstance().GetMousePosX() && Pos.x + width / 2 >CDirectInput::GetInstance().GetMousePosX())
	{
		if (Pos.y - height / 2 < CDirectInput::GetInstance().GetMousePosY() && Pos.y + height / 2 >CDirectInput::GetInstance().GetMousePosY())
		{
			return true;
		}
	}
	return false;
}

void Button::ChangeClick()
{
	switch (m_type)
	{
	case ButtonType::RADIO:
		m_sts = 1 - m_sts;
		break;
	case ButtonType::TOGGLE:
		m_sts = true;
		break;
	default:
		break;
	}
}

void Button::ChangeMouseOn()
{
	m_OnMouse = true;
}

bool Button::GetSts()
{
	return m_sts;
}

void Button::Draw()
{
	m_sprite.Draw();
	if (m_type == ButtonType::TOGGLE)
	{
		m_sts = false;
	}
	m_OnMouse = false;
}

//解放処理
void Button::UnInit()
{
	m_sprite.UnInit();
}

