#include "Sea.h"
#include "ModelMgr.h"

bool Sea::Init()
{
	//s—ñ‰Šú‰»
	DX11MtxIdentity(m_mtx);
	m_model = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::ROAD_DIRT)].modelname);

	m_mtx._41 = -300;
	m_mtx._42 = -604;
	m_mtx._43 = -350;


	return true;
}

void Sea::Draw()
{
    m_model->Draw(m_mtx);
}

void Sea::Finalize()
{
    m_model->Uninit();
}
