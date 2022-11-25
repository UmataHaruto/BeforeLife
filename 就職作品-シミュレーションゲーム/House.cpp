#include "House.h"

bool House::Init()
{
	return false;
}

void House::Init(Data data, MODELID id)
{
	DX11MtxIdentity(m_mtx);
	CModel* pmodel = new CModel();
	*pmodel = *ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(id)].modelname);

	m_model =pmodel;
	m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);

	m_pos = data.pos;

	m_obb.Init(m_model);
	m_obb_entrance.CreateBox(20, 60, 20, XMFLOAT3(-10,0,-55));

	m_data = data;
}

void House::Draw()
{
	// ƒ‚ƒfƒ‹•`‰æ
	m_model->Draw(m_mtx);

	//‹«ŠEbox•\Ž¦
	if (!GameButton::GetInstance().GetDebug()) {
		m_obb.Draw();
		m_obb_entrance.Draw();
	}
}

void House::Update()
{
	//ˆÊ’u‚ðGameObject‚É”½‰f
	m_pos = m_data.pos;
	m_mtx._41 = m_pos.x;
	m_mtx._42 = m_pos.y;
	m_mtx._43 = m_pos.z;

	m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);

	//‹«ŠE‹…‚ÌXV
	m_obb.Update(GetMtx());

	m_obb_entrance.Update(GetMtx());
}

void House::Finalize()
{
	m_model->Uninit();
}
