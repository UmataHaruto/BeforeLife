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

	//村人を追加
	void CreateVillager(Player::Data data);

	//村人を一人のみ選択
	void SelectOneVillager(int idx);

	//村人
	std::vector<Player*>m_villager;

private:

};
