#pragma once
#include	<DirectXMath.h>
#include	"collision.h"
#include	"DatFileLoader.h"
#include    "CModel.h"
#include    <vector>

// 地形データの面の方程式情報
enum class PLANETYPE {
	WALL,					// 壁
	FLOOR					// 床
};

struct PLANEINFO{
	PLANE		plane;					// 平面の方程式
	DirectX::XMFLOAT3	p0, p1, p2;		// 頂点座標
	PLANETYPE type = PLANETYPE::WALL;	// 種類 
};

void StageHitInit();
void StageHitExit();
void CreateStagePlaneInfoAll(std::vector<XMFLOAT3>& vertices, std::vector<int>& indices);
bool SearchAllSurface(float x, float z, DirectX::XMFLOAT3& ans);
bool SearchAllSurfaceWall(float x, float y, float z, float radius, const XMFLOAT3 direction, XMFLOAT3& ans);
bool SearchAllSurfaceNearestCamera(XMFLOAT3 pos, const XMFLOAT3 direction, XMFLOAT3& ans);
