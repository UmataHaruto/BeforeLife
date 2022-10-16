#include	"player.h"
#include	"drawaxis.h"

bool Player::Init() {
	// 行列初期化
	DX11MtxIdentity(m_mtx);

	m_model = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname);

	XMMATRIX scale = XMMatrixScaling(0.2, 0.2, 0.2);
	XMMATRIX mtx;
	mtx = DirectX::XMLoadFloat4x4(&m_mtx);
	mtx = XMMatrixMultiply(mtx, scale);
	DirectX::XMStoreFloat4x4(&m_mtx, mtx);

	// ASSIMPを使用したアニメーションの読み込み
	bool sts = m_model->LoadAnimation("assets/ModelData/FemaleWalk.x");
	if (!sts) {
		MessageBox(NULL, "load enemy animation error", "error", MB_OK);
		return false;
	}
	return true;
}

void Player::Draw() {

	// モデル描画
	m_model->Draw(m_mtx);
}

void Player::DrawWithAxis() {

	// モデル描画
	drawaxis(m_mtx, 200, m_pos);
	Draw();

}

void Player::Update() {

	XMFLOAT3 angle = { 0,0,0 };
	bool keyinput = false;

	//上キー入力(連打可)
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_UP))
	//{
	//	angle.x -= 0.6f;
	//	keyinput = true;
	//}

	////下キー入力(連打可)
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_DOWN))
	//{
	//	angle.x += 0.6f;
	//	keyinput = true;
	//}

	////左キー入力(連打可)
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT))
	//{
	//	angle.y -= 1.0f;
	//	keyinput = true;

	//	angle.z += 0.8f;
	//	keyinput = true;
	//}

		//左キー入力(連打可)
	//if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_0))
	//{
	//	m_model->ChangeSelectType((SELECT_SHADER_TYPE)(m_model->GetSelectType() + 1));
	//}

	////右キー入力(連打可)
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT))
	//{
	//	angle.y += 1.0f;
	//	keyinput = true;

	//	angle.z -= 0.8f;
	//	keyinput = true;
	//}

	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_SPACE))
	{
		animno++;
		unsigned int animnummax = m_model->GetAnimationNum();
		if (animno >= animnummax) {
			animno = 0;
		}
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

	if (keyinput)
	{
		XMFLOAT4 qt;    //クオータニオン

		//行列からクオータニオンを生成
		DX11GetQtfromMatrix(m_mtx,qt);

		XMFLOAT4 qtx, qty, qtz;    //クオータニオン

		//指定軸回転のクオータニオンを生成
		DX11QtRotationAxis(qtx,axisX,angle.x);
		DX11QtRotationAxis(qty, axisY, angle.y);
		DX11QtRotationAxis(qtz, axisZ, angle.z);

		//クオータニオンを合成
		XMFLOAT4 tempqt1;
		DX11QtMul(tempqt1, qt, qtx);

		XMFLOAT4 tempqt2;
		DX11QtMul(tempqt2, qty, qtz);

		XMFLOAT4 tempqt3;
		DX11QtMul(tempqt3, tempqt1,tempqt2);

		//クオータニオンをノーマライズ
		DX11QtNormalize(tempqt3, tempqt3);

		//クオータニオンから行列を作成
		DX11MtxFromQt(m_mtx, tempqt3);

	}

	m_model->Update(animno,m_mtx);

	//Z軸を取り出す
	axisZ.x = m_mtx._31;
	axisZ.y = m_mtx._32;
	axisZ.z = m_mtx._33;
	axisX.w = 0.0f;

	/*m_pos.x += axisZ.x * 0.1f;
	m_pos.y += axisZ.y * 0.1f;
	m_pos.z += axisZ.z * 0.1f;*/

	//画面端へいった場合戻ってくる
	if (m_pos.x > 300) {
		m_pos.x = -300;
	}
	if (m_pos.x < -300) {
		m_pos.x = 300;

	}
	if (m_pos.z > 300) {
		m_pos.z = -300;

	}
	if (m_pos.z < -300) {
		m_pos.z = 300;

	}

	m_mtx._41 = m_pos.x;
	m_mtx._42 = m_pos.y;
	m_mtx._43 = m_pos.z;

}

void Player::Finalize() {
	//m_model->Uninit();
}

void Player::SetCameraMode(CameraMode mode)
{
	camera_mode = mode;
}

CameraMode Player::GetCameraMode()
{
	return camera_mode;
}
