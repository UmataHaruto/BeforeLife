#include	<d3d11.h>
#include	<DirectXMath.h>
#include	<wrl/client.h>
#include	<string>
#include	"dx11mathutil.h"
#include	"dx11util.h"
#include	"Shader.h"
#include	"shaderhashmap.h"
#include	"DX11Settransform.h"
#include	"quad2d.h"
#include    "CDirectInput.h"
#include "Sprite2DMgr.h"

// comptr
using Microsoft::WRL::ComPtr;

// �g�p���钸�_�V�F�[�_�[��
const char* vsfilename[] = {
	"shader/vsquad2d.hlsl"
};

// �g�p����s�N�Z���V�F�[�_�[��
const char* psfilename[] = {
	"shader/psquad2d.hlsl"
};

// ��`�̏�����
bool Quad2D::Init(uint32_t width, 
	uint32_t height, 
	DirectX::XMFLOAT4 color, 
	DirectX::XMFLOAT2* uv) {

	DX11MtxIdentity(m_worldmtx);	// �����p��

	// �s�N�Z���V�F�[�_�[�𐶐�
	bool sts = ShaderHashmap::GetInstance()->SetPixelShader(psfilename[0]);
	if (!sts) {
		MessageBox(nullptr, "SetPixelShader error", "error", MB_OK);
		return false;
	}

	// ���_�f�[�^�̒�`
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// �G�������g��
	unsigned int numElements = ARRAYSIZE(layout);

	// ���_�V�F�[�_�[�𐶐�
	sts = ShaderHashmap::GetInstance()->SetVertexShader(
		vsfilename[0],			// ���_�V�F�[�_�[�t�@�C����
		layout,					// ���_���C�A�E�g
		numElements);			// �G�������g��

	// ���_������
	updateVertex(width, height, color, uv);

	// �f�o�C�X���擾
	ID3D11Device* device;
	device = GetDX11Device();

	// ���_�o�b�t�@�𐶐�
	sts = CreateVertexBufferWrite(
		device,						// �f�o�C�X�I�u�W�F�N�g
		sizeof(Quad2D::Vertex),		// �P���_������o�C�g��
		4,							// ���_��
		m_vertex,					// ���_�f�[�^�i�[�������擪�A�h���X
		&m_vertexbuffer				// ���_�o�b�t�@
	);
	if (!sts) {
		MessageBox(nullptr, "CreateVertexBuffer error", "error", MB_OK);
		return false;
	}

	// �C���f�b�N�X�o�b�t�@�𐶐�
	uint32_t index[4] = { 0,1,2,3 };

	sts = CreateIndexBuffer(
		device,						// �f�o�C�X�I�u�W�F�N�g
		4,							// �C���f�b�N�X��
		index,						// �C���f�b�N�X�i�[�������擪�A�h���X
		&m_indexbuffer				// �C���f�b�N�X�o�b�t�@
	);
	if (!sts) {
		MessageBox(nullptr, "Quad2D CreateIndexBuffer error", "error", MB_OK);
		return false;
	}

	// �r���[�|�[�g�T�C�Y�Z�b�g
	SetViewPortSize(DX11GetScreenWidth(), DX11GetScreenHeight());

	return true;
}

void Quad2D::Update()
{
	//����
	static bool SwitchInvisible = true;
	XMFLOAT4 color;
	switch (type)
	{
	
	case UILIST::BLACKBACK_START:
		color = m_color;
		color.w -= 0.05;

		SetColor(color);

		if (color.w <= 0)
		{
			SetStatus(UISTATUS::DEAD);
		}
		break;
	
	case UILIST::BLACKBACK_END:
		color = m_color;
		color.w += 0.05;

		SetColor(color);

		if (color.w >= 10)
		{
			SetStatus(UISTATUS::DEAD);
		}
		break;

	case UILIST::CLOUDBACK_START:
		color = m_color;
		color.w -= 0.05;

		SetColor(color);

		if (color.w <= 0)
		{
			SetStatus(UISTATUS::DEAD);
		}
		break;

	case UILIST::CLOUDBACK_END:
		color = m_color;
		color.w += 0.05;

		SetColor(color);

		if (color.w >= 10)
		{
			SetStatus(UISTATUS::DEAD);
		}
		break;

	case UILIST::ICON_MOUSE:
		SetPosition(CDirectInput::GetInstance().GetMousePosX(), CDirectInput::GetInstance().GetMousePosY(),0);
		break;
	default:
		break;
	}
	//�A�j���[�V�����̏ꍇ
	if (m_isAnimation)
	{
		XMFLOAT2 uv[4];

		int index = m_animationNo;

		//��������
		int u = index % m_cutU;
		//�c������
		int v = index / m_cutU;

		//1�R�}�̃T�C�Y
		float size_x = 1 / (float)m_cutU;
		float size_y = 1 / (float)m_cutV;

		//�A�j���[�V�����ԍ��ɉ����ăJ�b�g��ύX

		uv[0].x = u * size_x;
		uv[0].y = v * size_y;
		uv[1].x = (u * size_x) + size_x;
		uv[1].y = v * size_y;
		uv[2].x = u * size_x;
		uv[2].y = (v * size_y) + size_y;
		uv[3].x = (u * size_x) + size_x;
		uv[3].y = (v * size_y) + size_y;

		//uv���Z�b�g
		updateVertex(m_width, m_height,m_color, uv);
		updateVbuffer();

		//�A�j���[�V�����x�����J�E���g�A�b�v
		m_delaycount++;

		if (m_delaycount >= m_animationDelay) {
			m_delaycount = 0;
			//�A�j���[�V�����J�E���g��i�߂�
			if (m_animationNo < m_cutU * m_cutV - 1) {
				m_animationNo++;
			}
			//���[�v����ꍇ
			else if (m_isLoop)
			{
				m_animationNo = 0;
			}
			//�^�[�Q�b�g�ł͂Ȃ��ꍇ
			else
			{
				m_sts = UISTATUS::DEAD;
			}
		}
	}

}

