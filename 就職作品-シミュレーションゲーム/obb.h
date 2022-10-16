#pragma once
#include	<DirectXMath.h>
#include	<string>
#include	"CBox.h"

class DatFileLoader;

// OBB用構造体
struct OBB {
	DirectX::XMFLOAT3	vecAxisX;		// X軸
	DirectX::XMFLOAT3	vecAxisY;		// Y軸
	DirectX::XMFLOAT3	vecAxisZ;		// Z軸
	DirectX::XMFLOAT3	center;			// 中心座標（ローカル座標系）
	DirectX::XMFLOAT3	currentcenter;	// BBOXの現在位置
	float	fLengthX;					// ＢＢＯＸ Ｘサイズ	
	float	fLengthY;					// ＢＢＯＸ Ｙサイズ
	float	fLengthZ;					// ＢＢＯＸ Ｚサイズ
};

class COBB
{
private:
	std::string		m_name;				// 名前（ボーン名をセット）
	OBB				m_obbinfo;			// OBB BBOX情報
	DirectX::XMFLOAT4X4	m_matworld;		// OBB 描画用ワールド変換行列
	CBox			m_boxmesh;			// BBOX mesh

	//bool CompareLength(OBB* pboxA,OBB* pboxB,D3DXVECTOR3* pvecSeparate,D3DXVECTOR3* pvecDistance)
	//「距離」が「２つの影の合計」より大きい場合（非衝突）はFALSE  等しいか小さい場合（衝突）はTRUE
	// この関数は、OBBCollisionDetection関数のみに呼び出されるサブルーチン
	bool CompareLength(
		const OBB& pboxA,
		const OBB& pboxB,
		const DirectX::XMFLOAT3& pvecSeparate,		// 分離軸
		const DirectX::XMFLOAT3& pvecDistance);		// ２つのオブジェクトを結ぶベクトル

	void CalculateBoundingBox(DatFileLoader* datdata);		// AABBを求める

public:
	COBB();

	~COBB();

	void Init(DatFileLoader* datdata) {
		CalculateBoundingBox(datdata);	// OBB作成
		CreateBoxMesh();				// メッシュ作成
	}

	void Exit() {
		m_boxmesh.Exit();
	}

	void Update(DirectX::XMFLOAT4X4 matworld);

	void Draw();

	bool Collision(COBB& obb);

	DirectX::XMFLOAT3 GetPosition() {
		return m_obbinfo.currentcenter;
	}

	OBB GetOBB() {
		return m_obbinfo;
	}

	void CalculateOBB(std::vector<XMFLOAT3>& vertex);		// 頂点群から固有ベクトルを使用してＯＢＢを作る

	void CreateBoxMesh() {
		m_boxmesh.Init(
			m_obbinfo.fLengthX,
			m_obbinfo.fLengthY,
			m_obbinfo.fLengthZ
		);
	}

	void SetName(std::string name) {
		m_name = name;
	}

	std::string& GetName() {
		return m_name;
	}

	void SetColor(DirectX::XMFLOAT4 col) {
		m_boxmesh.SetColor(col);
	}

	XMFLOAT4 GetColor() {
		return m_boxmesh.GetColor();
	}
};