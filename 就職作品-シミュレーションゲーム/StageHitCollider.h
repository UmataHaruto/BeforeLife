#pragma once
#include	<DirectXMath.h>
#include	"collision.h"
#include	"DatFileLoader.h"
#include    "CModel.h"
#include    <vector>

// �n�`�f�[�^�̖ʂ̕��������
enum class PLANETYPE {
	WALL,					// ��
	FLOOR					// ��
};

struct PLANEINFO{
	PLANE		plane;					// ���ʂ̕�����
	DirectX::XMFLOAT3	p0, p1, p2;		// ���_���W
	PLANETYPE type = PLANETYPE::WALL;	// ��� 
};

void StageHitInit();
void StageHitExit();
void CreateStagePlaneInfoAll(std::vector<XMFLOAT3>& vertices, std::vector<int>& indices);
bool SearchAllSurface(float x, float z, DirectX::XMFLOAT3& ans);
bool SearchAllSurfaceWall(float x, float y, float z, float radius, const XMFLOAT3 direction, XMFLOAT3& ans);
bool SearchAllSurfaceNearestCamera(XMFLOAT3 pos, const XMFLOAT3 direction, XMFLOAT3& ans);
