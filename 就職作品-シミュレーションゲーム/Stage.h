#pragma once
#include <DirectXTex.h>
#include "Block.h"
#define MAP_WIDTH 200
#define MAP_HEIGHT 200
#define MAP_DEPTH 1

#define STAGE_MAX 10
#define LEVEL_MAX 10

//�}�b�v���\������N���X
class Stage
{
public:

	//������
	void Init();
	//�`��
	void Draw();

	//�t�B�[���h���Z�b�g
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
	Block Field[MAP_WIDTH][MAP_HEIGHT][MAP_DEPTH];    //�t�B�[���h
	int TimeLimit;    //�^�C�����~�b�g
	char Name[10];    //�X�e�[�W���O
	int Record;       //���R�[�h
};


