#include "Stage.h"
#include "dx11mathutil.h"

void Stage::Init()
{

}

void Stage::Draw()
{
	//ÉXÉeÅ[ÉWÇÃï`âÊ
	for (int z = 0; z < MAP_DEPTH; z++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				XMFLOAT4X4 mtxtrans;
				XMFLOAT4X4 mtxrottrans;
				XMFLOAT3 trans = { (y * 12.5f) - 100,(z * -12.5f),(x * -12.5f) + 200 };

				DX11MtxTranslation(trans, mtxtrans);
				//ÉÇÉfÉãï`âÊ
				switch (Field[z][x][y].GetBlockId())
				{
				case BLOCKTYPE::EMPTY:

					break;

				case BLOCKTYPE::GRASS:
					Field[z][x][y].SetPos(trans);
					Field[z][x][y].Draw();
					break;
				}
			}
		}
	}
}



void Stage::SetField(int x, int y, int z, Block data)
{
	Field[x][y][z] = data;
}

void Stage::SetLimit(int time)
{
	//å¿äEÇÃ999Çí¥Ç¶ÇƒÇ¢Ç»Ç¢Ç©
	if (time <= 999) {
		TimeLimit = time;
	}
	else {
		TimeLimit = 999;
	}
}

void Stage::SetRecord(int Record)
{
	//å¿äEÇÃ999999Çí¥Ç¶ÇƒÇ¢Ç»Ç¢Ç©
	if (Record <= 999999) {
		this->Record = Record;
	}
	else {
		this->Record = 999999;
	}
}

void Stage::RoadStageData()
{
	FILE* fp;

	fp = fopen("assets/StageData/STAGE1.txt", "r");

	int Hairetu[MAP_DEPTH][MAP_HEIGHT][MAP_WIDTH];

	BLOCKTYPE type;

	for (int y = 0; y < MAP_DEPTH; y++) {
		for (int z = 0; z < MAP_HEIGHT; z++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				fscanf(fp, "%d,", &type);
				Block block;
				Vector3 pos = {y,z,x};
				block.Init(pos, type);
				Field[y][z][x] = block;
			}
		}
	}

	fclose(fp);
}

Block Stage::GetField(int x,int y,int z)
{
	return Field[x][y][z];
}

