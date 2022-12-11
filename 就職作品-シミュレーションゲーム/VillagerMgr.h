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

	//���l��ǉ�
	void CreateVillager(Player::Data data);

	//���l����l�̂ݑI��
	void SelectOneVillager(int idx);

	//���l
	std::vector<Player*>m_villager;

private:

};
