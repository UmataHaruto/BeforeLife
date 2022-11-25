#include	"player.h"
#include	"drawaxis.h"
#include    "myimgui.h"
#include    "Application.h"
#include "dx11mathutil.h"
#include "Sprite2DMgr.h"
#include "mousepostoworld.h"
#include "ResourceManager.h"
#include "BuildingMgr.h"

Player::PlayerInitData Player::ObjectInitData[] = {
	{ Player::NONE,				Player::PLAYER_BODY,			{ 0, 0, 0 },				{ 0, 0, 0 } },	// �{��
	{ Player::PLAYER_BODY,		Player::PLAYER_HAIR,			{ 0, 10, 0 },				{ 0, 0, 0 } },	// ���̖�

};

bool Player::Init()
{
	return true;
}

bool Player::Init(Data data) {
	// �s�񏉊���
	DX11MtxIdentity(m_mtx);
	//���̃��f��
	CModel* model = new CModel();
	*model = *ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname);
	m_model = model;
	m_obb.CreateBox(250,1700,250,XMFLOAT3(0,0,0));
	//�����f��
	model = new CModel();
	*model = *ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::HAIR_00)].modelname);
	m_hair = model;	
	//�K�v�ɂȂ�����|�C���^��t�^(null�͑f��)
	m_tools = nullptr;
	XMMATRIX scale = XMMatrixScaling(0.03, 0.03, 0.03);
	XMMATRIX mtx;

    //�A���������Z�b�g
	m_carry.num = 0;
	m_carry.tag = ItemType::ITEM_NONE;

	//�s���D��x�̏�����
	for (int i = 0; i < (int)ActionType::ACTION_MAX; i++)
	{
		ActionPriority priority;
		priority.action = (ActionType)i;
		priority.priority = 1;
		m_action_priority.push_back(priority);
	}
	//��ƗD��x�̏�����
	for (int i = 0; i < (int)WorkType::WORK_MAX; i++)
	{
		WorkPriority priority;
		priority.work = (WorkType)i;
		priority.priority = 1;
		m_work_priority.push_back(priority);
	}

	//�A�j���[�V�����f�[�^�̏�����
	m_animdata.idledelay = 15;
	m_animdata.timer = 0;
	m_animdata.animno = AnimationType::IDLE_00;

	//���W���w��
	SetPos(data.pos);
	m_first_name = data.firstname;
	m_last_name = data.lastname;

	mtx = DirectX::XMLoadFloat4x4(&m_mtx);
	mtx = XMMatrixMultiply(mtx, scale);
	DirectX::XMStoreFloat4x4(&m_mtx, mtx);

	// �v���C���s�񏉊���(�����ʒu)�i�����̂��Ƃ������l�����s����쐬�j
	for (int i = 0; i < static_cast<int>(PARTS::PARTS_MAX) - 2; i++) {
		DX11MakeWorldMatrix(m_mtxlocalpose[i], ObjectInitData[i].FirstAngle, ObjectInitData[i].FirstPosition);
	}

	// �e�q�֌W���l�������s��ύX
	CaliculateParentChildMtx();					//�O���[�o�����W�ɕϊ�

	//�f�o�b�O�p
	m_action_priority[(int)ActionType::WORK].priority = 5;
	m_work_priority[(int)WorkType::MINE].priority = 5;

	return true;
}

void Player::Draw() {

	// ���f���`��
	m_model->Draw(m_mtxParentChild[0]);
	m_hair->Draw(m_mtxParentChild[1]);
	//�f��Ŗ�����
	if (m_tools != nullptr) {
		m_tools->Draw(m_mtxParentChild[2]);
	}

	//���Ebox�\��
	if (!GameButton::GetInstance().GetDebug()) {
		m_obb.Draw();
	}
}

void Player::DrawWithAxis() {

	// ���f���`��
	drawaxis(m_mtx, 50, m_pos);
	Draw();

}

