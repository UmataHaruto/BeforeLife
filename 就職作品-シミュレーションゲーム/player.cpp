#include	"player.h"
#include	"drawaxis.h"
#include    "myimgui.h"
#include    "Application.h"
#include "dx11mathutil.h"
#include "Sprite2DMgr.h"
#include "mousepostoworld.h"
#include "ResourceManager.h"
#include "BuildingMgr.h"
#include "RouteSearch.h"
#include "House.h"

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
	m_obb.Init(m_model);
	m_obb.CreateBox(250, 1700, 250, XMFLOAT3(0, 0, 0));
	//�����f��
	m_hair = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::HAIR_00)].modelname);
	//���F
	m_haircolor.x = (float)(rand() % 255) / 255;
	m_haircolor.y = (float)(rand() % 255) / 255;
	m_haircolor.z = (float)(rand() % 255) / 255;

	//�K�v�ɂȂ�����|�C���^��t�^(null�͑f��)
	m_tools = nullptr;
	XMMATRIX scale = XMMatrixScaling(0.03, 0.03, 0.03);
	XMMATRIX mtx;

	//�A���������Z�b�g
	m_carry.num = 0;
	m_carry.tag = ItemType::ITEM_NONE;

	//�����̉Ƃ�����ꍇ�o�^
	for (int i = 0; i < BuildingMgr::GetInstance().GetHouse().size(); i++)
	{
		if (BuildingMgr::GetInstance().GetHouse()[i]->SetResident(this))
		{
			break;
		}
	}

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
	data.pos.y += 9.5;
	SetPos(data.pos);
	m_first_name = data.firstname;
	m_last_name = data.lastname;

	m_mood = data.mood;
	SetHitpoint(data.hp_max, data.hp);
	SetStamina(data.stamina_max, data.stamina);

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
	m_action_priority[(int)ActionType::REST].priority = 5;
	m_work_priority[(int)WorkType::MINE].priority = 5;

	return true;
}

void Player::Draw() {
	//���F��K�p

	ID3D11DeviceContext* devcontext;
	ID3D11Buffer* cb = GameButton::GetInstance().GetHairConstantBuffer();
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	devcontext->UpdateSubresource(cb,
		0,
		nullptr,
		&m_haircolor,
		0, 0);

	// �R���X�^���g�o�b�t�@4����8���W�X�^�փZ�b�g�i���_�V�F�[�_�[�p�j
	devcontext->VSSetConstantBuffers(8, 1, &cb);
	// �R���X�^���g�o�b�t�@4����8���W�X�^�փZ�b�g(�s�N�Z���V�F�[�_�[�p)
	devcontext->PSSetConstantBuffers(8, 1, &cb);

	// ���f���`��
	m_model->Draw(m_mtxParentChild[0]);
	m_hair->Draw(m_mtxParentChild[1]);
	//�f��Ŗ�����
	if (m_tools != nullptr) {
		m_tools->Draw(m_mtxParentChild[2]);
	}

	static int RoutePreviewDegree = 0;

	if (RoutePreviewDegree < 360) {
		RoutePreviewDegree++;
	}
	else
	{
		RoutePreviewDegree = 0;
	}

	//���Ebox�\��
	if (!GameButton::GetInstance().GetDebug()) {
		m_obb.Draw();
	}
	for (int i = 0; i < moveque.size(); i++)
	{
		XMFLOAT4X4 mtx;
		XMFLOAT4X4 Rotation;
		XMMATRIX RotationMtx;
		XMMATRIX PositionMtx;
		DX11MtxIdentity(mtx);

		mtx._41 = moveque[i].x;
		mtx._42 = m_pos.y;
		mtx._43 = moveque[i].y;

		DX11MtxRotationY(RoutePreviewDegree, Rotation);
		RotationMtx = XMLoadFloat4x4(&Rotation);
		PositionMtx = XMLoadFloat4x4(&mtx);
		PositionMtx = XMMatrixMultiply(RotationMtx, PositionMtx);
		XMStoreFloat4x4(&mtx, PositionMtx);

		ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::ROUTE_PREVIEW)].modelname)->Draw(mtx);
	}
}

