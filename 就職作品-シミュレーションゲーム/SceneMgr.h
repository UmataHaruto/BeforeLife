#pragma once
#include "ModelMgr.h"

enum class SCENETYPE
{
	TITLE,
	GAME,
};

class SceneMgr {

public:

	bool SceneChange = false;    //シーン遷移中

	//更新
	void Update(void);

	static SceneMgr& GetInstance() {
		static SceneMgr Instance;
		return Instance;
	}

	void ChangeScene(SCENETYPE type)
	{
		SceneNo = type;
		SceneChange = false;
	}

	SCENETYPE GetSceneNo(void)
	{
		return SceneNo;
	}

	void SetInitStatus(int no,bool sts)
	{
		InitStatus[no] = sts;
	}
	bool GetInitStatus()
	{
		bool sts = true;
		for (int i = 0; i < 5; i++)
		{
			if (!InitStatus[i])
			{
				sts = false;
			}
		}
		return sts;
	}

	void AddLoadingRatio()
	{
		loading_ratio += 1 / (float)ModelMgr::GetInstance().g_modellist.size();

		if (loading_ratio > 1.0)
		{
			loading_ratio = 1.0;
		}
		if (loading_ratio < 0)
		{
			loading_ratio = 0;
		}
	}

	void SetEndFlg(bool flg)
	{
		EndFlg = flg;
	}

	bool GetEndFlg()
	{
		return EndFlg;
	}

	float GetLoadingRatio()
	{
		return loading_ratio;
	}
private:
	SCENETYPE SceneNo = SCENETYPE::TITLE;
	float loading_ratio = 0.0f;
	bool InitStatus[5] = {false,false,false,false,false};
	bool EndFlg = false;
};