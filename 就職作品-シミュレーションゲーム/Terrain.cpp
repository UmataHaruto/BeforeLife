#include "Terrain.h"
#include "ModelMgr.h"

bool Terrain::Init()
{
	//s—ñ‰Šú‰»
	DX11MtxIdentity(m_mtx);
	m_model = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::TERRAIN)].modelname);

	m_mtx._41 = -300;
	m_mtx._42 = -554;
	m_mtx._43 = -350;


	return true;
}

void Terrain::Draw()
{
    m_model->Draw(m_mtx);
}

void Terrain::Finalize()
{
    m_model->Uninit();
}
