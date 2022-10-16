#pragma once

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "gameobject.h"

//�O���錾
class CModel;

//�e�X�e�[�^�X
enum class BULLETSTATUS
{
	LIVE,
	DEAD,
};

class Bullet:public GameObject
{
public:
	//������
	bool Init();

	//�`��
	void Draw();

	//�X�V
	void Update();

	//�폜
	void Finalize();

	//���f���̃A�h���X���Z�b�g
	void SetModel(CModel* p)
	{
		m_pmodel = p;
	}

	//���f����n��
	CModel* GetModel()
	{
		return m_pmodel;
	}

	void SetPos(DirectX::XMFLOAT3 pos)
	{
		m_pos = pos;
	}

	//�������Z�b�g
	void SetDirection(DirectX::XMFLOAT4X4 mtx)
	{
		m_mtx = mtx;
		m_direction = DirectX::XMFLOAT3(mtx._31,mtx._32,mtx._33);
	}
	//�e������������Ԃ�
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
	BULLETSTATUS					     m_sts;    //�X�e�[�^�X
	CModel*                              m_pmodel;  //�RD���f��
	DirectX::XMFLOAT3                    m_direction;  //�����x�N�g��
	float                                m_speed = 2.0f; //�X�s�[�h
	uint32_t                             m_life = 60; //HP
	std::vector<std::unique_ptr<Bullet>> g_bullets;
};