#pragma once
#include "myimgui.h"
#include "CDirectxGraphics.h"

class GameButton
{
public:

	//ボタン種類
	enum GameButtonType {

		GAMEBUTTON_STOP,
		GAMEBUTTON_PLAY_00,
		GAMEBUTTON_PLAY_01,
		GAMEBUTTON_PLAY_02,

		GAMEBUTTON_MAX,
		GAMEBUTTON_NONE = -1,
	};

	//ボタン種類
	enum SelectButtonType {

		SELECTBUTTON_MOUSE,
		SELECTBUTTON_ALL,
		SELECTBUTTON_CUT,
		SELECTBUTTON_MINE,
		SELECTBUTTON_COLLECT,
		SELECTBUTTON_CANCEL,

		SELECTBUTTON_MAX,
		SELECTBUTTON_NONE = -1,
	};
	//コンストラクタ
	GameButton();
	//デストラクタ
	~GameButton();

	//インスタンス
	static GameButton& GetInstance() {
		static GameButton Instance;
		return Instance;
	}

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();
	//削除
	void UnInit();

	//ゲームラジオボタン配列内をクリア
	void ClearGameRadio();

	//セレクトラジオボタン配列内をクリア
	void ClearSelectRadio();

private:
	ImGuiIO* m_io;
	bool* m_windowActivate;
	ImGuiWindowFlags m_window_flags = 0;
	
	ID3D11ShaderResourceView* m_windowback_texture_view;

	ID3D11ShaderResourceView* m_texture_view[GAMEBUTTON_MAX];

	ID3D11ShaderResourceView* m_select_texture_view[SELECTBUTTON_MAX];


	GameButtonType m_Game_HoverButton;
	SelectButtonType m_Select_HoverButton;

	//ボタン状態(トリガー)
	bool m_TriggerButton[GAMEBUTTON_MAX];
	//ボタン状態(ラジオ)
	bool m_RadioButton[GAMEBUTTON_MAX];

	bool m_Select_RadioButton[SELECTBUTTON_MAX];

	ImVec2 m_pos;
	ImVec2 m_size;                         // Size of the image we want to make visible
	ImVec2 m_game_uv_min[GAMEBUTTON_MAX];                 // Top-left
	ImVec2 m_game_uv_max[GAMEBUTTON_MAX];                 // Lower-right
	ImVec2 m_select_uv_min[SELECTBUTTON_MAX];                 // Top-left
	ImVec2 m_select_uv_max[SELECTBUTTON_MAX];                 // Lower-right
	ImVec4 m_tint_col;   // No tint
	ImVec4 m_border_col; // 50% opaque white

	ImVec4 m_bg_col;             // Black background
	ImVec4* m_text_color;
	ImGuiStyle* m_style;
};

