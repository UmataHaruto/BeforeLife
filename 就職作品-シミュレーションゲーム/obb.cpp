#include	"obb.h"
#include	"DX11Settransform.h"

using namespace DirectX;

bool COBB::CompareLength(
	const OBBinfo& boxA,
	const OBBinfo& boxB,
	const XMFLOAT3& vecSeparate,		// 分離軸
	const XMFLOAT3& vecDistance)		// 中心を結ぶベクトル
{
	float fDistance;

	// 分離軸に射影した中心間を結ぶベクトル
	DX11Vec3Dot(fDistance, vecDistance, vecSeparate);	// 内積を計算
	fDistance = fabsf(fDistance);		// 絶対値

	//分離軸上にボックスAを射影した影の長さ
	float fShadowA = 0;

	//分離軸上にボックスＢを射影した影の長さ
	float fShadowB = 0;

	// この部分を完成させる（練習問題）

	//ボックスAの”影”を算出
	float fShadowAx;
	float fShadowAy;
	float fShadowAz;

	// X軸を分離軸に射影
	DX11Vec3Dot(fShadowAx, vecSeparate, boxA.vecAxisX);
	fShadowAx = fabsf(fShadowAx * (boxA.fLengthX / 2));

	// Y軸を分離軸に射影
	DX11Vec3Dot(fShadowAy, vecSeparate, boxA.vecAxisY);
	fShadowAy = fabsf(fShadowAy * (boxA.fLengthY / 2));

	// Z軸を分離軸に射影
	DX11Vec3Dot(fShadowAz, vecSeparate, boxA.vecAxisZ);
	fShadowAz = fabsf(fShadowAz * (boxA.fLengthZ / 2));

	fShadowA = fShadowAx + fShadowAy + fShadowAz;

	//ボックスBの”影”を算出
	float fShadowBx;
	float fShadowBy;
	float fShadowBz;

	// X軸を分離軸に射影
	DX11Vec3Dot(fShadowBx, vecSeparate, boxB.vecAxisX);
	fShadowBx = fabsf(fShadowBx * (boxB.fLengthX / 2));

	// Y軸を分離軸に射影
	DX11Vec3Dot(fShadowBy, vecSeparate, boxB.vecAxisY);
	fShadowBy = fabsf(fShadowBy * (boxB.fLengthY / 2));

	// Z軸を分離軸に射影
	DX11Vec3Dot(fShadowBz, vecSeparate, boxB.vecAxisZ);
	fShadowBz = fabsf(fShadowBz * (boxB.fLengthZ / 2));

	fShadowB = fShadowBx + fShadowBy + fShadowBz;

	if (fDistance > fShadowA + fShadowB) {
		return false;
	}

	return true;
}

void COBB::CalculateBoundingBox(std::vector<XMFLOAT3>& vertices) {

	int				numvertices;			// 頂点数

	float* x;						// 頂点座標データ
	float* y;						// 頂点座標データ
	float* z;						// 頂点座標データ

	float			minx, miny, minz;		// 最小値（Ｘ，Ｙ，Ｚ）
	float			maxx, maxy, maxz;		// 最大値（Ｘ，Ｙ、Ｚ）
	float			cx, cy, cz;				// 中心座標


	numvertices = vertices.size();			// 頂点数をゲット

											// 頂点数分の座標格納エリア確保
	x = new float[numvertices];
	y = new float[numvertices];
	z = new float[numvertices];

	// 頂点をすべて取り出す
	for (int i = 0; i < numvertices; i++)
	{
		x[i] = vertices[i].x;	// Ｘ座標取得
		y[i] = vertices[i].y;	// Ｙ座標取得
		z[i] = vertices[i].z;	// Ｚ座標取得
	}

	// ＸＹＺそれぞれについて、最大値、最小値を求める
	for (int i = 0; i < numvertices; i++)
	{
		if (i == 0) {
			minx = maxx = x[i];
			miny = maxy = y[i];
			minz = maxz = z[i];
		}
		if (minx >= x[i]) minx = x[i];
		if (miny >= y[i]) miny = y[i];
		if (minz >= z[i]) minz = z[i];
		if (maxx <= x[i]) maxx = x[i];
		if (maxy <= y[i]) maxy = y[i];
		if (maxz <= z[i]) maxz = z[i];
	}

	cx = (maxx + minx) / 2;		// 球の中心座標を求める
	cy = (maxy + miny) / 2;		// 球の中心座標を求める
	cz = (maxz + minz) / 2;		// 球の中心座標を求める

	float distance;
	float maxdistance = 0;

	// 全頂点と中心座標との距離を求める
	for (int i = 0; i < numvertices; i++)
	{
		// 2点間の距離を求める
		distance = (x[i] - cx) * (x[i] - cx) + (y[i] - cy) * (y[i] - cy) + (z[i] - cz) * (z[i] - cz);
		if (maxdistance < distance) maxdistance = distance;
	}
	// 最大半径を計算
	maxdistance = sqrtf(maxdistance);
	delete[] z;
	delete[] y;
	delete[] x;

	m_obbinfo.currentcenter.x = cx;				// ＢＢＯＸの中心をセット
	m_obbinfo.currentcenter.y = cy;
	m_obbinfo.currentcenter.z = cz;

	m_obbinfo.center.x = cx;					// ＢＢＯＸの中心をセット
	m_obbinfo.center.y = cy;
	m_obbinfo.center.z = cz;

	// 長さをセット
	m_obbinfo.fLengthX = fabsf(maxx - minx);
	m_obbinfo.fLengthY = fabsf(maxy - miny);
	m_obbinfo.fLengthZ = fabsf(maxz - minz);

	// 3軸をセット
	m_obbinfo.vecAxisX = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_obbinfo.vecAxisY = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_obbinfo.vecAxisZ = XMFLOAT3(0.0f, 0.0f, 1.0f);
}

