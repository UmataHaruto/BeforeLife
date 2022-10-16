#pragma once
#include	"gameobject.h"
#include	"CModel.h"
#include	"dx11mathutil.h"
#include	"CDirectInput.h"

class Effect :public GameObject {
private:
	CModel*	m_model;// �R�c���f��
public:
	//������
	bool Init();

	//�`��
	void Draw();

	void DrawWithAxis();

	//�X�V
	void Update(XMFLOAT4X4 camera_mtx);

	//�I������
	void Finalize();

	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
};