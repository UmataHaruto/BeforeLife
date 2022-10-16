#include "Effect.h"

#include	"drawaxis.h"
#include    "DX11util.h"
#include    "dx11mathutil.h"

bool Effect::Init() {
	// 行列初期化
	DX11MtxIdentity(m_mtx);

	m_mtx._43 += 150;
	m_mtx._42 -= 10;

	return true;
}

void Effect::Draw() {
	// モデル描画
	m_model->Draw(m_mtx);
}

void Effect::DrawWithAxis() {

	// モデル描画
	drawaxis(m_mtx, 200, m_pos);
	Draw();

}

void Effect::Update(XMFLOAT4X4 camera_mtx) {

	XMFLOAT3 angle = { 0,0,0 };
	bool keyinput = false;

	XMFLOAT4 axisX;  //X軸
	XMFLOAT4 axisY;  //Y軸
	XMFLOAT4 axisZ;  //Z軸

	//X軸を取り出す
	axisX.x = m_mtx._11;
	axisX.y = m_mtx._12;
	axisX.z = m_mtx._13;
	axisX.w = 0.0f;

	//Y軸を取り出す
	axisY.x = m_mtx._21;
	axisY.y = m_mtx._22;
	axisY.z = m_mtx._23;
	axisX.w = 0.0f;

	//Z軸を取り出す
	axisZ.x = m_mtx._31;
	axisZ.y = m_mtx._32;
	axisZ.z = m_mtx._33;
	axisX.w = 0.0f;
}

void Effect::Finalize() {
	m_model->Uninit();
}
