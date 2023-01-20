#include "ResourceManager.h"
#include "dx11mathutil.h"
#include "CCamera.h"
#include "BuildingMgr.h"
void ResourceManager::Init()
{
	//インスタンシング描画初期化
	{
		CModel instancing;

		instancing.InitiInstancing(
			1000,
			"assets/Modeldata/tree/conifer00/conifer01.fbx",
			"shader/vsinstance.fx",
			"shader/ps.fx",
			"assets/Modeldata/tree/conifer00/"
		);
		m_resources_Instance.push_back(instancing);
	}
	{
		CModel instancing;

		instancing.InitiInstancing(
			1000,
			"assets/Modeldata/Ore/Normal/Ore_Normal.obj",
			"shader/vsinstance.fx",
			"shader/ps.fx",
			"assets/Modeldata/Ore/Normal/"
		);
		m_resources_Instance.push_back(instancing);
	}
	{
		CModel instancing;

		instancing.InitiInstancing(
			1000,
			"assets/Modeldata/Ore/Normal/Ore_Normal.obj",
			"shader/vsinstance.fx",
			"shader/ps.fx",
			"assets/Modeldata/Ore/Normal/"
		);
		m_resources_Instance.push_back(instancing);
	}
	for (int i = 0; i < (int)ItemType::ITEM_MAX; i++)
	{
		m_item[i] = 0;
	}
}

void ResourceManager::Update()
{
	

	for (int i = 0; i < (int)ItemType::ITEM_MAX; i++)
	{
		m_item[i] = 0;
	}

	for (int i = 0; i < m_installation_resources.size(); i++)
	{
		m_installation_resources[i]->Update();
	}
	for (int i = 0; i < BuildingMgr::GetInstance().GetSouko().size(); i++)
	{
		for (int j = 0; j < (int)ItemType::ITEM_MAX; j++) {
			m_item[j] += BuildingMgr::GetInstance().GetSouko()[i]->GetItemNum((ItemType)j);
		}
	}

}

void ResourceManager::Draw()
{
	//フラスタム内に存在するか
	XMVECTOR Eye = XMVectorSet(
		CCamera::GetInstance()->GetEye().x,
		CCamera::GetInstance()->GetEye().y,
		CCamera::GetInstance()->GetEye().z,
		0.0f);

	XMVECTOR At = XMVectorSet(
		CCamera::GetInstance()->GetLookat().x,
		CCamera::GetInstance()->GetLookat().y,
		CCamera::GetInstance()->GetLookat().z,
		0.0f);

	XMVECTOR Up = XMVectorSet(
		CCamera::GetInstance()->GetUp().x,
		CCamera::GetInstance()->GetUp().y,
		CCamera::GetInstance()->GetUp().z,
		0.0f);

	XMFLOAT4X4 pro = CCamera::GetInstance()->GetProjectionMatrix();
	XMMATRIX projection = XMLoadFloat4x4(&pro);
	XMMATRIX View = XMMatrixLookAtLH(Eye, At, Up);
	View = XMMatrixMultiply(View, projection);

	XMFLOAT4X4 ans;
	XMStoreFloat4x4(&ans, View);

	//インスタンシング対象モデル入れ替えの為
	//Drawで更新を行う
	for (int j = 0; j < m_resources_Instance.size(); j++)
	{
		m_resources_pos_world.clear();
		for (int i = 0; i < m_resources.size(); i++)
		{
			m_resources[i]->Update();
			//耐久０以下は削除
			if (m_resources[i]->GetData()->Endurance <= 0)
			{
				m_resources.erase(m_resources.begin() + i);
			}
			else
			{
				//対象描画物のみ描画
				if ((int)m_resources[i]->GetData()->type == j)
				{
					//描画用の座標のみをベクターに格納
					m_resources_pos_world.push_back(m_resources[i]->GetMtx());
				}
			}
		}
		m_resources_Instance[j].InstanceUpdate(m_resources_pos_world);
		m_resources_Instance[j].DrawInstance(m_resources_pos_world.size());
	}

	//デバッグ時のみコライダー描画
		//境界box表示
	if (!GameButton::GetInstance().GetDebug()) {
		for (int i = 0; i < m_resources.size(); i++)
		{
			m_resources[i]->DrawCollider();
		}
	}

	for (int i = 0; i < m_installation_resources.size(); i++)
	{
		if (IsInFrustum(m_installation_resources[i]->GetPos(), ans)) {
			m_installation_resources[i]->Draw();
		}
	}
}

void ResourceManager::DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in)
{
	//フラスタム内に存在するか
	XMVECTOR Eye = XMVectorSet(
		CCamera::GetInstance()->GetEye().x,
		CCamera::GetInstance()->GetEye().y,
		CCamera::GetInstance()->GetEye().z,
		0.0f);

	XMVECTOR At = XMVectorSet(
		CCamera::GetInstance()->GetLookat().x,
		CCamera::GetInstance()->GetLookat().y,
		CCamera::GetInstance()->GetLookat().z,
		0.0f);

	XMVECTOR Up = XMVectorSet(
		CCamera::GetInstance()->GetUp().x,
		CCamera::GetInstance()->GetUp().y,
		CCamera::GetInstance()->GetUp().z,
		0.0f);

	XMFLOAT4X4 pro = CCamera::GetInstance()->GetProjectionMatrix();
	XMMATRIX projection = XMLoadFloat4x4(&pro);
	XMMATRIX View = XMMatrixLookAtLH(Eye, At, Up);
	View = XMMatrixMultiply(View, projection);

	XMFLOAT4X4 ans;
	XMStoreFloat4x4(&ans, View);

	for (int i = 0; i < m_resources.size(); i++)
	{
		if (IsInFrustum(m_resources[i]->GetPos(), ans)) {
			m_resources[i]->DrawShadow(layout_in, vs_in, ps_in);
		}
	}

	for (int i = 0; i < m_installation_resources.size(); i++)
	{
		if (IsInFrustum(m_installation_resources[i]->GetPos(), ans)) {
			m_installation_resources[i]->DrawShadow(layout_in, vs_in, ps_in);
		}
	}
}

void ResourceManager::Uninit()
{
	for (int i = 0; i < m_resources.size(); i++)
	{
		m_resources[i]->Finalize();
	}
}

void ResourceManager::CreateResource(Resource::Data data, MODELID model)
{
	Resource* resource = new Resource();
	resource->Init(data,model);
	m_resources.push_back(resource);
}

void ResourceManager::CreateInstallationResource(Resource::Data data, MODELID model)
{
	Resource* resource = new Resource();
	resource->Init(data, model);
	m_installation_resources.push_back(resource);
}

int ResourceManager::GetItem(ItemType type)
{
	return m_item[(int)type];
}

void ResourceManager::EraseInstallation(Resource* p)
{
	for (int i = 0; i < m_installation_resources.size(); i++)
	{
		if (m_installation_resources[i] == p)
		{
			m_installation_resources.erase(m_installation_resources.begin() + i);
		}
	}
}

std::vector<Resource*> ResourceManager::GetInstallationResource()
{
	return m_installation_resources;
}
