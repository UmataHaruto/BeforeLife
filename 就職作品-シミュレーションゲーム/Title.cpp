#include "Title.h"
#include <cinttypes>
#include "dx11util.h"
#include "CModel.h"
#include "CCamera.h"
#include "DX11Settransform.h"
#include "dx11mathutil.h"
#include "Application.h"
#include "CDirectInput.h"
#include "Simulation.h"
#include "ModelMgr.h"
#include "2dsystem.h"
#include "Sprite2DMgr.h"
#include "myimgui.h"
#include "SceneMgr.h"
#include "NameGenerator.h"
#include <thread>

ID3D11ShaderResourceView* m_title_button_texture[5];
ID3D11ShaderResourceView* m_loading_bar_texture[3];

void TitleInit()
{
	// DX11������
	DX11Init(
		Application::Instance()->GetHWnd(),
		Application::CLIENT_WIDTH,
		Application::CLIENT_HEIGHT,
		false);

	// �J�������K�v
	DirectX::XMFLOAT3 eye(500, 0, -500);		// �J�����̈ʒu
	DirectX::XMFLOAT3 lookat(0, 0, 0);		// �����_
	DirectX::XMFLOAT3 up(0, 1, 0);			// �J�����̏�����x�N�g��

	CCamera::GetInstance()->Init(
		10.0f,							// �j�A�N���b�v
		10000.0f,						// �t�@�[�N���b�v
		XM_PI / 3.0f,					// ����p
		static_cast<float>(Application::CLIENT_WIDTH),		// �X�N���[����
		static_cast<float>(Application::CLIENT_HEIGHT),		// �X�N���[���̍���
		eye, lookat, up);					// �J�����̃f�[�^

	// ���s�������Z�b�g
	DX11LightInit(
		DirectX::XMFLOAT4(1, 1, -1, 0));

	// DIRECTINPUT������
	CDirectInput::GetInstance().Init(
		Application::Instance()->GetHInst(),
		Application::Instance()->GetHWnd(),
		Application::CLIENT_WIDTH,			// �X�N���[����
		Application::CLIENT_HEIGHT			// �X�N���[���̍���
	);
	//�X�v���C�g�}�l�[�W���[�̏�����
	Sprite2DMgr::GetInstance().Init();

	// IMGUI������
	imguiInit();

	// �A���t�@�u�����h�L����
	TurnOnAlphablend();

	//�QD�̏�����
	Init2D();
	//�摜�\��
	ID3D11Device* device = CDirectXGraphics::GetInstance()->GetDXDevice();
	ID3D11DeviceContext* device11Context = GetDX11DeviceContext();

	CreatetSRVfromFile("assets/sprite/UI/TitleBg.png", device, device11Context, &m_title_button_texture[0]);
	CreatetSRVfromFile("assets/sprite/UI/NewGame.png", device, device11Context, &m_title_button_texture[1]);
	CreatetSRVfromFile("assets/sprite/UI/Option.png", device, device11Context, &m_title_button_texture[2]);
	CreatetSRVfromFile("assets/sprite/UI/Exit.png", device, device11Context, &m_title_button_texture[3]);
	CreatetSRVfromFile("assets/sprite/UI/LoadingBg.png", device, device11Context, &m_title_button_texture[4]);

	CreatetSRVfromFile("assets/sprite/UI/ProgressBar_Back.png", device, device11Context, &m_loading_bar_texture[0]);
	CreatetSRVfromFile("assets/sprite/UI/ProgressBar.png", device, device11Context, &m_loading_bar_texture[1]);
	CreatetSRVfromFile("assets/sprite/UI/ProgressBar_Frame.png", device, device11Context, &m_loading_bar_texture[2]);

	//�t�F�[�h�C��
	Sprite2DMgr::GetInstance().CreateUI(UILIST::BLACKBACK_START, 1000, 530, 0, 11, 5.5, XMFLOAT4(1, 1, 1, 1));

	//MouseCursor����
	Sprite2DMgr::GetInstance().CreateUI(UILIST::ICON_MOUSE, 1000, 530, 0, 0.2, 0.2, XMFLOAT4(1, 1, 1, 1));

	//SoundMgr::GetInstance().XA_Play("assets/sound/BGM/Vopna.wav");

}

void TitleUpdate()
{

	static bool InitThread = false;

	CDirectInput::GetInstance().GetKeyBuffer();

	CDirectInput::GetInstance().GetMouseState();

	//�X�v���C�g�}�l�[�W���[�̍X�V
	Sprite2DMgr::GetInstance().Update();
}

