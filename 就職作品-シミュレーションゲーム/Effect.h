#pragma once
#include	"gameobject.h"
#include	"CModel.h"
#include	"dx11mathutil.h"
#include	"CDirectInput.h"

class Effect :public GameObject {
private:
	CModel*	m_model;// ３Ｄモデル
public:
	//初期化
	bool Init();

	//描画
	void Draw();

	void DrawWithAxis();

	//更新
	void Update(XMFLOAT4X4 camera_mtx);

	//終了処理
	void Finalize();

	//モデルセット
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
};