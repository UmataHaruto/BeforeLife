#pragma once
#include	<DirectXMath.h>
#include "gameobject.h"
#include "CModel.h"

//グリッド表現用座標
struct Vector3
{
	float x;
	float y;
	float z;
};

//ブロックの種類
enum class BLOCKTYPE
{
	EMPTY,
	GRASS,
	STONE,
	_MAX
};

class Block:public GameObject
{
public:
	Block();
	bool Init();
	bool Init(Vector3 pos,BLOCKTYPE id);
	//描画処理
	void Draw();
	//モデルセット
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	void SetPos(DirectX::XMFLOAT3 pos);

	BLOCKTYPE GetBlockId()
	{
		return m_id;
	}
	//終了処理
	void Finalize();
private:
	CModel* m_model;// ３Ｄモデルのポインタ
	BLOCKTYPE m_id;
	Vector3 m_Gridpos;
};
