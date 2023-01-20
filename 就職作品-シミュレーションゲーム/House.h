#pragma once
#include "gameobject.h"
#include "CModel.h"
#include "ModelMgr.h"
#include "GameButton.h"
#include "dx11mathutil.h"
#include <vector>
#include "obb.h"
#include "NameGenerator.h"
#include "player.h"

class Player;

class House : public GameObject
{

public:
	//Player�ւ̎Q��
	struct Data
	{
		GameButton::HouseButtonType type;    //�^�C�v
		DirectX::XMFLOAT3 pos;               //���W
		std::vector<Player*> residents;      //�Z�l���X�g
		int residents_max;                   //�ő���e�l��
	};

	bool Init();
	void Init(Data data,MODELID id);

	//�`��
	void Draw();
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);

	//�X�V
	void Update();
	//�I������
	void Finalize();

	bool SetResident(Player* player)
	{
		//�폜���ꂽ�v���C���[���Q�Ƃ��Ă��Ȃ���
		if (player == nullptr)
		{
			return false;
		}
		//�����Z�l���o�^����Ă��Ȃ���
		bool overlap = false;
		for (int i = 0; i < m_residents.size(); i++)
		{
			if (m_residents[i] == player)
			{
				overlap = true;
			}
			//����c���ȊO���������Ă��Ȃ���
			if (m_residents[i]->GetName(NameGenerator::NAMETYPE::FAMILLY) != player->GetName(NameGenerator::NAMETYPE::FAMILLY))
			{
				overlap = true;
			}
		}
		if (!overlap)
		{
			m_residents.push_back(player);
			player->SetHouse(this);
			return true;
		}
		else
		{
			return false;
		}
	}

	//�o�E���f�B���O�X�t�B�A���擾
	COBB GetOBB(void)
	{
		return m_obb;
	}

	//���f���f�[�^���擾
	CModel* GetModel() {
		return m_model;
	}

	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	//���W���Z�b�g
	void SetPos(XMFLOAT3 pos)
	{
		m_pos = pos;
		m_mtx._41 = m_pos.x;
		m_mtx._42 = m_pos.y;
		m_mtx._43 = m_pos.z;

	}
private:
	COBB m_obb;
	COBB m_obb_entrance;
	Data m_data;
	CModel* m_model;// ���f���f�[�^�̃|�C���^
	int Resident_max;
	std::vector<Player*> m_residents;
};

