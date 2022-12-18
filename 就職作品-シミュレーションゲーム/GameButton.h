#pragma once
#include "myimgui.h"
#include "CDirectxGraphics.h"
#include "ResourceManager.h"

class GameButton
{
public:

	//�{�^�����
	enum GameButtonType {

		GAMEBUTTON_STOP,
		GAMEBUTTON_PLAY_00,
		GAMEBUTTON_PLAY_01,
		GAMEBUTTON_PLAY_02,

		GAMEBUTTON_MAX,
		GAMEBUTTON_NONE = -1,
	};

	//�{�^�����
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

	//���z�{�^��
	enum BuildButtonType
	{
		BUILDBUTTON_HOUSE,
		BUILDBUTTON_SOUKO,
		BUILDBUTTON_ROAD,

		BUILDBUTTON_MAX,
		BUILDBUTTON_NONE = -1,
 

	};

	//���z-��
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

	//���z-�q��
	enum SoukoButtonType
	{
		SOUKO_SMALL,

		SOUKOBUTTON_MAX,
		SOUKOBUTTON_NONE = -1,
	};

	//���z-���H
	enum RoadButtonType
	{
		ROAD_DIRT,

		ROADBUTTON_MAX,
		ROADBUTTON_NONE = -1,
	};
	//�R���X�g���N�^
	GameButton();
	//�f�X�g���N�^
	~GameButton();

	//�C���X�^���X
	static GameButton& GetInstance() {
		static GameButton Instance;
		return Instance;
	}

	//������
	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw();

	//�v���r���[���f���̕`��
	void PrevieModelDraw();
	//�폜
	void UnInit();

	//�Q�[�����W�I�{�^���z������N���A
	void ClearGameRadio();

	//�Z���N�g���W�I�{�^���z������N���A
	void ClearSelectRadio();

	//���z���W�I�{�^���z������N���A
	void ClearBuildRadio();

	//�n�E�X���W�I�{�^���z������N���A
	void ClearHouseRadio();

	//�q�Ƀ��W�I�{�^���z������N���A
	void ClearSoukoRadio();
	//�f�o�b�O���ǂ���
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
	
	//�z�u�v���r���[�\��
	std::unique_ptr<Resource> modelpreview;

	DirectX::XMFLOAT3 m_mouseworldpos;

	//�E�B���h�E�e�N�X�`��
	ID3D11ShaderResourceView* m_windowback_texture_view;
	//�I��͈̓e�N�X�`��
	ID3D11ShaderResourceView* m_selectarea_texture_view;

	ID3D11ShaderResourceView* m_system_action_texture_view;

	ID3D11ShaderResourceView* m_texture_view[GAMEBUTTON_MAX];

	ID3D11ShaderResourceView* m_select_texture_view[SELECTBUTTON_MAX];

	ID3D11ShaderResourceView* m_build_texture_view[BUILDBUTTON_MAX];

	ID3D11ShaderResourceView* m_build_house_texture_view[HOUSEBUTTON_MAX];

	ID3D11ShaderResourceView* m_build_souko_texture_view[SOUKOBUTTON_MAX];

	ID3D11ShaderResourceView* m_build_road_texture_view[ROADBUTTON_MAX];

	ID3D11ShaderResourceView* m_action_priority_texture;
	//���\�[�X�\���p�e�N�X�`��
	ID3D11ShaderResourceView* m_resource_preview_texture[(int)ItemType::ITEM_MAX];

	//�Z�l���X�g�p�e�N�X�`��
	ID3D11ShaderResourceView* m_party_list_texture[(int)PartyListType::LIST_MAX];

	GameButtonType m_Game_HoverButton;
	SelectButtonType m_Select_HoverButton;
	BuildButtonType m_Build_HoverButton;
	HouseButtonType m_Build_House_HoverButton;
	SoukoButtonType m_Build_Souko_HoverButton;
	RoadButtonType m_Build_Road_HoverButton;

	//�{�^�����(�g���K�[)
	bool m_TriggerButton[GAMEBUTTON_MAX];
	//�{�^�����(���W�I)
	bool m_RadioButton[GAMEBUTTON_MAX];
	
	//�I���{�^��(���W�I)
	bool m_Select_RadioButton[SELECTBUTTON_MAX];

	//���z�{�^��(���W�I)
	bool m_Build_RadioButton[BUILDBUTTON_MAX];

	//���z_�n�E�X�{�^��(���W�I)
	bool m_Build_House_RadioButton[SOUKOBUTTON_MAX];

	//���z_�q�Ƀ{�^��(���W�I)
	bool m_Build_Souko_RadioButton[SOUKOBUTTON_MAX];

	//���z_���H�{�^��(���W�I)
	bool m_Build_Road_RadioButton[ROADBUTTON_MAX];

	//���z��

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

	//�t�H���g
	ImFont* font_medieval;
	ImFont* font_genei;
};

