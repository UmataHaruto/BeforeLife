#include "VillagerMgr.h"
#include "CCamera.h"

void VillagerMgr::Init()
{

};

void VillagerMgr::Update()
{
	for (int i = 0; i < m_villager.size(); i++)
	{
		m_villager[i]->Update();
	}
};

void VillagerMgr::Draw()
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

	for (int i = 0; i < m_villager.size(); i++)
	{
		if (IsInFrustum(m_villager[i]->GetPos(), ans)) {
			m_villager[i]->Draw();
		}
	}
}
void VillagerMgr::DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in)
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

	for (int i = 0; i < m_villager.size(); i++)
	{
		if (IsInFrustum(m_villager[i]->GetPos(), ans)) {
			m_villager[i]->DrawShadow(layout_in, vs_in, ps_in);
		}
	}
}
;

void VillagerMgr::Uninit()
{
	for (int i = 0; i < m_villager.size(); i++)
	{
		m_villager[i]->Finalize();
	}
};

void VillagerMgr::CreateVillager(Player::Data data)
{
	Player* player = new Player();
	player->Init(data);
	m_villager.push_back(player);
}
void VillagerMgr::SelectOneVillager(int idx)
{
	for (int i = 0; i < m_villager.size(); i++)
	{
		m_villager[i]->SetSelect(false);
	}
	m_villager[idx]->SetSelect(true);

};