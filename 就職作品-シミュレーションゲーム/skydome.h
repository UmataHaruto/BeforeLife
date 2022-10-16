#pragma once
#include "gameobject.h"
#include "CModel.h"
#include "player.h"

class Skydome :public GameObject
{
private:
	CModel* m_model;

public:
	bool Init();
	
	void Draw();

	void Update(XMFLOAT3 pPos);

	void Finalize();

	//モデルセット
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
};