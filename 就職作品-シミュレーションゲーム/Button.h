#pragma once
#include "dx11mathutil.h"
#include "quad2d.h"
#include "CDirectInput.h"

//ボタンのクリック種類
enum class ButtonType
{
	RADIO,
	TOGGLE,
};

class Window;
//ボタンの種類
class Button
{
public:

	Window* parent;

	Button(ButtonType type, XMFLOAT2 Pos,XMFLOAT2 Size,const char* TexturePath);

	//ボタンの生成と初期設定
	void Init();

	//更新処理
	void Update();

	//あたり判定と更新
	bool CheckHit();

	//ON,OFF切り替え
	void ChangeClick();

	//マウスのOn情報を変更
	void ChangeMouseOn();

	//クリック状況を取得
	bool GetSts();

	//解放処理
	void UnInit();

	//描画
	void Draw();

private:

	Quad2D m_sprite;
	//ボタンの状態
	bool m_sts = false;

	//ボタン上にマウスが存在している
	bool m_OnMouse = false;
	//ボタンタイプ
	ButtonType m_type;

};

