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
	TPS,    //�O�l�̎��_
	FPS,    //��l�̎��_
};

class Player :public GameObject {
public:
	//�p�[�c
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

	//����
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
		NONE,                   //�s������
		SEARCH_INSTALLATION,    //�ړ��Ώۂֈړ���
		CARRY,                  //�^����
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

	// �\���̌^�^�O
	struct	PlayerInitData {
		Player::PARTS		ParentObjectNo;		// �e�I�u�W�F�N�g�m��
		Player::PARTS		ModelNo;			// ���f���ԍ�
		DirectX::XMFLOAT3	FirstPosition;		// �ŏ��̈ʒu
		DirectX::XMFLOAT3	FirstAngle;			// �ŏ��̊p�x
	};
	//�s���D��x
	struct ActionPriority
	{
		ActionType action;    //�A�N�V�������ʎq
		int priority;    //�D��x
	};

	//��ƗD��x
	struct WorkPriority
	{
		WorkType work;    //�A�N�V�������ʎq
		int priority;    //�D��x
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
		float idledelay;    //Idle�ؑ֒x��
		float timer;    //�^�C�}�[
		AnimationType animno;

	};

	//�����z�u
	static PlayerInitData ObjectInitData[];

	//������
	bool Init();
	bool Init(Data data);

	//�`��
	void Draw();
	void DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);
	void DrawWithAxis();

	//�X�V
	void Update();

	//�I������
	void Finalize();

	//�J�������[�h��ύX
	void SetCameraMode(CameraMode mode);

	//�Z���N�g��Ԃ̕ύX
	void SetSelect(bool input);

	//�ړ���̎w��
	void SetMovePoint(XMFLOAT3 pos);
	//�J�������[�h���擾
	CameraMode GetCameraMode();

	//���F��ݒ�
	void SetHairColor(XMFLOAT4 color)
	{
		m_haircolor = color;
	}

	//�A���A�C�e�����Z�b�g
	void SetCarryItem(ItemType tag,int num);

	//���f���f�[�^���擾
	inline CModel* GetModel() {
		return m_model;
	}

	//�o�E���f�B���O�X�t�B�A���擾
	inline COBB GetOBB(void)
	{
		return m_obb;
	}

	//�A�j���[�V�����f�[�^�擾
	inline AnimationData GetAnimData(void)
	{
		return m_animdata;
	}

	//���O���擾
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

	//���f���Z�b�g
	inline void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	//���W���Z�b�g
	inline void SetPos(XMFLOAT3 pos)
	{
		m_pos = pos;
	}
	//�Ƃ��Z�b�g
	inline void SetHouse(House* house)
	{
		m_house = house;
	}
	//�q�b�g�|�C���g���Z�b�g
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

	//�X�^�~�i���Z�b�g
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

	//�ő�q�b�g�|�C���g���擾
	inline float GetMaxHitpoint()
	{
		return m_hp_max;
	}

	//�ő�X�^�~�i���擾
	inline float GetMaxStamina()
	{
		return m_stamina_max;
	}

	//�q�b�g�|�C���g���擾
	inline float GetHitpoint()
	{
		return m_hp;
	}

	//�X�^�~�i���擾
	inline float GetStamina()
	{
		return m_stamina;
	}

	//�@�����擾
	inline float GetMood()
	{
		return m_mood;
	}

	//�I���󋵂��擾
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

	//�L�����N�^�[��
	std::string m_first_name;
	std::string m_last_name;
	//����
	GENDER m_gender;

	float m_mood;         //�@��(0 �` 100)
	float m_hp_max;       //�ő�q�b�g�|�C���g
	float m_hp;           //�q�b�g�|�C���g
	float m_stamina_max;  //�ő�X�^�~�i
	float m_stamina;      //�X�^�~�i
	XMFLOAT4 m_haircolor; //���F
	bool at_entrance = false;
	bool m_is_sleeping = false;//�������
	bool m_is_talking = false;

	//�^���A�C�e��
	Souko::Item m_carry;
	bool m_iscarry;

	CarryStatus m_carry_status = CarryStatus::NONE;

	CModel* m_model;// ����
	CModel* m_tools;// ����
	CModel* m_hair;//��
	COBB m_obb;    //�R���W����
	COBB m_perceptual_area;    //�m�o�̈�
	//����
	House* m_house;

	//�P���̃X�P�W���[��
	std::array<Schedule,24> m_schedule;

	//�ړ��ڕW
	XMFLOAT3 m_movepos;
	//�ړ��ڕW�L���[
	std::vector<XMFLOAT2> m_moveque;
	//�����L��
	std::vector<Resource*> m_resource_memory;
	//�ݒu���L��
	std::vector<Resource*> m_installation_memory;
	//�ړ���
	bool m_ismoving = false;

	//�I�����
	bool m_isselect = false;

	//�s���D��x
	std::vector<ActionPriority> m_action_priority;

	//��ƗD��x
	std::vector<WorkPriority> m_work_priority;

	CameraMode camera_mode;//�J�������[�h 0 = TPS,1 = FPS
	AnimationData m_animdata;


	void UpdateLocalpose();				// ���[�J���|�[�Y���X�V����
	void CaliculateParentChildMtx();	// �e�q�֌W���l�������s����쐬
	DirectX::XMFLOAT4X4			m_mtxlocalpose[PARTS_MAX];	// �����̂��Ƃ������l�����s��
	DirectX::XMFLOAT4X4			m_mtxParentChild[PARTS_MAX];	// �e�q�֌W���l�������s��

	//�s���֐�
	void Rest();

	void Sleep();

	void Talk();

	//��Ɗ֐�(��Ƃ����Ȃ��ꍇ false��Ԃ�)
	bool Work_Mine(void);
	bool Work_Carry(void);
	void Emotion(EMOTION emote);
	void DecreaseStamina(float dec);
};