// �`��
void Quad2D::Draw() {

	// �f�o�C�X�R���e�L�X�g���擾
	ID3D11DeviceContext* devcontext;
	devcontext = GetDX11DeviceContext();

	// �e�N�X�`�����Z�b�g
	devcontext->PSSetShaderResources(0, 1, &m_srv);

	// ���W�ϊ��p�̍s����Z�b�g
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, m_worldmtx);

	unsigned int stride = sizeof(Quad2D::Vertex);	// �X�g���C�h���Z�b�g�i�P���_������̃o�C�g���j
	unsigned  offset = 0;						// �I�t�Z�b�g�l���Z�b�g

	// ���_�o�b�t�@���f�o�C�X�R���e�L�X�g�փZ�b�g
	devcontext->IASetVertexBuffers(
		0,									// �X�^�[�g�X���b�g
		1,									// ���_�o�b�t�@��
		&m_vertexbuffer,		// ���_�o�b�t�@�̐擪�A�h���X
		&stride,							// �X�g���C�h
		&offset);							// �I�t�Z�b�g

	// �C���f�b�N�X�o�b�t�@���f�o�C�X�R���e�L�X�g�փZ�b�g
	devcontext->IASetIndexBuffer(
		m_indexbuffer,				// �C���f�b�N�X�o�b�t�@
		DXGI_FORMAT_R32_UINT,				// �t�H�[�}�b�g
		0);									// �I�t�Z�b�g

	// �g�|���W�[���Z�b�g
	devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ���_�V�F�[�_�[�A�s�N�Z���V�F�[�_�[�擾
	ID3D11VertexShader* vs;
	vs = ShaderHashmap::GetInstance()->GetVertexShader(vsfilename[0]);
	ID3D11PixelShader* ps;
	ps = ShaderHashmap::GetInstance()->GetPixelShader(psfilename[0]);

	// ���_���C�A�E�g�擾
	ID3D11InputLayout* layout;
	layout = ShaderHashmap::GetInstance()->GetVertexLayout(vsfilename[0]);

	devcontext->VSSetShader(vs, nullptr, 0);
	devcontext->GSSetShader(nullptr, nullptr, 0);
	devcontext->HSSetShader(nullptr, nullptr, 0);
	devcontext->DSSetShader(nullptr, nullptr, 0);
	devcontext->PSSetShader(ps, nullptr, 0);

	// ���_�t�H�[�}�b�g���Z�b�g
	devcontext->IASetInputLayout(layout);

	// �h���[�R�[�����s
	devcontext->DrawIndexed(
		4,						// �C���f�b�N�X��
		0,						// �J�n�C���f�b�N�X
		0);						// ����_�C���f�b�N�X
}

void Quad2D::UnInit() {
	if (m_vertexbuffer != nullptr) {
		m_vertexbuffer->Release();
		m_vertexbuffer = nullptr;
	}
	if (m_indexbuffer != nullptr) {
		m_indexbuffer->Release();
		m_indexbuffer = nullptr;
	}
	if (m_srv != nullptr) {
		m_srv->Release();
		m_srv = nullptr;
	}
	//if (m_tex != nullptr) {
	//	m_tex->Release();
	//	m_tex = nullptr;
	//}
}

// �g��A�k��
void Quad2D::SetScale(float sx, float sy, float sz) {
	DX11MtxScale(sx, sy, sz, m_worldmtx);
	m_width = sx;
	m_height = sy;
}

// �ʒu���Z�b�g
void Quad2D::SetPosition(float x, float y, float z) {
	m_worldmtx._41 = x;
	m_worldmtx._42 = y;
	m_worldmtx._43 = z;
}

// Z����]
void Quad2D::SetRotation(float angle) {
	DX11MtxRotationZ(angle, m_worldmtx);
}

void Quad2D::SetUV(DirectX::XMFLOAT2* uv)
{
	for (int i = 0; i < 4; i++) {
		m_vertex[i].tex = uv[i];
	}
}

// ���_�f�[�^�X�V
void Quad2D::updateVertex(uint32_t width, 
	uint32_t height, 
	DirectX::XMFLOAT4 color, 
	DirectX::XMFLOAT2* uv) {

	m_width = static_cast<float>(width);
	m_height = static_cast<float>(height);

	Quad2D::Vertex	v[4] = {
		// ���W													// �J���[	// UV	
		XMFLOAT3(	-m_width / 2.0f,	-m_height / 2.0f, 0.0f),	color,		uv[0],
		XMFLOAT3(	 m_width / 2.0f,	-m_height / 2.0f, 0.0f),	color,		uv[1],
		XMFLOAT3(	-m_width / 2.0f,	 m_height / 2.0f, 0.0f),	color,		uv[2],
		XMFLOAT3(	 m_width / 2.0f,	 m_height / 2.0f, 0.0f),	color,		uv[3]
	};

	for (int i = 0; i < 4; i++) {
		m_vertex[i] = v[i];
	}
}

// ���_�o�b�t�@�X�V
void Quad2D::updateVbuffer() {
	D3D11_MAPPED_SUBRESOURCE pData;
	HRESULT hr = GetDX11DeviceContext()->Map(m_vertexbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		memcpy_s(pData.pData, pData.RowPitch, (void*)(m_vertex), sizeof(Quad2D::Vertex) * 4);
		GetDX11DeviceContext()->Unmap(m_vertexbuffer, 0);
	}
}