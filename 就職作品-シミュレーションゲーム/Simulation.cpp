#pragma comment(lib,"dxguid.lib")
#include	<array>
#include	"player.h"
#include	"CCamera.h"
#include    "skydome.h"
#include    "Effect.h"
#include    "DX11util.h"
#include    "mystring.h"
#include    "2dsystem.h"
#include    "Shader.h"
#include "DirectXMath.h"
#include "BoundingSphere.h"
#include "Enemy.h"
#include "ModelMgr.h"
#include "Application.h"
#include "CLight.h"
#include "Bullet.h"
#include "CBillBoard.h"
#include "Stage.h"
#include "Sprite2DMgr.h"
#include "Button.h"
#include "myimgui.h"
#include <tchar.h>
#include	"WICTextureLoader.h"
#include "GameButton.h"

Player g_player;
Enemy g_enemy;    //�G�I�u�W�F�N�g
Skydome g_skybox;       // �X�J�C�h�[��
Effect Fog;    //Fog�G�t�F�N�g
Effect Sea;    //�C�I�u�W�F�N�g
Bullet bullets;    //�e�N���X
CBillBoard fireSmoke;    //�X���[�N
Block block;
XMFLOAT4X4 FirstCameraPos;

BoundingSphere	g_bs;	// ���E��

//���Ԍv��
DWORD starttime;
DWORD endtime;
DWORD timer;

//�����I�����_�����O
const DXGI_SURFACE_DESC* pDesc;
ID3D11Device* pDevice;
ID3D11Texture2D* mpTex;
ID3D11SamplerState* mpSmp;
ID3D11Buffer* mpQuadVB;		//�f�o�b�O�`��p���_�o�b�t�@
ID3D11RenderTargetView* targetview;
ID3D11Resource*	g_TexResource;

ID3D11VertexShader* m_pVertexShader = nullptr;		// ���_�V�F�[�_�[���ꕨ
ID3D11PixelShader* m_pPixelShader = nullptr;		// �s�N�Z���V�F�[�_�[���ꕨ
uint32_t mQuadStride;
ID3D11ShaderResourceView* SRV;

enum class CAMERAMODE
{
	FIXED,
	DELAYTPS,
	TPS,
	FPS,
};

CAMERAMODE cameratype = CAMERAMODE::TPS;

//���݂̈ړ���
int nowcnt;

//���`��Ԃ̉�
int lerpcnt = 20;

void  SimulationInit() {

	for (int i = 0; i < ModelMgr::GetInstance().g_modellist.size(); i++)
	{
		ModelMgr::GetInstance().LoadModel(
			ModelMgr::GetInstance().g_modellist[i].modelname,
			ModelMgr::GetInstance().g_modellist[i].vsfilename,
			ModelMgr::GetInstance().g_modellist[i].psfilename,
			ModelMgr::GetInstance().g_modellist[i].texfolder);
	}

	//�QD�̏�����
	Init2D();
	//�QD�e�L�X�g������
	MyString::InitMyString();

	//�J���������ʒu�̏�����
	DX11MtxIdentity(FirstCameraPos);

	// IMGUI������
	imguiInit();

	//SetTime = 6:00
	Application::GAME_TIME = 360;

	//�Q�[���{�^��������
	GameButton::GetInstance().Init();

	//�X�v���C�g�}�l�[�W���[�̏�����
	Sprite2DMgr::GetInstance().Init();

	//�{�^���̏�����
	//MenuButton = new Button(ButtonType::TOGGLE,XMFLOAT2(1260,20),XMFLOAT2(30,30), "assets/sprite/UI/MenuButton.png");

	fireSmoke.LoadTexTure("assets/ModelData/Smoke.png");
	fireSmoke.Init(0, 0, 200, 100, 100, XMFLOAT4(1, 1, 1, 1));
	Sprite2DMgr::GetInstance().CreateUI(UILIST::BLACKBACK_START, 1000, 530, 0, 11, 5.5, XMFLOAT4(1, 1, 1, 1));

	//MouseCursor����
	Sprite2DMgr::GetInstance().CreateUI(UILIST::ICON_MOUSE, 1000, 530, 0, 0.2,0.2, XMFLOAT4(1, 1, 1, 1));

	// ���C�g������
	XMFLOAT4 lightpos(0, 0, 1, 0);

	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 1.0f;
	uv[3].x = 1.0f;
	uv[3].y = 1.0f;

	fireSmoke.SetUV(uv);


	// �v���C��������
	//g_player.Init();
	//g_player.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname));

	block.Init();

	//�v���C���[������
	g_player.Init();
	g_player.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname));


	//�G�l�~�[������
	g_enemy.Init();
	g_enemy.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::ENEMY)].modelname));

	//�X�J�C�h�[��������
	g_skybox.Init();
	g_skybox.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::SKYBOX)].modelname));

	//���G�t�F�N�g�̏�����
	Fog.Init();
	Fog.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::FOG)].modelname));

	//�C�̏�����
	Sea.Init();
	Sea.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::SEA)].modelname));

	//�X�e�[�W�����[�h
	Stage::GetInstance().RoadStageData();

	//���f���̑S���_�𔲂��o��

	//�J�n���Ԃ̐ݒ�
	starttime = timeGetTime();
}

