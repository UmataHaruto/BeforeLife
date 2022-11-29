#pragma once
#include <vector>
#include "House.h"
#include "Souko.h"
#include "obb.h"

class BuildingMgr
{
public:

	struct CollisionData
	{
		XMFLOAT3 position;
		int width;
		int height;
	};

	static BuildingMgr& GetInstance() {
		static BuildingMgr Instance;
		return Instance;
	}

	void Init();
	void Update();
	void Draw();
	void Uninit();

	void CreateHouse(House::Data data, MODELID model);
	void CreateSouko(Souko::Data data, MODELID model);

	int GetItemNum(ItemType tag);

	std::vector<CollisionData> GetAllBuildingPosition();

	std::vector<COBB> GetAllObb();

private:
	//–¯‰Æ
	std::vector<House*>m_houses;
	//‘qŒÉ
	std::vector<Souko*>m_soukos;
};

