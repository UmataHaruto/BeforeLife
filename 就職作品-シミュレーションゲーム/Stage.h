#pragma once
#include <DirectXTex.h>
#include "Block.h"
#define MAP_WIDTH 200
#define MAP_HEIGHT 200
#define MAP_DEPTH 1

#define STAGE_MAX 10
#define LEVEL_MAX 10

//マップを構成するクラス
class Stage
{
public:

	//初期化
	void Init();
	//描画
	void Draw();

	//フィールドをセット
	void SetField(int x, int y, int z,Block data);
	void SetLimit(int time);
	void SetRecord(int Record);
	void ResetStageData();
	void RoadStageData();

	Block GetField(int x,int y,int z);

	static Stage& GetInstance() {
		static Stage Instance;
		return Instance;
	}

private:
	Block Field[MAP_WIDTH][MAP_HEIGHT][MAP_DEPTH];    //フィールド
	int TimeLimit;    //タイムリミット
	char Name[10];    //ステージ名前
	int Record;       //レコード
};