void  SimulationExit() {

	// �v���C���I������
	g_player.Finalize();

	g_enemy.Finalize();

	Sea.Finalize();

	MyString::ClearMyString();

	//�X�J�C�h�[���I������
	g_skybox.Finalize();

	imguiExit();

}

void  SimulationUpdate() {

	//�o�ߎ��Ԃ�ݒ�(�~���b)
	endtime = timeGetTime();
	timer = (endtime - starttime);

	CDirectInput::GetInstance().GetMouseState();

	//�X�v���C�g�}�l�[�W���[�̍X�V
	Sprite2DMgr::GetInstance().Update();

	//�{�^���X�V
	GameButton::GetInstance().Update();

	//�{�^���ƃ}�E�X�̂����蔻��
	//if (MenuButton->CheckHit())
	//{
	//	//�}�E�X���{�^����ɂ���
	//	MenuButton->ChangeMouseOn();
	//	//�{�^�����N���b�N���ꂽ
	//if(CDirectInput::GetInstance().GetMouseLButtonTrigger()){
	//	Sprite2DMgr::GetInstance().CreateWindowSprite(XMFLOAT2(400, 300), XMFLOAT2(600, 400),XMFLOAT4(1,1,1,1),true, "assets/sprite/UI/Window.png");
	//	MenuButton->ChangeClick();
	//}
	//}

	//�{�^���X�V
	//MenuButton->Update();

	// �v���C���X�V
	g_player.Update();

	//�G�l�~�[�X�V
	g_enemy.Update();

	Fog.Update(CCamera::GetInstance()->GetCameraMatrix());

	Sea.Update(CCamera::GetInstance()->GetCameraMatrix());

	//�e�X�V
	bullets.Update();

	//F5�L�[����(�g���K�[)
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_F5))
	{
		switch (cameratype)
		{
		case CAMERAMODE::FIXED:
			cameratype = CAMERAMODE::DELAYTPS;

			break;

		case CAMERAMODE::DELAYTPS:
			cameratype = CAMERAMODE::TPS;
			break;

		case CAMERAMODE::TPS:
			cameratype = CAMERAMODE::FPS;
			break;

		case CAMERAMODE::FPS:
			cameratype = CAMERAMODE::FIXED;
			break;

		default:
			break;
		}
	}
	//�X�J�C�h�[���X�V
	g_skybox.Update(XMFLOAT3(0,0,0));
	XMFLOAT4X4 Rotation;
	XMMATRIX RotationMtx;
	XMMATRIX PositionMtx;
	XMFLOAT3 CameraAngle = {0,0,0};
	XMFLOAT3 Position = CCamera::GetInstance()->GetEye();
	XMFLOAT4X4 CameraMtx = CCamera::GetInstance()->GetCameraMatrix();
	float speed = 1;
	float dot = 0;

	DX11MtxIdentity(Rotation);
	//�J�����^�C�v�ɉ����Đݒ�
	switch (cameratype)
	{
	case CAMERAMODE::FIXED:
		CCamera::GetInstance()->FixedCamera(XMFLOAT3(0,0,0));
		break;

	case CAMERAMODE::DELAYTPS:
		CCamera::GetInstance()->DelayTPSCamera(g_player.GetMtx());
		break;

	case CAMERAMODE::TPS:
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) {
			FirstCameraPos._41 -= speed * FirstCameraPos._11;
			FirstCameraPos._43 -= speed * FirstCameraPos._13;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D)) {
			FirstCameraPos._41 += speed * FirstCameraPos._11;
			FirstCameraPos._43 += speed * FirstCameraPos._13;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			FirstCameraPos._41 += speed * FirstCameraPos._31;
			FirstCameraPos._43 += speed * FirstCameraPos._33;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S)) {
			FirstCameraPos._41 -= speed * FirstCameraPos._31;
			FirstCameraPos._43 -= speed * FirstCameraPos._33;
		}

		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT)) {
			CameraAngle.y-= 0.5;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT)) {
			CameraAngle.y += 0.5;
		}

		DX11MtxRotationY(CameraAngle.y, Rotation);
		RotationMtx = XMLoadFloat4x4(&Rotation);
		PositionMtx = XMLoadFloat4x4(&FirstCameraPos);
		PositionMtx = XMMatrixMultiply(RotationMtx, PositionMtx);
		XMStoreFloat4x4(&FirstCameraPos, PositionMtx);

		CCamera::GetInstance()->TPSCamera(FirstCameraPos);
		break;

	default:

		break;
	}


}

