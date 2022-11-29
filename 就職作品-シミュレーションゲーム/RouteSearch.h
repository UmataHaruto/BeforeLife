#pragma once
#include "Stage.h"

//Astar�A���S���Y��

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

//�o�H�T��
class RouteSearch
{

public:
	static RouteSearch& GetInstance() {
		static RouteSearch Instance;
		return Instance;
	}

	//�X�e�[�W�����蔻����X�V
	void InitStageCollider();

	std::vector<XMFLOAT2> SearchRoute(XMFLOAT3 start,XMFLOAT3 goal);

private:
	/*
	0 = �ړ��\�}�X
	1 = ��
	2 = start
	3 = goal
	4 = route
	*/
	AStar m_stagecollider[MAP_WIDTH][MAP_HEIGHT];
};

