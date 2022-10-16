#include "skydome.h"
#include "dx11mathutil.h"

bool Skydome::Init()
{
    //行列初期化
    DX11MtxIdentity(m_mtx);

    return true;
}

void Skydome::Draw()
{
	m_model->Draw(m_mtx);
}

void Skydome::Update(XMFLOAT3 pPos)
{
	//座標をプレイヤーと同期する

	m_mtx._41 = pPos.x;
	m_mtx._42 = pPos.y - 100;
	m_mtx._43 = pPos.z;



}

void Skydome::Finalize()
{
	m_model->Uninit();
}
