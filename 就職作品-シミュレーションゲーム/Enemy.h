#pragma once
#include <random>
#include "gameobject.h"
#include "CModel.h"
#include "dx11mathutil.h"
#include "CDirectInput.h"

class Enemy:public GameObject
{
private:
	CModel *m_model;

	//状態
	enum class STATUS {
		S1_MOVE_FLY,
		S1_MOVE_BOMB,
	};

	enum class ACTION {
		ACT_LEFT = 0,
		ACT_RIGTH,
		ACT_UP,
		ACT_DOWN,
		ACT_ROLLP,
		ACT_ROLLM,
		ACT_STRAIGHT,

		MAX_ACTION
	};

	float m_speed = 0.01;    //スピード
	XMFLOAT3 m_angle;    //回転角

	std::mt19937 m_mt;    //乱数発生用
	std::mt19937 m_mt2;   

	ACTION m_action;    //アクション種別
	unsigned int m_actionKeepTime;    //アクションカウンタ

public:

	//コンストラクタ
	Enemy();

	//デストラクタ
	~Enemy();

    //初期化
	bool Init();

	//描画
	void Draw();

	//座標軸込み描画
	void DrawWithAxis();

	//更新
	void Update();

	//削除
	void Finalize();

	//自動操縦
	void AutoPilot();

	CModel* GetModel() {
		return m_model;
	}

	//モデルセット
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
};