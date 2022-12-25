#pragma once
#include "gameobject.h"
#include "CModel.h"
#include "ModelMgr.h"
#include "GameButton.h"
#include "dx11mathutil.h"
#include "player.h"
#include <vector>
#include "obb.h"

class Road : public GameObject
{
public:

	bool Init();
	void Init(XMFLOAT3 position,MODELID id);

	//�`��
	void Draw();
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);

	//�X�V
	void Update();
	//�I������
	void Finalize();

	//�o�E���f�B���O�X�t�B�A���擾
	inline COBB GetOBB(void)
	{
		return m_obb;
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
		m_pos = pos;
		m_mtx._41 = m_pos.x;
		m_mtx._42 = m_pos.y;
		m_mtx._43 = m_pos.z;

	}

	//�X�s�[�h�{�����擾
	inline float GetSpeed()
	{
		return m_speed;
	}
private:
	COBB m_obb;
	float m_speed;    //�X�s�[�h�{��
	CModel* m_model;// ���f���f�[�^�̃|�C���^
};

