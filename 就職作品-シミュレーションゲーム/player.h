#pragma once
#include	"gameobject.h"
#include	"CModel.h"
#include	"dx11mathutil.h"
#include	"CDirectInput.h"
#include    "ModelMgr.h"
#include    "NameGenerator.h"
#include    "Souko.h"
#include    "obb.h"
#include    <array>

class House;

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
		REST,
		ACTION_MAX
	};

	enum class WorkType
	{
		CUT,
		MINE,
		COLLECT,
		CARRY,
		CONSTRUCTION,

		WORK_MAX
	};

	enum class CarryStatus
	{
		NONE,                   //行動無し
		SEARCH_INSTALLATION,    //移動対象へ移動中
		CARRY,                  //運搬中
	};

	enum class EMOTION
	{
		NONE,
		HEART,
		SWEATING,
		FANNY,
		TALK,
		QUESTION,
		EMOTION_MAX,
	};

	enum class Schedule
	{
		WORK,
		SLEEP,
		FREE,
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

		float mood;
		float hp_max;
		float hp;
		float stamina_max;
		float stamina;

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
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);
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

	//髪色を設定
	void SetHairColor(XMFLOAT4 color)
	{
		m_haircolor = color;
	}

	//輸送アイテムをセット
	void SetCarryItem(ItemType tag,int num);

	//モデルデータを取得
	inline CModel* GetModel() {
		return m_model;
	}

	//バウンディングスフィアを取得
	inline COBB GetOBB(void)
	{
		return m_obb;
	}

	//アニメーションデータ取得
	inline AnimationData GetAnimData(void)
	{
		return m_animdata;
	}

	//名前を取得
	inline std::string GetName(NameGenerator::NAMETYPE type)
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
	inline void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	//座標をセット
	inline void SetPos(XMFLOAT3 pos)
	{
		m_pos = pos;
	}
	//家をセット
	inline void SetHouse(House* house)
	{
		m_house = house;
	}
	//ヒットポイントをセット
	inline void SetHitpoint(float max,float hp)
	{
		m_hp_max = max;
		if(hp >= 0)
		{
		    m_hp = hp;
		}
		else if(hp >= m_hp_max)
		{
			m_hp = m_hp_max;
		}
		else
		{
			m_hp = 0;
		}
	}

	//スタミナをセット
	inline void SetStamina(float max,float stamina)
	{
		m_stamina_max = max;
		if (stamina >= 0)
		{
			m_stamina = stamina;
		}
		else if (stamina >= m_stamina_max)
		{
			m_stamina = m_stamina_max;
		}
		else
		{
			m_stamina = 0;
		}
	}

	//最大ヒットポイントを取得
	inline float GetMaxHitpoint()
	{
		return m_hp_max;
	}

	//最大スタミナを取得
	inline float GetMaxStamina()
	{
		return m_stamina_max;
	}

	//ヒットポイントを取得
	inline float GetHitpoint()
	{
		return m_hp;
	}

	//スタミナを取得
	inline float GetStamina()
	{
		return m_stamina;
	}

	//機嫌を取得
	inline float GetMood()
	{
		return m_mood;
	}

	//選択状況を取得
	inline bool GetSelect()
	{
		return m_isselect;
	}

	inline std::vector<WorkPriority> GetWorkPriority()
	{
		return m_work_priority;
	}

	inline void SetWorkPriority(int idx, int priority)
	{
		m_work_priority[idx].priority = priority;
	}

	inline std::array<Schedule, 24> GetSchedule()
	{
		return m_schedule;
	}

	void SetSchedule(std::array<Schedule, 24> input)
	{
		m_schedule = input;
	}
private:
	void AnimationUpdate();

	//キャラクター名
	std::string m_first_name;
	std::string m_last_name;
	//性別
	GENDER m_gender;

	float m_mood;         //機嫌(0 ～ 100)
	float m_hp_max;       //最大ヒットポイント
	float m_hp;           //ヒットポイント
	float m_stamina_max;  //最大スタミナ
	float m_stamina;      //スタミナ
	XMFLOAT4 m_haircolor; //髪色
	bool at_entrance = false;
	bool m_is_sleeping = false;//睡眠状態
	bool m_is_talking = false;

	//運搬アイテム
	Souko::Item m_carry;
	bool m_iscarry;

	CarryStatus m_carry_status = CarryStatus::NONE;

	CModel* m_model;// 胴体
	CModel* m_tools;// 道具
	CModel* m_hair;//髪
	COBB m_obb;    //コリジョン
	COBB m_perceptual_area;    //知覚領域
	//自宅
	House* m_house;

	//１日のスケジュール
	std::array<Schedule,24> m_schedule;

	//移動目標
	XMFLOAT3 m_movepos;
	//移動目標キュー
	std::vector<XMFLOAT2> m_moveque;
	//資源記憶
	std::vector<Resource*> m_resource_memory;
	//設置物記憶
	std::vector<Resource*> m_installation_memory;
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

	//行動関数
	void Rest();

	void Sleep();

	void Talk();

	//作業関数(作業をしない場合 falseを返す)
	bool Work_Mine(void);
	bool Work_Carry(void);
	void Emotion(EMOTION emote);
	void DecreaseStamina(float dec);
};