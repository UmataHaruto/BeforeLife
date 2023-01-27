#include "House.h"
#include "player.h"

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

	//Z‹ƒ^ƒCƒv‚É‰ž‚¶‚½‰ŠúÝ’è
	switch (data.type)
	{
	case GameButton::HouseButtonType::HOUSE_SMALL:
		Resident_max = 3;
		break;
	default:
		break;
	}

	m_obb.Init(m_model);
	m_obb_entrance.CreateBox(20, 60, 20, XMFLOAT3(-10,0,-75));

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

void House::DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in)
{
	// ƒ‚ƒfƒ‹•`‰æ
	m_model->DrawShadow(m_mtx, layout_in, vs_in, ps_in);
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
