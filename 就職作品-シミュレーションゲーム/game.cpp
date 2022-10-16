#include <cinttypes>
#include "dx11util.h"
#include "CModel.h"
#include "CCamera.h"
#include "DX11Settransform.h"
#include "dx11mathutil.h"
#include "Application.h"
#include "CDirectInput.h"
#include "Simulation.h"
#include "ModelMgr.h"


void GameInit() {
	// DX11������
	DX11Init(
		Application::Instance()->GetHWnd(),
		Application::CLIENT_WIDTH,
		Application::CLIENT_HEIGHT,
		false);

	// �J�������K�v
	DirectX::XMFLOAT3 eye(0, 00, -30);		// �J�����̈ʒu
	DirectX::XMFLOAT3 lookat(0, 0, 0);		// �����_
	DirectX::XMFLOAT3 up(0, 1, 0);			// �J�����̏�����x�N�g��

	CCamera::GetInstance()->Init(
		10.0f,							// �j�A�N���b�v
		10000.0f,						// �t�@�[�N���b�v
		XM_PI / 3.0f,					// ����p
		static_cast<float>(Application::CLIENT_WIDTH),		// �X�N���[����
		static_cast<float>(Application::CLIENT_HEIGHT),		// �X�N���[���̍���
		eye, lookat, up);					// �J�����̃f�[�^

	// ���s�������Z�b�g
	DX11LightInit(
		DirectX::XMFLOAT4(1, 1, -1, 0));

	// DIRECTINPUT������
	CDirectInput::GetInstance().Init(
		Application::Instance()->GetHInst(),
		Application::Instance()->GetHWnd(),
		Application::CLIENT_WIDTH,			// �X�N���[����
		Application::CLIENT_HEIGHT			// �X�N���[���̍���
		);

	// �A���t�@�u�����h�L����
	TurnOnAlphablend();

	// �󒆐평����
	SimulationInit();


}


void GameInput(uint64_t dt) {
	CDirectInput::GetInstance().GetKeyBuffer();
}

void GameUpdate(uint64_t dt) {

	// �󒆐�X�V
	SimulationUpdate();

}

void GameRender(uint64_t dt) {

	float col[4] = { 0.5f,0.8f,0.8f,1 };

	// �`��O����
	DX11BeforeRender(col);

	XMFLOAT4X4 mtx;

	// �v���W�F�N�V�����ϊ��s��擾
	mtx = CCamera::GetInstance()->GetProjectionMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::PROJECTION, mtx);

	// �r���[�ϊ��s����擾
	mtx = CCamera::GetInstance()->GetCameraMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::VIEW, mtx);

	// �󒆐�`��
	SimulationDraw();


	// �`��㏈��
	DX11AfterRender();
}

void GameDispose() {

	// �󒆐�I������
	SimulationExit();
	DX11Uninit();
}