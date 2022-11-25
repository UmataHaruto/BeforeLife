#include "CCamera.h"
#include	<array>

XMFLOAT3 CCamera::Lerp(XMFLOAT3 from, XMFLOAT3 to, double t)
{
		XMFLOAT3 ans;

		ans.x = from.x * t + to.x * (1.0 - t);
		ans.y = from.y * t + to.y * (1.0 - t);
		ans.z = from.z * t + to.z * (1.0 - t);

		return ans;

}

void CCamera::FPSCamera(XMFLOAT4X4 mtx)
{
	//�s��擾
	//XMFLOAT4X4 mtx;
	//mtx = g_player.GetMtx();

	//Z���擾
	XMFLOAT3 zaxis;
	zaxis.x = mtx._31;
	zaxis.y = mtx._32;
	zaxis.z = mtx._33;

	//�����_�v�Z
	XMFLOAT3 lookat;
	lookat.x = mtx._41 + zaxis.x;
	lookat.y = mtx._42 + zaxis.y;
	lookat.z = mtx._43 + zaxis.z;

	//�J�����ʒu�v�Z
	XMFLOAT3 eye;
	eye.x = mtx._41;
	eye.y = mtx._42;
	eye.z = mtx._43;

	//�J�����ʒu�v�Z
	XMFLOAT3 up;
	up.x = mtx._21;
	up.y = mtx._22;
	up.z = mtx._23;

	//�J�����N���X�ɃZ�b�g
	CCamera::GetInstance()->SetLookat(lookat);
	CCamera::GetInstance()->SetEye(eye);
	CCamera::GetInstance()->SetUp(up);

	CCamera::GetInstance()->CreateCameraMatrix(); //�r���[�ϊ��s���
}

void CCamera::TPSCamera(XMFLOAT4X4 mtx)
{
	//�r���[�ϊ��s��̎擾

	static XMFLOAT3 zaxis;    //Z��
	zaxis.x = mtx._31;
	zaxis.y = mtx._32;
	zaxis.z = mtx._33;

	static XMFLOAT3 yaxis;    //Y��
	yaxis.x = mtx._21;
	yaxis.y = mtx._22;
	yaxis.z = mtx._23;

	static XMFLOAT3 pos;    //position
	pos.x = mtx._41;
	pos.y = mtx._42;
	pos.z = mtx._43;

	XMFLOAT3 eye;    //���_
	XMFLOAT3 lookat;    //�����_
	XMFLOAT3 up;    //������x�N�g��

	lookat = pos;

	eye.x = pos.x - ((150 * m_Zoom) * zaxis.x) + (yaxis.x * (10 * m_Zoom));
	eye.y = pos.y - ((150 * m_Zoom) * zaxis.y) + (yaxis.y * (300 * m_Zoom));
	eye.z = pos.z - ((150 * m_Zoom) * zaxis.z) + (yaxis.z * (10 * m_Zoom));

	up = yaxis;

	//�Y�[������
	//eye.x = (lookat.x - eye.x) * m_Zoom;
	//eye.y = (lookat.y - eye.y) * m_Zoom;
	//eye.z = (lookat.z - eye.z) * m_Zoom;

	//�J�����N���X�ɃZ�b�g
	CCamera::GetInstance()->SetLookat(lookat);
	CCamera::GetInstance()->SetEye(eye);
	CCamera::GetInstance()->SetUp(up);

	CCamera::GetInstance()->CreateCameraMatrix(); //�r���[�ϊ��s��쐬


}

void CCamera::DelayTPSCamera(XMFLOAT4X4 mtx)
{
	static bool firstflag = false;    //100�܂ŕۑ����Ă��邩�������t���O

	//�r���[�ϊ��s��̎擾
	static int svidx = 0;  //�ۑ��ʒu�������C���f�b�N�X�l



	static std::array<XMFLOAT3, 100> zaxis;    //Z��
	zaxis[svidx].x = mtx._31;
	zaxis[svidx].y = mtx._32;
	zaxis[svidx].z = mtx._33;

	static std::array<XMFLOAT3, 100> yaxis;    //Y��
	yaxis[svidx].x = mtx._21;
	yaxis[svidx].y = mtx._22;
	yaxis[svidx].z = mtx._23;

	static std::array<XMFLOAT3, 100> pos;    //Z��
	pos[svidx].x = mtx._41;
	pos[svidx].y = mtx._42;
	pos[svidx].z = mtx._43;

	//�v���C���[�Ƃ̋��������ȏ㗣�ꂽ�ꍇ�␮
	if (mtx._41 >= 300 || mtx._41 <= -300 || mtx._43 >= 300 || mtx._43 <= -300)
	{
		zaxis.fill(XMFLOAT3(0.0f, 0.0f, 0.0f));
		yaxis.fill(XMFLOAT3(0.0f, 0.0f, 0.0f));
		pos.fill(XMFLOAT3(0.0f, 0.0f, 0.0f));
		svidx = 0;

		//���݈ʒu���P�ڂɃZ�b�g
		zaxis[svidx].x = mtx._31;
		zaxis[svidx].y = mtx._32;
		zaxis[svidx].z = mtx._33;

		yaxis[svidx].x = mtx._21;
		yaxis[svidx].y = mtx._22;
		yaxis[svidx].z = mtx._23;

		pos[svidx].x = mtx._41;
		pos[svidx].y = mtx._42;
		pos[svidx].z = mtx._43;
		firstflag = false;
	}

	svidx++;

	if (svidx > zaxis.size() - 1)
	{
		svidx = 0;
		firstflag = true;
	}

	//30�t���[���O�̏����g�p���ăJ�������쐬
	int useidx;
	useidx = svidx - 30;
	if (useidx < 0)
	{
		if (firstflag == false)
		{

				useidx =0;

		}
		else
		{
			useidx = svidx + zaxis.size() - 30;
		}
	}

	XMFLOAT3 eye;    //���_
	XMFLOAT3 lookat;    //�����_
	XMFLOAT3 up;    //������x�N�g��

	lookat = pos[useidx];
	lookat.x = pos[useidx].x + 50 * zaxis[useidx].x;
	lookat.y = pos[useidx].y + 50 * zaxis[useidx].y;
	lookat.z = pos[useidx].z + 50 * zaxis[useidx].z;

	eye.x = pos[useidx].x - 30 * zaxis[useidx].x + 30 * yaxis[useidx].x;
	eye.y = pos[useidx].y - 30 * zaxis[useidx].y + 30 * yaxis[useidx].y;
	eye.z = pos[useidx].z - 30 * zaxis[useidx].z + 30 * yaxis[useidx].z;

	up = yaxis[useidx];

	//�J�����N���X�ɃZ�b�g
	CCamera::GetInstance()->SetLookat(lookat);
	CCamera::GetInstance()->SetEye(eye);
	CCamera::GetInstance()->SetUp(up);

	CCamera::GetInstance()->CreateCameraMatrix(); //�r���[�ϊ��s��쐬


}

void CCamera::FixedCamera(XMFLOAT3 lookat)
{
	CCamera::GetInstance()->SetLookat(lookat);

	CCamera::GetInstance()->CreateCameraMatrix(); //�r���[�ϊ��s��쐬
}