void COBB::Update(DirectX::XMFLOAT4X4 matworld) {
	// 軸をセット
	m_obbinfo.vecAxisX = XMFLOAT3(matworld._11, matworld._12, matworld._13);
	m_obbinfo.vecAxisY = XMFLOAT3(matworld._21, matworld._22, matworld._23);
	m_obbinfo.vecAxisZ = XMFLOAT3(matworld._31, matworld._32, matworld._33);

	// 描画用の行列を更新
	m_matworld = matworld;

	// OBBの中心座標を更新
	DX11Vec3MulMatrix(m_obbinfo.currentcenter, m_obbinfo.center, matworld);

	// 現在位置を補正（BBOXの中心とモデルの原点位置は異なるため）
	m_matworld._41 = m_obbinfo.currentcenter.x;
	m_matworld._42 = m_obbinfo.currentcenter.y;
	m_matworld._43 = m_obbinfo.currentcenter.z;

	//初期間隔を適応
	m_matworld._41 += m_interval.x;
	m_matworld._42 += m_interval.y;
	m_matworld._43 += m_interval.z;
}

void COBB::Draw() {
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, m_matworld);

	m_boxmesh.Draw();

}

bool COBB::Collision(COBB& obb) {
	//分離軸
	XMFLOAT3 vecSeparate;

	// 2つのオブジェクトを結んだベクトルを計算
	XMFLOAT3 vecDistance;
	XMFLOAT3 obbpos = obb.GetPosition();
	vecDistance.x = m_obbinfo.currentcenter.x - obbpos.x;
	vecDistance.y = m_obbinfo.currentcenter.y - obbpos.y;
	vecDistance.z = m_obbinfo.currentcenter.z - obbpos.z;

	OBBinfo boxB, boxA;
	boxA = this->GetOBB();
	boxB = obb.GetOBB();

	bool sts;			// 戻り値

	// OBB-A軸リスト
	XMFLOAT3* OBB_A_Axis[3] = {
			&boxA.vecAxisX,
			&boxA.vecAxisY,
			&boxA.vecAxisZ,
	};

	// OBB-B軸リスト
	XMFLOAT3* OBB_B_Axis[3] = {
		&boxB.vecAxisX,
		&boxB.vecAxisY,
		&boxB.vecAxisZ,
	};

	// OBB-Aの３軸を分離軸にしてチェック
	for (int i = 0; i < 3; i++) {
		vecSeparate.x = OBB_A_Axis[i]->x;
		vecSeparate.y = OBB_A_Axis[i]->y;
		vecSeparate.z = OBB_A_Axis[i]->z;

		DX11Vec3Normalize(vecSeparate, vecSeparate);	// 正規化

		sts = CompareLength(boxA,			// OBB-A
			boxB,							// OBB-B
			vecSeparate,					// 分離軸
			vecDistance);					// ２つのオブジェクトを結ぶベクトル
		if (sts == false) {
			return false;
		}
	}

	// OBB-Bの３軸を分離軸にしてチェック
	for (int i = 0; i < 3; i++) {
		vecSeparate.x = OBB_B_Axis[i]->x;
		vecSeparate.y = OBB_B_Axis[i]->y;
		vecSeparate.z = OBB_B_Axis[i]->z;

		DX11Vec3Normalize(vecSeparate, vecSeparate);	// 正規化

		sts = CompareLength(boxA,			// OBB-A
			boxB,							// OBB-B
			vecSeparate,					// 分離軸
			vecDistance);					// ２つのオブジェクトを結ぶベクトル
		if (sts == false) {
			return false;
		}
	}

	// 外積を分離軸として計算
	for (int p1 = 0; p1 < 3; p1++) {
		for (int p2 = 0; p2 < 3; p2++) {
			XMFLOAT3	crossseparate;					// 外積ベクトル

			// 外積を計算する
			DX11Vec3Cross(crossseparate, *OBB_A_Axis[p1], *OBB_B_Axis[p2]);
			DX11Vec3Normalize(crossseparate, crossseparate);	// 正規化

			sts = CompareLength(
				boxA,				// OBB-A
				boxB,				// OBB-B
				crossseparate,		// 分離軸
				vecDistance);		// ２つのオブジェクトを結ぶベクトル
			if (sts == false) {
				return false;
			}
		}
	}

	return true;
}