#include "Block.h"
#include "ModelMgr.h"
#include "dx11mathutil.h"

Block::Block()
{
    // 行列初期化
    DX11MtxIdentity(m_mtx);

    m_model = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::GRASS)].modelname);
}

bool Block::Init()
{
    // 行列初期化
    DX11MtxIdentity(m_mtx);


    m_model = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::GRASS)].modelname);
    return true;
}

bool Block::Init(Vector3 pos, BLOCKTYPE id)
{
    m_Gridpos = pos;
    m_id = id;
    return false;
}

void Block::SetPos(DirectX::XMFLOAT3 pos)
{
    m_pos = pos;
    m_mtx._41 = m_pos.x;
    m_mtx._42 = m_pos.y;
    m_mtx._43 = m_pos.z;

}

void Block::Draw() {

    // モデル描画
    m_model->Draw(m_mtx);
}

void Block::Finalize()
{
}