void Player::DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in)
{
	// ���f���`��
	m_model->DrawShadow(m_mtxParentChild[0], layout_in, vs_in, ps_in);
	m_hair->DrawShadow(m_mtxParentChild[1], layout_in, vs_in, ps_in);
	//�f��Ŗ�����
	if (m_tools != nullptr) {
		m_tools->DrawShadow(m_mtxParentChild[2], layout_in, vs_in, ps_in);
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
	XMFLOAT4X4 obbmtx = m_mtx;

	m_obb.Update(obbmtx);

	//�}�E�X�N���b�N��
	if (m_isselect) {
		if (io.MouseClicked[0]) {

			XMFLOAT3 g_mousepoint = SearchMousePoint(g_nearp, g_farp);

			if (g_mousepoint.y > -20 && io.MousePos.x > 180 && io.MousePos.y > 150) {
				m_isselect = false;
				Sprite2DMgr::GetInstance().CreateAnimation(
					EFFECTLIST::TARGETCIRCLE,
					g_mousepoint.x,
					g_mousepoint.y,
					g_mousepoint.z,
					30,
					30,
					XMFLOAT4(1, 1, 1, 1),
					nullptr);
				SoundMgr::GetInstance().XA_Play("assets/sound/SE/Click_Walk_00.wav");

				//�ړ���̎w��
				RouteSearch::GetInstance().InitStageCollider();
				movepos = XMFLOAT3(g_mousepoint.x, g_mousepoint.y, g_mousepoint.z);
				moveque = RouteSearch::GetInstance().SearchRoute(m_pos, movepos);
				//�ŏ��̈ړ�����L���[�̍Ō�ɂ���
				if (moveque.size() > 0) {
					XMFLOAT2 backque = moveque[moveque.size() - 1];
					moveque.pop_back();
					movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

					m_ismoving = true;
				}
			}
		}
	}
	//�m�[�}���V�F�[�_�[�ɕύX
	m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);
	//�I�𔻒�
	if (m_isselect)
	{
		m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NORMAL);
	}
	if (CheckMouseSelect(XMFLOAT3(m_mtx._41, m_mtx._42, m_mtx._43))) {
		m_isselect = true;
	}
	//�A�C�e���^�����̏ꍇ
	if (m_carry.num > 0 && m_carry.tag != ItemType::ITEM_NONE)
	{
		m_tools = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CREATE_PLANE)].modelname);
		m_iscarry = true;
	}
	if (m_animdata.animno == AnimationType::CARRY_IDLE && m_carry.num <= 0)
	{
		m_animdata.animno == AnimationType::IDLE_00;
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
	//�s�����Ӗ����Ȃ��Ȃ��ꍇ�x�e��Ԃֈڍs
	bool iswork = false;
	static float timer = 0;
	timer += io.DeltaTime;

	switch (action)
	{
	case Player::ActionType::WORK:
		//��ƕ���
		switch (work)
		{
		case Player::WorkType::CUT:
			break;

		case Player::WorkType::MINE:
			iswork = Work_Mine();
			break;

		case Player::WorkType::COLLECT:
			break;

		case Player::WorkType::CARRY:
			iswork = Work_Carry();
			break;

		case Player::WorkType::CONSTRUCTION:
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

	case Player::ActionType::REST:
		//�Ƃ������ꍇ�͜p�j���Ȃ�
		if (timer > 5 && m_house != nullptr) {
			timer = 0;
			Rest();
		}
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
		DX11GetQtfromMatrix(m_mtx, qt);

		XMFLOAT4 qtx, qty, qtz;    //�N�I�[�^�j�I��

		//�w�莲��]�̃N�I�[�^�j�I���𐶐�
		DX11QtRotationAxis(qtx, axisX, angle.x);
		DX11QtRotationAxis(qty, axisY, angle.y);
		DX11QtRotationAxis(qtz, axisZ, angle.z);

		//�N�I�[�^�j�I��������
		XMFLOAT4 tempqt1;
		DX11QtMul(tempqt1, qt, qtx);

		XMFLOAT4 tempqt2;
		DX11QtMul(tempqt2, qty, qtz);

		XMFLOAT4 tempqt3;
		DX11QtMul(tempqt3, tempqt1, tempqt2);

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
		if (m_carry.num > 0)
		{
			m_animdata.animno = AnimationType::CARRY_RUN;
		}

		if (m_pos.x != movepos.x || m_pos.y != movepos.y || m_pos.z != movepos.z) {

			//�����ꏊ���ړ���Ɠ����ꍇ�U�������OFF
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

		}
		//���̍\�����ƃq�b�g���Ă��Ȃ���
		bool hit = false;

		std::vector<COBB> obbs = BuildingMgr::GetInstance().GetAllObb();

		for (int i = 0; i < obbs.size(); i++)
		{
			if (obbs[i].Collision(m_obb))
			{
				hit = true;
			}
		}
		if (m_pos.x > movepos.x)
		{
			m_pos.x--;
		}
		if (m_pos.x < movepos.x)
		{
			m_pos.x++;
		}
		if (m_pos.y > movepos.y)
		{
			m_pos.y--;
		}
		if (m_pos.y < movepos.y)
		{
			m_pos.y++;
		}

		if (m_pos.z > movepos.z)
		{
			m_pos.z--;
		}
		if (m_pos.z < movepos.z)
		{
			m_pos.z++;

		}

		//�`�F�b�N�|�C���g�ɓ��B����
		if (fabs(m_pos.x - movepos.x) < 2.0f && fabs(m_pos.y - movepos.y) < 2.0f && fabs(m_pos.z - movepos.z) < 2.0f)
		{
			//�ŏ��̈ړ�����L���[�̍Ō�ɂ���
			if (moveque.size() > 0) {
				XMFLOAT2 backque = moveque[moveque.size() - 1];
				moveque.pop_back();
				movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);
			}
		}
		//�ړI�n�ɓ��B
		if (moveque.size() <= 0)
		{
			m_animdata.animno = AnimationType::IDLE_00;
			if (m_carry.num > 0)
			{
				m_animdata.animno = AnimationType::CARRY_IDLE;
			}
			m_ismoving = false;
			Moveinit = false;
		}
	}

	m_model->Update((int)m_animdata.animno, m_mtx);

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

void Player::Rest()
{
	//�ړ����̏ꍇ����Ȃ�
	if (!m_ismoving)
	{
		//�Ƃ̎��ӂ�T��
		XMFLOAT3 move_pos = XMFLOAT3(-999, -999, -999);

		//�ړ��挟�����ő�Q�O��܂Œ��I
		for (int i = 0; i < 20; i++)
		{
			move_pos.x = (rand() % (int)m_house->GetPos().x) + 200;
			move_pos.z = (rand() % (int)m_house->GetPos().z) + 200;
			switch (rand() % 3)
			{
			case 0:
				move_pos.x = -move_pos.x;
				break;
			case 1:
				move_pos.z = -move_pos.z;
				break;
			case 2:
				move_pos.x = -move_pos.x;
				move_pos.z = -move_pos.z;
				break;
			default:
				break;
			}
			if (!RouteSearch::GetInstance().IsHitBuilding(move_pos))
			{
				move_pos.y = m_pos.y;
				break;
			}
		}
		//���[�g�������ł����ꍇ
		if (move_pos.y != -999)
		{
			movepos = move_pos;
			m_ismoving = true;
			//�ړ���̎w��
			moveque = RouteSearch::GetInstance().SearchRoute(m_pos, movepos);
			//�ŏ��̈ړ�����L���[�̍Ō�ɂ���
			if (moveque.size() > 0) {
				XMFLOAT2 backque = moveque[moveque.size() - 1];
				moveque.pop_back();
				movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

				m_ismoving = true;
			}
		}
	}

}

bool Player::Work_Mine(void)
{
	bool iswork = false;
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
	for (int i = 0; i < ResourceManager::GetInstance().m_resources.size(); i++)
	{
		float length;
		DX11p2pLength(m_pos, ResourceManager::GetInstance().m_resources[i]->GetPos(), length);
		if (maxlength > length)
		{
			index = i;
			maxlength = length;
		}
	}

	//�����������ꍇ�A����D��
	if (ResourceManager::GetInstance().m_resources.size() == 0)
	{
		iswork = false;
		Work_Carry();
	}

	//�ړ��n�_���w��
	if (ResourceManager::GetInstance().m_resources.size() != 0 && ismine == false && m_ismoving == false) {
		iswork = true;
		m_ismoving = true;
		movepos = ResourceManager::GetInstance().m_resources[index]->GetPos();

		//�ړ���̎w��
		RouteSearch::GetInstance().InitStageCollider();
		moveque = RouteSearch::GetInstance().SearchRoute(m_pos, movepos);
		//�ŏ��̈ړ�����L���[�̍Ō�ɂ���
		if (moveque.size() > 0) {
			XMFLOAT2 backque = moveque[moveque.size() - 1];
			moveque.pop_back();
			movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

			m_ismoving = true;
		}
	}
	//�ړI�n�ɓ��B����
	if (ResourceManager::GetInstance().m_resources.size() != 0 && m_obb.Collision(*ResourceManager::GetInstance().m_resources[index]->GetOBB()) && moveque.size() == 0)
	{
		//m_ismoving = false;
		ismine = true;
	}
	else
	{
		ismine = false;
	}

	//�̌@
	if (ResourceManager::GetInstance().m_resources.size() != 0) {
		if (ismine && m_obb.Collision(*ResourceManager::GetInstance().m_resources[index]->GetOBB()))
		{
			iswork = true;
			m_animdata.animno = AnimationType::MINE;
			//��������
			if (!mineinit)
			{
				Sprite2DMgr::GetInstance().CreateCircleProgressBar(ResourceManager::GetInstance().m_resources[index]->GetData(), 15, 15);
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
						//�n�ʂɃA�C�e����z�u����
						Resource::Data data = *ResourceManager::GetInstance().m_resources[index]->GetData();
						switch (data.type)
						{
						case ItemType::WOOD:
							ResourceManager::GetInstance().CreateInstallationResource(data, MODELID::CREATE_WOOD);
							break;

						case ItemType::ORE_COAL:
							ResourceManager::GetInstance().CreateInstallationResource(data, MODELID::CREATE_COAL);
							break;

						case ItemType::ORE_IRON:
							ResourceManager::GetInstance().CreateInstallationResource(data, MODELID::CREATE_IRON);
							break;

						case ItemType::ORE_GOLD:
							ResourceManager::GetInstance().CreateInstallationResource(data, MODELID::CREATE_GOLD);
							break;
						default:
							break;
						}
						m_animdata.animno = AnimationType::IDLE_00;

					}
				}
			}
		}
	}
	return iswork;
}

