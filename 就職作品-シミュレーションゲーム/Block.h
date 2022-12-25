#pragma once
#include	<DirectXMath.h>
#include "gameobject.h"
#include "CModel.h"

//�O���b�h�\���p���W
struct Vector3
{
	float x;
	float y;
	float z;
};

//�u���b�N�̎��
enum class BLOCKTYPE
{
	EMPTY,
	GRASS,
	STONE,
	_MAX
};

class Block:public GameObject
{
public:
	Block();
	bool Init();
	bool Init(Vector3 pos,BLOCKTYPE id);
	//�`�揈��
	void Draw();
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);

	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
	//���f���f�[�^���擾
	CModel* GetModel() {
		return m_model;
	}
	void SetPos(DirectX::XMFLOAT3 pos);

	BLOCKTYPE GetBlockId()
	{
		return m_id;
	}
	//�I������
	void Finalize();
private:
	CModel* m_model;// �R�c���f���̃|�C���^
	BLOCKTYPE m_id;
	Vector3 m_Gridpos;
};
