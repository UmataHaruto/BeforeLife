//*****************************************************************************
//!	@file	StageHit.cpp
//!	@brief	
//!	@note	ステージとの当たり判定
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
// グローバル変数
//-----------------------------------------------------------------------------
PLANEINFO*		g_PlaneInfo=nullptr;
int				g_NumFaces;

//==============================================================================
//!	@fn		StageHitInit
//!	@brief　地形との当たり判定初期化処理
//!	@param	メッシュ
//!	@param	デバイスオブジェクト
//!	@retval	なし
//==============================================================================
void StageHitInit(){
	std::vector<XMFLOAT3> vertices;		// 頂点データ
	std::vector<int> indices;			// インデックスデータ

	const ModelData& md = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::TERRAIN_COLLISION)].modelname)->GetModelData();
	//const ModelData& md = Stage::GetInstance().GetField(0,0,0).GetModel()->GetModelData();
	const std::vector<Mesh>& meshes = md.GetMeshes();

	// 頂点データを取り出す

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
	//				//初回のみ
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

		// 頂点データを取り出す
	for (auto m : meshes) {
		for (auto v : m.m_vertices) {
			v.m_Pos.x += 550;
			v.m_Pos.y += 5;
			v.m_Pos.z -= 550;
			vertices.emplace_back(v.m_Pos);
		}
	}

	// インデックスデータを取り出す
	for (auto m : meshes) {
		for (auto idx : m.m_indices) {
			indices.emplace_back(idx);
		}
	}

	// 平面の方程式を作成
 	CreateStagePlaneInfoAll(vertices, indices);
}

//==============================================================================
//!	@fn		StageHitExit
//!	@brief　地形との当たり判定終了処理
//!	@retval	なし
//==============================================================================
void StageHitExit(){
	if(g_PlaneInfo != nullptr){
		delete[] g_PlaneInfo;
	}
}

//==============================================================================
//!	@fn		CreateStagePlaneInfoAll
//!	@brief　指定されたメッシュのポリゴンについて当たり判定を行うための平面方程式情報を生成する
//!	@param	メッシュ
//!	@param	デバイスオブジェクト
//!	@retval	なし
//==============================================================================
void CreateStagePlaneInfoAll(std::vector<XMFLOAT3>& vertices, std::vector<int>& indices)
{
	g_NumFaces = indices.size() / 3;				// ３角形数計算

	g_PlaneInfo = new PLANEINFO[g_NumFaces];		// 面数分確保

	// 面数分ループ
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

		//平面の方程式を作成する
		CreatePlaneInfo(g_PlaneInfo[facecnt].p0, g_PlaneInfo[facecnt].p1, g_PlaneInfo[facecnt].p2, g_PlaneInfo[facecnt].plane);
	}
}

//==============================================================================
//!	@fn		SearchAllSurface
//!	@brief　指定されたメッシュすべての３角形ポリゴンについての交点を求める
//!	@param	Ｘ座標
//!	@param	Ｚ座標
//!	@param	交点
//!	@retval	true 交点あり false 交点なし
//==============================================================================
bool SearchAllSurface(float x, float z, XMFLOAT3& ans){

	XMFLOAT3		DownVector(0, 1, 0);				// 直線の方向ベクトル
	XMFLOAT3		CrossPoint;							// 交点
	XMFLOAT3		CurrentPosition(x, 0, z);			// 現在位置（高さ情報なし）

	bool FindFlag = false;								// ポリゴンが見つかったかどうか
	bool status;										

	// 全ポリゴン数分ループ
	for (int i = 0; i<g_NumFaces; i++){
		// 平面と直線の交点を求める
		status = LinetoPlaneCross(g_PlaneInfo[i].plane,	//平面の方程式
			CurrentPosition,							//直線の起点（球の現在位置）
			DownVector,									//直線の方向ベクトル
			CrossPoint);								//交点座標
		if (status){
			// 交点が３角形の内部にあるか？
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
//!	@brief　指定されたメッシュのすべての壁（３角形ポリゴン）についての交点を求める（最も近いもの）
//!	@param	Ｘ座標(現在位置)
//!	@param	Ｙ座標(現在位置)
//!	@param	Ｚ座標(現在位置)
//!	@param	境界球の半径　
//!	@param	移動方向
//!	@param	交点
//!	@retval	true 交点あり false 交点なし
//==============================================================================
bool SearchAllSurfaceWall(float x,float y ,float z,float radius,const XMFLOAT3 direction,XMFLOAT3& ans) {

	XMFLOAT3		Forward = direction;				// 移動方向ベクトル
	XMFLOAT3		IntersectionPoint;					// 交点
	XMFLOAT3		CurrentPosition(x, y, z);			// 現在位置

	float			r2 = radius * radius;				// 半径の２乗

	bool FindFlag = false;								// ポリゴンが見つかったかどうか
	bool status;

	// 全ポリゴン数分ループ
	for (int i = 0; i < g_NumFaces; i++) {

		// 床か？（床なら次へ）
		if (g_PlaneInfo[i].type == PLANETYPE::FLOOR) {
			continue;
		}

		// 壁と直線の交点を求める
		status = LinetoPlaneCross(g_PlaneInfo[i].plane,	//平面の方程式
			CurrentPosition,							//直線の起点（球の現在位置）
			Forward,									//直線の方向ベクトル
			IntersectionPoint);							//交点座標
		if (status) {
			// 交点が３角形の内部にあるか？
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
//!	@brief　指定されたメッシュのすべての壁（３角形ポリゴン）についての交点を求める（最もカメラに近いもの）
//!	@param	Ｘ座標(現在位置)
//!	@param	Ｙ座標(現在位置)
//!	@param	Ｚ座標(現在位置)
//!	@param	境界球の半径　
//!	@param	移動方向
//!	@param	交点
//!	@retval	true 交点あり false 交点なし
//==============================================================================
bool SearchAllSurfaceNearestCamera(XMFLOAT3 camerapos, const XMFLOAT3 direction, XMFLOAT3& ans) {

	XMFLOAT3		Forward = direction;					// 移動方向ベクトル
	XMFLOAT3		IntersectionPoint;						// 交点
	XMFLOAT3		CurrentPosition(camerapos.x, camerapos.y, camerapos.z);	// カメラ位置

	std::vector<XMFLOAT3> IntersectionPointList;

	bool FindFlag = false;								// ポリゴンが見つかったかどうか
	bool status;

	// 全ポリゴン数分ループ
	for (int i = 0; i < g_NumFaces; i++) {

		// 地形と直線の交点を求める
		status = LinetoPlaneCross(g_PlaneInfo[i].plane,	//平面の方程式
			CurrentPosition,							//直線の起点（カメラ位置）
			Forward,									//直線の方向ベクトル
			IntersectionPoint);							//交点座標
		if (status) {
			// 交点が３角形の内部にあるか？
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