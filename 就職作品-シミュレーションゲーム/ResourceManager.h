#pragma once
#include "Resource.h"
#include "ModelMgr.h"

class ResourceManager
{
public:
	static ResourceManager& GetInstance() {
		static ResourceManager Instance;
		return Instance;
	}

	void Init();
	void Update();
	void Draw();
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);
	void Uninit();

	//‘Œ¹
	std::vector<Resource*>m_resources;

	void CreateResource(Resource::Data data,MODELID model);
	void CreateInstallationResource(Resource::Data data, MODELID model);

	int GetItem(ItemType type);

	void EraseInstallation(int index);

	std::vector<Resource*> GetInstallationResource();

private:
	//ƒAƒCƒeƒ€Š”
	int m_item[(int)ItemType::ITEM_MAX];

	std::vector<CModel> m_resources_Instance;
	std::vector<XMFLOAT4X4> m_resources_pos_world;
	//‘Œ¹
	std::vector<Resource*>m_installation_resources;

};