void  SimulationDraw() {


	//�RD�`��
	TurnOnZbuffer();

	Stage::GetInstance().Draw();

	// �v���C���`��
	g_player.Draw();

	g_skybox.Draw();

	//2D�`��
	TurnOffZbuffer();

	Init2D();
	
	GameButton::GetInstance().Draw();

	Sprite2DMgr::GetInstance().Draw(CCamera::GetInstance()->GetCameraMatrix());

	//�{�^���`��
	//MenuButton->Draw();

	//�E�B���h�E���W
	//Screen���W���o��
	XMFLOAT4X4 PlayuerPos = g_player.GetMtx();
	XMMATRIX worldMatrix = XMMatrixTranslation(PlayuerPos._41, PlayuerPos._42, PlayuerPos._43);
	XMMATRIX view_mtx = XMLoadFloat4x4(&CCamera::GetInstance()->GetCameraMatrix());
	XMMATRIX projection_mtx = XMLoadFloat4x4(&CCamera::GetInstance()->GetProjectionMatrix());
	//�X�N���[�����W�ϊ��s��
	XMFLOAT4X4 Screen;
	DX11MtxIdentity(Screen);
	Screen._11 = Application::CLIENT_WIDTH / 2;
	Screen._41 = Application::CLIENT_WIDTH / 2;
	Screen._22 = Application::CLIENT_HEIGHT / 2;
	Screen._42 = Application::CLIENT_HEIGHT / 2;
	Screen._33 = 1;
	Screen._44 = 1;
	XMMATRIX Screen_mtx = XMLoadFloat4x4(&Screen);


	ID3D11DeviceContext* pdevice;
	CDirectXGraphics::GetInstance()->GetDXDevice()->GetImmediateContext(&pdevice);

	XMMATRIX mtx = XMMatrixMultiply(worldMatrix,view_mtx);
	mtx = XMMatrixMultiply(mtx, projection_mtx);
	mtx = XMMatrixMultiply(mtx,Screen_mtx);


	XMFLOAT4X4 position;
	XMStoreFloat4x4(&position, mtx);

	//�\���e�L�X�g��ݒ�
	char str[128];
	sprintf_s<128>(str, "PlayerScreenPos x:%f, y:%f",position._41, position._42);
	MyString::DrawString(10, 275, 20, 20, XMFLOAT4(1, 1, 1, 1), str);
	ImGuiIO& io = ImGui::GetIO();

	sprintf_s<128>(str, "MouseScreenPos x:%f, y:%f",io.MousePos.x, io.MousePos.y);
	MyString::DrawString(10, 295, 20, 20, XMFLOAT4(1, 1, 1, 1), str);


	//Score�\��
	switch (cameratype)
	{
	case CAMERAMODE::FIXED:
		sprintf_s<128>(str, "CameraMode FixedCamera");
		MyString::DrawString(10, 75, 20, 20, XMFLOAT4(1, 1, 1, 1), str);

		break;

	case CAMERAMODE::DELAYTPS:
		sprintf_s<128>(str, "CameraMode DelayCamera");
		MyString::DrawString(10, 75, 20, 20, XMFLOAT4(1, 1, 1, 1), str);
		break;

	case CAMERAMODE::TPS:
		sprintf_s<128>(str, "CameraMode TPSCamera");
		MyString::DrawString(10, 75, 20, 20, XMFLOAT4(1, 1, 1, 1), str);

		sprintf_s<128>(str, "MousePos X:%d Y:%d", CDirectInput::GetInstance().GetMousePosX(), CDirectInput::GetInstance().GetMousePosY());
		MyString::DrawString(10, 15, 20, 20, XMFLOAT4(1, 1, 1, 1), str);
		break;

	case CAMERAMODE::FPS:
		sprintf_s<128>(str, "CameraMode FPSCamera");
		MyString::DrawString(10, 75, 20, 20, XMFLOAT4(1, 1, 1, 1), str);
		break;

	default:
		break;
	}
	sprintf_s<128>(str, "I usually watch a movie on my days off. ");
	MyString::DrawString(10, 105, 20, 20, XMFLOAT4(1, 1, 1, 1), str);


	//�QD�e�L�X�g�̕\��
	MyString::DrawMyString();

}