bool Player::Work_Carry(void)
{
	bool iswork = false;
	int index = 0;
	float maxlength = 0;
	static bool routeinit = false;

	switch (m_carry_status)
	{
	case Player::CarryStatus::NONE:
		//�ŒZ����
		for (int i = 0; i < ResourceManager::GetInstance().GetInstallationResource().size(); i++)
		{
			float length;
			DX11p2pLength(m_pos, ResourceManager::GetInstance().GetInstallationResource()[index]->GetPos(), length);
			if (maxlength > length)
			{
				index = i;
				maxlength = length;
			}
		}
		//�ړ��n�_���w��
		if (ResourceManager::GetInstance().GetInstallationResource().size() != 0 && m_ismoving == false) {
			iswork = true;
			m_ismoving = true;
			movepos = ResourceManager::GetInstance().GetInstallationResource()[index]->GetPos();

			//�ړ���̎w��
			RouteSearch::GetInstance().InitStageCollider();
			moveque = RouteSearch::GetInstance().SearchRoute(m_pos, movepos);
			//�ŏ��̈ړ�����L���[�̍Ō�ɂ���
			if (moveque.size() > 0) {
				XMFLOAT2 backque = moveque[moveque.size() - 1];
				moveque.pop_back();
				movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

				m_ismoving = true;
			}
		}

		//�ړI�n�ɓ��B����
		if (ResourceManager::GetInstance().GetInstallationResource().size() != 0) {
			if (m_obb.Collision(*ResourceManager::GetInstance().GetInstallationResource()[index]->GetOBB()) && moveque.size() == 0)
			{
				iswork = true;
				routeinit = false;
				m_ismoving = false;
				m_carry_status = CarryStatus::CARRY;

				//�A���A�C�e���̐ݒ�
				Souko::Item item;
				item.tag = ResourceManager::GetInstance().GetInstallationResource()[index]->GetData()->type;
				item.num = ResourceManager::GetInstance().GetInstallationResource()[index]->GetData()->amount;
				SetCarryItem(item.tag, item.num);
				m_animdata.animno = AnimationType::CARRY_IDLE;
				//�ݒu���폜
				ResourceManager::GetInstance().EraseInstallation(index);
			}
		}
		break;

	case Player::CarryStatus::SEARCH_INSTALLATION:

		break;

	case Player::CarryStatus::CARRY:

		//�A�j���[�V�������Z�b�g
		if (m_ismoving)
		{
			m_animdata.animno = AnimationType::CARRY_RUN;
		}
		else
		{
			m_animdata.animno = AnimationType::CARRY_IDLE;
		}

		//�ŒZ����
		for (int i = 0; i < BuildingMgr::GetInstance().GetSouko().size(); i++)
		{
			float length;
			DX11p2pLength(m_pos, BuildingMgr::GetInstance().GetSouko()[i]->GetEntranceOBB().GetPosition(), length);
			if (maxlength > length)
			{
				index = i;
				maxlength = length;
			}
		}

		//�ړ��n�_���w��
		if (BuildingMgr::GetInstance().GetSouko().size() != 0 && m_ismoving == false && routeinit == false) {
			iswork = true;
			m_ismoving = true;
			movepos = BuildingMgr::GetInstance().GetSouko()[index]->GetEntranceOBB().GetPosition();
			movepos.x += BuildingMgr::GetInstance().GetSouko()[index]->GetEntranceOBB().GetInterval().x;
			movepos.y += BuildingMgr::GetInstance().GetSouko()[index]->GetEntranceOBB().GetInterval().y;
			movepos.z += BuildingMgr::GetInstance().GetSouko()[index]->GetEntranceOBB().GetInterval().z;
			//�ړ���̎w��
			RouteSearch::GetInstance().InitStageCollider();
			moveque = RouteSearch::GetInstance().SearchRoute(m_pos, movepos);
			//�ŏ��̈ړ�����L���[�̍Ō�ɂ���
			if (moveque.size() > 0) {
				routeinit = true;
				XMFLOAT2 backque = moveque[moveque.size() - 1];
				moveque.pop_back();
				movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

				m_ismoving = true;
			}
		}

		//�ړI�n�ɓ��B����
		if (BuildingMgr::GetInstance().GetSouko().size() != 0) {
			if (fabs(m_pos.x - movepos.x) < 20.0f && fabs(m_pos.y - movepos.y) < 20.0f && fabs(m_pos.z - movepos.z) < 20.0f && moveque.size() == 0)
			{
				int remaining = BuildingMgr::GetInstance().GetSouko()[index]->PushItem(m_carry.tag, m_carry.num);

				//�q�ɂɓ���؂�ꍇ
				if (remaining <= 0)
				{
					m_carry.tag = ItemType::ITEM_NONE;
					m_carry.num = 0;
				}
				else
				{
					m_carry.num = remaining;
				}
				m_ismoving = false;
				m_tools = nullptr;
				m_carry_status = CarryStatus::NONE;
				routeinit = false;
			}
		}
		break;

	default:
		break;
	}

	//�q�ɂ����݂��Ȃ��ꍇ
	if (BuildingMgr::GetInstance().GetSouko().size() == 0)
	{
		iswork = false;
	};

	return iswork;
}
