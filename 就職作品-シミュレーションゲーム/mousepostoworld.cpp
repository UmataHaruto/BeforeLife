#include	<DirectXMath.h>
#include	"DX11util.h"
#include	"dx11mathutil.h"
#include    "StageHitCollider.h"
#include	"CCamera.h"
#include	"CDirectInput.h"
#include    "GameButton.h"

using namespace DirectX;

/*----------------------------------------------
	�}�E�X���W�����[���h���W�ɕϊ�����
------------------------------------------------*/
void MousePosToWorld(const XMFLOAT4X4& mtxView,		// �r���[�ϊ��s��
	const XMFLOAT4X4& mtxProjection,				// �v���W�F�N�V�����ϊ��s��
	float MouseX,									// �}�E�X���W�@�w
	float MouseY,									// �}�E�X���W�@�x
	float z,										// �y�l�i�O�D�Of����P�D�Of�j
	float screen_width,								// �X�N���[������
	float screen_height,							// �X�N���[����
	XMFLOAT3& worldpos) {							// �j�A�����[���h���W

	XMFLOAT4X4		mtxInvTotal;		// �t�s��
	XMFLOAT4X4		InvView, InvProjection, InvViewPort;

	XMFLOAT4X4		mtxViewPort;		// �r���[�|�[�g�s��

	XMFLOAT3		screenpoint(MouseX, MouseY, z);

	// �r���[�|�[�g�s�񐶐�
	DX11MtxIdentity(mtxViewPort);
	mtxViewPort._11 = screen_width / 2.0f;
	mtxViewPort._22 = -screen_height / 2.0f;
	mtxViewPort._41 = screen_width / 2.0f;
	mtxViewPort._42 = screen_height / 2.0f;

	// �t�s����v�Z
	DX11MtxInverse(InvView, mtxView);
	DX11MtxInverse(InvProjection, mtxProjection);
	DX11MtxInverse(InvViewPort, mtxViewPort);

	XMFLOAT4X4 tempmtx;
	// �X�N���[�����W�i�j�A�N���b�v�j
	DX11MtxMultiply(tempmtx, InvViewPort, InvProjection);
	DX11MtxMultiply(mtxInvTotal, tempmtx, InvView);

	// �s��~�x�N�g��
//	DX11Vec3MulMatrix(worldpos, screenpoint, mtxInvTotal);

	worldpos.x = screenpoint.x * mtxInvTotal._11 +
		screenpoint.y * mtxInvTotal._21 +
		screenpoint.z * mtxInvTotal._31 +
		1.0f * mtxInvTotal._41;

	worldpos.y = screenpoint.x * mtxInvTotal._12 +
		screenpoint.y * mtxInvTotal._22 +
		screenpoint.z * mtxInvTotal._32 +
		1.0f * mtxInvTotal._42;

	worldpos.z = screenpoint.x * mtxInvTotal._13 +
		screenpoint.y * mtxInvTotal._23 +
		screenpoint.z * mtxInvTotal._33 +
		1.0f * mtxInvTotal._43;

	float w = screenpoint.x * mtxInvTotal._14 +
		screenpoint.y * mtxInvTotal._24 +
		screenpoint.z * mtxInvTotal._34 +
		1.0f * mtxInvTotal._44;

	worldpos.x = worldpos.x / w;
	worldpos.y = worldpos.y / w;
	worldpos.z = worldpos.z / w;

	return;
}

XMFLOAT3 MousePosWPosFar(uint32_t mousex, uint32_t mousey) {

	// �v���W�F�N�V�����ϊ��s����擾
	XMFLOAT4X4 mtxprojection = CCamera::GetInstance()->GetProjectionMatrix();

	// �r���[�ϊ��s����擾
	XMFLOAT4X4 mtxview = CCamera::GetInstance()->GetCameraMatrix();

	float screen_width = static_cast<float>(DX11GetScreenWidth());
	float screen_height = static_cast<float>(DX11GetScreenHeight());

	XMFLOAT3 worldpos;

	MousePosToWorld(mtxview,		// �r���[�ϊ��s��
		mtxprojection,				// �v���W�F�N�V�����ϊ��s��
		mousex,						// �}�E�X���W�@�w
		mousey,						// �}�E�X���W�@�x
		1.0f,						// �y�l�i�O�D�Of����P�D�Of�j
		screen_width,				// �X�N���[������
		screen_height,				// �X�N���[����
		worldpos);

	return worldpos;
}

XMFLOAT3 MousePosWPosNear(uint32_t mousex,uint32_t mousey) {

	// �v���W�F�N�V�����ϊ��s����擾
	XMFLOAT4X4 mtxprojection = CCamera::GetInstance()->GetProjectionMatrix();

	// �r���[�ϊ��s����擾
	XMFLOAT4X4 mtxview = CCamera::GetInstance()->GetCameraMatrix();

	float screen_width = static_cast<float>(DX11GetScreenWidth());
	float screen_height = static_cast<float>(DX11GetScreenHeight());

	XMFLOAT3 worldpos;

	MousePosToWorld(mtxview,		// �r���[�ϊ��s��
		mtxprojection,				// �v���W�F�N�V�����ϊ��s��
		mousex,						// �}�E�X���W�@�w
		mousey,						// �}�E�X���W�@�x
		0.0f,						// �y�l�i�O�D�Of����P�D�Of�j
		screen_width,				// �X�N���[������
		screen_height,				// �X�N���[����
		worldpos);

	return worldpos;
}

XMFLOAT3 SearchMousePoint(XMFLOAT3& nearp,XMFLOAT3& farp) {

	// �}�E�X�w���W�@�x���W���擾
	uint32_t mousex = CDirectInput::GetInstance().GetMousePosX();
	uint32_t mousey = CDirectInput::GetInstance().GetMousePosY();

	farp = MousePosWPosFar(mousex, mousey);
	nearp = MousePosWPosNear(mousex, mousey);

	XMFLOAT3 direction;

	direction.x = farp.x - nearp.x;
	direction.y = farp.y - nearp.y;
	direction.z = farp.z - nearp.z;

	XMFLOAT3 ans;

	XMFLOAT3 camerapos = CCamera::GetInstance()->GetEye();

	// �J�����Ɉ�ԋ߂��ꏊ���Ƃ��Ă���
	bool sts = SearchAllSurfaceNearestCamera(camerapos, direction, ans);
	if (sts) {
		GameButton::GetInstance().SetMouseWorld(ans);
		return ans;
	}
	else {
		return farp;
	}
}
