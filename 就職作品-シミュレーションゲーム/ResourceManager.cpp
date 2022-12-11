#include "ResourceManager.h"
#include "dx11mathutil.h"
#include "CCamera.h"
#include "BuildingMgr.h"
void ResourceManager::Init()
{
	for (int i = 0; i < (int)ItemType::ITEM_MAX; i++)
	{
		m_item[i] = 0;
	}
}

void ResourceManager::Update()
{
	for (int i = 0; i < m_resources.size(); i++)
	{
		m_resources[i]->Update();
		//‘Ï‹v‚OˆÈ‰º‚Ííœ
		if (m_resources[i]->GetData()->Endurance <= 0)
		{
			m_resources.erase(m_resources.begin() + i);
		}
	}

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
	//ƒtƒ‰ƒXƒ^ƒ€“à‚É‘¶Ý‚·‚é‚©
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
		if(IsInFrustum(m_resources[i]->GetPos(),ans)){
			m_resources[i]->Draw();
		}
	}

	for (int i = 0; i < m_installation_resources.size(); i++)
	{
		if (IsInFrustum(m_installation_resources[i]->GetPos(), ans)) {
			m_installation_resources[i]->Draw();
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

void ResourceManager::EraseInstallation(int index)
{
	m_installation_resources.erase(m_installation_resources.begin() + index);
}

std::vector<Resource*> ResourceManager::GetInstallationResource()
{
	return m_installation_resources;
}
