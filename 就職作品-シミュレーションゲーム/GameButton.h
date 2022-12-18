#pragma once
#include "myimgui.h"
#include "CDirectxGraphics.h"
#include "ResourceManager.h"

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

	//建築ボタン
	enum BuildButtonType
	{
		BUILDBUTTON_HOUSE,
		BUILDBUTTON_SOUKO,
		BUILDBUTTON_ROAD,

		BUILDBUTTON_MAX,
		BUILDBUTTON_NONE = -1,
 

	};

	//建築-家
	enum HouseButtonType
	{
		HOUSE_SMALL,

		HOUSEBUTTON_MAX,
		HOUSEBUTTON_NONE = -1,
	};

	enum PartyListType
	{
		BAR_BACK,
		BAR_FRAME,
		BAR_NAME,
		BAR_HELTH,
		BAR_STAMINA,
		ICON_HELTH,
		ICON_STAMINA,
		ICON_EMOTE,

		LIST_MAX,
	};

	//建築-倉庫
	enum SoukoButtonType
	{
		SOUKO_SMALL,

		SOUKOBUTTON_MAX,
		SOUKOBUTTON_NONE = -1,
	};

	//建築-道路
	enum RoadButtonType
	{
		ROAD_DIRT,

		ROADBUTTON_MAX,
		ROADBUTTON_NONE = -1,
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

	//プレビューモデルの描画
	void PrevieModelDraw();
	//削除
	void UnInit();

	//ゲームラジオボタン配列内をクリア
	void ClearGameRadio();

	//セレクトラジオボタン配列内をクリア
	void ClearSelectRadio();

	//建築ラジオボタン配列内をクリア
	void ClearBuildRadio();

	//ハウスラジオボタン配列内をクリア
	void ClearHouseRadio();

	//倉庫ラジオボタン配列内をクリア
	void ClearSoukoRadio();
	//デバッグかどうか
	bool GetDebug()
	{
		return m_debug;
	}

	Resource* GetModelPreview()
	{
		return modelpreview.get();
	}

     void SetMouseWorld(XMFLOAT3 pos);

private:
	ImGuiIO* m_io;
	bool* m_windowActivate;
	bool modelpreviewflg = false;
	ImGuiWindowFlags m_window_flags = 0;
	
	//配置プレビュー表示
	std::unique_ptr<Resource> modelpreview;

	DirectX::XMFLOAT3 m_mouseworldpos;

	//ウィンドウテクスチャ
	ID3D11ShaderResourceView* m_windowback_texture_view;
	//選択範囲テクスチャ
	ID3D11ShaderResourceView* m_selectarea_texture_view;

	ID3D11ShaderResourceView* m_system_action_texture_view;

	ID3D11ShaderResourceView* m_texture_view[GAMEBUTTON_MAX];

	ID3D11ShaderResourceView* m_select_texture_view[SELECTBUTTON_MAX];

	ID3D11ShaderResourceView* m_build_texture_view[BUILDBUTTON_MAX];

	ID3D11ShaderResourceView* m_build_house_texture_view[HOUSEBUTTON_MAX];

	ID3D11ShaderResourceView* m_build_souko_texture_view[SOUKOBUTTON_MAX];

	ID3D11ShaderResourceView* m_build_road_texture_view[ROADBUTTON_MAX];

	ID3D11ShaderResourceView* m_action_priority_texture;
	//リソース表示用テクスチャ
	ID3D11ShaderResourceView* m_resource_preview_texture[(int)ItemType::ITEM_MAX];

	//住人リスト用テクスチャ
	ID3D11ShaderResourceView* m_party_list_texture[(int)PartyListType::LIST_MAX];

	GameButtonType m_Game_HoverButton;
	SelectButtonType m_Select_HoverButton;
	BuildButtonType m_Build_HoverButton;
	HouseButtonType m_Build_House_HoverButton;
	SoukoButtonType m_Build_Souko_HoverButton;
	RoadButtonType m_Build_Road_HoverButton;

	//ボタン状態(トリガー)
	bool m_TriggerButton[GAMEBUTTON_MAX];
	//ボタン状態(ラジオ)
	bool m_RadioButton[GAMEBUTTON_MAX];
	
	//選択ボタン(ラジオ)
	bool m_Select_RadioButton[SELECTBUTTON_MAX];

	//建築ボタン(ラジオ)
	bool m_Build_RadioButton[BUILDBUTTON_MAX];

	//建築_ハウスボタン(ラジオ)
	bool m_Build_House_RadioButton[SOUKOBUTTON_MAX];

	//建築_倉庫ボタン(ラジオ)
	bool m_Build_Souko_RadioButton[SOUKOBUTTON_MAX];

	//建築_道路ボタン(ラジオ)
	bool m_Build_Road_RadioButton[ROADBUTTON_MAX];

	//建築物

	ImVec2 m_pos;
	ImVec2 m_size;                         // Size of the image we want to make visible
	ImVec2 m_game_uv_min[GAMEBUTTON_MAX];                 // Top-left
	ImVec2 m_game_uv_max[GAMEBUTTON_MAX];                 // Lower-right
	ImVec2 m_select_uv_min[SELECTBUTTON_MAX];                 // Top-left
	ImVec2 m_select_uv_max[SELECTBUTTON_MAX];                 // Lower-right
	ImVec2 m_build_uv_min[BUILDBUTTON_MAX];                 // Top-left
	ImVec2 m_build_uv_max[BUILDBUTTON_MAX];                 // Lower-right
	ImVec2 m_build_house_uv_min[HOUSEBUTTON_MAX];                 // Top-left
	ImVec2 m_build_house_uv_max[HOUSEBUTTON_MAX];                 // Lower-right
	ImVec2 m_build_souko_uv_min[HOUSEBUTTON_MAX];                 // Top-left
	ImVec2 m_build_souko_uv_max[HOUSEBUTTON_MAX];                 // Lower-right
	ImVec2 m_build_road_uv_min[ROADBUTTON_MAX];                 // Top-left
	ImVec2 m_build_road_uv_max[ROADBUTTON_MAX];                 // Lower-right
	ImVec4 m_tint_col;   // No tint
	ImVec4 m_border_col; // 50% opaque white
	bool m_debug = false;
	ImVec4 m_bg_col;             // Black background
	ImVec4* m_text_color;
	ImGuiStyle* m_style;

	//フォント
	ImFont* font_medieval;
	ImFont* font_genei;
};

