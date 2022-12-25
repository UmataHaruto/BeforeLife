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

	//���l��ǉ�
	void CreateVillager(Player::Data data);

	//���l����l�̂ݑI��
	void SelectOneVillager(int idx);

	//���l
	std::vector<Player*>m_villager;

private:

};
