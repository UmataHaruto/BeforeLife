#pragma once
#include	<DirectXMath.h>
#include	"dx11mathutil.h"

// �v���g�^�C�v�錾
// ���ʂ̕����������߂�
void CreatePlaneInfo(	const DirectX::XMFLOAT3& p1,		// ���_�P
						const DirectX::XMFLOAT3& p2,		// ���_�Q
						const DirectX::XMFLOAT3& p3 ,		// ���_�R
						PLANE& plane);						// ���ʂ̕�����

// ���ʂƒ����̌�_�����߂�
bool LinetoPlaneCross(	const PLANE& plane,					//���ʂ̕�����
						const DirectX::XMFLOAT3& line_o,	//�������ʂ�_
						const DirectX::XMFLOAT3& line_v,	//�����̕����x�N�g��
							  DirectX::XMFLOAT3& ans);		//��_���W

// ���ʂƒ����̌�_�����߂�
bool LinetoPlaneCross(
	const PLANE&		plane,		//���ʂ̕�����
	const XMFLOAT3&		p0,			//�����̋N�_
	const XMFLOAT3&		wv,			//�����̕����x�N�g��
	float&				t,			// ��_�ʒu���	
	XMFLOAT3&			ans);		//��_���W


// ���O����
bool CheckInTriangle(	const DirectX::XMFLOAT3& a,			// ���_�P
						const DirectX::XMFLOAT3& b,			// ���_�Q
						const DirectX::XMFLOAT3& c,			// ���_�R
						const DirectX::XMFLOAT3& p);		// ���肷��_���W

// ��_�����߂�
float LengthPointToPlane(	const DirectX::XMFLOAT3& p,		// �_���W
							const PLANE& plane,				// ���ʂ̕�����
							DirectX::XMFLOAT3& crosspoint);	// ��_
