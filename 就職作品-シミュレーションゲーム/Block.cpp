#include "Block.h"
#include "ModelMgr.h"
#include "dx11mathutil.h"

Block::Block()
{
    // s—ñ‰Šú‰»
    DX11MtxIdentity(m_mtx);

    m_model = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::GRASS)].modelname);
}

bool Block::Init()
{
    // s—ñ‰Šú‰»
    DX11MtxIdentity(m_mtx);


    m_model = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::GRASS)].modelname);
    return true;
}

bool Block::Init(Vector3 pos, BLOCKTYPE id)
{
    m_Gridpos = pos;
    m_pos.x = pos.x;
    m_pos.y = pos.y;
    m_pos.z = pos.z;

    m_mtx._41 = m_pos.x;
    m_mtx._42 = m_pos.y;
    m_mtx._43 = m_pos.z;

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

    // ƒ‚ƒfƒ‹•`‰æ
    m_model->Draw(m_mtx);
}

void Block::DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in)
{
    m_model->DrawShadow(m_mtx, layout_in, vs_in, ps_in);
}

void Block::Finalize()
{
}