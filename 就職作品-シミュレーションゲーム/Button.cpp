#include "Button.h"
#include "2dsystem.h"

//�{�^������
Button::Button(ButtonType type, XMFLOAT2 Pos,XMFLOAT2 Size, const char* TexturePath)
{
	Init2D();

    //����������
	Init();

	//�^�C�v�ݒ�
	m_type = type;

	//�{�^��UI�̐���
	m_sprite.LoadTexture(TexturePath);
	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 0.5f;
	uv[3].x = 1.0f;
	uv[3].y = 0.5f;

	//��������
	m_sprite.Init(Size.x, Size.y, XMFLOAT4(1, 1, 1, 1), uv);

	//�F�ݒ�
	m_sprite.SetColor(XMFLOAT4(1,1,1,1));
	//�T�C�Y�ݒ�
	//m_sprite.SetScale(Size.x, Size.y, 0);
	//�ʒu�ݒ�
	m_sprite.SetPosition(Pos.x, Pos.y, 0);
	//�X�e�[�^�X�ݒ�
	m_sprite.SetStatus(UISTATUS::LIVE);

	m_sprite.SetAnimation(false, false);
	m_sprite.SetCutUV(1, 1);

	//���_�X�V
	m_sprite.updateVbuffer();
}

//����������
void Button::Init()
{
	m_sts = false;
}

void Button::Update()
{
	m_sprite.Update();

	if (m_type == ButtonType::TOGGLE)
	{
	}

	//�X�e�[�^�X�ɉ����ĉ摜��ύX
	XMFLOAT2 uv[4];
	//�}�E�X���{�^���̏�ɂ���
	if (m_OnMouse)
	{
		//�N���b�N����Ă���
		if (m_sts)
		{
			uv[0].x = 0.0f;
			uv[0].y = 0.75f;
			uv[1].x = 1.0f;
			uv[1].y = 0.75f;
			uv[2].x = 0.0f;
			uv[2].y = 1.0f;
			uv[3].x = 1.0f;
			uv[3].y = 1.0f;
		}
		//�N���b�N����Ă��Ȃ�
		else
		{
			uv[0].x = 0.0f;
			uv[0].y = 0.5f;
			uv[1].x = 1.0f;
			uv[1].y = 0.5f;
			uv[2].x = 0.0f;
			uv[2].y = 0.75f;
			uv[3].x = 1.0f;
			uv[3].y = 0.75f;
		}
	}
	//Mouse���{�^���̏�ɂȂ�
	else
	{
		//�N���b�N����Ă���
		if (m_sts)
		{
			uv[0].x = 0.0f;
			uv[0].y = 0.25f;
			uv[1].x = 1.0f;
			uv[1].y = 0.25f;
			uv[2].x = 0.0f;
			uv[2].y = 0.5f;
			uv[3].x = 1.0f;
			uv[3].y = 0.5f;
		}
		//�N���b�N����Ă��Ȃ�
		else
		{
			uv[0].x = 0.0f;
			uv[0].y = 0.0f;
			uv[1].x = 1.0f;
			uv[1].y = 0.0f;
			uv[2].x = 0.0f;
			uv[2].y = 0.25f;
			uv[3].x = 1.0f;
			uv[3].y = 0.25f;
		}
	}
	m_sprite.SetUV(uv);
	m_sprite.updateVbuffer();
}

//�����蔻��
bool Button::CheckHit()
{
	XMFLOAT3 Pos = m_sprite.GetWorldPos();

	float width = m_sprite.GetSize().x;
	float height = m_sprite.GetSize().y;
	//�����蔻��
	if (Pos.x - width / 2 < CDirectInput::GetInstance().GetMousePosX() && Pos.x + width / 2 >CDirectInput::GetInstance().GetMousePosX())
	{
		if (Pos.y - height / 2 < CDirectInput::GetInstance().GetMousePosY() && Pos.y + height / 2 >CDirectInput::GetInstance().GetMousePosY())
		{
			return true;
		}
	}
	return false;
}

void Button::ChangeClick()
{
	switch (m_type)
	{
	case ButtonType::RADIO:
		m_sts = 1 - m_sts;
		break;
	case ButtonType::TOGGLE:
		m_sts = true;
		break;
	default:
		break;
	}
}

void Button::ChangeMouseOn()
{
	m_OnMouse = true;
}

bool Button::GetSts()
{
	return m_sts;
}

void Button::Draw()
{
	m_sprite.Draw();
	if (m_type == ButtonType::TOGGLE)
	{
		m_sts = false;
	}
	m_OnMouse = false;
}

//�������
void Button::UnInit()
{
	m_sprite.UnInit();
}

