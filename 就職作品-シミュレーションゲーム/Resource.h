#pragma once
#include "gameobject.h"
#include "CDirectxGraphics.h"
#include "CModel.h"
#include "ModelMgr.h"
#include "obb.h"
enum class ItemType
{
	WOOD,
	ORE_COAL,
	ORE_IRON,
	ORE_GOLD,
	IRON,
	GOLD,
	HERB,
	ITEM_MAX,
	ITEM_NONE = -1,

};

class Resource :public GameObject 
{
public:

	struct Data
	{
		ItemType type;
		DirectX::XMFLOAT3 pos;
		int EnduranceMax;         //�����ϋv�l
		int Endurance;            //�ϋv�l
		int Hardness;             //�d�x
		int amount;               //�̏W���̗�
		
		bool isInstallation;      //�ݒu���
	};

	bool Init();
	void Init(Data input, MODELID model);

	//�`��
	void Draw();

	//�X�V
	void Update();
	//�I������
	void Finalize();

	//���f���ɉ����������蔻��ɍX�V
	void InitColision()
	{
		// ���E��������
		m_obb.Init(m_model);
	}

	//�o�E���f�B���O�X�t�B�A���擾
	inline COBB* GetOBB(void)
	{
		return &m_obb;
	}

	inline Data* GetData(void)
	{
		return &data;
	}

	//���f���f�[�^���擾
	inline CModel* GetModel() {
		return m_model;
	}

	//���f���Z�b�g
	inline void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	//���W���Z�b�g
	inline void SetPos(XMFLOAT3 pos)
	{
		data.pos = pos;
		m_pos = pos;
		m_mtx._41 = m_pos.x;
		m_mtx._42 = m_pos.y;
		m_mtx._43 = m_pos.z;

	}

	//�z�΂ɗ^������_���[�W(��b�l)
	void HitDamage(int damage);
private:
	Data data;
	COBB m_obb;
	CModel* m_model;// ���f���f�[�^�̃|�C���^


};

