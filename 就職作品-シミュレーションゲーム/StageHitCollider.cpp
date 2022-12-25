//*****************************************************************************
//!	@file	StageHit.cpp
//!	@brief	
//!	@note	�X�e�[�W�Ƃ̓����蔻��
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<DirectXMath.h>
#include	<vector>
#include	<algorithm>
#include	"StageHitCollider.h"
#include	"collision.h"
#include	"DatFileLoader.h"
#include    "Stage.h"
#include    "ModelMgr.h"

using namespace DirectX;

//-----------------------------------------------------------------------------
// �O���[�o���ϐ�
//-----------------------------------------------------------------------------
PLANEINFO*		g_PlaneInfo=nullptr;
int				g_NumFaces;

//==============================================================================
//!	@fn		StageHitInit
//!	@brief�@�n�`�Ƃ̓����蔻�菉��������
//!	@param	���b�V��
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//==============================================================================
void StageHitInit(){
	std::vector<XMFLOAT3> vertices;		// ���_�f�[�^
	std::vector<int> indices;			// �C���f�b�N�X�f�[�^

	const ModelData& md = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::TERRAIN_COLLISION)].modelname)->GetModelData();
	//const ModelData& md = Stage::GetInstance().GetField(0,0,0).GetModel()->GetModelData();
	const std::vector<Mesh>& meshes = md.GetMeshes();

	// ���_�f�[�^�����o��

	//int index = 0;
	//for (int z = 0; z < MAP_DEPTH; z++) {
	//	for (int y = 0; y < MAP_HEIGHT; y++) {
	//		for (int x = 0; x < MAP_WIDTH; x++) {
	//			for (auto m : meshes) {
	//				for (auto v : m.m_vertices) {

	//					XMFLOAT3 stagepos = Stage::GetInstance().GetField(x, y, z).GetPos();

	//					v.m_Pos.x += stagepos.x;
	//					v.m_Pos.y += stagepos.y;
	//					v.m_Pos.z += stagepos.z;

	//					vertices.emplace_back(v.m_Pos);
	//				}
	//				//����̂�
	//				if (z == 0 && y == 0 && x == 0) {
	//					for (auto idx : m.m_indices) {
	//						indices.emplace_back(idx);
	//					}
	//				}
	//				else
	//				{
	//					for (int i = 0; i < m.m_indices.size(); i++)
	//					{
	//						indices.emplace_back(indices[i] + ((index) * m.m_vertices.size()));
	//					}
	//				}
	//				index++;
	//			}
	//		}
	//	}
	//}

		// ���_�f�[�^�����o��
	for (auto m : meshes) {
		for (auto v : m.m_vertices) {
			v.m_Pos.x += 550;
			v.m_Pos.y += 5;
			v.m_Pos.z -= 550;
			vertices.emplace_back(v.m_Pos);
		}
	}

	// �C���f�b�N�X�f�[�^�����o��
	for (auto m : meshes) {
		for (auto idx : m.m_indices) {
			indices.emplace_back(idx);
		}
	}

	// ���ʂ̕��������쐬
 	CreateStagePlaneInfoAll(vertices, indices);
}

//==============================================================================
//!	@fn		StageHitExit
//!	@brief�@�n�`�Ƃ̓����蔻��I������
//!	@retval	�Ȃ�
//==============================================================================
void StageHitExit(){
	if(g_PlaneInfo != nullptr){
		delete[] g_PlaneInfo;
	}
}

