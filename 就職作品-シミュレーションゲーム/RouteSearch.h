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

	void SetId(int id);
	void SetNumber(int num);
	void SetStatus(Status sts);
	void SetParent(AStar* parent);
	void SetScore(int score);

	int GetId();
	int GetNumber();
	AStar::Status GetStatus();
	int GetScore();
	AStar* GetParent();
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
};

