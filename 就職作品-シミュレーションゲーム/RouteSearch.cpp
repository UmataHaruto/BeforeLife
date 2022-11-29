#include "RouteSearch.h"
#include "BuildingMgr.h"

void RouteSearch::InitStageCollider()
{
	//全てを0にする
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			m_stagecollider[i][j].SetNumber(0);
			m_stagecollider[i][j].SetScore(9999);
			m_stagecollider[i][j].SetStatus(AStar::Status::NORMAL);
			m_stagecollider[i][j].SetId((i * MAP_WIDTH) + j);
		}
	}
	//移動不可エリアを追加
	std::vector<BuildingMgr::CollisionData> collisiondata = BuildingMgr::GetInstance().GetAllBuildingPosition();

	for (int i = 0; i < collisiondata.size(); i++)
	{
		//対象マスを設定
		float x_min = collisiondata[i].position.x - (collisiondata[i].width / 2);
		float y_min = fabs(collisiondata[i].position.z) - (collisiondata[i].height / 2);

		float x_max = collisiondata[i].position.x + (collisiondata[i].width / 2);
		float y_max = fabs(collisiondata[i].position.z) + (collisiondata[i].height / 2);

		int x_min_idx = x_min / 12.5f;
		int y_min_idx = y_min / 12.5f;

		int x_max_idx = x_max / 12.5f;
		int y_max_idx = y_max / 12.5f;

		//範囲を１で埋める
		for (int y = y_min_idx; y < y_max_idx; y++) {
			for (int x = x_min_idx; x < x_max_idx; x++)
			{
				m_stagecollider[y][x].SetNumber(1);
			}
		}
	}
}

std::vector<XMFLOAT2> RouteSearch::SearchRoute(XMFLOAT3 start, XMFLOAT3 goal)
{
	XMINT2 startpos = {(int32_t)(start.x / 12.5),(int32_t)(fabs(start.z) / 12.5)};
	XMINT2 goalpos = { (int32_t)(goal.x / 12.5),(int32_t)(fabs(goal.z) / 12.5)};

	std::vector<XMFLOAT2> route;

	//開始地点を設定
	m_stagecollider[startpos.y][startpos.x].SetNumber(2);
	//終了地点を設定
	m_stagecollider[goalpos.y][goalpos.x].SetNumber(3);

	//最初のオープン
	int score = fabs(goalpos.x - startpos.x) + fabs( goalpos.y - startpos.y);
	m_stagecollider[startpos.y][startpos.x].SetScore(score);
	m_stagecollider[startpos.y][startpos.x].SetStatus(AStar::Status::OPEN);
	//最初は親がいない
	m_stagecollider[startpos.y][startpos.x].SetParent(nullptr);

	//ゴールした
	bool isgoal = false;
	//ゴール不可能
	bool notgoal = false;
	while (1)
	{
		notgoal = true;
		//上下左右をオープン
		for (int i = 0; i < MAP_HEIGHT; i++)
		{
			for (int j = 0; j < MAP_WIDTH; j++)
			{
				if (m_stagecollider[i][j].GetStatus() == AStar::Status::OPEN) {
					//ゴールに達した場合
					if (m_stagecollider[i][j].GetNumber() == 3)
					{
						isgoal = true;
						AStar* parent = m_stagecollider[i][j].GetParent();
						while (1)
						{
							if (parent->GetParent() == nullptr)
							{
								break;
							}

							parent->SetNumber(4);
							XMFLOAT2 temp;
							temp.x = parent->GetId() % MAP_HEIGHT;
							temp.x *= 12.5;
							temp.x += 12.5 / 2;

							temp.y = parent->GetId() - (parent->GetId() % MAP_HEIGHT);
							temp.y *= -12.5;
							temp.y -= 12.5 / 2;
							route.push_back(temp);
							parent = parent->GetParent();
						}
						break;
					}

					notgoal = false;
					m_stagecollider[i][j].SetStatus(AStar::Status::CLOSE);
					//左を確認
					if (j - 1 >= 0)
					{
						if (m_stagecollider[i][j - 1].GetStatus() != AStar::Status::CLOSE && m_stagecollider[i][j - 1].GetNumber() != 1)
						{
							int score = fabs(goalpos.x - (j - 1)) + fabs(goalpos.y - i);
							m_stagecollider[i][j - 1].SetScore(score);
							m_stagecollider[i][j - 1].SetStatus(AStar::Status::OPEN);
							m_stagecollider[i][j - 1].SetParent(&m_stagecollider[i][j]);
						}
					}
					//右を確認
					if (j + 1 <= MAP_WIDTH)
					{
						if (m_stagecollider[i][j + 1].GetStatus() != AStar::Status::CLOSE && m_stagecollider[i][j + 1].GetNumber() != 1)
						{
							int score = fabs(goalpos.x - (j + 1)) + fabs(goalpos.y - i);
							m_stagecollider[i][j + 1].SetScore(score);
							m_stagecollider[i][j + 1].SetStatus(AStar::Status::OPEN);
							m_stagecollider[i][j + 1].SetParent(&m_stagecollider[i][j]);

						}
					}
					//上を確認
					if (i - 1 >= 0)
					{
						if (m_stagecollider[i - 1][j].GetStatus() != AStar::Status::CLOSE && m_stagecollider[i - 1][j].GetNumber() != 1)
						{
							int score = fabs(goalpos.x - j) + fabs(goalpos.y - (i - 1));
							m_stagecollider[i - 1][j].SetScore(score);
							m_stagecollider[i - 1][j].SetStatus(AStar::Status::OPEN);
							m_stagecollider[i - 1][j].SetParent(&m_stagecollider[i][j]);

						}
					}
					//下を確認
					if (i - 1 <= MAP_HEIGHT)
					{
						if (m_stagecollider[i + 1][j].GetStatus() != AStar::Status::CLOSE && m_stagecollider[i + 1][j].GetNumber() != 1)
						{
							int score = fabs(goalpos.x - j) + fabs(goalpos.y - (i + 1));
							m_stagecollider[i + 1][j].SetScore(score);
							m_stagecollider[i + 1][j].SetStatus(AStar::Status::OPEN);
							m_stagecollider[i + 1][j].SetParent(&m_stagecollider[i][j]);

						}
					}
				}
			}
		}
		if (isgoal)
		{
			break;
		}
		if (notgoal)
		{
			printf("ゴールへ到達不可能です");
			break;
		}
	}
	//テスト描画(コンソール)
	system("cls");

	for (int i = 0; i < MAP_WIDTH; i++)
	{
		for (int j = 0; j < MAP_HEIGHT; j++)
		{
			switch (m_stagecollider[i][j].GetNumber())
			{
			case 0:
				printf(".");
				break;

			case 1:
				printf("x");
				break;

			case 2:
				printf("P");
				break;

			case 3:
				printf("G");
				break;

			case 4:
				printf("*");
				break;
			default:
				break;
			}
		}
		printf("\n");
	}
	return route;

}

void AStar::SetId(int id)
{
	m_id = id;
}

void AStar::SetNumber(int num)
{
	m_number = num;
}

void AStar::SetStatus(Status sts)
{
	m_status = sts;
}

void AStar::SetParent(AStar* parent)
{
	m_parent = parent;
}

void AStar::SetScore(int score)
{
	m_score = score;
}

int AStar::GetId()
{
	return m_id;
}

int AStar::GetNumber()
{
	return m_number;
}

AStar::Status AStar::GetStatus()
{
	return m_status;
}

int AStar::GetScore()
{
	return m_score;
}

AStar* AStar::GetParent()
{
	return m_parent;
}
