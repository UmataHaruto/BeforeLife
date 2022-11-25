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
#include "CBillBoard.h"
#include "Stage.h"
#include "Sprite2DMgr.h"
#include "Button.h"
#include "myimgui.h"
#include <tchar.h>
#include	"WICTextureLoader.h"
#include "GameButton.h"
#include "mousepostoworld.h"
#include "VillagerMgr.h"
#include "NameGenerator.h"
#include "ResourceManager.h"
#include "XAudio2.h"
#include "StageHitCollider.h"
#include "Terrain.h"
#include <thread>
#include "BuildingMgr.h"
//GIT TEST


Skydome g_skybox;       // �X�J�C�h�[��
Terrain g_terrain;    //�n�`(�O��)
XMFLOAT4X4 FirstCameraPos;

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

void InitThred00()
{
	for (int i = 0; i < 6; i++)
	{
		ModelMgr::GetInstance().LoadModel(
			ModelMgr::GetInstance().g_modellist[i].modelname,
			ModelMgr::GetInstance().g_modellist[i].vsfilename,
			ModelMgr::GetInstance().g_modellist[i].psfilename,
			ModelMgr::GetInstance().g_modellist[i].texfolder);
	}
}

void InitThred01()
{
	for (int i = 6; i < 9; i++)
	{
		ModelMgr::GetInstance().LoadModel(
			ModelMgr::GetInstance().g_modellist[i].modelname,
			ModelMgr::GetInstance().g_modellist[i].vsfilename,
			ModelMgr::GetInstance().g_modellist[i].psfilename,
			ModelMgr::GetInstance().g_modellist[i].texfolder);
	}
}

void InitThred02()
{
	for (int i = 9; i < 11; i++)
	{
		ModelMgr::GetInstance().LoadModel(
			ModelMgr::GetInstance().g_modellist[i].modelname,
			ModelMgr::GetInstance().g_modellist[i].vsfilename,
			ModelMgr::GetInstance().g_modellist[i].psfilename,
			ModelMgr::GetInstance().g_modellist[i].texfolder);
	}
}

void InitThred03()
{
	for (int i = 11; i < ModelMgr::GetInstance().g_modellist.size(); i++)
	{
		ModelMgr::GetInstance().LoadModel(
			ModelMgr::GetInstance().g_modellist[i].modelname,
			ModelMgr::GetInstance().g_modellist[i].vsfilename,
			ModelMgr::GetInstance().g_modellist[i].psfilename,
			ModelMgr::GetInstance().g_modellist[i].texfolder);
	}

}

void InitThred04()
{
	//���O�����̏�����
	NameGenerator::GetInstance().Init();
	SoundMgr::GetInstance().XA_Initialize();

	//�X�v���C�g�}�l�[�W���[�̏�����
	Sprite2DMgr::GetInstance().Init();

	// IMGUI������
	imguiInit();
}
void SearchMousePosThread()
{
	XMFLOAT3 g_nearp = { 0,0,0 };
	XMFLOAT3 g_farp = { 0,0,0 };

	SearchMousePoint(g_nearp, g_farp);
}
void  SimulationInit() {

	//�������[�h���}���`�X���b�h�ōs��
	std::thread thr1(InitThred00);
	std::thread thr2(InitThred01);
	std::thread thr3(InitThred02);
	std::thread thr4(InitThred03);
	std::thread thr5(InitThred04);

	thr1.join();
	thr2.join();
	thr3.join();
	thr4.join();
	thr5.join();

	//�Q�[���{�^��������
	GameButton::GetInstance().Init();


	// ASSIMP���g�p�����A�j���[�V�����̓ǂݍ���
	bool sts = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname)->LoadAnimation("assets/ModelData/male_adult/animation/male_adult_ver1.1.fbx");
	sts = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CONIFER00)].modelname)->LoadAnimation("assets/Modeldata/tree/conifer00/conifer01.fbx");
	
	//�X�e�[�W�����[�h
	Stage::GetInstance().RoadStageData();


	//���\�[�X������
	ResourceManager::GetInstance().Init();

	//���z��������
	BuildingMgr::GetInstance().Init();

	//�X�e�[�W�R���W�������쐬
	StageHitInit();

	//�QD�̏�����
	Init2D();
	//�QD�e�L�X�g������
	MyString::InitMyString();

	//���l�}�l�[�W���[�̏�����
	VillagerMgr::GetInstance().Init();
	Player::Data pdata;
	pdata.pos = XMFLOAT3(500, 0, -500);
	pdata.firstname = NameGenerator::GetInstance().CreateName(NameGenerator::MALE);
	pdata.lastname = NameGenerator::GetInstance().CreateName(NameGenerator::FAMILLY);
	VillagerMgr::GetInstance().CreateVillager(pdata);

	//�J���������ʒu�̏�����
	DX11MtxIdentity(FirstCameraPos);
	FirstCameraPos._41 = VillagerMgr::GetInstance().m_villager[0]->GetPos().x;
	FirstCameraPos._42 = VillagerMgr::GetInstance().m_villager[0]->GetPos().y;
	FirstCameraPos._43 = VillagerMgr::GetInstance().m_villager[0]->GetPos().z;

	CCamera::GetInstance()->SetEye(VillagerMgr::GetInstance().m_villager[0]->GetPos());
	CCamera::GetInstance()->CreateCameraMatrix();

	//SetTime = 6:00
	Application::GAME_TIME = 360;

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

	//�X�J�C�h�[��������
	g_skybox.Init();
	g_skybox.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::SKYBOX)].modelname));

	//�n�`������
	g_terrain.Init();

	//�J�n���Ԃ̐ݒ�
	starttime = timeGetTime();

}

