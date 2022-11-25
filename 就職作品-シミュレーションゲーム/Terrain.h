#pragma once
#include "gameobject.h"
#include "CModel.h"


class Terrain :public GameObject
{
private:
	CModel* m_model;

public:
	bool Init();

	void Draw();
	void Finalize();

	//モデルセット
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
};