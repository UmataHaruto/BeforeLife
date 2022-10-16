#pragma once
#include "Button.h"

//ウィンドウクラス(CWindowクラスとは別)
class Window
{
public:
	
	//複数のボタンを持つことができる
	std::vector<Button> m_Buttons;

	//コンストラクタ
	Window(XMFLOAT2 Pos, XMFLOAT2 Size,XMFLOAT4 color, bool CancelKey, const char* TexturePath);
	//初期化
 	void Init();
	//更新
	void Update();
	//描画
	void Draw();
	//解放
	void UnInit();

	//破壊フラグを取得
	bool GetDestroy();

	//クリックイベント
	virtual void ClickEvent_00();
	virtual void ClickEvent_01();
	virtual void ClickEvent_02();
	virtual void ClickEvent_03();
	virtual void ClickEvent_04();
	virtual void ClickEvent_05();
	virtual void ClickEvent_06();
	virtual void ClickEvent_07();
	virtual void ClickEvent_08();
	virtual void ClickEvent_09();

protected:

	//ウィンドウの画像
	Quad2D m_sprite;

	//破壊フラグ
	bool m_IsDestroy = false;
};