void Player::Update() {

	XMFLOAT3 angle = { 0,0,0 };
	bool keyinput = false;
	ImGuiIO& io = ImGui::GetIO();
	// �}�E�X���W�����[���h���W�ɕϊ�
	XMFLOAT3 g_nearp = { 0,0,0 };
	XMFLOAT3 g_farp = { 0,0,0 };

	//���E���̍X�V
	m_obb.Update(GetMtx());

	//�}�E�X�N���b�N��
	if (m_isselect) {
		if (io.MouseClicked[0]) {
			m_isselect = false;

			XMFLOAT3 g_mousepoint = SearchMousePoint(g_nearp, g_farp);
			Sprite2DMgr::GetInstance().CreateAnimation(
				EFFECTLIST::TARGETCIRCLE,
				g_mousepoint.x,
				g_mousepoint.y,
				g_mousepoint.z,
				30,
				30,
				XMFLOAT4(1, 1, 1, 1),
				nullptr);
			printf("x:%f,y:%f,z:%f\n", g_mousepoint.x, g_mousepoint.y, g_mousepoint.z);

			//�ړ���̎w��
			movepos = XMFLOAT3(g_mousepoint.x, g_mousepoint.y, g_mousepoint.z);
			m_ismoving = true;
		}
	}
	//�m�[�}���V�F�[�_�[�ɕύX
	m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);
	m_hair->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);
	//�I�𔻒�
	if (m_isselect)
	{
		m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NORMAL);
		m_hair->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NORMAL);
	}
	if (CheckMouseSelect(XMFLOAT3(m_mtx._41, m_mtx._42, m_mtx._43))) {
		m_isselect = true;
	}
	//�A�C�e���^�����̏ꍇ
	if (m_carry.num > 0 && m_carry.tag != ItemType::ITEM_NONE)
	{
		m_iscarry = true;
		switch (m_carry.tag)
		{
		case ItemType::WOOD:
			m_tools = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CREATE_WOOD)].modelname);
		break;

		case ItemType::ORE_COAL:
			m_tools = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CREATE_COAL)].modelname);
			break;

		case ItemType::ORE_IRON:
			m_tools = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CREATE_IRON)].modelname);
			break;

		case ItemType::ORE_GOLD:
			m_tools = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CREATE_GOLD)].modelname);
			break;
		default:
			break;
		}
	}

	//�m�肵���s��
	ActionType action = m_action_priority[0].action;
	WorkType work = m_work_priority[0].work;
	
	//�s�����m��
	{
		int index = 0;
		for (int i = 0; i < m_action_priority.size(); i++)
		{
			if (m_action_priority[index].priority < m_action_priority[i].priority)
			{
				action = m_action_priority[i].action;
				index = i;
			}
		}
	}

	//��Ƃ��m��
	{
		int index = 0;
		for (int i = 0; i < m_work_priority.size(); i++)
		{
			if (m_work_priority[index].priority < m_work_priority[i].priority)
			{
				work = m_work_priority[i].work;
				index = i;
			}
		}
	}

	//�s������
	switch (action)
	{
	case Player::ActionType::WORK:
		//��ƕ���
		switch (work)
		{
		case Player::WorkType::CUT:
			break;

		case Player::WorkType::MINE:
			Work_Mine();
			break;

		case Player::WorkType::COLLECT:
			break;

		case Player::WorkType::WORK_MAX:
			break;

		default:
			break;
		}
		break;

	case Player::ActionType::SLEEP:
		break;
		
	case Player::ActionType::ENTERTAINMENT:
		break;

	case Player::ActionType::ACTION_MAX:
		break;

	default:
		break;
	}

	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_SPACE))
	{
		m_animdata.animno = (AnimationType)((int)m_animdata.animno + 1);
		unsigned int animnummax = m_model->GetAnimationNum();
		if ((int)m_animdata.animno >= animnummax) {
			m_animdata.animno = AnimationType::CARRY_IDLE;
		}
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_UP))
	{
		m_pos.z++;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_DOWN))
	{
		m_pos.z--;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_2))
	{
		m_tools = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CREATE_IRON)].modelname);
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_3))
	{
		m_tools = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PICKAXE)].modelname);
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_4))
	{
		m_tools = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::AXE)].modelname);
	}
	XMFLOAT4 axisX;  //X��
	XMFLOAT4 axisY;  //Y��
	XMFLOAT4 axisZ;  //Z��

	//X�������o��
	axisX.x = m_mtx._11;
	axisX.y = m_mtx._12;
	axisX.z = m_mtx._13;
	axisX.w = 0.0f;

	//Y�������o��
	axisY.x = m_mtx._21;
	axisY.y = m_mtx._22;
	axisY.z = m_mtx._23;
	axisX.w = 0.0f;

	//Z�������o��
	axisZ.x = m_mtx._31;
	axisZ.y = m_mtx._32;
	axisZ.z = m_mtx._33;
	axisX.w = 0.0f;

	if (keyinput)
	{
		XMFLOAT4 qt;    //�N�I�[�^�j�I��

		//�s�񂩂�N�I�[�^�j�I���𐶐�
		DX11GetQtfromMatrix(m_mtx,qt);

		XMFLOAT4 qtx, qty, qtz;    //�N�I�[�^�j�I��

		//�w�莲��]�̃N�I�[�^�j�I���𐶐�
		DX11QtRotationAxis(qtx,axisX,angle.x);
		DX11QtRotationAxis(qty, axisY, angle.y);
		DX11QtRotationAxis(qtz, axisZ, angle.z);

		//�N�I�[�^�j�I��������
		XMFLOAT4 tempqt1;
		DX11QtMul(tempqt1, qt, qtx);

		XMFLOAT4 tempqt2;
		DX11QtMul(tempqt2, qty, qtz);

		XMFLOAT4 tempqt3;
		DX11QtMul(tempqt3, tempqt1,tempqt2);

		//�N�I�[�^�j�I�����m�[�}���C�Y
		DX11QtNormalize(tempqt3, tempqt3);

		//�N�I�[�^�j�I������s����쐬
		DX11MtxFromQt(m_mtx, tempqt3);

	}
	//�ړ�����
	static bool Moveinit = false;

	if (m_ismoving && movepos.y > -50 && movepos.y < 50)
	{
		//walk animation
		    m_animdata.animno = AnimationType::WALK;
			XMVECTOR Pos = XMVectorSet(m_pos.x, m_pos.y, -m_pos.z, 0.0f);
			XMVECTOR At = XMVectorSet(movepos.x, movepos.y, -movepos.z, 0.0f);;
			XMVECTOR Up = XMVectorSet(0, 1, 0, 0.0f);
			//Y����]�ȊO��؂�
			ALIGN16 XMMATRIX lookat;
			XMFLOAT4X4 lotateX;
			XMMATRIX LotateXMtx;
			XMFLOAT4X4 lotateZ;
			XMMATRIX LotateZMtx;

			DX11MtxRotationX(0, lotateX);
			LotateXMtx = XMLoadFloat4x4(&lotateX);
			DX11MtxRotationZ(0, lotateZ);
			LotateZMtx = XMLoadFloat4x4(&lotateZ);
			lookat = XMMatrixMultiply(LotateXMtx, lookat);
			lookat = XMMatrixMultiply(LotateZMtx, lookat);

			XMMATRIX scale = XMMatrixScaling(0.03, 0.03, 0.03);
			lookat = XMMatrixLookAtLH(Pos, At, Up);
			lookat = XMMatrixMultiply(lookat, scale);

			XMStoreFloat4x4(&m_mtx, lookat);
        
			//���̍\�����ƃq�b�g���Ă��Ȃ���
			bool hit = false;

			std::vector<COBB> obbs = BuildingMgr::GetInstance().GetAllObb();

			for (int i = 0; i < obbs.size(); i++)
			{
				if (GetOBB().Collision(obbs[i]))
				{
					hit = true;
				}
			}
		if (m_pos.x > movepos.x)
		{
			if (!hit) {
				m_pos.x--;
			}
			else
			{
				m_pos.x++;
			}
		}
		if (m_pos.x < movepos.x)
		{
			if (!hit) {
				m_pos.x++;
			}
			else
			{
				m_pos.x--;
			}		}

		if (m_pos.y > movepos.y)
		{
			if (!hit) {
				m_pos.y--;
			}
			else
			{
				m_pos.y++;
			}
		}
		if (m_pos.y < movepos.y)
		{
			if (!hit) {
				m_pos.y++;
			}
			else
			{
				m_pos.y--;
			}
		}

		if (m_pos.z > movepos.z)
		{
			if (!hit) {
				m_pos.z--;
			}
			else
			{
				m_pos.z++;
			}
		}
		if (m_pos.z < movepos.z)
		{
			if (!hit) {
				m_pos.z++;
			}
			else
			{
				m_pos.z--;
			}
		}

		//�ړI�n�ɓ��B����
		if (fabs(m_pos.x - movepos.x) < 2.0f && fabs(m_pos.y - movepos.y) < 2.0f && fabs(m_pos.z - movepos.z) < 2.0f)
		{
			m_animdata.animno = AnimationType::IDLE_00;
			m_ismoving = false;
			Moveinit = false;
		}
	}

	m_model->Update((int)m_animdata.animno,m_mtx);

	//�A�j���[�V�������X�V
	AnimationUpdate();

	//Z�������o��
	axisZ.x = m_mtx._31;
	axisZ.y = m_mtx._32;
	axisZ.z = m_mtx._33;
	axisX.w = 0.0f;

	m_mtx._41 = m_pos.x;
	m_mtx._42 = m_pos.y;
	m_mtx._43 = m_pos.z;

	// ���[�J���|�[�Y���X�V����
	UpdateLocalpose();

	// �e�q�֌W���l�������s����v�Z����
	CaliculateParentChildMtx();
}

