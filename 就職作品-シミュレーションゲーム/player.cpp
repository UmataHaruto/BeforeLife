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
#include "VillagerMgr.h"

Player::PlayerInitData Player::ObjectInitData[] = {
	{ Player::NONE,				Player::PLAYER_BODY,			{ 0, 0, 0 },				{ 0, 0, 0 } },	// 本体
	{ Player::PLAYER_BODY,		Player::PLAYER_HAIR,			{ 0, 10, 0 },				{ 0, 0, 0 } },	// 髪の毛

};

bool Player::Init()
{
	return true;
}

bool Player::Init(Data data) {
	// 行列初期化
	DX11MtxIdentity(m_mtx);
	//胴体モデル
	CModel* model = new CModel();
	*model = *ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname);
	m_model = model;
	m_obb.Init(m_model);
	m_obb.CreateBox(1000, 1700, 1000, XMFLOAT3(0, 0, 0));
	m_perceptual_area.CreateBox(10000,700,10000, XMFLOAT3(0, 0, 0));
	//髪モデル
	m_hair = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::HAIR_00)].modelname);
	//髪色
	m_haircolor.x = (float)(rand() % 255) / 255;
	m_haircolor.y = (float)(rand() % 255) / 255;
	m_haircolor.z = (float)(rand() % 255) / 255;

	//初期スケジュールを設定
	for (int i = 0; i < 7; i++)
	{
		m_schedule[i] = Schedule::SLEEP;
	}
	for (int i = 8; i < 21; i++)
	{
		m_schedule[i] = Schedule::WORK;
	}
	for (int i = 22; i < 24; i++)
	{
		m_schedule[i] = Schedule::SLEEP;
	}
	//必要になったらポインタを付与(nullは素手)
	m_tools = nullptr;
	XMMATRIX scale = XMMatrixScaling(0.03, 0.03, 0.03);
	XMMATRIX mtx;

	//輸送物をリセット
	m_carry.num = 0;
	m_carry.tag = ItemType::ITEM_NONE;

	//既存の家がある場合登録
	for (int i = 0; i < BuildingMgr::GetInstance().GetHouse().size(); i++)
	{
		if (BuildingMgr::GetInstance().GetHouse()[i]->SetResident(this))
		{
			break;
		}
	}

	//行動優先度の初期化
	for (int i = 0; i < (int)ActionType::ACTION_MAX; i++)
	{
		ActionPriority priority;
		priority.action = (ActionType)i;
		priority.priority = 1;
		m_action_priority.push_back(priority);
	}
	//作業優先度の初期化
	for (int i = 0; i < (int)WorkType::WORK_MAX; i++)
	{
		WorkPriority priority;
		priority.work = (WorkType)i;
		priority.priority = 1;
		m_work_priority.push_back(priority);
	}

	//アニメーションデータの初期化
	m_animdata.idledelay = 15;
	m_animdata.timer = 0;
	m_animdata.animno = AnimationType::IDLE_00;

	//座標を指定
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

	// プレイヤ行列初期化(初期位置)（自分のことだけを考えた行列を作成）
	for (int i = 0; i < static_cast<int>(PARTS::PARTS_MAX) - 2; i++) {
		DX11MakeWorldMatrix(m_mtxlocalpose[i], ObjectInitData[i].FirstAngle, ObjectInitData[i].FirstPosition);
	}

	// 親子関係を考慮した行列変更
	CaliculateParentChildMtx();					//グローバル座標に変換

	//デバッグ用
	m_action_priority[(int)ActionType::WORK].priority = 5;
	m_work_priority[(int)WorkType::MINE].priority = 5;

	return true;
}

void Player::Draw() {
	//髪色を適用

	ID3D11DeviceContext* devcontext;
	ID3D11Buffer* cb = GameButton::GetInstance().GetHairConstantBuffer();
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	devcontext->UpdateSubresource(cb,
		0,
		nullptr,
		&m_haircolor,
		0, 0);

	// コンスタントバッファ4をｂ8レジスタへセット（頂点シェーダー用）
	devcontext->VSSetConstantBuffers(8, 1, &cb);
	// コンスタントバッファ4をｂ8レジスタへセット(ピクセルシェーダー用)
	devcontext->PSSetConstantBuffers(8, 1, &cb);

	// モデル描画
	m_model->Draw(m_mtxParentChild[0]);
	m_hair->Draw(m_mtxParentChild[1]);
	//素手で無い時
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

	//境界box表示
	if (!GameButton::GetInstance().GetDebug()) {
		m_obb.Draw();
		m_perceptual_area.Draw();
	}
	if (!m_is_sleeping) {
		for (int i = 0; i < m_moveque.size(); i++)
		{
			XMFLOAT4X4 mtx;
			XMFLOAT4X4 Rotation;
			XMMATRIX RotationMtx;
			XMMATRIX PositionMtx;
			DX11MtxIdentity(mtx);

			mtx._41 = m_moveque[i].x;
			mtx._42 = m_pos.y;
			mtx._43 = m_moveque[i].y;

			DX11MtxRotationY(RoutePreviewDegree, Rotation);
			RotationMtx = XMLoadFloat4x4(&Rotation);
			PositionMtx = XMLoadFloat4x4(&mtx);
			PositionMtx = XMMatrixMultiply(RotationMtx, PositionMtx);
			XMStoreFloat4x4(&mtx, PositionMtx);

			ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::ROUTE_PREVIEW)].modelname)->Draw(mtx);
		}
	}
}

