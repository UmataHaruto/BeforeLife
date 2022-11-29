#pragma once
#include	<DirectXMath.h>
#include	"CModel.h"
#include	"Box.h"

class COBB
{
public:
	// OBB�p�\����
	struct OBBinfo {
		DirectX::XMFLOAT3	vecAxisX;		// X��
		DirectX::XMFLOAT3	vecAxisY;		// Y��
		DirectX::XMFLOAT3	vecAxisZ;		// Z��
		DirectX::XMFLOAT3	center;			// ���S���W�i���[�J�����W�n�j
		DirectX::XMFLOAT3	currentcenter;	// BBOX�̌��݈ʒu
		float	fLengthX;		// �a�a�n�w �w�T�C�Y	
		float	fLengthY;		// �a�a�n�w �x�T�C�Y
		float	fLengthZ;		// �a�a�n�w �y�T�C�Y
	};

	//�T�C�Y���擾
	XMFLOAT3 GetBoxSize()
	{
		XMFLOAT3 size;

		size.x = m_obbinfo.fLengthX;
		size.y = m_obbinfo.fLengthY;
		size.z = m_obbinfo.fLengthZ;

		return size;
	}

private:
	OBBinfo				m_obbinfo;	// OBB BBOX���
	DirectX::XMFLOAT4X4	m_matworld;	// OBB �`��p���[���h�ϊ��s��
	XMFLOAT3 m_interval;            // OBB���S�_����̋���(�����l0:���S)
	Box			m_boxmesh;			// BBOX mesh

	//bool CompareLength(OBB* pboxA,OBB* pboxB,D3DXVECTOR3* pvecSeparate,D3DXVECTOR3* pvecDistance)
	//�u�����v���u�Q�̉e�̍��v�v���傫���ꍇ�i��Փˁj��FALSE  ���������������ꍇ�i�Փˁj��TRUE
	// ���̊֐��́AOBBCollisionDetection�֐��݂̂ɌĂяo�����T�u���[�`��
	bool CompareLength(
		const OBBinfo& pboxA,
		const OBBinfo& pboxB,
		const DirectX::XMFLOAT3& pvecSeparate,		// ������
		const DirectX::XMFLOAT3& pvecDistance);		// �Q�̃I�u�W�F�N�g�����ԃx�N�g��

	void CreateBoxMesh() {
		m_boxmesh.Init(
			m_obbinfo.fLengthX,
			m_obbinfo.fLengthY,
			m_obbinfo.fLengthZ
		);
	}

	void CalculateBoundingBox(std::vector<XMFLOAT3>& vertices);

public:

	COBB() {
		DX11MtxIdentity(m_matworld);	// �P�ʍs��
	}

	~COBB() {
		Exit();
	}

	void Init(CModel* pmodel) {

		std::vector<XMFLOAT3> vertices;		// ���_�f�[�^
		std::vector<int> indices;			// �C���f�b�N�X�f�[�^

		const ModelData& md =	pmodel->GetModelData();
		const std::vector<Mesh>& meshes = md.GetMeshes();

		// ���_�f�[�^�����o��
		for (auto& m : meshes) {
			for (auto& v : m.m_vertices) {
				vertices.emplace_back(v.m_Pos);
			}
		}

		//�C���^�[�o������
		m_interval = { 0,0,0 };

		CalculateBoundingBox(vertices);	// AABB�쐬
		CreateBoxMesh();				// ���b�V���쐬
	}

	//�傫�����w�肵�č쐬
	void CreateBox(float width, float height, float depth,XMFLOAT3 interval)
	{
		m_interval = interval;
		// �������Z�b�g
		m_obbinfo.fLengthX = width;
		m_obbinfo.fLengthY = height;
		m_obbinfo.fLengthZ = depth;

		// 3�����Z�b�g
		m_obbinfo.vecAxisX = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_obbinfo.vecAxisY = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_obbinfo.vecAxisZ = XMFLOAT3(0.0f, 0.0f, 1.0f);

		m_boxmesh.Init(width,height,depth);
	}


	void Exit() {
		m_boxmesh.Exit();
	}

	void Update(DirectX::XMFLOAT4X4 matworld);

	void Draw();

	bool Collision(COBB& obb);

	void SetBoxColor(bool sts)
	{
		m_boxmesh.m_ishit = sts;
	}

	DirectX::XMFLOAT3 GetPosition() {
		return m_obbinfo.currentcenter;
	}

	OBBinfo GetOBB() {
		return m_obbinfo;
	}
};