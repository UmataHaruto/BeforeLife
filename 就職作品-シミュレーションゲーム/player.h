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

		ACTION_MAX
	};

	enum class WorkType
	{
		CUT,
		MINE,
		COLLECT,

		WORK_MAX
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

	//�A���A�C�e�����Z�b�g
	void SetCarryItem(ItemType tag,int num);

	//���f���f�[�^���擾
	CModel* GetModel() {
		return m_model;
	}

	//�o�E���f�B���O�X�t�B�A���擾
	COBB GetOBB(void)
	{
		return m_obb;
	}

	//�A�j���[�V�����f�[�^�擾
	AnimationData GetAnimData(void)
	{
		return m_animdata;
	}

	//���O���擾
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

	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}

	//���W���Z�b�g
	void SetPos(XMFLOAT3 pos)
	{
		m_pos = pos;
	}
private:
	void AnimationUpdate();

	//�L�����N�^�[��
	std::string m_first_name;
	std::string m_last_name;
	//����
	GENDER m_gender;

	//�^���A�C�e��
	Souko::Item m_carry;
	bool m_iscarry;

	CModel* m_model;// ����
	CModel* m_tools;// ����
	CModel* m_hair;//��
	COBB m_obb;    //�R���W����

	//�ړ��ڕW
	XMFLOAT3 movepos;

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

	//��Ɗ֐�
	void Work_Mine(void);
};