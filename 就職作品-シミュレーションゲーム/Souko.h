#pragma once
#include "gameobject.h"
#include "GameButton.h"
#include <vector>
#include "Resource.h"
#include "obb.h"

class Souko :public GameObject
{
public:

	struct Item
	{
		ItemType tag;
		int num;
	};

	struct Data
	{
		GameButton::SoukoButtonType type;    //�^�C�v
		DirectX::XMFLOAT3 pos;               //���W
		std::vector<Item> items;      //�A�C�e�����X�g
		int store_max;                   //�ő���[��
	};

	bool Init();
	void Init(Data data, MODELID id);

	//�`��
	void Draw();
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);

	//�X�V
	void Update();
	//�I������
	void Finalize();

	//�o�E���f�B���Obox���擾
	inline COBB GetOBB(void)
	{
		return m_obb;
	}

	//�o�E���f�B���Obox���擾
	inline COBB GetEntranceOBB(void)
	{
		return m_obb_entrance;
	}

	//�f�[�^���擾
	inline Data GetData()
	{
		return m_data;
	}

	//���f���f�[�^���擾
	inline CModel* GetModel() {
		return m_model;
	}

	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	//���W���Z�b�g
	inline void SetPos(XMFLOAT3 pos)
	{
		m_pos = pos;
		m_mtx._41 = m_pos.x;
		m_mtx._42 = m_pos.y;
		m_mtx._43 = m_pos.z;

	}

	inline int GetItemNum(ItemType tag)
	{
		for (int i = 0; i < m_data.items.size(); i++)
		{
			if(m_data.items[i].tag == tag){
				return m_data.items[i].num;
			}
		}
		return 0;
	}

	//�A�C�e�����i�[(�߂�l:�i�[�ł��Ȃ�������)
	int PushItem(ItemType tag,int input);
private:
	COBB m_obb;
	COBB m_obb_entrance;
	Data m_data;
	CModel* m_model;// ���f���f�[�^�̃|�C���^
	int m_itemnum = 0;

};

