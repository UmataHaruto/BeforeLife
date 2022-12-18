#pragma once
#include <vector>
#include "House.h"
#include "Souko.h"
#include "Road.h"
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
	void CreateRoad(XMFLOAT3 position,MODELID model);

	int GetItemNum(ItemType tag);

	std::vector<CollisionData> GetAllBuildingPosition();

	std::vector<COBB> GetAllObb();
	std::vector<std::unique_ptr<Souko>>& GetSouko();
	std::vector<std::unique_ptr<Road>>& GetRoad();
private:
	//–¯‰Æ
	std::vector<std::unique_ptr<House>> m_houses;
	//‘qŒÉ
	std::vector<std::unique_ptr<Souko>> m_soukos;
	//“¹˜H
	std::vector<std::unique_ptr<Road>> m_roads;

};

