#pragma once
#include "gameobject.h"
#include "CDirectxGraphics.h"
#include "CModel.h"
#include "ModelMgr.h"
#include "obb.h"
enum class ItemType
{
	WOOD,
	ORE_COAL,
	ORE_IRON,
	ORE_GOLD,
	IRON,
	GOLD,
	HERB,
	ITEM_MAX,
	ITEM_NONE = -1,

};

class Resource :public GameObject 
{
public:

	struct Data
	{
		ItemType type;
		DirectX::XMFLOAT3 pos;
		int EnduranceMax;         //初期耐久値
		int Endurance;            //耐久値
		int Hardness;             //硬度
		int amount;               //採集物の量
		
		bool isInstallation;      //設置状態
	};

	bool Init();
	void Init(Data input, MODELID model);

	//描画
	void Draw();

	//更新
	void Update();
	//終了処理
	void Finalize();

	//モデルに応じた当たり判定に更新
	void InitColision()
	{
		// 境界球初期化
		m_obb.Init(m_model);
	}

	//バウンディングスフィアを取得
	inline COBB* GetOBB(void)
	{
		return &m_obb;
	}

	inline Data* GetData(void)
	{
		return &data;
	}

	//モデルデータを取得
	inline CModel* GetModel() {
		return m_model;
	}

	//モデルセット
	inline void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	//座標をセット
	inline void SetPos(XMFLOAT3 pos)
	{
		data.pos = pos;
		m_pos = pos;
		m_mtx._41 = m_pos.x;
		m_mtx._42 = m_pos.y;
		m_mtx._43 = m_pos.z;

	}

	//鉱石に与えられるダメージ(基礎値)
	void HitDamage(int damage);
private:
	Data data;
	COBB m_obb;
	CModel* m_model;// モデルデータのポインタ


};

