//========================================================
//CBBoxGeneraterクラス
//		ワンスキンモデル用のBBOXの生成を行うためのクラス
//========================================================
#pragma once

class CBBoxGenerater
{
public:
	static const int N = 4;
	static const int MAX = 100;
	static const double TOL;
public:
	CBBoxGenerater(void);
	~CBBoxGenerater(void);

	// 共分散行列を作成する
	XMFLOAT4X4 GetCovarianceMatrix(std::vector<XMFLOAT3>& vertices);

	XMFLOAT4X4 GetCovarianceMatrix(
		XMFLOAT3* vertex,			// 頂点データ配列 
		unsigned int* index,		// ボーンに影響を受ける頂点インデックスの配列
		int numVertices);			// ボーンに影響を受ける頂点数

	bool GetEigenVector(XMFLOAT4X4& a, XMFLOAT4X4& x);						// 固有ベクトルを求める

	OBB CaclBBox(XMFLOAT3 *vertex, unsigned int *index, int numVertices, XMFLOAT4X4 eigenmat);
	OBB CaclBBox(std::vector<XMFLOAT3>& vertices, XMFLOAT4X4 eigenmat);
};
