#pragma once
#include "myimgui.h"
#include "CDirectxGraphics.h"

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
	//�폜
	void UnInit();

	//�Q�[�����W�I�{�^���z������N���A
	void ClearGameRadio();

	//�Z���N�g���W�I�{�^���z������N���A
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

	//�{�^�����(�g���K�[)
	bool m_TriggerButton[GAMEBUTTON_MAX];
	//�{�^�����(���W�I)
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

