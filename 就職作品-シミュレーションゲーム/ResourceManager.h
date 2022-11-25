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

	int GetItem(ItemType type);

private:
	//ƒAƒCƒeƒ€Š”
	int m_item[(int)ItemType::ITEM_MAX];
};