void Player::Finalize() {
	m_model->Uninit();
	m_hair->Uninit();
}

void Player::SetCameraMode(CameraMode mode)
{
	camera_mode = mode;
}

void Player::SetSelect(bool input)
{
	m_isselect = input;
}

CameraMode Player::GetCameraMode()
{
	return camera_mode;
}

void Player::SetCarryItem(ItemType tag, int num)
{
	m_carry.num = num;
	m_carry.tag = tag;
}

void Player::AnimationUpdate()
{
	ImGuiIO& io = ImGui::GetIO();
	const aiScene* s = m_model->m_animationcontainer[m_model->m_AnimFileIdx]->GetScene();
	aiAnimation* animation = s->mAnimations[(int)m_animdata.animno];

	m_animdata.timer += io.DeltaTime;

	//����Idle���
	if (m_animdata.animno == AnimationType::IDLE_00 && m_animdata.timer > m_animdata.idledelay)
	{
		m_animdata.animno = AnimationType::IDLE_02;
		m_animdata.timer = 0;
	}
	//Idle2����߂�
	
	if (m_animdata.animno == AnimationType::IDLE_02 && m_model->GetModelData().m_AnimFrame >= animation->mDuration)
	{
		m_animdata.animno = AnimationType::IDLE_00;
		m_animdata.timer = 0;
	}
}