void  SimulationExit() {


	VillagerMgr::GetInstance().Uninit();

	ResourceManager::GetInstance().Uninit();

	BuildingMgr::GetInstance().Uninit();

	MyString::ClearMyString();

	//�X�J�C�h�[���I������
	g_skybox.Finalize();

	g_terrain.Finalize();

	imguiExit();

}

void  SimulationUpdate() {

	ImGuiIO& io = ImGui::GetIO();

	//�o�ߎ��Ԃ�ݒ�(�~���b)
	endtime = timeGetTime();
	timer = (endtime - starttime);

	CDirectInput::GetInstance().GetMouseState();

	//�����}�l�[�W���[�̍X�V
	ResourceManager::GetInstance().Update();

	//���z���}�l�[�W���[�̍X�V
	BuildingMgr::GetInstance().Update();


	//���l�}�l�[�W���[�̍X�V
	VillagerMgr::GetInstance().Update();

	//�X�e�[�W�X�V
	Stage::GetInstance().Update();

	//�u���b�N�I��
	//Stage::GetInstance().SearchBlock();

	//�X�v���C�g�}�l�[�W���[�̍X�V
	Sprite2DMgr::GetInstance().Update();

	//�{�^���X�V
	GameButton::GetInstance().Update();

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
	XMFLOAT3 FocusPos = CCamera::GetInstance()->GetLookat();
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
		CCamera::GetInstance()->DelayTPSCamera(VillagerMgr::GetInstance().m_villager[0]->GetMtx());
		break;

	case CAMERAMODE::TPS:
		//�Y�[���{�� �1.0f
		float zoom;
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

		if (io.MouseWheel != 0)
		{
			if (io.MouseWheel > 0)
			{
				CCamera::GetInstance()->SetZoom(CCamera::GetInstance()->GetZoom() - 0.1);
			}
			else {
				CCamera::GetInstance()->SetZoom(CCamera::GetInstance()->GetZoom() + 0.1);
			}
		}
		break;

	default:

		break;
	}


}

void  SimulationDraw() {
	//�RD�`��
	TurnOnZbuffer();

	Stage::GetInstance().Draw();
	//�����`��
	ResourceManager::GetInstance().Draw();

	//���z���`��
	BuildingMgr::GetInstance().Draw();
	GameButton::GetInstance().PrevieModelDraw();

	//���l�`��
	VillagerMgr::GetInstance().Draw();

	g_skybox.Draw();

	g_terrain.Draw();
	//2D�`��
	TurnOffZbuffer();

	Init2D();

	GameButton::GetInstance().Draw();

	Sprite2DMgr::GetInstance().Draw(CCamera::GetInstance()->GetCameraMatrix());
}