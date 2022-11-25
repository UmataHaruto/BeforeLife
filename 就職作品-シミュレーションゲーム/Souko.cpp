#include "Souko.h"

bool Souko::Init()
{
	return false;
}

void Souko::Init(Data data, MODELID id)
{
	DX11MtxIdentity(m_mtx);
	CModel* pmodel = new CModel();
	*pmodel = *ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(id)].modelname);

	m_model = pmodel;
	m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);

	m_pos = data.pos;

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

	//アイテム数格納コンテナ
	for (int i = 0; i < (int)ItemType::ITEM_MAX; i++)
	{
		Item item;
		item.tag = (ItemType)i;
		item.num = 0;
		data.items.push_back(item);
	}

	m_obb.Init(m_model);
	m_obb_entrance.CreateBox(35, 60, 20, XMFLOAT3(-5, 0, -60));

	m_data = data;
}

void Souko::Draw()
{
	// モデル描画
	m_model->Draw(m_mtx);

	//境界box表示
	if (!GameButton::GetInstance().GetDebug()) {
		m_obb.Draw();
		m_obb_entrance.Draw();
	}
}

void Souko::Update()
{
	//現在の格納量
	int num = 0;

	for (int i = 0; i < m_data.items.size(); i++)
	{
		num += m_data.items[i].num;
	}

	m_itemnum = num;

	//位置をGameObjectに反映
	m_pos = m_data.pos;
	m_mtx._41 = m_pos.x;
	m_mtx._42 = m_pos.y;
	m_mtx._43 = m_pos.z;

	m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);

	//境界球の更新
	m_obb.Update(GetMtx());

	m_obb_entrance.Update(GetMtx());
}

void Souko::Finalize()
{
	m_model->Uninit();
}

int Souko::PushItem(ItemType tag, int input)
{

	for (int i = 0; i < m_data.items.size(); i++)
	{
		if (m_data.items[i].tag == tag)
		{
			int residue = 0;

			//格納数をオーバーする場合
			if (m_itemnum + input > m_data.store_max)
			{
				int inputnum = m_data.store_max - m_itemnum;
				m_data.items[i].num += inputnum;
				residue = m_itemnum + input - m_data.store_max;
				return residue;
			}
			else {
				m_data.items[i].num += input;
			}
		}
		//異常終了:不正なタグ
		else
		{
			return -1;
		}
	}

	return 0;
}
