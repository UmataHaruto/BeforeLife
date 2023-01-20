#pragma once
#include "gameobject.h"
#include "CModel.h"
#include "ModelMgr.h"
#include "GameButton.h"
#include "dx11mathutil.h"
#include <vector>
#include "obb.h"
#include "NameGenerator.h"
#include "player.h"

class Player;

class House : public GameObject
{

public:
	//Playerへの参照
	struct Data
	{
		GameButton::HouseButtonType type;    //タイプ
		DirectX::XMFLOAT3 pos;               //座標
		std::vector<Player*> residents;      //住人リスト
		int residents_max;                   //最大収容人数
	};

	bool Init();
	void Init(Data data,MODELID id);

	//描画
	void Draw();
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);

	//更新
	void Update();
	//終了処理
	void Finalize();

	bool SetResident(Player* player)
	{
		//削除されたプレイヤーを参照していないか
		if (player == nullptr)
		{
			return false;
		}
		//同じ住人が登録されていないか
		bool overlap = false;
		for (int i = 0; i < m_residents.size(); i++)
		{
			if (m_residents[i] == player)
			{
				overlap = true;
			}
			//同一苗字以外が入居していないか
			if (m_residents[i]->GetName(NameGenerator::NAMETYPE::FAMILLY) != player->GetName(NameGenerator::NAMETYPE::FAMILLY))
			{
				overlap = true;
			}
		}
		if (!overlap)
		{
			m_residents.push_back(player);
			player->SetHouse(this);
			return true;
		}
		else
		{
			return false;
		}
	}

	//バウンディングスフィアを取得
	COBB GetOBB(void)
	{
		return m_obb;
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
private:
	COBB m_obb;
	COBB m_obb_entrance;
	Data m_data;
	CModel* m_model;// モデルデータのポインタ
	int Resident_max;
	std::vector<Player*> m_residents;
};

