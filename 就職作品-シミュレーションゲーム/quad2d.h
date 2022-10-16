#pragma once

#include	<d3d11.h>
#include	<DirectXMath.h>
#include	<wrl/client.h>
#include	<string>
#include	"dx11mathutil.h"
#include	"dx11util.h"
#include	"Shader.h"
#include	"shaderhashmap.h"
#include	"DX11Settransform.h"
#include	"uncopyable.h"

//�G�t�F�N�g���X�g
enum class UILIST
{
	BLACKBACK_START,
	BLACKBACK_END,
	ICON_MOUSE,
	_MAX,
};

enum class UISTATUS
{
	LIVE,
	DEAD
};

// ��`�N���X
class Quad2D{
public:
	// ���_�f�[�^
	struct Vertex {
		DirectX::XMFLOAT3 pos;		// ���W		
		DirectX::XMFLOAT4 color;	// �J���[
		DirectX::XMFLOAT2 tex;		// �e�N�X�`��
	};

	ID3D11Buffer* m_vertexbuffer;			// ���_�o�b�t�@

	// �r���[�|�[�g�̃T�C�Y���Z�b�g
	bool SetViewPortSize(uint32_t width,uint32_t height) {

		ID3D11Buffer* CBViewPort;

		// �r���[�|�[�g�p�̒萔�o�b�t�@
		struct ConstantBufferViewPort {
			uint32_t	Width[4];			// �r���[�|�[�g��
			uint32_t	Height[4];			// �r���[�|�[�g����
		};

		// �r���[�|�[�g�萔�o�b�t�@�ɒl���Z�b�g
		bool sts = CreateConstantBufferWrite(
			GetDX11Device(), 
			sizeof(ConstantBufferViewPort), 
			&CBViewPort);
		if (!sts) {
			return false;
		}

		ConstantBufferViewPort cb;
		cb.Width[0] = width;
		cb.Height[0] = height;

		D3D11_MAPPED_SUBRESOURCE pData;
		HRESULT hr = GetDX11DeviceContext()->Map(CBViewPort, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
		if (SUCCEEDED(hr)) {
			memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(ConstantBufferViewPort));
			GetDX11DeviceContext()->Unmap(CBViewPort, 0);
		}
		else {
			return false;
		}

		// ���_�V�F�[�_�[�ɃZ�b�g
		GetDX11DeviceContext()->VSSetConstantBuffers(5, 1,&CBViewPort);

		return true;
	}

	// ��`�̏�����
	bool Init(uint32_t width, uint32_t height, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2* uv);

	//�X�V����
	void Update();

	// �`��
	void Draw();

	void UnInit();

	//�X�e�[�^�X��n��
	UISTATUS GetStatus()
	{
		return m_sts;
	}

	//�|�W�V������n��
	Vertex GetVertex()
	{
		return m_vertex[0];
	}

	XMFLOAT3 GetWorldPos()
	{
		XMFLOAT3 pos;
		pos.x = m_worldmtx._41;
		pos.y = m_worldmtx._42;
		pos.z = m_worldmtx._43;
		return pos;
	}

	//�X�e�[�^�X��ݒ�
	void SetStatus(UISTATUS sts)
	{
		m_sts = sts;
	}

	void SetColor(XMFLOAT4 color)
	{
		m_color = color;
		for (int i = 0; i < 4; i++) {
			m_vertex[i].color = color;
		}
		updateVbuffer();
	}

	//�~�j�}�b�v���W��ݒ�
	void SetMinimapPos(XMFLOAT3* pos)
	{
		m_minimapPos = pos;
	}

	XMFLOAT2 GetSize()
	{
		return XMFLOAT2(m_width,m_height);
	}

	//�~�j�}�b�v���W���擾
	XMFLOAT3* GetMinimapPos()
	{
		return m_minimapPos;
	}

	//�A�j���[�V�����ݒ�
	void SetAnimation(bool isAnimaiton, bool isLoop)
	{
		m_isAnimation = isAnimaiton;
		m_isLoop = isLoop;
	}

	//�A�j���[�V�����x����ݒ�
	void SetAnimationDelay(int Delay)
	{
		m_animationDelay = Delay;
		m_delaycount = 0;
	}

	//UV�J�b�g�����Z�b�g
	void SetCutUV(uint16_t CutU, uint16_t CutV)
	{
		m_cutU = CutU;
		m_cutV = CutV;
	}

	// �g��A�k��
	void SetScale(float sx, float sy, float sz);

	// �ʒu���Z�b�g
	void SetPosition(float x, float y, float z);

	// Z����]
	void SetRotation(float angle);

	//UV�ݒ�
	void SetUV(DirectX::XMFLOAT2* uv);

	// ��`�̐i�s�������擾�i���摜��������ɍ���Ă��邱�Ƃ�O��j
	DirectX::XMFLOAT3 GetForward() {
		DirectX::XMFLOAT3 forward;
		forward.x = m_worldmtx._21;
		forward.y = m_worldmtx._22;
		forward.z = m_worldmtx._23;
		return forward;
	}

	// ��`�̉��������擾
	DirectX::XMFLOAT3 GetSide() {
		DirectX::XMFLOAT3 side;
		side.x = m_worldmtx._11;
		side.y = m_worldmtx._12;
		side.z = m_worldmtx._13;
		return side;
	}

	UILIST GetType()
	{
		return type;
	}

	void SetType(UILIST ui)
	{
		type = ui;
	}

	void SetMtx(XMFLOAT4X4 InMtx)
	{
		m_worldmtx = InMtx;
	}

	// ���_�f�[�^�X�V
	void updateVertex(uint32_t width, uint32_t height, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2* uv);
	// ���_�o�b�t�@�X�V
	void updateVbuffer();

	// �e�N�X�`���Z�b�g
	void LoadTexture(const char* filename) {
		// �e�N�X�`���ǂݍ���
		bool sts = CreateSRVfromFile(filename,
			GetDX11Device(),
			GetDX11DeviceContext(),
			&m_tex, &m_srv);
		if (!sts) {
			std::string str("SRV �쐬�G���[ ");
			std::string strfile(filename);
			MessageBox(nullptr, (str+strfile).c_str(), "error", MB_OK);
			return;
		}
	}

private:
	DirectX::XMFLOAT4X4		m_worldmtx;				// ���[���h�ϊ��s��
	ID3D11Buffer*	m_indexbuffer;			// �C���f�b�N�X�o�b�t�@
	Quad2D::Vertex			m_vertex[4];			// ��`�S���_
	float					m_width;				// ��
	float					m_height;				// ����
	XMFLOAT4                m_color;           //�F���
	bool                    m_isAnimation;     //�A�j���[�V������
	bool                    m_isLoop;          //�A�j���[�V���������[�v���邩
	uint32_t                m_cutU;    //���̕�����
	uint32_t                m_cutV;    //�c�̕�����
	int                     m_animationDelay;    //�A�j���[�V�����̒x���x
	int                     m_delaycount;    //�A�j���[�V�����x���J�E���^�[
	uint32_t                m_animationNo = 0;    //�A�j���[�V�����ԍ�
	UILIST                  type;   //UI�^�C�v
	UISTATUS                m_sts;    //Status
	XMFLOAT3*                m_minimapPos;    //�~�j�}�b�v��̍��W
	ID3D11ShaderResourceView* m_srv;			// SRV
	ID3D11Resource* m_tex;					// �e�N�X�`�����\�[�X
};