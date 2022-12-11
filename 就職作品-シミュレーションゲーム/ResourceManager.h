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

	//‘Œ¹
	std::vector<Resource*>m_installation_resources;

};

