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
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);

	void Uninit();

	//村人を追加
	void CreateVillager(Player::Data data);

	//村人を一人のみ選択
	void SelectOneVillager(int idx);

	//村人
	std::vector<Player*>m_villager;

private:

};