//==============================================================================
//!	@fn		CreateStagePlaneInfoAll
//!	@brief�@�w�肳�ꂽ���b�V���̃|���S���ɂ��ē����蔻����s�����߂̕��ʕ��������𐶐�����
//!	@param	���b�V��
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//==============================================================================
void CreateStagePlaneInfoAll(std::vector<XMFLOAT3>& vertices, std::vector<int>& indices)
{
	g_NumFaces = indices.size() / 3;				// �R�p�`���v�Z

	g_PlaneInfo = new PLANEINFO[g_NumFaces];		// �ʐ����m��

	// �ʐ������[�v
	for (int facecnt = 0; facecnt < g_NumFaces; facecnt++) {

		g_PlaneInfo[facecnt].p0.x = vertices[indices[facecnt * 3]].x;
		g_PlaneInfo[facecnt].p0.y = vertices[indices[facecnt * 3]].y;
		g_PlaneInfo[facecnt].p0.z = vertices[indices[facecnt * 3]].z;

		g_PlaneInfo[facecnt].p1.x = vertices[indices[facecnt * 3 + 1]].x;
		g_PlaneInfo[facecnt].p1.y = vertices[indices[facecnt * 3 + 1]].y;
		g_PlaneInfo[facecnt].p1.z = vertices[indices[facecnt * 3 + 1]].z;

		g_PlaneInfo[facecnt].p2.x = vertices[indices[facecnt * 3 + 2]].x;
		g_PlaneInfo[facecnt].p2.y = vertices[indices[facecnt * 3 + 2]].y;
		g_PlaneInfo[facecnt].p2.z = vertices[indices[facecnt * 3 + 2]].z;

		//���ʂ̕��������쐬����
		CreatePlaneInfo(g_PlaneInfo[facecnt].p0, g_PlaneInfo[facecnt].p1, g_PlaneInfo[facecnt].p2, g_PlaneInfo[facecnt].plane);
	}
}

//==============================================================================
//!	@fn		SearchAllSurface
//!	@brief�@�w�肳�ꂽ���b�V�����ׂĂ̂R�p�`�|���S���ɂ��Ă̌�_�����߂�
//!	@param	�w���W
//!	@param	�y���W
//!	@param	��_
//!	@retval	true ��_���� false ��_�Ȃ�
//==============================================================================
bool SearchAllSurface(float x, float z, XMFLOAT3& ans){

	XMFLOAT3		DownVector(0, 1, 0);				// �����̕����x�N�g��
	XMFLOAT3		CrossPoint;							// ��_
	XMFLOAT3		CurrentPosition(x, 0, z);			// ���݈ʒu�i�������Ȃ��j

	bool FindFlag = false;								// �|���S���������������ǂ���
	bool status;										

	// �S�|���S���������[�v
	for (int i = 0; i<g_NumFaces; i++){
		// ���ʂƒ����̌�_�����߂�
		status = LinetoPlaneCross(g_PlaneInfo[i].plane,	//���ʂ̕�����
			CurrentPosition,							//�����̋N�_�i���̌��݈ʒu�j
			DownVector,									//�����̕����x�N�g��
			CrossPoint);								//��_���W
		if (status){
			// ��_���R�p�`�̓����ɂ��邩�H
			if (CheckInTriangle(g_PlaneInfo[i].p0, g_PlaneInfo[i].p1, g_PlaneInfo[i].p2, CrossPoint)){
				CurrentPosition.y = CrossPoint.y;
				FindFlag = true;
				break;
			}
		}
	}
	if (FindFlag){
		ans = CurrentPosition;
		return true;
	}
	else{
		return false;
	}
}

//==============================================================================
//!	@fn		SearchAllSurfaceWall
//!	@brief�@�w�肳�ꂽ���b�V���̂��ׂĂ̕ǁi�R�p�`�|���S���j�ɂ��Ă̌�_�����߂�i�ł��߂����́j
//!	@param	�w���W(���݈ʒu)
//!	@param	�x���W(���݈ʒu)
//!	@param	�y���W(���݈ʒu)
//!	@param	���E���̔��a�@
//!	@param	�ړ�����
//!	@param	��_
//!	@retval	true ��_���� false ��_�Ȃ�
//==============================================================================
bool SearchAllSurfaceWall(float x,float y ,float z,float radius,const XMFLOAT3 direction,XMFLOAT3& ans) {

	XMFLOAT3		Forward = direction;				// �ړ������x�N�g��
	XMFLOAT3		IntersectionPoint;					// ��_
	XMFLOAT3		CurrentPosition(x, y, z);			// ���݈ʒu

	float			r2 = radius * radius;				// ���a�̂Q��

	bool FindFlag = false;								// �|���S���������������ǂ���
	bool status;

	// �S�|���S���������[�v
	for (int i = 0; i < g_NumFaces; i++) {

		// �����H�i���Ȃ玟�ցj
		if (g_PlaneInfo[i].type == PLANETYPE::FLOOR) {
			continue;
		}

		// �ǂƒ����̌�_�����߂�
		status = LinetoPlaneCross(g_PlaneInfo[i].plane,	//���ʂ̕�����
			CurrentPosition,							//�����̋N�_�i���̌��݈ʒu�j
			Forward,									//�����̕����x�N�g��
			IntersectionPoint);							//��_���W
		if (status) {
			// ��_���R�p�`�̓����ɂ��邩�H
			if (CheckInTriangle(g_PlaneInfo[i].p0, g_PlaneInfo[i].p1, g_PlaneInfo[i].p2, IntersectionPoint)) {
				XMFLOAT3 v = { 
					IntersectionPoint.x - CurrentPosition.x,
					IntersectionPoint.y - CurrentPosition.y,
					IntersectionPoint.z - CurrentPosition.z
				};
				float lng2 = v.x*v.x + v.y*v.y + v.z*v.z;
				if ((lng2 - r2) <= 0.0f) {
					FindFlag = true;
					break;
				}
			}
		}
	}
	if (FindFlag) {
		ans = IntersectionPoint;
		return true;
	}
	else {
		return false;
	}
}

