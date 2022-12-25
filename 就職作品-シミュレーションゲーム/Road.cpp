#include "Road.h"

bool Road::Init()
{
	return false;
}

void Road::Init(XMFLOAT3 position,MODELID id)
{
	DX11MtxIdentity(m_mtx);
	m_model = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(id)].modelname);

	m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);

	m_pos = position;

	m_obb.Init(m_model);
}

void Road::Draw()
{
	// ���f���`��
	m_model->Draw(m_mtx);

	//���Ebox�\��
	if (!GameButton::GetInstance().GetDebug()) {
		m_obb.Draw();
	}
}

void Road::DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in)
{
	m_model->DrawShadow(m_mtx, layout_in, vs_in, ps_in);
}

void Road::Update()
{
	//�ʒu��GameObject�ɔ��f
	m_mtx._41 = m_pos.x;
	m_mtx._42 = m_pos.y;
	m_mtx._43 = m_pos.z;

	m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);

	//���E���̍X�V
	m_obb.Update(GetMtx());
}

void Road::Finalize()
{
	m_model->Uninit();
}
