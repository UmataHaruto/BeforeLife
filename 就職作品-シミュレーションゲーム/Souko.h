#pragma once
#include "gameobject.h"
#include "GameButton.h"
#include <vector>
#include "Resource.h"
#include "obb.h"

class Souko :public GameObject
{
public:

	struct Item
	{
		ItemType tag;
		int num;
	};

	struct Data
	{
		GameButton::SoukoButtonType type;    //タイプ
		DirectX::XMFLOAT3 pos;               //座標
		std::vector<Item> items;      //住人リスト
		int store_max;                   //最大収納数
	};

	bool Init();
	void Init(Data data, MODELID id);

	//描画
	void Draw();

	//更新
	void Update();
	//終了処理
	void Finalize();

	//バウンディングboxを取得
	COBB GetOBB(void)
	{
		return m_obb;
	}

	//データを取得
	Data GetData()
	{
		return m_data;
	}

	//モデルデータを取得
	CModel* GetModel() {
		return m_model;
	}

	//モデルセット
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	//座標をセット
	void SetPos(XMFLOAT3 pos)
	{
		m_pos = pos;
		m_mtx._41 = m_pos.x;
		m_mtx._42 = m_pos.y;
		m_mtx._43 = m_pos.z;

	}

	//アイテムを格納(戻り値:格納できなかった数)
	int PushItem(ItemType tag,int input);
private:
	COBB m_obb;
	COBB m_obb_entrance;
	Data m_data;
	CModel* m_model;// モデルデータのポインタ
	int m_itemnum = 0;

};