void Player::UpdateLocalpose()
{
	// �e�p�[�c�̉�]�p�x(�O�t���[������̕ψʗ�)
	XMFLOAT3			partsangle[static_cast<int>(PARTS_MAX)];
	XMFLOAT3			partstrans[static_cast<int>(PARTS_MAX)];

	// �ǂ̃p�[�c�𓮂�����
	int idx = 0;

	// �p�x�ƈړ��ʂ�������
	for (int i = 0; i < static_cast<int>(PARTS_MAX); i++) {
		partsangle[i].x = 0.0f;
		partsangle[i].y = 0.0f;
		partsangle[i].z = 0.0f;
		partstrans[i].x = 0.0f;
		partstrans[i].y = 0.0f;
		partstrans[i].z = 0.0f;
	}

	// �p�[�c�̊p�x���[�J���|�[�Y��\���s����v�Z
	XMFLOAT4X4 partsmtx;
	DX11MakeWorldMatrix(partsmtx, partsangle[idx], partstrans[idx]);
	DX11MtxMultiply(m_mtxlocalpose[idx], partsmtx, m_mtxlocalpose[idx]);
}

void Player::CaliculateParentChildMtx()
{
	m_mtxlocalpose[PLAYER_BODY] = m_mtx;

	// �{��
	m_mtxParentChild[PLAYER_BODY] = m_mtxlocalpose[PLAYER_BODY];

	// ��
	BONE bone = m_model->GetModelData().GetBone("male_adult:Head");
	m_mtxlocalpose[PLAYER_HAIR] = DX11MtxaiToDX(bone.Matrix);
	DX11MtxMultiply(m_mtxParentChild[PLAYER_HAIR], m_mtxlocalpose[PLAYER_HAIR], m_mtxlocalpose[PLAYER_BODY]);
	XMMATRIX scale = XMMatrixScaling(0.03, 0.03, 0.03);
	XMMATRIX mtx;

	//����
	bone = m_model->GetModelData().GetBone("male_adult:LeftHand");
	m_mtxlocalpose[PLAYER_TOOLS] = DX11MtxaiToDX(bone.Matrix);
	DX11MtxMultiply(m_mtxParentChild[PLAYER_TOOLS], m_mtxlocalpose[PLAYER_TOOLS], m_mtxlocalpose[PLAYER_BODY]);

}

