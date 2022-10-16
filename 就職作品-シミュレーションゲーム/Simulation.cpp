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
Enemy g_enemy;    //敵オブジェクト
Skydome g_skybox;       // スカイドーム
Effect Fog;    //Fogエフェクト
Effect Sea;    //海オブジェクト
Bullet bullets;    //弾クラス
CBillBoard fireSmoke;    //スモーク
Block block;
XMFLOAT4X4 FirstCameraPos;

BoundingSphere	g_bs;	// 境界球

//時間計測
DWORD starttime;
DWORD endtime;
DWORD timer;

//試験的レンダリング
const DXGI_SURFACE_DESC* pDesc;
ID3D11Device* pDevice;
ID3D11Texture2D* mpTex;
ID3D11SamplerState* mpSmp;
ID3D11Buffer* mpQuadVB;		//デバッグ描画用頂点バッファ
ID3D11RenderTargetView* targetview;
ID3D11Resource*	g_TexResource;

ID3D11VertexShader* m_pVertexShader = nullptr;		// 頂点シェーダー入れ物
ID3D11PixelShader* m_pPixelShader = nullptr;		// ピクセルシェーダー入れ物
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

//現在の移動数
int nowcnt;

//線形補間の回数
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

	//２Dの初期化
	Init2D();
	//２Dテキスト初期化
	MyString::InitMyString();

	//カメラ初期位置の初期化
	DX11MtxIdentity(FirstCameraPos);

	// IMGUI初期化
	imguiInit();

	//SetTime = 6:00
	Application::GAME_TIME = 360;

	//ゲームボタン初期化
	GameButton::GetInstance().Init();

	//スプライトマネージャーの初期化
	Sprite2DMgr::GetInstance().Init();

	//ボタンの初期化
	//MenuButton = new Button(ButtonType::TOGGLE,XMFLOAT2(1260,20),XMFLOAT2(30,30), "assets/sprite/UI/MenuButton.png");

	fireSmoke.LoadTexTure("assets/ModelData/Smoke.png");
	fireSmoke.Init(0, 0, 200, 100, 100, XMFLOAT4(1, 1, 1, 1));
	Sprite2DMgr::GetInstance().CreateUI(UILIST::BLACKBACK_START, 1000, 530, 0, 11, 5.5, XMFLOAT4(1, 1, 1, 1));

	//MouseCursor生成
	Sprite2DMgr::GetInstance().CreateUI(UILIST::ICON_MOUSE, 1000, 530, 0, 0.2,0.2, XMFLOAT4(1, 1, 1, 1));

	// ライト初期化
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


	// プレイヤ初期化
	//g_player.Init();
	//g_player.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname));

	block.Init();

	//プレイヤー初期化
	g_player.Init();
	g_player.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname));


	//エネミー初期化
	g_enemy.Init();
	g_enemy.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::ENEMY)].modelname));

	//スカイドーム初期化
	g_skybox.Init();
	g_skybox.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::SKYBOX)].modelname));

	//煙エフェクトの初期化
	Fog.Init();
	Fog.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::FOG)].modelname));

	//海の初期化
	Sea.Init();
	Sea.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::SEA)].modelname));

	//ステージをロード
	Stage::GetInstance().RoadStageData();

	//モデルの全頂点を抜き出す

	//開始時間の設定
	starttime = timeGetTime();
}

void  SimulationExit() {

	// プレイヤ終了処理
	g_player.Finalize();

	g_enemy.Finalize();

	Sea.Finalize();

	MyString::ClearMyString();

	//スカイドーム終了処理
	g_skybox.Finalize();

	imguiExit();

}

void  SimulationUpdate() {

	//経過時間を設定(ミリ秒)
	endtime = timeGetTime();
	timer = (endtime - starttime);

	CDirectInput::GetInstance().GetMouseState();

	//スプライトマネージャーの更新
	Sprite2DMgr::GetInstance().Update();

	//ボタン更新
	GameButton::GetInstance().Update();

	//ボタンとマウスのあたり判定
	//if (MenuButton->CheckHit())
	//{
	//	//マウスがボタン上にある
	//	MenuButton->ChangeMouseOn();
	//	//ボタンがクリックされた
	//if(CDirectInput::GetInstance().GetMouseLButtonTrigger()){
	//	Sprite2DMgr::GetInstance().CreateWindowSprite(XMFLOAT2(400, 300), XMFLOAT2(600, 400),XMFLOAT4(1,1,1,1),true, "assets/sprite/UI/Window.png");
	//	MenuButton->ChangeClick();
	//}
	//}

	//ボタン更新
	//MenuButton->Update();

	// プレイヤ更新
	g_player.Update();

	//エネミー更新
	g_enemy.Update();

	Fog.Update(CCamera::GetInstance()->GetCameraMatrix());

	Sea.Update(CCamera::GetInstance()->GetCameraMatrix());

	//弾更新
	bullets.Update();

	//F5キー入力(トリガー)
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
	//スカイドーム更新
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
	//カメラタイプに応じて設定
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


	//３D描画
	TurnOnZbuffer();

	Stage::GetInstance().Draw();

	// プレイヤ描画
	g_player.Draw();

	g_skybox.Draw();

	//2D描画
	TurnOffZbuffer();

	Init2D();
	
	GameButton::GetInstance().Draw();

	Sprite2DMgr::GetInstance().Draw(CCamera::GetInstance()->GetCameraMatrix());

	//ボタン描画
	//MenuButton->Draw();

	//ウィンドウ座標
	//Screen座標を出力
	XMFLOAT4X4 PlayuerPos = g_player.GetMtx();
	XMMATRIX worldMatrix = XMMatrixTranslation(PlayuerPos._41, PlayuerPos._42, PlayuerPos._43);
	XMMATRIX view_mtx = XMLoadFloat4x4(&CCamera::GetInstance()->GetCameraMatrix());
	XMMATRIX projection_mtx = XMLoadFloat4x4(&CCamera::GetInstance()->GetProjectionMatrix());
	//スクリーン座標変換行列
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

	//表示テキストを設定
	char str[128];
	sprintf_s<128>(str, "PlayerScreenPos x:%f, y:%f",position._41, position._42);
	MyString::DrawString(10, 275, 20, 20, XMFLOAT4(1, 1, 1, 1), str);
	ImGuiIO& io = ImGui::GetIO();

	sprintf_s<128>(str, "MouseScreenPos x:%f, y:%f",io.MousePos.x, io.MousePos.y);
	MyString::DrawString(10, 295, 20, 20, XMFLOAT4(1, 1, 1, 1), str);


	//Score表示
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


	//２Dテキストの表示
	MyString::DrawMyString();

}