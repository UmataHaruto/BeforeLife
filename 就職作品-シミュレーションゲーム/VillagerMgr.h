#pragma once
#include "player.h"

class VillagerMgr
{
public:
	static VillagerMgr& GetInstance() {
		static VillagerMgr Instance;
		return Instance;
	}

	void Init();
	void Update();
	void Draw();
	void Uninit();

	//ë∫êlÇí«â¡
	void CreateVillager(Player::Data data);

	//ë∫êl
	std::vector<Player*>m_villager;

private:

};
