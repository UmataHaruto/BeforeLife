#pragma once
#include "Button.h"

//�E�B���h�E�N���X(CWindow�N���X�Ƃ͕�)
class Window
{
public:
	
	//�����̃{�^���������Ƃ��ł���
	std::vector<Button> m_Buttons;

	//�R���X�g���N�^
	Window(XMFLOAT2 Pos, XMFLOAT2 Size,XMFLOAT4 color, bool CancelKey, const char* TexturePath);
	//������
 	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw();
	//���
	void UnInit();

	//�j��t���O���擾
	bool GetDestroy();

	//�N���b�N�C�x���g
	virtual void ClickEvent_00();
	virtual void ClickEvent_01();
	virtual void ClickEvent_02();
	virtual void ClickEvent_03();
	virtual void ClickEvent_04();
	virtual void ClickEvent_05();
	virtual void ClickEvent_06();
	virtual void ClickEvent_07();
	virtual void ClickEvent_08();
	virtual void ClickEvent_09();

protected:

	//�E�B���h�E�̉摜
	Quad2D m_sprite;

	//�j��t���O
	bool m_IsDestroy = false;
};

