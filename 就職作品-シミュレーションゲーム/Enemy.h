#pragma once
#include <random>
#include "gameobject.h"
#include "CModel.h"
#include "dx11mathutil.h"
#include "CDirectInput.h"

class Enemy:public GameObject
{
private:
	CModel *m_model;

	//���
	enum class STATUS {
		S1_MOVE_FLY,
		S1_MOVE_BOMB,
	};

	enum class ACTION {
		ACT_LEFT = 0,
		ACT_RIGTH,
		ACT_UP,
		ACT_DOWN,
		ACT_ROLLP,
		ACT_ROLLM,
		ACT_STRAIGHT,

		MAX_ACTION
	};

	float m_speed = 0.01;    //�X�s�[�h
	XMFLOAT3 m_angle;    //��]�p

	std::mt19937 m_mt;    //���������p
	std::mt19937 m_mt2;   

	ACTION m_action;    //�A�N�V�������
	unsigned int m_actionKeepTime;    //�A�N�V�����J�E���^

public:

	//�R���X�g���N�^
	Enemy();

	//�f�X�g���N�^
	~Enemy();

    //������
	bool Init();

	//�`��
	void Draw();

	//���W�����ݕ`��
	void DrawWithAxis();

	//�X�V
	void Update();

	//�폜
	void Finalize();

	//�������c
	void AutoPilot();

	CModel* GetModel() {
		return m_model;
	}

	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
};