void TitleDraw()
{
	float col[4] = { 0.5f,0.8f,0.8f,1 };

	// �`��O����
	DX11BeforeRender(col);

	XMFLOAT4X4 mtx;

	// �v���W�F�N�V�����ϊ��s��擾
	mtx = CCamera::GetInstance()->GetProjectionMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::PROJECTION, mtx);

	// �r���[�ϊ��s����擾
	mtx = CCamera::GetInstance()->GetCameraMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::VIEW, mtx);

	//2D�`��
	TurnOffZbuffer();

	Init2D();

	//Imgui
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

	static bool* m_windowActivate;
	static bool isloading = false;
	ImGui::SetNextWindowPos(ImVec2(-30,-20));
	ImGui::SetNextWindowSize(ImVec2(1400, 800));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.3f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.3f));

	ImGui::Begin(u8"�^�C�g��", m_windowActivate, window_flags);

	if (!isloading) {
		ImGui::Image(m_title_button_texture[0], ImVec2(1320, 800), ImVec2(0, 0), ImVec2(1, 1),ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
		ImGui::SetCursorPos(ImVec2(520, 500));
		//PLAY02�{�^��
		if (ImGui::ImageButton(m_title_button_texture[1], ImVec2(300, 70), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(1, 1, 1, 0), ImVec4(1, 1, 1, 1)))
		{
			Sprite2DMgr::GetInstance().CreateUI(UILIST::BLACKBACK_START, 1000, 530, 0, 11, 5.5, XMFLOAT4(1, 1, 1, 1));

			isloading = true;
		}
		ImGui::SetCursorPos(ImVec2(550, 570));
		//PLAY02�{�^��
		if (ImGui::ImageButton(m_title_button_texture[2], ImVec2(225, 70), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(1, 1, 1, 0), ImVec4(1, 1, 1, 1)))
		{
			printf("hit");
		}
		ImGui::SetCursorPos(ImVec2(590, 640));
		//PLAY02�{�^��
		if (ImGui::ImageButton(m_title_button_texture[3], ImVec2(150, 70), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(1, 1, 1, 0), ImVec4(1, 1, 1, 1)))
		{
			SceneMgr::GetInstance().SetEndFlg(true);
		}
	}
	//���[�h���
	else
	{
		//���[�h��ʔw�i
		ImGui::Image(m_title_button_texture[4], ImVec2(1320, 800), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 0));

		std::string name = std::to_string((int)(SceneMgr::GetInstance().GetLoadingRatio() * 100)) + std::string("%%");
		ImGui::SetWindowFontScale(3.4);

		ImGui::SetCursorPos(ImVec2(617, 547));
		ImGui::TextColored(ImVec4(0, 0, 0, 1), name.c_str());
		ImGui::SetWindowFontScale(3);

		ImGui::SetCursorPos(ImVec2(620, 550));
		ImGui::TextColored(ImVec4(1, 1, 1, 1), name.c_str());
		ImGui::SetWindowFontScale(1);

		//�v���O���X�o�[�w�i
		ImGui::SetCursorPos(ImVec2(300, 600));
		ImGui::Image(m_loading_bar_texture[0], ImVec2(700, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 0));

		//�v���O���X�o�[
		ImGui::SetCursorPos(ImVec2(320, 615));
		ImGui::Image(m_loading_bar_texture[1], ImVec2(700 * SceneMgr::GetInstance().GetLoadingRatio(), 25), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 0));

		//�v���O���X�o�[�t���[��
		ImGui::SetCursorPos(ImVec2(300, 600));
		ImGui::Image(m_loading_bar_texture[2], ImVec2(700, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 0));

		if (SceneMgr::GetInstance().GetInitStatus())
		{
			//�t�F�[�h�C��
			Sprite2DMgr::GetInstance().CreateUI(UILIST::CLOUDBACK_START, 1000, 530, 0, 11, 5.5, XMFLOAT4(1, 1, 1, 1));
			SoundMgr::GetInstance().XA_Stop("assets/sound/BGM/Vopna.wav");
			SimulationInit();
			SceneMgr::GetInstance().ChangeScene(SCENETYPE::GAME);
		}
	}
	ImGui::PopStyleColor(3);

	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	Sprite2DMgr::GetInstance().Draw(CCamera::GetInstance()->GetCameraMatrix());
	
	// �`��㏈��
	DX11AfterRender();
}

void TitleDispose()
{
}
