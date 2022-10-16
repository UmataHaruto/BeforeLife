#include "GameButton.h"
#include "DX11util.h"
#include <tchar.h>
#include "WICTextureLoader.h"
#include <iostream>
#include <vector>
#include "Shader.h"
#include "Application.h"

GameButton::GameButton()
{
}

GameButton::~GameButton()
{
}

void GameButton::Init()
{
	m_Game_HoverButton = GAMEBUTTON_NONE;

	m_io = &ImGui::GetIO();
	m_windowActivate = new bool();
	*m_windowActivate = true;

	//�{�^��������(�g���K�[)
	for (int i = 0; i < GAMEBUTTON_MAX; i++)
	{
		m_TriggerButton[i] = false;
	}

	for (int i = 0; i < GAMEBUTTON_MAX; i++)
	{
		m_RadioButton[i] = false;
	}

	for (int i = 0; i < SELECTBUTTON_MAX; i++)
	{
		m_Select_RadioButton[i] = false;
	}
	m_RadioButton[GAMEBUTTON_PLAY_00] = true;
	m_Select_RadioButton[SELECTBUTTON_MOUSE] = true;

	//�摜�\��
	ID3D11Device* device = CDirectXGraphics::GetInstance()->GetDXDevice();
	ID3D11DeviceContext* device11Context = GetDX11DeviceContext();

	//SRV����
	//�Q�[���{�^��
	CreatetSRVfromFile("assets/sprite/UI/Window.png", device, device11Context, &m_windowback_texture_view);
	CreatetSRVfromFile("assets/sprite/UI/PoseButton_00.png", device, device11Context, &m_texture_view[GAMEBUTTON_STOP]);
	CreatetSRVfromFile("assets/sprite/UI/PlayButton_00.png", device, device11Context, &m_texture_view[GAMEBUTTON_PLAY_00]);
	CreatetSRVfromFile("assets/sprite/UI/PlayButton_01.png", device, device11Context, &m_texture_view[GAMEBUTTON_PLAY_01]);
	CreatetSRVfromFile("assets/sprite/UI/PlayButton_02.png", device, device11Context, &m_texture_view[GAMEBUTTON_PLAY_02]);
	//�Z���N�g�{�^��
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_Mouse.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_MOUSE]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_ALL.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_ALL]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_Cut.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_CUT]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_Mine.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_MINE]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_COLLECT.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_COLLECT]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_CANCCEL.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_CANCEL]);

}


void GameButton::Update()
{
	//�{�^���C�x���g
	if (m_RadioButton[GAMEBUTTON_STOP])
	{
		Application::Instance()->GAME_SPEED = 0;
	}
	if (m_RadioButton[GAMEBUTTON_PLAY_00])
	{
		Application::Instance()->GAME_SPEED = 1.0f;
	}	
	if (m_RadioButton[GAMEBUTTON_PLAY_01])
	{
		Application::Instance()->GAME_SPEED = 3;

	}
	if (m_RadioButton[GAMEBUTTON_PLAY_02])
	{
		Application::Instance()->GAME_SPEED = 5;

	}
}

