#define NOMINMAX
#include	<windows.h>
#include	"CModel.h"
#include	"Shader.h"
#include	"CDirectxGraphics.h"
#include	"DX11Settransform.h"
#include	"ModelData.h"

bool CModel::Init(const char* filename, const char* vsfile, const char* psfile, std::string texfolder) {

	bool sts;

	//�V�F�[�_�[�|�C���^�����Z�b�g
	for (int i = 0; i < SELECT_SHADER_TYPE_MAX; i++)
	{
		m_pSelectPixelShader[i] = nullptr;
	}

	std::string fname(filename);

	sts = m_assimpfile.Load(texfolder, fname);
	if (!sts) {
		char str[128];
		sprintf_s(str, 128, "%s load ERROR!!", filename);
		MessageBox(nullptr, str, "error", MB_OK);
		return false;
	}
	m_filetype = eASSIMPFILE;

	// ���_�f�[�^�̒�`�i�A�j���[�V�����Ή��j
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDEX",	0, DXGI_FORMAT_R32G32B32A32_SINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEWEIGHT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	unsigned int numElements = ARRAYSIZE(layout);

	ID3D11Device* device;
	device = CDirectXGraphics::GetInstance()->GetDXDevice();

	// ���_�V�F�[�_�[�I�u�W�F�N�g�𐶐��A�����ɒ��_���C�A�E�g������
	sts = CreateVertexShader(device,
		vsfile,
		"main",
		"vs_4_0",
		layout,
		numElements,
		&m_pVertexShader,
		&m_pVertexLayout);
	if (!sts) {
		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
		return false;
	}

	// �s�N�Z���V�F�[�_�[�𐶐�
	sts = CreatePixelShader(			// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
		device,		// �f�o�C�X�I�u�W�F�N�g
		psfile,
		"main",
		"ps_4_0",
		&m_pPixelShader);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	// select�s�N�Z���V�F�[�_�[�𐶐�
	//normal
	sts = CreatePixelShader(			// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
		device,		// �f�o�C�X�I�u�W�F�N�g
		"shader/ps_Select_Normal.fx",
		"main",
		"ps_4_0",
		&m_pSelectPixelShader[SELECT_SHADER_TYPE_NOORMAL]);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	//gathering
	sts = CreatePixelShader(			// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
		device,		// �f�o�C�X�I�u�W�F�N�g
		"shader/ps_Select_Gathering.fx",
		"main",
		"ps_4_0",
		&m_pSelectPixelShader[SELECT_SHADER_TYPE_GATHERING]);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	//Create
	sts = CreatePixelShader(			// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
		device,		// �f�o�C�X�I�u�W�F�N�g
		"shader/ps_Select_Create.fx",
		"main",
		"ps_4_0",
		&m_pSelectPixelShader[SELECT_SHADER_TYPE_CREATE]);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	//chancel
	sts = CreatePixelShader(			// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
		device,		// �f�o�C�X�I�u�W�F�N�g
		"shader/ps_Select_Cancel.fx",
		"main",
		"ps_4_0",
		&m_pSelectPixelShader[SELECT_SHADER_TYPE_CANCEL]);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	//Range
	sts = CreatePixelShader(			// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
		device,		// �f�o�C�X�I�u�W�F�N�g
		"shader/ps_Select_Range.fx",
		"main",
		"ps_4_0",
		&m_pSelectPixelShader[SELECT_SHADER_TYPE_RANGE]);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	return true;
}

void CModel::Uninit() {

	// �A�j���[�V�����f�[�^���
	for (auto sa : m_animationcontainer) {
		sa->Exit();
		delete sa;
	}

	// ���_�V�F�[�_�[���
	if (m_pVertexShader) {
		m_pVertexShader->Release();
		m_pVertexShader = nullptr;
	}

	// �s�N�Z���V�F�[�_�[���
	if (m_pPixelShader) {
		m_pPixelShader->Release();
		m_pPixelShader = nullptr;
	}

	// ���_���C�A�E�g���
	if (m_pVertexLayout) {
		m_pVertexLayout->Release();
		m_pVertexLayout = nullptr;
	}
}

// �A�j���[�V�����X�V
void CModel::Update(
	unsigned int animno,
	const XMFLOAT4X4& mtxworld) {

	// �����X�L���f�[�^�X�V
	m_assimpfile.Update(animno, mtxworld, m_AnimFileIdx, m_animationcontainer);

	// OBB���X�V
	m_assimpfile.UpdateOBB(mtxworld);
}

void CModel::Draw(XMFLOAT4X4& mtxworld) {

	ID3D11DeviceContext* devcontext;			// �f�o�C�X�R���e�L�X�g
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();
	// ���_�t�H�[�}�b�g���Z�b�g
	devcontext->IASetInputLayout(m_pVertexLayout);
	// ���_�V�F�[�_�[���Z�b�g
	devcontext->VSSetShader(m_pVertexShader, nullptr, 0);
	// �s�N�Z���V�F�[�_�[���Z�b�g(�Z���N�g��Ԃɉ�����)
	switch (m_selecttype)
	{
	case SELECT_SHADER_TYPE_NOORMAL:
		devcontext->PSSetShader(m_pSelectPixelShader[SELECT_SHADER_TYPE_NOORMAL], nullptr, 0);
		break;
	case SELECT_SHADER_TYPE_GATHERING:
		devcontext->PSSetShader(m_pSelectPixelShader[SELECT_SHADER_TYPE_GATHERING], nullptr, 0);
		break;
	case SELECT_SHADER_TYPE_CREATE:
		devcontext->PSSetShader(m_pSelectPixelShader[SELECT_SHADER_TYPE_CREATE], nullptr, 0);
		break;
	case SELECT_SHADER_TYPE_CANCEL:
		devcontext->PSSetShader(m_pSelectPixelShader[SELECT_SHADER_TYPE_CANCEL], nullptr, 0);
		break;
	case SELECT_SHADER_TYPE_RANGE:
		devcontext->PSSetShader(m_pSelectPixelShader[SELECT_SHADER_TYPE_RANGE], nullptr, 0);
		break;
	case SELECT_SHADER_TYPE_NONE:
		devcontext->PSSetShader(m_pPixelShader, nullptr, 0);
		break;
	default:
		devcontext->PSSetShader(m_pPixelShader, nullptr, 0);
		break;
	}
	// �`��
	m_assimpfile.Draw(devcontext, mtxworld);
}