void Player::Work_Mine(void)
{
	ImGuiIO& io = ImGui::GetIO();
	//��ԋ߂��̍z�΂𔻒f
	int index = 0;
	float maxlength = 0;
	//�̌@��
	static bool ismine = false;
	//�̌@��������
	static bool mineinit = false;
	static float miningtimer = 0;
	miningtimer += io.DeltaTime;
	//�ŒZ����
	for(int i = 0; i < ResourceManager::GetInstance().m_resources.size(); i++)
	{
		float length;
		DX11p2pLength(m_pos, ResourceManager::GetInstance().m_resources[i]->GetPos(),length);
		if (maxlength > length)
		{
			index = i;
			maxlength = length;
		}
	}
	//�ړ��n�_���w��
	if (ResourceManager::GetInstance().m_resources.size() != 0) {
		m_ismoving = true;
		movepos = ResourceManager::GetInstance().m_resources[index]->GetPos();
		//�ړI�n�ɓ��B����
		if (fabs(m_pos.x - movepos.x) < 20.0f && fabs(m_pos.y - movepos.y) < 20.0f && fabs(m_pos.z - movepos.z) < 20.0f && m_ismoving == true)
		{
			m_ismoving = false;
			ismine = true;
		}

		//�̌@
		if (ismine)
		{
			m_animdata.animno = AnimationType::MINE;
			//��������
			if (!mineinit)
			{
				Sprite2DMgr::GetInstance().CreateCircleProgressBar(ResourceManager::GetInstance().m_resources[index]->GetData(), 15,15);
				mineinit = true;
			}
			else
			{
				//���Ԍo�߂Ń_���[�W��^����
				if (miningtimer >= 0.8)
				{
					miningtimer = 0;
					if (rand() % 5 == 0) {
						ResourceManager::GetInstance().m_resources[index]->HitDamage(10);
						SoundMgr::GetInstance().XA_Play("assets/sound/SE/Mining_01.wav");
						SoundMgr::GetInstance().SetVolume(0.1, "assets/sound/SE/Mining_01.wav");
					}
					else
					{
						ResourceManager::GetInstance().m_resources[index]->HitDamage(5);
						SoundMgr::GetInstance().XA_Play("assets/sound/SE/Mining_00.wav");
						SoundMgr::GetInstance().SetVolume(0.1, "assets/sound/SE/Mining_00.wav");
					}
					if (ResourceManager::GetInstance().m_resources[index]->GetData()->Endurance <= 0)
					{
						ismine = false;
						mineinit = false;

						//�A���A�C�e���̐ݒ�
						Souko::Item item;
						item.tag = ResourceManager::GetInstance().m_resources[index]->GetData()->type;
						item.num = ResourceManager::GetInstance().m_resources[index]->GetData()->amount;
						SetCarryItem(item.tag,item.num);
						m_animdata.animno = AnimationType::CARRY_IDLE;
					}
				}
			}
		}
	}
}