void Player::DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in)
{
	// モデル描画
	m_model->DrawShadow(m_mtxParentChild[0], layout_in, vs_in, ps_in);
	m_hair->DrawShadow(m_mtxParentChild[1], layout_in, vs_in, ps_in);
	//素手で無い時
	if (m_tools != nullptr) {
		m_tools->DrawShadow(m_mtxParentChild[2], layout_in, vs_in, ps_in);
	}
}

void Player::DrawWithAxis() {

	// モデル描画
	drawaxis(m_mtx, 50, m_pos);
	Draw();

}

void Player::Update() {

	XMFLOAT3 angle = { 0,0,0 };
	bool keyinput = false;
	ImGuiIO& io = ImGui::GetIO();
	// マウス座標をワールド座標に変換
	XMFLOAT3 g_nearp = { 0,0,0 };
	XMFLOAT3 g_farp = { 0,0,0 };

	//境界球の更新
	m_obb.Update(m_mtx);
	m_perceptual_area.Update(m_mtx);

	//マウスクリック時
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

				//移動先の指定
				RouteSearch::GetInstance().InitStageCollider();
				m_movepos = XMFLOAT3(g_mousepoint.x, g_mousepoint.y, g_mousepoint.z);
				m_moveque = RouteSearch::GetInstance().SearchRoute(m_pos, m_movepos);
				//最初の移動先をキューの最後にする
				if (m_moveque.size() > 0) {
					XMFLOAT2 backque = m_moveque[m_moveque.size() - 1];
					m_moveque.pop_back();
					m_movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

					m_ismoving = true;
				}
			}
		}
	}
	//ノーマルシェーダーに変更
	m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NONE);
	//選択判定
	if (m_isselect)
	{
		m_model->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_NORMAL);
	}
	if (CheckMouseSelect(XMFLOAT3(m_mtx._41, m_mtx._42, m_mtx._43))) {
		m_isselect = true;
	}

	if (m_animdata.animno == AnimationType::CARRY_IDLE && m_carry.num <= 0)
	{
		m_animdata.animno == AnimationType::IDLE_00;
	}

	//感知領域に触れた資源を記憶する
	for (int i = 0; i < ResourceManager::GetInstance().m_resources.size(); i++)
	{
		if (m_ismoving&&m_perceptual_area.Collision(*ResourceManager::GetInstance().m_resources[i]->GetOBB()))
		{

			//既存の記憶が無いかチェック
			bool collect = false;
			for (int j = 0; j < m_resource_memory.size(); j++)
			{
				if (m_resource_memory[j] == ResourceManager::GetInstance().m_resources[i])
				{
					collect = true;
					break;
				}
			}
			if (!collect)
			{
				//記憶を追加
				m_resource_memory.push_back(ResourceManager::GetInstance().m_resources[i]);
			}
		}
	}
	for (int i = 0; i < ResourceManager::GetInstance().GetInstallationResource().size(); i++)
	{
		if (m_ismoving && m_perceptual_area.Collision(*ResourceManager::GetInstance().GetInstallationResource()[i]->GetOBB()))
		{
			//既存の記憶が無いかチェック
			bool collect = false;

			for (int j = 0; j < m_installation_memory.size(); j++)
			{
				if (m_installation_memory[j] == ResourceManager::GetInstance().GetInstallationResource()[i])
				{
					collect = true;
					break;
				}
			}
			if (!collect)
			{
				//記憶を追加
				m_installation_memory.push_back(ResourceManager::GetInstance().GetInstallationResource()[i]);
			}
		}
	}

	//確定した行動
	ActionType action = m_action_priority[0].action;
	WorkType work = m_work_priority[0].work;

	//行動優先度の初期化
	m_action_priority.clear();
	for (int i = 0; i < (int)ActionType::ACTION_MAX; i++)
	{
		ActionPriority priority;
		priority.action = (ActionType)i;
		priority.priority = 1;
		m_action_priority.push_back(priority);
	}

	//行動を確定(スケジュール参照)
	{
		int time = Application::GAME_TIME / 60;
		switch (m_schedule[time])
		{
		case Schedule::WORK:
			m_action_priority[(int)ActionType::WORK].priority = 5;
			break;
		case Schedule::SLEEP:
			action = ActionType::SLEEP;
			m_action_priority[(int)ActionType::SLEEP].priority = 5;
			break;
		case Schedule::FREE:
			action = ActionType::REST;
			m_action_priority[(int)ActionType::REST].priority = 5;
			break;
		default:
			break;
		}
	}

	//行動を確定
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

	//作業を確定
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

	//アイテム運送中の場合
	if (m_carry.num > 0 && m_carry.tag != ItemType::ITEM_NONE)
	{
		m_tools = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CREATE_PLANE)].modelname);
		m_iscarry = true;
		//アイテムの輸送を最優先とする
		work = WorkType::CARRY;
		m_carry_status = CarryStatus::CARRY;
	}

	//行動分岐
	//行動が意味をなさない場合休憩状態へ移行
	bool iswork = false;
	static float timer = 0;
	timer += io.DeltaTime;

	switch (action)
	{
	case Player::ActionType::WORK:
		m_is_sleeping = false;
		at_entrance = false;

		//作業分岐
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
		Sleep();
		break;

	case Player::ActionType::ENTERTAINMENT:
		break;

	case Player::ActionType::REST:
		//会話クールタイムが無い場合
		if (m_talk_cooltime > 0)
		{
			m_talk_cooltime -= io.DeltaTime;
			if (m_talk_cooltime < 0)
			{
				m_talk_cooltime = 0;
			}
		}
		if (m_talk_cooltime == 0) {
			//会話
			Talk();
		}
		else
		{
			//家が無い場合は徘徊しない
			if (timer > 5 && m_house != nullptr) {
				timer = 0;
				Rest();
			}
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

		XMFLOAT4 axisX;  //X軸
		XMFLOAT4 axisY;  //Y軸
		XMFLOAT4 axisZ;  //Z軸

		//X軸を取り出す
		axisX.x = m_mtx._11;
		axisX.y = m_mtx._12;
		axisX.z = m_mtx._13;
		axisX.w = 0.0f;

		//Y軸を取り出す
		axisY.x = m_mtx._21;
		axisY.y = m_mtx._22;
		axisY.z = m_mtx._23;
		axisX.w = 0.0f;

		//Z軸を取り出す
		axisZ.x = m_mtx._31;
		axisZ.y = m_mtx._32;
		axisZ.z = m_mtx._33;
		axisX.w = 0.0f;
		//睡眠時,会話時のみ行動不能
		if (!m_is_sleeping || !m_is_talking)
		{

		if (keyinput)
		{
			XMFLOAT4 qt;    //クオータニオン

			//行列からクオータニオンを生成
			DX11GetQtfromMatrix(m_mtx, qt);

			XMFLOAT4 qtx, qty, qtz;    //クオータニオン

			//指定軸回転のクオータニオンを生成
			DX11QtRotationAxis(qtx, axisX, angle.x);
			DX11QtRotationAxis(qty, axisY, angle.y);
			DX11QtRotationAxis(qtz, axisZ, angle.z);

			//クオータニオンを合成
			XMFLOAT4 tempqt1;
			DX11QtMul(tempqt1, qt, qtx);

			XMFLOAT4 tempqt2;
			DX11QtMul(tempqt2, qty, qtz);

			XMFLOAT4 tempqt3;
			DX11QtMul(tempqt3, tempqt1, tempqt2);

			//クオータニオンをノーマライズ
			DX11QtNormalize(tempqt3, tempqt3);

			//クオータニオンから行列を作成
			DX11MtxFromQt(m_mtx, tempqt3);

		}
		//移動処理
		static bool Moveinit = false;

		if (m_ismoving && m_movepos.y > -50 && m_movepos.y < 50)
		{
			//walk animation
			m_animdata.animno = AnimationType::WALK;
			if (m_carry.num > 0)
			{
				m_animdata.animno = AnimationType::CARRY_RUN;
			}

			if (m_pos.x != m_movepos.x || m_pos.y != m_movepos.y || m_pos.z != m_movepos.z) {

				//向く場所が移動先と同じ場合振り向きをOFF
				XMVECTOR Pos = XMVectorSet(m_pos.x, m_pos.y, -m_pos.z, 0.0f);
				XMVECTOR At = XMVectorSet(m_movepos.x, m_movepos.y, -m_movepos.z, 0.0f);
				XMVECTOR Up = XMVectorSet(0, 1, 0, 0.0f);

				//Y軸回転以外を切る
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
			//他の構造物とヒットしていないか
			bool hit = false;

			std::vector<COBB> obbs = BuildingMgr::GetInstance().GetAllObb();

			for (int i = 0; i < obbs.size(); i++)
			{
				if (obbs[i].Collision(m_obb))
				{
					hit = true;
				}
			}
			if (m_pos.x > m_movepos.x)
			{
				m_pos.x--;
			}
			if (m_pos.x < m_movepos.x)
			{
				m_pos.x++;
			}
			if (m_pos.y > m_movepos.y)
			{
				m_pos.y--;
			}
			if (m_pos.y < m_movepos.y)
			{
				m_pos.y++;
			}

			if (m_pos.z > m_movepos.z)
			{
				m_pos.z--;
			}
			if (m_pos.z < m_movepos.z)
			{
				m_pos.z++;

			}

			//チェックポイントに到達した
			if (fabs(m_pos.x - m_movepos.x) < 2.0f && fabs(m_pos.y - m_movepos.y) < 2.0f && fabs(m_pos.z - m_movepos.z) < 2.0f)
			{
				//最初の移動先をキューの最後にする
				if (m_moveque.size() > 0) {
					XMFLOAT2 backque = m_moveque[m_moveque.size() - 1];
					m_moveque.pop_back();
					m_movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);
				}
				//スタミナ消費
				DecreaseStamina(0.03);

			}
			//目的地に到達
			if (m_moveque.size() <= 0)
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
	}
	m_model->Update((int)m_animdata.animno, m_mtx);

	//アニメーションを更新
	AnimationUpdate();

	//Z軸を取り出す
	axisZ.x = m_mtx._31;
	axisZ.y = m_mtx._32;
	axisZ.z = m_mtx._33;
	axisX.w = 0.0f;

	m_mtx._41 = m_pos.x;
	m_mtx._42 = m_pos.y;
	m_mtx._43 = m_pos.z;

	// ローカルポーズを更新する
	UpdateLocalpose();

	// 親子関係を考慮した行列を計算する
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

	//現在Idle状態
	if (m_animdata.animno == AnimationType::IDLE_00 && m_animdata.timer > m_animdata.idledelay)
	{
		m_animdata.animno = AnimationType::IDLE_02;
		m_animdata.timer = 0;
	}
	//Idle2から戻る

	if (m_animdata.animno == AnimationType::IDLE_02 && m_model->GetModelData().m_AnimFrame >= animation->mDuration)
	{
		m_animdata.animno = AnimationType::IDLE_00;
		m_animdata.timer = 0;
	}
}

void Player::UpdateLocalpose()
{
	// 各パーツの回転角度(前フレームからの変位量)
	XMFLOAT3			partsangle[static_cast<int>(PARTS_MAX)];
	XMFLOAT3			partstrans[static_cast<int>(PARTS_MAX)];

	// どのパーツを動かすか
	int idx = 0;

	// 角度と移動量を初期化
	for (int i = 0; i < static_cast<int>(PARTS_MAX); i++) {
		partsangle[i].x = 0.0f;
		partsangle[i].y = 0.0f;
		partsangle[i].z = 0.0f;
		partstrans[i].x = 0.0f;
		partstrans[i].y = 0.0f;
		partstrans[i].z = 0.0f;
	}

	// パーツの角度ローカルポーズを表す行列を計算
	XMFLOAT4X4 partsmtx;
	DX11MakeWorldMatrix(partsmtx, partsangle[idx], partstrans[idx]);
	DX11MtxMultiply(m_mtxlocalpose[idx], partsmtx, m_mtxlocalpose[idx]);
}

void Player::CaliculateParentChildMtx()
{
	m_mtxlocalpose[PLAYER_BODY] = m_mtx;

	// 本体
	m_mtxParentChild[PLAYER_BODY] = m_mtxlocalpose[PLAYER_BODY];

	// 髪
	BONE bone = m_model->GetModelData().GetBone("male_adult:Head");
	m_mtxlocalpose[PLAYER_HAIR] = DX11MtxaiToDX(bone.Matrix);
	DX11MtxMultiply(m_mtxParentChild[PLAYER_HAIR], m_mtxlocalpose[PLAYER_HAIR], m_mtxlocalpose[PLAYER_BODY]);
	XMMATRIX scale = XMMatrixScaling(0.03, 0.03, 0.03);
	XMMATRIX mtx;

	//道具
	bone = m_model->GetModelData().GetBone("male_adult:LeftHand");
	m_mtxlocalpose[PLAYER_TOOLS] = DX11MtxaiToDX(bone.Matrix);
	DX11MtxMultiply(m_mtxParentChild[PLAYER_TOOLS], m_mtxlocalpose[PLAYER_TOOLS], m_mtxlocalpose[PLAYER_BODY]);

}

void Player::Rest()
{
	//移動中の場合,家無しは入らない
	if (!m_ismoving && m_house != nullptr)
	{
		//家の周辺を探索
		XMFLOAT3 move_pos = XMFLOAT3(-999, -999, -999);

		//移動先検索を最大２０回まで抽選
		for (int i = 0; i < 20; i++)
		{
			move_pos.x = (rand() % (int)m_house->GetPos().x) + 100;
			move_pos.z = (rand() % (int)m_house->GetPos().z) + 100;
			switch (rand() % 3)
			{
			case 0:
				move_pos.x = move_pos.x - ((move_pos.x - m_house->GetPos().x)*2);
				break;
			case 1:
				move_pos.z = move_pos.z - ((move_pos.z - m_house->GetPos().z) * 2);
				break;
			case 2:
				move_pos.x = move_pos.x - ((move_pos.x - m_house->GetPos().x) * 2);
				move_pos.z = move_pos.z - ((move_pos.z - m_house->GetPos().z) * 2);
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
		//ルートが発見できた場合
		if (move_pos.y != -999)
		{
			m_movepos = move_pos;
			m_ismoving = true;
			//移動先の指定
			RouteSearch::GetInstance().InitStageCollider();
			m_moveque = RouteSearch::GetInstance().SearchRoute(m_pos, m_movepos);
			//最初の移動先をキューの最後にする
			if (m_moveque.size() > 0) {
				XMFLOAT2 backque = m_moveque[m_moveque.size() - 1];
				m_moveque.pop_back();
				m_movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

				m_ismoving = true;
			}
		}
	}

}

bool Player::Work_Mine(void)
{
	bool iswork = false;
	ImGuiIO& io = ImGui::GetIO();
	//一番近くの鉱石を判断
	int index = 0;
	float maxlength = 0;
	//採掘中
	static bool ismine = false;
	//採掘初期処理
	static bool mineinit = false;
	static float miningtimer = 0;
	miningtimer += io.DeltaTime;
	//最短距離
	for (int i = 0; i < m_resource_memory.size(); i++)
	{
		float length;
		DX11p2pLength(m_pos, m_resource_memory[i]->GetPos(), length);
		if (maxlength > length)
		{
			index = i;
			maxlength = length;
		}
	}

	//資源が無い場合探索を優先
	if (m_resource_memory.size() == 0)
	{
		iswork = false;
		Rest();
	}

	//移動地点を指定
	if (m_resource_memory.size() != 0 && ismine == false && m_ismoving == false) {
		iswork = true;
		m_ismoving = true;
		m_movepos = m_resource_memory[index]->GetPos();

		//移動先の指定
		RouteSearch::GetInstance().InitStageCollider();
		m_moveque = RouteSearch::GetInstance().SearchRoute(m_pos, m_movepos);
		//最初の移動先をキューの最後にする
		if (m_moveque.size() > 0) {
			XMFLOAT2 backque = m_moveque[m_moveque.size() - 1];
			m_moveque.pop_back();
			m_movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

			m_ismoving = true;
		}
	}
	//目的地に到達した
	if (m_resource_memory.size() != 0 && m_obb.Collision(*m_resource_memory[index]->GetOBB()) && m_moveque.size() == 0)
	{
		//m_ismoving = false;
		ismine = true;
	}
	else
	{
		ismine = false;
	}

	//採掘
	if (m_resource_memory.size() != 0) {
		if (ismine && m_obb.Collision(*m_resource_memory[index]->GetOBB()))
		{
			iswork = true;
			m_animdata.animno = AnimationType::MINE;
			//初期処理
			if (!mineinit)
			{
				Sprite2DMgr::GetInstance().CreateCircleProgressBar(m_resource_memory[index]->GetData(), 15, 15);
				mineinit = true;
			}
			else
			{
				//時間経過でダメージを与える
				if (miningtimer >= 0.8)
				{
					miningtimer = 0;
					if (rand() % 5 == 0) {
						m_resource_memory[index]->HitDamage(10);
						SoundMgr::GetInstance().XA_Play("assets/sound/SE/Mining_01.wav");
						SoundMgr::GetInstance().SetVolume(0.1, "assets/sound/SE/Mining_01.wav");
					}
					else
					{
						m_resource_memory[index]->HitDamage(5);
						SoundMgr::GetInstance().XA_Play("assets/sound/SE/Mining_00.wav");
						SoundMgr::GetInstance().SetVolume(0.1, "assets/sound/SE/Mining_00.wav");
					}
					if (m_resource_memory[index]->GetData()->Endurance <= 0)
					{
						ismine = false;
						mineinit = false;
						//地面にアイテムを配置する
						Resource::Data data = *m_resource_memory[index]->GetData();
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
						m_resource_memory.erase(m_resource_memory.begin() + index);
						//感情エモートを生成
						Emotion(EMOTION::NONE);
						//スタミナ消費
						DecreaseStamina(3);

					}
				}
			}
		}
	}
	return iswork;
}

bool Player::Work_Carry(void)
{
	ImGuiIO& io = ImGui::GetIO();
	bool iswork = false;
	int index = 0;
	float maxlength = 0;
	static bool routeinit = false;

	switch (m_carry_status)
	{
	case Player::CarryStatus::NONE:
		//最短距離
		for (int i = 0; i < m_installation_memory.size(); i++)
		{
			float length;
			DX11p2pLength(m_pos, m_installation_memory[index]->GetPos(), length);
			if (maxlength > length)
			{
				index = i;
				maxlength = length;
			}
		}
		//記憶に無い場合は探索する
		if (m_installation_memory.size() == 0)
		{
			Rest();
			static float RestTimer = 0;
			RestTimer += io.DeltaTime;
			if (RestTimer >= 5)
			{
				RestTimer = 0;
				Emotion(EMOTION::QUESTION);
			}
		}
		//移動地点を指定
		if (m_installation_memory.size() != 0 && m_ismoving == false) {
			iswork = true;
			m_ismoving = true;
			m_movepos = m_installation_memory[index]->GetPos();

			//移動先の指定
			RouteSearch::GetInstance().InitStageCollider();
			m_moveque = RouteSearch::GetInstance().SearchRoute(m_pos, m_movepos);
			//最初の移動先をキューの最後にする
			if (m_moveque.size() > 0) {
				XMFLOAT2 backque = m_moveque[m_moveque.size() - 1];
				m_moveque.pop_back();
				m_movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

				m_ismoving = true;
			}
		}

		//目的地に到達した
		if (m_installation_memory.size() != 0) {
			if (m_obb.Collision(*m_installation_memory[index]->GetOBB()) && m_moveque.size() == 0)
			{
				iswork = true;
				routeinit = false;
				m_ismoving = false;
				m_carry_status = CarryStatus::CARRY;

				//輸送アイテムの設定
				Souko::Item item;
				item.tag = m_installation_memory[index]->GetData()->type;
				item.num = m_installation_memory[index]->GetData()->amount;
				SetCarryItem(item.tag, item.num);
				//設置物削除
				ResourceManager::GetInstance().EraseInstallation(m_installation_memory[index]);
				m_installation_memory.erase(m_installation_memory.begin() + index);
				m_animdata.animno = AnimationType::CARRY_IDLE;
			}
		}
		break;

	case Player::CarryStatus::SEARCH_INSTALLATION:

		break;

	case Player::CarryStatus::CARRY:

		//アニメーションをセット
		if (m_ismoving)
		{
			m_animdata.animno = AnimationType::CARRY_RUN;
		}
		else
		{
			m_animdata.animno = AnimationType::CARRY_IDLE;
		}

		//最短距離
		//０番目の倉庫が最大の場合のみindexを空にする
		if ((BuildingMgr::GetInstance().GetSouko().size() > 0 &&
			BuildingMgr::GetInstance().GetSouko()[0]->GetData().items[(int)m_carry.tag].num >= BuildingMgr::GetInstance().GetSouko()[0]->GetData().store_max))
		{
			index = -1;
		}
		for (int i = 0; i < BuildingMgr::GetInstance().GetSouko().size(); i++)
		{
			//納品先の倉庫が容量オーバーでない
			if (BuildingMgr::GetInstance().GetSouko()[i]->GetAllItemNum() < BuildingMgr::GetInstance().GetSouko()[i]->GetData().store_max)
			{
				float length;
				DX11p2pLength(m_pos, BuildingMgr::GetInstance().GetSouko()[i]->GetEntranceOBB().GetPosition(), length);
				//indexが空の場合は挿入
				if (index < 0)
				{
					index = i;
					maxlength = length;
				}
				if (maxlength > length)
				{
					index = i;
					maxlength = length;
				}
			}
		}

		//格納先が存在しない
		if (index < 0)
		{
			return false;
		}

		//移動地点を指定
		if (BuildingMgr::GetInstance().GetSouko().size() != 0 && m_ismoving == false && routeinit == false) {
			iswork = true;
			m_ismoving = true;
			m_movepos = BuildingMgr::GetInstance().GetSouko()[index]->GetEntranceOBB().GetPosition();
			m_movepos.x += BuildingMgr::GetInstance().GetSouko()[index]->GetEntranceOBB().GetInterval().x;
			m_movepos.y += BuildingMgr::GetInstance().GetSouko()[index]->GetEntranceOBB().GetInterval().y;
			m_movepos.z += BuildingMgr::GetInstance().GetSouko()[index]->GetEntranceOBB().GetInterval().z;
			//移動先の指定
			RouteSearch::GetInstance().InitStageCollider();
			m_moveque = RouteSearch::GetInstance().SearchRoute(m_pos, m_movepos);
			//最初の移動先をキューの最後にする
			if (m_moveque.size() > 0) {
				routeinit = true;
				XMFLOAT2 backque = m_moveque[m_moveque.size() - 1];
				m_moveque.pop_back();
				m_movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

				m_ismoving = true;
			}
		}

		//目的地に到達した
		if (BuildingMgr::GetInstance().GetSouko().size() != 0) {
			if (fabs(m_pos.x - m_movepos.x) < 20.0f && fabs(m_pos.y - m_movepos.y) < 20.0f && fabs(m_pos.z - m_movepos.z) < 20.0f && m_moveque.size() == 0)
			{
				//感情エモートを生成
				Emotion(EMOTION::NONE);
                //スタミナ消費
				DecreaseStamina(2);
				int remaining = BuildingMgr::GetInstance().GetSouko()[index]->PushItem(m_carry.tag, m_carry.num);

				//倉庫に入り切る場合
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

	//倉庫が存在しない場合
	if (BuildingMgr::GetInstance().GetSouko().size() == 0)
	{
		iswork = false;
	};

	return iswork;
}

void Player::Emotion(EMOTION emote)
{

	EFFECTLIST hukidashi = EFFECTLIST::HUKIDASHI_QUESTION;

	switch (emote)
	{
	case Player::EMOTION::NONE:
		//体調に応じて出力
		if (m_stamina / m_stamina_max > 0.6)
		{
			hukidashi = EFFECTLIST::HUKIDASHI_ONPU;
		}
		else
		{
			hukidashi = EFFECTLIST::HUKIDASHI_ASE;
		}
		break;

	case Player::EMOTION::HEART:
		hukidashi = EFFECTLIST::HUKIDASHI_HEART;
		break;

	case Player::EMOTION::SWEATING:
		hukidashi = EFFECTLIST::HUKIDASHI_ASE;
		break;

	case Player::EMOTION::FANNY:
		hukidashi = EFFECTLIST::HUKIDASHI_ONPU;
		break;

	case Player::EMOTION::TALK:
		hukidashi = EFFECTLIST::HUKIDASHI_TALK;
		break;

	case Player::EMOTION::QUESTION:
		hukidashi = EFFECTLIST::HUKIDASHI_QUESTION;
		break;

	case Player::EMOTION::EMOTION_MAX:
		break;

	default:
		break;
	}

	//感情エモートを生成
	Sprite2DMgr::GetInstance().CreateHormingEffect(
		hukidashi,
		m_pos.x,
		m_pos.y,
		m_pos.z,
		20,
		20,
		XMFLOAT4(1, 1, 1, 1),
		&m_pos,
		XMFLOAT3(0, 20, 0));
}

void Player::DecreaseStamina(float dec)
{
	if (m_stamina - dec < 0)
	{
		m_stamina = 0;
	}
	else
	{
		m_stamina -= dec;
	}
}

void Player::Sleep()
{
	ImGuiIO& io = ImGui::GetIO();

	//移動地点を指定
	if (m_house != nullptr && m_ismoving == false && !m_is_sleeping) {
		m_ismoving = true;
		m_movepos = m_house->GetEntrance().GetPosition();
		m_movepos.x += m_house->GetEntrance().GetInterval().x;
		m_movepos.y += m_house->GetEntrance().GetInterval().y;
		m_movepos.z += m_house->GetEntrance().GetInterval().z;

		//移動先の指定
		RouteSearch::GetInstance().InitStageCollider();
		m_moveque = RouteSearch::GetInstance().SearchRoute(m_pos, m_movepos);
		//最初の移動先をキューの最後にする
		if (m_moveque.size() > 0) {
			XMFLOAT2 backque = m_moveque[m_moveque.size() - 1];
			m_moveque.pop_back();
			m_movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

			m_ismoving = true;
		}
	}
	//目的地に到達した
	if (m_house != nullptr && m_moveque.size() == 0 && !m_is_sleeping)
	{
		at_entrance = true;
	}
	//エントランスに到着
	if (at_entrance && !m_is_sleeping)
	{
		m_movepos = m_house->GetPos();
		if (m_pos.x != m_movepos.x || m_pos.y != m_movepos.y || m_pos.z != m_movepos.z) {

			//向く場所が移動先と同じ場合振り向きをOFF
			XMVECTOR Pos = XMVectorSet(m_pos.x, m_pos.y, -m_pos.z, 0.0f);
			XMVECTOR At = XMVectorSet(m_movepos.x, m_movepos.y, -m_movepos.z, 0.0f);;
			XMVECTOR Up = XMVectorSet(0, 1, 0, 0.0f);

			//Y軸回転以外を切る
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
	
		if (m_pos.x > m_movepos.x)
		{
			m_pos.x--;
		}
		if (m_pos.x < m_movepos.x)
		{
			m_pos.x++;
		}
		if (m_pos.y > m_movepos.y)
		{
			m_pos.y--;
		}
		if (m_pos.y < m_movepos.y)
		{
			m_pos.y++;
		}

		if (m_pos.z > m_movepos.z)
		{
			m_pos.z--;
		}
		if (m_pos.z < m_movepos.z)
		{
			m_pos.z++;
		}
		if (fabs(m_pos.x - m_movepos.x) < 2.0f && fabs(m_pos.y - m_movepos.y) < 2.0f && fabs(m_pos.z - m_movepos.z) < 2.0f)
		{
			m_is_sleeping = true;
		}
	}
	//睡眠中
	if (m_is_sleeping)
	{
		//回復
		if (m_hp + io.DeltaTime > m_hp_max)
		{
			m_hp = m_hp_max;
		}
		else
		{
			m_hp += io.DeltaTime;
		}
		//回復
		if (m_stamina + io.DeltaTime > m_stamina_max)
		{
			m_stamina = m_stamina_max;
		}
		else
		{
			m_stamina += io.DeltaTime;
		}
		//睡眠時間が終了間近になった場合
		int index = Application::GAME_TIME / 60;
		for (int i = index; i < m_schedule.size(); i++)
		{
			if (m_schedule[i] == Schedule::SLEEP)
			{
				index++;
			}
			else
			{
				break;
			}
		}
		int sleep_limit = ((index - 1) * 60) + 59;
		if (Application::GAME_TIME >= sleep_limit)
		{
			//玄関にテレポート
			m_pos = m_house->GetEntrance().GetPosition();
			m_pos.x += m_house->GetEntrance().GetInterval().x;
			m_pos.y += m_house->GetEntrance().GetInterval().y;
			m_pos.z += m_house->GetEntrance().GetInterval().z;
			m_is_sleeping = false;
			at_entrance = false;
			m_moveque.clear();
		}
	}
}

void Player::Talk()
{
	ImGuiIO& io = ImGui::GetIO();
	static int talk_start_time = 0;

	static float talk_interval = 0;
    
	talk_interval += io.DeltaTime;

	//set animation
	if (m_is_talking)
	{
		//会話対象が消えた場合は処理しない
		if (talk_target == nullptr)
		{
			return void();
		}

		m_animdata.animno = AnimationType::TALK;

		//お互いを見る
		XMVECTOR Pos = XMVectorSet(m_pos.x, m_pos.y, -m_pos.z, 0.0f);
		XMVECTOR At = XMVectorSet(talk_target->GetPos().x, talk_target->GetPos().y, -talk_target->GetPos().z, 0.0f);
		XMVECTOR Up = XMVectorSet(0, 1, 0, 0.0f);

		//Y軸回転以外を切る
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

	if (talk_interval > 3)
	{
		talk_interval = 0;
		//会話対象を設定
		if (!m_is_talking)
		{
			WorkType work = WorkType::WORK_MAX;
			WorkType work_target = WorkType::WORK_MAX;
			ActionType action_target = ActionType::ACTION_MAX;
			//優先作業を判別
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
			for (int i = 0; i < VillagerMgr::GetInstance().m_villager.size(); i++)
			{
				int index = 0;
				//優先作業を判別
				for (int j = 0; j < VillagerMgr::GetInstance().m_villager[i]->m_work_priority.size(); j++)
				{
					if (VillagerMgr::GetInstance().m_villager[i]->m_work_priority[index].priority < VillagerMgr::GetInstance().m_villager[i]->m_work_priority[j].priority)
					{
						index = j;
					}
				}
				work_target = VillagerMgr::GetInstance().m_villager[i]->m_work_priority[index].work;
				index = 0;

				//優先作業を判別
				for (int j = 0; j < VillagerMgr::GetInstance().m_villager[i]->m_action_priority.size(); j++)
				{
					if (VillagerMgr::GetInstance().m_villager[i]->m_action_priority[index].priority < VillagerMgr::GetInstance().m_villager[i]->m_action_priority[j].priority)
					{
						index = j;
					}
				}
				action_target = VillagerMgr::GetInstance().m_villager[i]->m_action_priority[index].action;

				//対象の行動優先が同じかつ休憩状態
				if (this->m_first_name != VillagerMgr::GetInstance().m_villager[i]->m_first_name && work == work_target && action_target == ActionType::REST)
				{
					talk_target = VillagerMgr::GetInstance().m_villager[i];
				}
			}
			//会話相手が存在する場合
			if (talk_target != nullptr)
			{
				//会話前の場合
				if (!m_is_talking)
				{
					//移動地点を指定

					m_ismoving = true;
					m_movepos = talk_target->GetPos();

					//移動先の指定
					RouteSearch::GetInstance().InitStageCollider();
					m_moveque = RouteSearch::GetInstance().SearchRoute(m_pos, m_movepos);
					//最初の移動先をキューの最後にする
					if (m_moveque.size() > 0) {
						XMFLOAT2 backque = m_moveque[m_moveque.size() - 1];
						m_moveque.pop_back();
						m_movepos = XMFLOAT3(backque.x, m_pos.y, backque.y);

						m_ismoving = true;
					}

					//目的地に到達した
					if (m_obb.Collision(talk_target->GetOBB()))
					{
						m_ismoving = false;
						m_is_talking = true;
						m_moveque.clear();
					}
				}
			}
		}
		//会話状態
		else
		{
			Emotion(EMOTION::TALK);
			//会話対象が消えた場合は処理しない
			if (talk_target == nullptr)
			{
				return void();
			}
			//記憶の交換を行う
			static int resource_memory_idx = 0;

			for (int i = 0; i < 3; i++)
			{
				//idxをインクリメント
				if (resource_memory_idx < m_resource_memory.size())
				{
					resource_memory_idx++;
				}

				int index = 0;
				bool duplication = false;
				for (int j = 0; j < talk_target->GetResourceMemory().size(); j++)
				{
					//相手側の記憶と照合
					if (m_resource_memory[resource_memory_idx] == talk_target->GetResourceMemory()[j])
					{
						index = j;
						duplication = true;
					}
				}
				//１つも被っている要素が無い場合
				if (!duplication)
				{
					Emotion(EMOTION::HEART);
					AddResourceMemory(talk_target->GetResourceMemory()[index]);
				}
			}
			if (!talk_target->m_is_talking && !m_ismoving)
			{
				m_is_talking = false;
				m_talk_cooltime = 20;
			}
			//会話終了
			if (resource_memory_idx >= m_resource_memory.size())
			{
				m_is_talking = false;
				m_talk_cooltime = 20;
			}

		}
	}
}
