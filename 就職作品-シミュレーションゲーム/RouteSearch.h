#pragma once
#include "Stage.h"

//Astarアルゴリズム

class AStar
{
public:

	enum class Status
	{
		NORMAL,
		OPEN,
		CLOSE,
		GOAL,
	};

	inline void SetId(int id)
	{
		m_id = id;
	}

	inline void SetNumber(int num)
	{
		m_number = num;
	}

	inline void SetStatus(Status sts)
	{
		m_status = sts;
	}

	inline void SetParent(AStar* parent)
	{
		m_parent = parent;
	}

	inline void SetScore(int score)
	{
		m_score = score;
	}

	inline int GetId()
	{
		return m_id;
	}

	inline int GetNumber()
	{
		return m_number;
	}

	inline Status GetStatus()
	{
		return m_status;
	}

	inline int GetScore()
	{
		return m_score;
	}

	inline AStar* GetParent()
	{
		return m_parent;
	}

private:
	Status m_status;
	AStar* m_parent;
	int m_id = 0;
	int m_number;
	int m_score;
};

//経路探索
class RouteSearch
{

public:
	static RouteSearch& GetInstance() {
		static RouteSearch Instance;
		return Instance;
	}

	//ステージ当たり判定を更新
	void InitStageCollider();

	//座標基準で建築物に接触しているか(接触時 = true)
	bool IsHitBuilding(XMFLOAT3 pos);

	std::vector<XMFLOAT2> SearchRoute(XMFLOAT3 start,XMFLOAT3 goal);

private:
	/*
	0 = 移動可能マス
	1 = 壁
	2 = start
	3 = goal
	4 = route
	*/
	AStar m_stagecollider[MAP_WIDTH][MAP_HEIGHT];
	//検索精度(最小12.5)
	float SearchBlockLength = 12.5;

};

