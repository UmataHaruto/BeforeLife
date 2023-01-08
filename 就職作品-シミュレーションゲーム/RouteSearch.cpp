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

		int x_min_idx = x_min / 12.5;
		int y_min_idx = (y_min / 12.5) - 2;

		int x_max_idx = x_max / 12.5;
		int y_max_idx = (y_max / 12.5) + 3;

		//範囲を１で埋める
		for (int y = y_min_idx; y < y_max_idx; y++) {
			for (int x = x_min_idx; x < x_max_idx; x++)
			{
				m_stagecollider[y][x].SetNumber(1);
			}
		}
	}
}

bool RouteSearch::IsHitBuilding(XMFLOAT3 pos)
{
	XMINT2 position = { (int32_t)(pos.x / 12.5),(int32_t)(fabs(pos.z) / 12.5) };
	if (position.x < 0 || position.y < 0)
	{
		return false;
	}
	if (m_stagecollider[position.y][position.x].GetNumber() == 1)
	{
		true;
	}
	return false;
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

	//ゴールが存在しない
	bool nothinggoal = false;

	//スタート地点が存在しない
	bool nothingstart = false;
	
	//∞ループ検知用
	int loopcount = 0;

	while (1)
	{

		loopcount++;
		notgoal = true;
		//スコアが最も低い値
		int score_min = 9998;
		//最低スコア番号
		int score_min_idx[2];
		score_min_idx[0] = 0;
		score_min_idx[1] = 0;

		nothinggoal = true;
		nothingstart = true;
		
		//上下左右をオープン
		for (int i = 0; i < MAP_HEIGHT; i++)
		{
			for (int j = 0; j < MAP_WIDTH; j++)
			{
				if (m_stagecollider[i][j].GetStatus() == AStar::Status::OPEN) {

					//スコアが最も低い値を抽出
					if (m_stagecollider[i][j].GetScore() < score_min)
					{
						score_min_idx[0] = i;
						score_min_idx[1] = j;
						score_min = m_stagecollider[i][j].GetScore();
					}

				}
				if (m_stagecollider[i][j].GetNumber() == 3) {
					nothinggoal = false;
				}
				if (m_stagecollider[i][j].GetNumber() == 2) {
					nothingstart = false;
				}
			}
		}

		//ゴールに達した場合
		if (m_stagecollider[score_min_idx[0]][score_min_idx[1]].GetNumber() == 3)
		{
			isgoal = true;
			if (m_stagecollider[score_min_idx[0]][score_min_idx[1]].GetParent() != nullptr) {
				AStar* parent = m_stagecollider[score_min_idx[0]][score_min_idx[1]].GetParent();
				while (1)
				{
					if (parent->GetParent() == nullptr)
					{
						break;
					}

					parent->SetNumber(4);
					XMFLOAT2 temp;
					temp.x = parent->GetId() % MAP_HEIGHT;
					temp.x *= SearchBlockLength;
					temp.x += SearchBlockLength / 2;

					temp.y = ((parent->GetId() - (parent->GetId() % MAP_HEIGHT)) / MAP_WIDTH);
					temp.y *= -SearchBlockLength;
					temp.y -= SearchBlockLength / 2;
					route.push_back(temp);
					parent = parent->GetParent();
				}
			}
			break;
		}

		notgoal = false;
		m_stagecollider[score_min_idx[0]][score_min_idx[1]].SetStatus(AStar::Status::CLOSE);
		//左を確認
		if (score_min_idx[1] - 1 >= 0)
		{
			if (m_stagecollider[score_min_idx[0]][score_min_idx[1] - 1].GetStatus() != AStar::Status::CLOSE && m_stagecollider[score_min_idx[0]][score_min_idx[1] - 1].GetNumber() != 1)
			{
				int score = fabs(goalpos.x - (score_min_idx[1] - 1)) + fabs(goalpos.y - score_min_idx[0]);
				m_stagecollider[score_min_idx[0]][score_min_idx[1] - 1].SetScore(score);
				m_stagecollider[score_min_idx[0]][score_min_idx[1] - 1].SetStatus(AStar::Status::OPEN);
				m_stagecollider[score_min_idx[0]][score_min_idx[1] - 1].SetParent(&m_stagecollider[score_min_idx[0]][score_min_idx[1]]);
			}
		}
		//左上を確認
		if (score_min_idx[1] - 1 >= 0 && score_min_idx[0] - 1 >= 0)
		{
			if (m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] - 1].GetStatus() != AStar::Status::CLOSE && m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] - 1].GetNumber() != 1)
			{
				int score = fabs(goalpos.x - (score_min_idx[1] - 1)) + fabs(goalpos.y - (score_min_idx[0] - 1));
				m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] - 1].SetScore(score);
				m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] - 1].SetStatus(AStar::Status::OPEN);
				m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] - 1].SetParent(&m_stagecollider[score_min_idx[0]][score_min_idx[1]]);
			}
		}
		//上を確認
		if (score_min_idx[0] - 1 >= 0)
		{
			if (m_stagecollider[score_min_idx[0] - 1][score_min_idx[1]].GetStatus() != AStar::Status::CLOSE && m_stagecollider[score_min_idx[0] - 1][score_min_idx[1]].GetNumber() != 1)
			{
				int score = fabs(goalpos.x - score_min_idx[1]) + fabs(goalpos.y - (score_min_idx[0] - 1));
				m_stagecollider[score_min_idx[0] - 1][score_min_idx[1]].SetScore(score);
				m_stagecollider[score_min_idx[0] - 1][score_min_idx[1]].SetStatus(AStar::Status::OPEN);
				m_stagecollider[score_min_idx[0] - 1][score_min_idx[1]].SetParent(&m_stagecollider[score_min_idx[0]][score_min_idx[1]]);

			}
		}
		//右上を確認
		if (score_min_idx[1] + 1 <= MAP_WIDTH && score_min_idx[0] - 1 >= 0)
		{
			if (m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] + 1].GetStatus() != AStar::Status::CLOSE && m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] + 1].GetNumber() != 1)
			{
				int score = fabs(goalpos.x - (score_min_idx[1] + 1)) + fabs(goalpos.y - (score_min_idx[0] - 1));
				m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] + 1].SetScore(score);
				m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] + 1].SetStatus(AStar::Status::OPEN);
				m_stagecollider[score_min_idx[0] - 1][score_min_idx[1] + 1].SetParent(&m_stagecollider[score_min_idx[0]][score_min_idx[1]]);
			}
		}
		//右を確認
		if (score_min_idx[1] + 1 <= MAP_WIDTH)
		{
			if (m_stagecollider[score_min_idx[0]][score_min_idx[1] + 1].GetStatus() != AStar::Status::CLOSE && m_stagecollider[score_min_idx[0]][score_min_idx[1] + 1].GetNumber() != 1)
			{
				int score = fabs(goalpos.x - (score_min_idx[1] + 1)) + fabs(goalpos.y - score_min_idx[0]);
				m_stagecollider[score_min_idx[0]][score_min_idx[1] + 1].SetScore(score);
				m_stagecollider[score_min_idx[0]][score_min_idx[1] + 1].SetStatus(AStar::Status::OPEN);
				m_stagecollider[score_min_idx[0]][score_min_idx[1] + 1].SetParent(&m_stagecollider[score_min_idx[0]][score_min_idx[1]]);

			}
		}
		//右下を確認
		if (score_min_idx[1] + 1 <= MAP_WIDTH && score_min_idx[0] + 1 <= MAP_HEIGHT)
		{
			if (m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] + 1].GetStatus() != AStar::Status::CLOSE && m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] + 1].GetNumber() != 1)
			{
				int score = fabs(goalpos.x - (score_min_idx[1] + 1)) + fabs(goalpos.y - (score_min_idx[0] + 1));
				m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] + 1].SetScore(score);
				m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] + 1].SetStatus(AStar::Status::OPEN);
				m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] + 1].SetParent(&m_stagecollider[score_min_idx[0]][score_min_idx[1]]);
			}
		}
		//下を確認
		if (score_min_idx[0] + 1 <= MAP_HEIGHT)
		{
			if (m_stagecollider[score_min_idx[0] + 1][score_min_idx[1]].GetStatus() != AStar::Status::CLOSE && m_stagecollider[score_min_idx[0] + 1][score_min_idx[1]].GetNumber() != 1)
			{
				int score = fabs(goalpos.x - score_min_idx[1]) + fabs(goalpos.y - (score_min_idx[0] + 1));
				m_stagecollider[score_min_idx[0] + 1][score_min_idx[1]].SetScore(score);
				m_stagecollider[score_min_idx[0] + 1][score_min_idx[1]].SetStatus(AStar::Status::OPEN);
				m_stagecollider[score_min_idx[0] + 1][score_min_idx[1]].SetParent(&m_stagecollider[score_min_idx[0]][score_min_idx[1]]);

			}
		}
		//左下を確認
		if (score_min_idx[0] + 1 <= MAP_HEIGHT && score_min_idx[1] - 1 >= 0)
		{
			if (m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] - 1].GetStatus() != AStar::Status::CLOSE && m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] - 1].GetNumber() != 1)
			{
				int score = fabs(goalpos.x - (score_min_idx[1] - 1)) + fabs(goalpos.y - (score_min_idx[0] + 1));
				m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] - 1].SetScore(score);
				m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] - 1].SetStatus(AStar::Status::OPEN);
				m_stagecollider[score_min_idx[0] + 1][score_min_idx[1] - 1].SetParent(&m_stagecollider[score_min_idx[0]][score_min_idx[1]]);
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
		if (nothinggoal)
		{
			printf("ゴールがありません");
			break;
		}
		if (nothingstart)
		{
			printf("スタートがありません");
			break;
		}
		//∞ループ時は強制退出
		if (loopcount > 200)
		{
			break;
		}
	}
	//テスト描画(コンソール)
	//system("cls");

	//for (int i = 0; i < MAP_WIDTH; i++)
	//{
	//	for (int j = 0; j < MAP_HEIGHT; j++)
	//	{
	//		switch (m_stagecollider[i][j].GetNumber())
	//		{
	//		case 0:
	//			printf(".");
	//			break;

	//		case 1:
	//			printf("x");
	//			break;

	//		case 2:
	//			printf("P");
	//			break;

	//		case 3:
	//			printf("G");
	//			break;

	//		case 4:
	//			printf("*");
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//	printf("\n");
	//}
	return route;

}

