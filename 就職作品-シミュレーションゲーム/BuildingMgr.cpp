#include "BuildingMgr.h"
#include "dx11mathutil.h"
#include "CCamera.h"

void BuildingMgr::Init()
{
}

void BuildingMgr::Update()
{
	for (int i = 0; i < m_houses.size(); i++)
	{
		m_houses[i]->Update();
	}
	for (int i = 0; i < m_soukos.size(); i++)
	{
		m_soukos[i]->Update();
	}
}

void BuildingMgr::Draw()
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

	for (int i = 0; i < m_houses.size(); i++)
	{
		if (IsInFrustum(m_houses[i]->GetPos(), ans)) {
			m_houses[i]->Draw();
		}
	}

	for (int i = 0; i < m_soukos.size(); i++)
	{
		if (IsInFrustum(m_soukos[i]->GetPos(), ans)) {
			m_soukos[i]->Draw();
		}
	}

}

void BuildingMgr::Uninit()
{
	for (int i = 0; i < m_houses.size(); i++)
	{
		m_houses[i]->Finalize();
	}
	for (int i = 0; i < m_soukos.size(); i++)
	{
		m_soukos[i]->Finalize();
	}

}

void BuildingMgr::CreateHouse(House::Data data, MODELID model)
{
	House* house = new House();
	house->Init(data, model);
	m_houses.push_back(house);

}

void BuildingMgr::CreateSouko(Souko::Data data, MODELID model)
{
	Souko* souko = new Souko();
	souko->Init(data, model);
	m_soukos.push_back(souko);
}

int BuildingMgr::GetItemNum(ItemType tag)
{
	int num = 0;
	for (int i = 0; i < m_soukos.size(); i++)
	{
		for (int j = 0; j < m_soukos[i]->GetData().items.size(); j++)
		{
			if (m_soukos[i]->GetData().items[j].tag == tag)
			{
				num += m_soukos[i]->GetData().items[j].num;
			}
		}
	}
    
	return num;
}

std::vector<COBB> BuildingMgr::GetAllObb()
{
	std::vector<COBB> obbs;

	for (int i = 0; i < m_houses.size(); i++)
	{
		obbs.push_back(m_houses[i]->GetOBB());
	}
	for (int i = 0; i < m_soukos.size(); i++)
	{
		obbs.push_back(m_soukos[i]->GetOBB());
	}

	return obbs;
}
