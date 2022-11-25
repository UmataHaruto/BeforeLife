#pragma once
#include	"gameobject.h"
#include	"CModel.h"
#include	"dx11mathutil.h"
#include	"CDirectInput.h"
#include    "ModelMgr.h"
#include    "NameGenerator.h"
#include    "Souko.h"
#include    "obb.h"

enum class CameraMode
{
	TPS,    //三人称視点
	FPS,    //一人称視点
};

class Player :public GameObject {
public:

	//パーツ
	enum PARTS
	{
		PLAYER_BODY,
		PLAYER_HAIR,
		PLAYER_TOOLS,
		NONE,
		END,
		PARTS_MAX,
	};

	enum  TOOLS
	{
		TOOLS_PICKAXE,
		TOOLS_AXE,
		TOOLS_BASKET,
		
		TOOLS_NONE = -1,
		TOOLS_MAX,

	};

	//性別
	enum class GENDER
	{
		MALE,
		FEMALE,
		OTHER,
	};

	enum class AnimationType
	{
		CARRY_IDLE,
		CARRY_RUN,
		CUT,
		IDLE_00,
		IDLE_01,
		IDLE_02,
		MINE,
		SLEEP,
		WALK,
		STOP,
	};

	enum class ActionType
	{
		WORK,
		SLEEP,
		ENTERTAINMENT,

		ACTION_MAX
	};

	enum class WorkType
	{
		CUT,
		MINE,
		COLLECT,

		WORK_MAX
	};

	// 構造体型タグ
	struct	PlayerInitData {
		Player::PARTS		ParentObjectNo;		// 親オブジェクトＮｏ
		Player::PARTS		ModelNo;			// モデル番号
		DirectX::XMFLOAT3	FirstPosition;		// 最初の位置
		DirectX::XMFLOAT3	FirstAngle;			// 最初の角度
	};
	//行動優先度
	struct ActionPriority
	{
		ActionType action;    //アクション識別子
		int priority;    //優先度
	};

	//作業優先度
	struct WorkPriority
	{
		WorkType work;    //アクション識別子
		int priority;    //優先度
	};
	struct Data
	{
		std::string firstname;
		std::string lastname;

		XMFLOAT3 pos;
	};

	struct AnimationData
	{
		float idledelay;    //Idle切替遅延
		float timer;    //タイマー
		AnimationType animno;

	};
	//初期配置
	static PlayerInitData ObjectInitData[];

	//初期化
	bool Init();
	bool Init(Data data);

	//描画
	void Draw();

	void DrawWithAxis();

	//更新
	void Update();

	//終了処理
	void Finalize();

	//カメラモードを変更
	void SetCameraMode(CameraMode mode);

	//セレクト状態の変更
	void SetSelect(bool input);

	//移動先の指定
	void SetMovePoint(XMFLOAT3 pos);
	//カメラモードを取得
	CameraMode GetCameraMode();

	//輸送アイテムをセット
	void SetCarryItem(ItemType tag,int num);

	//モデルデータを取得
	CModel* GetModel() {
		return m_model;
	}

	//バウンディングスフィアを取得
	COBB GetOBB(void)
	{
		return m_obb;
	}

	//アニメーションデータ取得
	AnimationData GetAnimData(void)
	{
		return m_animdata;
	}

	//名前を取得
	std::string GetName(NameGenerator::NAMETYPE type)
	{
		if (type == NameGenerator::NAMETYPE::FAMILLY)
		{
			return m_last_name;
		}
		else
		{
			return m_first_name;
		}
		return std::string("error!!");
	}

	//モデルセット
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	//座標をセット
	void SetPos(XMFLOAT3 pos)
	{
		m_pos = pos;
	}
private:
	void AnimationUpdate();

	//キャラクター名
	std::string m_first_name;
	std::string m_last_name;
	//性別
	GENDER m_gender;

	//運搬アイテム
	Souko::Item m_carry;
	bool m_iscarry;

	CModel* m_model;// 胴体
	CModel* m_tools;// 道具
	CModel* m_hair;//髪
	COBB m_obb;    //コリジョン

	//移動目標
	XMFLOAT3 movepos;

	//移動中
	bool m_ismoving = false;

	//選択状態
	bool m_isselect = false;

	//行動優先度
	std::vector<ActionPriority> m_action_priority;

	//作業優先度
	std::vector<WorkPriority> m_work_priority;

	CameraMode camera_mode;//カメラモード 0 = TPS,1 = FPS
	AnimationData m_animdata;


	void UpdateLocalpose();				// ローカルポーズを更新する
	void CaliculateParentChildMtx();	// 親子関係を考慮した行列を作成
	DirectX::XMFLOAT4X4			m_mtxlocalpose[PARTS_MAX];	// 自分のことだけを考えた行列
	DirectX::XMFLOAT4X4			m_mtxParentChild[PARTS_MAX];	// 親子関係を考慮した行列

	//作業関数
	void Work_Mine(void);
};