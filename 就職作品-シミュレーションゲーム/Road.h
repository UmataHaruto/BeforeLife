#pragma once
#include "gameobject.h"
#include "CModel.h"
#include "ModelMgr.h"
#include "GameButton.h"
#include "dx11mathutil.h"
#include "player.h"
#include <vector>
#include "obb.h"

class Road : public GameObject
{
public:

	bool Init();
	void Init(XMFLOAT3 position,MODELID id);

	//描画
	void Draw();
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);

	//更新
	void Update();
	//終了処理
	void Finalize();

	//バウンディングスフィアを取得
	inline COBB GetOBB(void)
	{
		return m_obb;
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
		m_pos = pos;
		m_mtx._41 = m_pos.x;
		m_mtx._42 = m_pos.y;
		m_mtx._43 = m_pos.z;

	}

	//スピード倍率を取得
	inline float GetSpeed()
	{
		return m_speed;
	}
private:
	COBB m_obb;
	float m_speed;    //スピード倍率
	CModel* m_model;// モデルデータのポインタ
};

