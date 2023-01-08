#pragma once
#include "gameobject.h"
#include "CModel.h"


class Sea :public GameObject
{
private:
	CModel* m_sea_model;
	CModel* m_sea_under_model;

public:
	bool Init();

	void Draw();
	void Finalize();

	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_sea_model = pmodel;
	}
};