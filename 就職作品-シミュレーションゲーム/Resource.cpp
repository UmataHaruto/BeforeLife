#include "Resource.h"
#include "ModelMgr.h"
#include "GameButton.h"

bool Resource::Init()
{
	return false;
}

void Resource::Init(Data input, MODELID model)
{
	DX11MtxIdentity(m_mtx);
	CModel* pmodel = new CModel();
	*pmodel = *ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(model)].modelname);
	m_model = pmodel;
	//モデルの全頂点を抜き出す
	const ModelData& md = m_model->GetModelData();
	const std::vector<Mesh>& meshes = md.GetMeshes();
	const XMFLOAT3 AmbientColor = { 1,1,1 };

	std::vector<XMFLOAT3> vertices;

	for (auto& m : meshes) {
		for (auto& v : m.m_vertices) {
			vertices.emplace_back(v.m_Pos);
		}
	}

	// 境界球初期化
	m_obb.Init(m_model);

	data = input;
}

void Resource::Draw()
{
	// モデル描画
	m_model->Draw(m_mtx);

	//境界box表示
	if (!GameButton::GetInstance().GetDebug()) {
		m_obb.Draw();
	}
}

void Resource::DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in)
{
	m_model->DrawShadow(m_mtx,layout_in,vs_in,ps_in);
}

void Resource::Update()
{
	//位置をGameObjectに反映
	m_pos = data.pos;
	m_mtx._41 = m_pos.x;
	m_mtx._42 = m_pos.y;
	m_mtx._43 = m_pos.z;

	if (data.type ==ItemType::WOOD) {
		m_model->Update(0, m_mtx);
	}

	//境界球の更新
	m_obb.Update(m_mtx);
}

void Resource::Finalize()
{
	m_model->Uninit();
}
//ダメージ値 = 基礎ダメージ - 硬度
void Resource::HitDamage(int damage)    
{
	if (data.Endurance - (damage - data.Hardness) < 0)
	{
		data.Endurance = 0;
	}
	else
	{
		data.Endurance -= damage - data.Hardness;
	}
}


