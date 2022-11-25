#pragma once
#include	"gameobject.h"
#include	"CModel.h"
#include	"dx11mathutil.h"
#include	"CDirectInput.h"
#include    "CBillBoard.h"
#include    "quad2d.h"
#include    "Window.h"
#include    "Resource.h"

#include	<wrl/client.h>
#include	<unordered_map>

class Sprite2DMgr{

	struct EFFECT_PATH
	{
		EFFECTLIST id;

		const char* texturepath;
		bool isAnimation;
		bool isLoop;
		bool isHorming;
		uint16_t cutU;
		uint16_t cutV;
		int AnimationDelay;
	};

	struct UI_PATH
	{
		UILIST id;

		const char* texturepath;
		bool isAnimation;
		bool isLoop;
		uint16_t cutU;
		uint16_t cutV;
		int AnimationDelay;
	};

	struct ProgressData
	{
		CBillBoard m_bar;
		CBillBoard m_targetimage;

		Resource::Data* m_data;
	};
private:
	Sprite2DMgr() {
	}
	//エフェクトテンプレート格納ベクター
	std::vector<EFFECT_PATH>g_EffectTemplate = {
	{EFFECTLIST::TARGETCIRCLE,"assets/sprite/TargetCircle.png",true,false,false,6,2,1},
	{EFFECTLIST::PROGRESSBAR_CIRCLE,"assets/sprite/Progressbar_Circle.png",true,false,false,10,6,1},
	{EFFECTLIST::WOOD,"assets/sprite/UI/Resource/Wood.png",false,false,false,1,1,1},
	{EFFECTLIST::ORE_COAL,"assets/sprite/UI/Resource/Coal.png",false,false,false,1,1,1},
	{EFFECTLIST::ORE_IRON,"assets/sprite/UI/Resource/ironore.png",false,false,false,1,1,1},
	{EFFECTLIST::ORE_GOLD,"assets/sprite/UI/Resource/Goldore.png",false,false,false,1,1,1},
	{EFFECTLIST::IRON,"assets/sprite/UI/Resource/Iron.png",false,false,false,1,1,1},
	{EFFECTLIST::GOLD,"assets/sprite/UI/Resource/Gold.png",false,false,false,1,1,1},
	};

	//UIテンプレート格納ベクター
	std::vector<UI_PATH>g_UITemplate = {
		
		{UILIST::BLACKBACK_START ,"assets/sprite/UI/BlackBack.png",false,false,1,1,1},
		{UILIST::BLACKBACK_END ,"assets/sprite/UI/BlackBack.png",false,false,1,1,1},
		{UILIST::ICON_MOUSE ,"assets/sprite/UI/MouseIcon.png",false,false,1,1,1},

	};


	//モデルデータ格納
	std::vector<std::shared_ptr<CBillBoard>>g_modelhash;

	//UIデータ格納
	std::vector<std::shared_ptr<Quad2D>>g_uihash;
public:
	//エフェクト格納ベクター
	std::vector<CBillBoard>g_effects = {};
	std::vector<Quad2D>g_UserInterfaces = {};
	std::vector<Window*>g_Windows = {};
	//プログレスバーデータ格納
	std::vector<ProgressData>g_bars = {};


	static Sprite2DMgr& GetInstance() {
		static Sprite2DMgr Instance;
		return Instance;
	}



	//初期化
	bool Init();

	//描画
	void Draw(XMFLOAT4X4 camera_mtx);

	//更新
	void Update();

	//削除
	void ClearEffect();
	void ClearUi();
	void ClearWindow();

	//終了処理
	void Finalize();

	//UIを作成
	void CreateUI(UILIST type, float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color);

	//ウィンドウを作成
	void CreateWindowSprite(DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 Size, DirectX::XMFLOAT4 color, bool CancelKey, const char* TexturePath);

	//ターゲットをリセット
	void ResetTarget();

	XMFLOAT3* GetTarget();

	//敵のミニマップアイコン作成
	void CreateEnemyMinimap(float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color,XMFLOAT3* EnemyPos);

	//UIアニメーションを作成
	void CreateAnimation(UILIST type, float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color);

	//プログレスバー生成
	void CreateCircleProgressBar(Resource::Data* resource,float xsize,float ysize);
	//エフェクトを作成
	void CreateEffect(EFFECTLIST type,float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color);

	//アニメーションエフェクトを作成
	void CreateAnimation(EFFECTLIST type,float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color,XMFLOAT3* ptarget);

	//ターゲット範囲内かを判定
	bool TargetSelect(XMFLOAT3* EnemyPos);

	//ターゲットが最大まで達しているか
	bool CheckFullTarget();

	//プレイヤーをミニマップに表示
	void MiniMapPlayer(XMFLOAT3* PlayerPos,XMFLOAT4X4* PlayerMtx);

	//敵をミニマップに表示
	void MiniMapEnemy();

	//レティクルを表示更新
	void ReticleUpdate(XMFLOAT3* PlayerPos, XMFLOAT4X4* PlayerMtx);

	//モデルのキーを取得
	std::shared_ptr<CBillBoard> GetModelPtr(EFFECTLIST type) {
		// 存在するかを確かめる
		return GetInstance().g_modelhash[(int)type];
	}

	//モデルのキーを取得
	std::shared_ptr<Quad2D> GetUIPtr(UILIST type) {
		// 存在するかを確かめる
		return GetInstance().g_uihash[(int)type];
	}
};