#pragma once
#include	<DirectXMath.h>

void MousePosToWorld(const DirectX::XMFLOAT4X4& mtxView,		// �r���[�ϊ��s��
	const DirectX::XMFLOAT4X4& mtxProjection,				// �v���W�F�N�V�����ϊ��s��
	float MouseX,									// �}�E�X���W�@�w
	float MouseY,									// �}�E�X���W�@�x
	float z,										// �y�l�i�O�D�Of����P�D�Of�j
	float screen_width,								// �X�N���[������
	float screen_height,							// �X�N���[����
	DirectX::XMFLOAT3& worldpos);
XMFLOAT3 SearchMousePoint(XMFLOAT3& nearp, XMFLOAT3& farp);