void GameButton::Draw()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();

	//�E�B���h�E�ݒ�
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::Begin(u8"�������", m_windowActivate, window_flags);

	m_style = &ImGui::GetStyle();
	m_text_color = m_style->Colors;

	//�e�N�X�`���ݒ�
	m_pos = ImGui::GetCursorScreenPos();
	m_size = ImVec2(32.0f, 32.0f);                         // Size of the image we want to make visible
	//UV�ݒ�
	for (int i = 0; i < GAMEBUTTON_MAX; i++) {
		m_game_uv_min[i] = ImVec2(0.0f, 0.0f);                 // Top-left
		m_game_uv_max[i] = ImVec2(1.00f, 0.25f);                 // Lower-right
	}
	for (int i = 0; i < SELECTBUTTON_MAX; i++) {
		m_select_uv_min[i] = ImVec2(0.0f, 0.0f);                 // Top-left
		m_select_uv_max[i] = ImVec2(1.00f, 0.25f);                 // Lower-right
	}
	m_tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	m_border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 50% opaque white

	m_bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);             // Black background
		//�w�i�F
	m_text_color[ImGuiCol_WindowBg] = ImVec4(0.01f, 0.37f, 0.56f, 0.90f);

	

	m_style->WindowRounding = 1.5f;
	m_style->AntiAliasedFill = true;
	m_style->AntiAliasedLines = true;

	//�z�o�[�{�^��������ꍇUV�ω�
	if (m_Game_HoverButton != -1) {
        m_game_uv_min[m_Game_HoverButton] = ImVec2(0.0f, 0.5f);
		m_game_uv_max[m_Game_HoverButton] = ImVec2(1.0f, 0.75f);
	}

	if (m_Select_HoverButton != -1) {
		m_select_uv_min[m_Select_HoverButton] = ImVec2(0.0f, 0.5f);
		m_select_uv_max[m_Select_HoverButton] = ImVec2(1.0f, 0.75f);
	}

	//�Q�[���{�^����puv
	for (int i = 0; i < GAMEBUTTON_MAX; i++)
	{
		if (m_TriggerButton[i])
		{
			m_game_uv_min[i].y += 0.25;
			m_game_uv_max[i].y += 0.25;
		}
		if (m_RadioButton[i])
		{
			m_game_uv_min[i].y += 0.25;
			m_game_uv_max[i].y += 0.25;
		}
	}

	//�Z���N�g�{�^����puv
	for (int i = 0; i <SELECTBUTTON_MAX; i++)
	{
		if (m_Select_RadioButton[i])
		{
			m_select_uv_min[i].y += 0.25;
			m_select_uv_max[i].y += 0.25;
		}
	}
	//�z�o�[���Z�b�g
	m_Game_HoverButton = GAMEBUTTON_NONE;
	m_Select_HoverButton = SELECTBUTTON_NONE;

	//�����ꂽ��
	static int PushNum = 0;
	ImGui::SetCursorPos(ImVec2(950,5));
	//�Q�[���{�^��---------------------------------------------------
	//STOP�{�^��
	if (ImGui::ImageButton(m_texture_view[GAMEBUTTON_STOP], m_size, m_game_uv_min[GAMEBUTTON_STOP], m_game_uv_max[GAMEBUTTON_STOP], 0, m_border_col, m_tint_col))
	{
		ClearGameRadio();
		m_RadioButton[GAMEBUTTON_STOP] = 1 - m_RadioButton[GAMEBUTTON_STOP];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Game_HoverButton = GAMEBUTTON_STOP;
	}
	ImGui::SetCursorPos(ImVec2(980,5));

	//PLAY00�{�^��
	if (ImGui::ImageButton(m_texture_view[GAMEBUTTON_PLAY_00], m_size, m_game_uv_min[GAMEBUTTON_PLAY_00], m_game_uv_max[GAMEBUTTON_PLAY_00], 0, m_border_col, m_tint_col))
	{
		ClearGameRadio();
		m_RadioButton[GAMEBUTTON_PLAY_00] = 1 - m_RadioButton[GAMEBUTTON_PLAY_00];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Game_HoverButton = GAMEBUTTON_PLAY_00;
	}
	ImGui::SetCursorPos(ImVec2(1010, 5));
	//PLAY01�{�^��
	if (ImGui::ImageButton(m_texture_view[GAMEBUTTON_PLAY_01], m_size, m_game_uv_min[GAMEBUTTON_PLAY_01], m_game_uv_max[GAMEBUTTON_PLAY_01], 0, m_border_col, m_tint_col))
	{
		ClearGameRadio();
		m_RadioButton[GAMEBUTTON_PLAY_01] = 1 - m_RadioButton[GAMEBUTTON_PLAY_01];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Game_HoverButton = GAMEBUTTON_PLAY_01;
	}
	ImGui::SetCursorPos(ImVec2(1040, 5));
	//PLAY02�{�^��
	if (ImGui::ImageButton(m_texture_view[GAMEBUTTON_PLAY_02], m_size, m_game_uv_min[GAMEBUTTON_PLAY_02], m_game_uv_max[GAMEBUTTON_PLAY_02], 0, m_border_col, m_tint_col))
	{
		ClearGameRadio();
		m_RadioButton[GAMEBUTTON_PLAY_02] = 1 - m_RadioButton[GAMEBUTTON_PLAY_02];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Game_HoverButton = GAMEBUTTON_PLAY_02;
	}
	//�Q�[���{�^��---------------------------------------------------

	//�Z���N�g�{�^��---------------------------------------------------
	//�w�i�F�폜
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));


	//�}�E�X�{�^��
	ImGui::SetCursorPos(ImVec2(1010, 610));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_MOUSE], ImVec2(60,60), m_select_uv_min[SELECTBUTTON_MOUSE], m_select_uv_max[SELECTBUTTON_MOUSE], 0, m_border_col, m_tint_col))
	{
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_MOUSE] = 1 - m_Select_RadioButton[SELECTBUTTON_MOUSE];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Select_HoverButton = SELECTBUTTON_MOUSE;
	}

	//ALL�{�^��
	ImGui::SetCursorPos(ImVec2(1045, 645));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_ALL], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_ALL], m_select_uv_max[SELECTBUTTON_ALL], 0, m_border_col, m_tint_col))
	{
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_ALL] = 1 - m_Select_RadioButton[SELECTBUTTON_ALL];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Select_HoverButton = SELECTBUTTON_ALL;
	}

	//���̃{�^��
	ImGui::SetCursorPos(ImVec2(1115, 645));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_CUT], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_CUT], m_select_uv_max[SELECTBUTTON_CUT], 0, m_border_col, m_tint_col))
	{
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_CUT] = 1 - m_Select_RadioButton[SELECTBUTTON_CUT];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Select_HoverButton = SELECTBUTTON_CUT;
	}

	//�̌@�{�^��
	ImGui::SetCursorPos(ImVec2(1080, 610));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_MINE], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_MINE], m_select_uv_max[SELECTBUTTON_MINE], 0, m_border_col, m_tint_col))
	{
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_MINE] = 1 - m_Select_RadioButton[SELECTBUTTON_MINE];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Select_HoverButton = SELECTBUTTON_MINE;
	}

	//�̏W�{�^��
	ImGui::SetCursorPos(ImVec2(1150, 610));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_COLLECT], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_COLLECT], m_select_uv_max[SELECTBUTTON_COLLECT], 0, m_border_col, m_tint_col))
	{
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_COLLECT] = 1 - m_Select_RadioButton[SELECTBUTTON_COLLECT];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Select_HoverButton = SELECTBUTTON_COLLECT;
	}

	//�L�����Z���{�^��
	ImGui::SetCursorPos(ImVec2(1185, 645));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_CANCEL], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_CANCEL], m_select_uv_max[SELECTBUTTON_CANCEL], 0, m_border_col, m_tint_col))
	{
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_CANCEL] = 1 - m_Select_RadioButton[SELECTBUTTON_CANCEL];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		m_Select_HoverButton = SELECTBUTTON_CANCEL;
	}

	ImGui::PopStyleColor(3);
	//�Z���N�g�{�^��---------------------------------------------------

	//���ԕ\���E�B���h�E
	ImGui::SetCursorPos(ImVec2(1072, 5));
	ImGui::Image(m_windowback_texture_view,ImVec2(150,32),ImVec2(0,0),ImVec2(1,1),m_tint_col,m_border_col);
	 ImGui::GetID(m_texture_view);

	 ImGui::SetCursorPos(ImVec2(0, 5));
	//�v�b�V����
	ImGui::Text(u8"�v�b�V����%d", PushNum);

	//������\���p
	std::string str;

	//�t���[�����[�g�\��
	str = u8"����FPS" + std::to_string((int)(1000.0f / m_io->Framerate)) + "ms/frame" + u8"(����" + std::to_string((int)m_io->Framerate) + "FPS)";

	ImGui::Text(str.c_str());
	ImGui::Text(u8"���_��%d, �C���f�b�N�X��:%d (�|���S����%d)", m_io->MetricsRenderVertices, m_io->MetricsRenderIndices, m_io->MetricsRenderIndices / 3);
	ImGui::Text(u8"���s�E�B���h�E��%d, ���蓖�Đ�:%d", m_io->MetricsRenderWindows, m_io->MetricsActiveAllocations);
	ImGui::SetCursorPos(ImVec2(1080, 5));
	ImGui::SetNextWindowContentSize(ImVec2(20,20));
	//time

	int hour = Application::Instance()->GAME_TIME / 60;
	int minutes = Application::Instance()->GAME_TIME % 60;
	ImGui::SetWindowFontScale(2);
	ImGui::Text(u8"%2d�� %2d��",hour,minutes);
	ImGui::SetWindowFontScale(1);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void GameButton::UnInit()
{
}

void GameButton::ClearGameRadio()
{
	for (int i = 0; i < GAMEBUTTON_MAX; i++)
	{
		m_RadioButton[i] = false;
	}
}

void GameButton::ClearSelectRadio()
{
	for (int i = 0; i < SELECTBUTTON_MAX; i++)
	{
		m_Select_RadioButton[i] = false;
	}
}
