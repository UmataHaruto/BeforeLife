#pragma once
#include	"gameobject.h"
#include	"CModel.h"
#include	"dx11mathutil.h"
#include	"CDirectInput.h"
#include    "ModelMgr.h"

enum class CameraMode
{
	TPS,    //�O�l�̎��_
	FPS,    //��l�̎��_
};

class Player :public GameObject {
private:
	CModel*	m_model;// �R�c���f��
	CameraMode camera_mode;//�J�������[�h 0 = TPS,1 = FPS
	int animno = 0;
public:

	//������
	bool Init();

	//�`��
	void Draw();

	void DrawWithAxis();

	//�X�V
	void Update();

	//�I������
	void Finalize();

	//�J�������[�h��ύX
	void SetCameraMode(CameraMode mode);

	//�J�������[�h���擾
	CameraMode GetCameraMode();

	//���f���f�[�^���擾
	const CModel* GetModel() {
		return m_model;
	}

	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
};