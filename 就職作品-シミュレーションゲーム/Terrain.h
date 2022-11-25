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

	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
};