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
	//更新
	void Update();
	//描画
	void Draw();
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);

	//フィールドをセット
	void SetField(int x, int y, int z,Block data);
	void SetLimit(int time);
	void SetRecord(int Record);
	void ResetStageData();
	void RoadStageData();

	//座標から目的のブロックを返す
	bool SearchBlock(XMFLOAT3 mousepos,Block* output);

	Block GetField(int x,int y,int z);

	inline static Stage& GetInstance() {
		static Stage Instance;
		return Instance;
	}

private:
	Block Field[MAP_WIDTH][MAP_HEIGHT][MAP_DEPTH];    //フィールド


	int TimeLimit;    //タイムリミット
	char Name[10];    //ステージ名前
	int Record;       //レコード
};


