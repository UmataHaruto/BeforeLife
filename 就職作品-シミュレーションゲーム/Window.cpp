#include "Window.h"
#include "CDirectInput.h"
#include "2dsystem.h"

Window::Window(XMFLOAT2 Pos, XMFLOAT2 Size,XMFLOAT4 color, bool CancelKey, const char* TexturePath)
{
	//��������
	Init();

	Init2D();

	//Window�̐���
	m_sprite.LoadTexture(TexturePath);
	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 1.0f;
	uv[3].x = 1.0f;
	uv[3].y = 1.0f;

	//��������
	m_sprite.Init(Size.x, Size.y, color, uv);

	//�F�ݒ�
	m_sprite.SetColor(color);

	//�ʒu�ݒ�
	m_sprite.SetPosition(Pos.x, Pos.y, 0);
	//�X�e�[�^�X�ݒ�
	m_sprite.SetStatus(UISTATUS::LIVE);

	m_sprite.SetAnimation(false, false);
	m_sprite.SetCutUV(1, 1);

	//���_�X�V
	m_sprite.updateVbuffer();

	Button board(ButtonType::TOGGLE, XMFLOAT2(660, 126), XMFLOAT2(25, 25), "assets/sprite/UI/CansselButton.png");
	if (CancelKey) {
		//�o�c�{�^���A���̏ꍇ
		m_Buttons.push_back(board);
	}
}

void Window::Init()
{
}

void Window::Update()
{
	//�E�B���h�E�̃X�v���C�g���X�V
	m_sprite.Update();

	//�{�^���̍X�V
	for (int i = 0; i < m_Buttons.size(); i++) {
		if (m_Buttons[i].CheckHit())
		{
			//�}�E�X���{�^����ɂ���
			m_Buttons[i].ChangeMouseOn();
			//�{�^�����N���b�N���ꂽ
			if (CDirectInput::GetInstance().GetMouseLButtonTrigger()) {
				m_Buttons[i].ChangeClick();
			}
		}
		
	}
	

}

void Window::Draw()
{
	//�E�B���h�E�̕\��
	m_sprite.Draw();
	//�{�^���̕`��
	for (int i = 0; i < m_Buttons.size(); i++) {
		m_Buttons[i].Draw();
	}
}

void Window::UnInit()
{
	//�E�B���h�E�̉��
	m_sprite.UnInit();

	//�{�^���̉��
	for (int i = 0; i < m_Buttons.size(); i++) {
		m_Buttons[i].UnInit();
	}
	m_Buttons.clear();
}

bool Window::GetDestroy()
{
	return m_IsDestroy;
}

void Window::ClickEvent_00()
{
}

void Window::ClickEvent_01()
{
}

void Window::ClickEvent_02()
{
}

void Window::ClickEvent_03()
{
}

void Window::ClickEvent_04()
{
}

void Window::ClickEvent_05()
{
}

void Window::ClickEvent_06()
{
}

void Window::ClickEvent_07()
{
}

void Window::ClickEvent_08()
{
}

void Window::ClickEvent_09()
{
}
