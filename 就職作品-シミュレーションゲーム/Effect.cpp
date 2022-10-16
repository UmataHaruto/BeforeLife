#include "Effect.h"

#include	"drawaxis.h"
#include    "DX11util.h"
#include    "dx11mathutil.h"

bool Effect::Init() {
	// �s�񏉊���
	DX11MtxIdentity(m_mtx);

	m_mtx._43 += 150;
	m_mtx._42 -= 10;

	return true;
}

void Effect::Draw() {
	// ���f���`��
	m_model->Draw(m_mtx);
}

void Effect::DrawWithAxis() {

	// ���f���`��
	drawaxis(m_mtx, 200, m_pos);
	Draw();

}

void Effect::Update(XMFLOAT4X4 camera_mtx) {

	XMFLOAT3 angle = { 0,0,0 };
	bool keyinput = false;

	XMFLOAT4 axisX;  //X��
	XMFLOAT4 axisY;  //Y��
	XMFLOAT4 axisZ;  //Z��

	//X�������o��
	axisX.x = m_mtx._11;
	axisX.y = m_mtx._12;
	axisX.z = m_mtx._13;
	axisX.w = 0.0f;

	//Y�������o��
	axisY.x = m_mtx._21;
	axisY.y = m_mtx._22;
	axisY.z = m_mtx._23;
	axisX.w = 0.0f;

	//Z�������o��
	axisZ.x = m_mtx._31;
	axisZ.y = m_mtx._32;
	axisZ.z = m_mtx._33;
	axisX.w = 0.0f;
}

void Effect::Finalize() {
	m_model->Uninit();
}
