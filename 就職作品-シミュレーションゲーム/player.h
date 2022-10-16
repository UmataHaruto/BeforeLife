#pragma once
#include	"gameobject.h"
#include	"CModel.h"
#include	"dx11mathutil.h"
#include	"CDirectInput.h"
#include    "ModelMgr.h"

enum class CameraMode
{
	TPS,    //三人称視点
	FPS,    //一人称視点
};

class Player :public GameObject {
private:
	CModel*	m_model;// ３Ｄモデル
	CameraMode camera_mode;//カメラモード 0 = TPS,1 = FPS
	int animno = 0;
public:

	//初期化
	bool Init();

	//描画
	void Draw();

	void DrawWithAxis();

	//更新
	void Update();

	//終了処理
	void Finalize();

	//カメラモードを変更
	void SetCameraMode(CameraMode mode);

	//カメラモードを取得
	CameraMode GetCameraMode();

	//モデルデータを取得
	const CModel* GetModel() {
		return m_model;
	}

	//モデルセット
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
};