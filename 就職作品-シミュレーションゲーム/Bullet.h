#pragma once

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "gameobject.h"

//前方宣言
class CModel;

//弾ステータス
enum class BULLETSTATUS
{
	LIVE,
	DEAD,
};

class Bullet:public GameObject
{
public:
	//初期化
	bool Init();

	//描画
	void Draw();

	//更新
	void Update();

	//削除
	void Finalize();

	//モデルのアドレスをセット
	void SetModel(CModel* p)
	{
		m_pmodel = p;
	}

	//モデルを渡す
	CModel* GetModel()
	{
		return m_pmodel;
	}

	void SetPos(DirectX::XMFLOAT3 pos)
	{
		m_pos = pos;
	}

	//方向をセット
	void SetDirection(DirectX::XMFLOAT4X4 mtx)
	{
		m_mtx = mtx;
		m_direction = DirectX::XMFLOAT3(mtx._31,mtx._32,mtx._33);
	}
	//弾が生存中かを返す
	bool isLive()
	{
		if (m_sts == BULLETSTATUS::LIVE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void FireBullet(DirectX::XMFLOAT4X4 mtx, DirectX::XMFLOAT3 ppos);
	
private:
	BULLETSTATUS					     m_sts;    //ステータス
	CModel*                              m_pmodel;  //３Dモデル
	DirectX::XMFLOAT3                    m_direction;  //向きベクトル
	float                                m_speed = 2.0f; //スピード
	uint32_t                             m_life = 60; //HP
	std::vector<std::unique_ptr<Bullet>> g_bullets;
};