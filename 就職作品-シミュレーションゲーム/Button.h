#pragma once
#include "dx11mathutil.h"
#include "quad2d.h"
#include "CDirectInput.h"

//�{�^���̃N���b�N���
enum class ButtonType
{
	RADIO,
	TOGGLE,
};

class Window;
//�{�^���̎��
class Button
{
public:

	Window* parent;

	Button(ButtonType type, XMFLOAT2 Pos,XMFLOAT2 Size,const char* TexturePath);

	//�{�^���̐����Ə����ݒ�
	void Init();

	//�X�V����
	void Update();

	//�����蔻��ƍX�V
	bool CheckHit();

	//ON,OFF�؂�ւ�
	void ChangeClick();

	//�}�E�X��On����ύX
	void ChangeMouseOn();

	//�N���b�N�󋵂��擾
	bool GetSts();

	//�������
	void UnInit();

	//�`��
	void Draw();

private:

	Quad2D m_sprite;
	//�{�^���̏��
	bool m_sts = false;

	//�{�^����Ƀ}�E�X�����݂��Ă���
	bool m_OnMouse = false;
	//�{�^���^�C�v
	ButtonType m_type;

};