//==============================================================================
//!	@fn		SearchAllSurfaceNearestCamera
//!	@brief�@�w�肳�ꂽ���b�V���̂��ׂĂ̕ǁi�R�p�`�|���S���j�ɂ��Ă̌�_�����߂�i�ł��J�����ɋ߂����́j
//!	@param	�w���W(���݈ʒu)
//!	@param	�x���W(���݈ʒu)
//!	@param	�y���W(���݈ʒu)
//!	@param	���E���̔��a�@
//!	@param	�ړ�����
//!	@param	��_
//!	@retval	true ��_���� false ��_�Ȃ�
//==============================================================================
bool SearchAllSurfaceNearestCamera(XMFLOAT3 camerapos, const XMFLOAT3 direction, XMFLOAT3& ans) {

	XMFLOAT3		Forward = direction;					// �ړ������x�N�g��
	XMFLOAT3		IntersectionPoint;						// ��_
	XMFLOAT3		CurrentPosition(camerapos.x, camerapos.y, camerapos.z);	// �J�����ʒu

	std::vector<XMFLOAT3> IntersectionPointList;

	bool FindFlag = false;								// �|���S���������������ǂ���
	bool status;

	// �S�|���S���������[�v
	for (int i = 0; i < g_NumFaces; i++) {

		// �n�`�ƒ����̌�_�����߂�
		status = LinetoPlaneCross(g_PlaneInfo[i].plane,	//���ʂ̕�����
			CurrentPosition,							//�����̋N�_�i�J�����ʒu�j
			Forward,									//�����̕����x�N�g��
			IntersectionPoint);							//��_���W
		if (status) {
			// ��_���R�p�`�̓����ɂ��邩�H
			if (CheckInTriangle(g_PlaneInfo[i].p0, g_PlaneInfo[i].p1, g_PlaneInfo[i].p2, IntersectionPoint)) {
				FindFlag = true;
				IntersectionPointList.emplace_back(IntersectionPoint);
				break;
			}
		}
	}

	if (FindFlag) {

		sort(IntersectionPointList.begin(), IntersectionPointList.end(), [=](auto const& lhs, auto const& rhs) {
			XMFLOAT3 temp;
			temp.x = lhs.x - camerapos.x;
			temp.y = lhs.y - camerapos.y;
			temp.z = lhs.x - camerapos.z;
			float length_l;
			length_l = temp.x*temp.x + temp.y*temp.y + temp.z*temp.z;

			temp.x = rhs.x - camerapos.x;
			temp.y = rhs.y - camerapos.y;
			temp.z = rhs.x - camerapos.z;
			float length_r;
			length_r = temp.x*temp.x + temp.y*temp.y + temp.z*temp.z;
			if (length_l <= length_r) {
				return true;
			}
			else {
				return false;
			}
		});

		ans = IntersectionPointList[0];

		return true;
	}
	else {
		return false;
	}
}

//******************************************************************************
//	End of file.
//******************************************************************************