#pragma once

#include	<directxmath.h>
#include	<d3d11.h>
#include	"DX11util.h"
#include	"Shader.h"
#include    "CModel.h"
#include    "XAudio2.h"
/*----------------------------------------------------------------------

	�r���{�[�h�N���X	

-----------------------------------------------------------------------*/

//�G�t�F�N�g���X�g
enum class EFFECTLIST
{
	TARGETCIRCLE,
	PROGRESSBAR_CIRCLE,
	WOOD,
	ORE_COAL,
	ORE_IRON,
	ORE_GOLD,
	IRON,
	GOLD,
	HUKIDASHI_HEART,
	HUKIDASHI_ONPU,
	HUKIDASHI_ASE,
	HUKIDASHI_TALK,
	HUKIDASHI_QUESTION,
	_MAX,
};

enum class EFFECTSTATUS
{
	LIVE,
	DEAD
};

// ���_�t�H�[�}�b�g
struct MyVertex {
	float				x, y, z;
	DirectX::XMFLOAT4	color;
	float				tu, tv;
};

class CBillBoard{
	CModel* m_model;// �R�c���f��

	DirectX::XMFLOAT4X4			m_mtx;			// �r���{�[�h�p�̍s��
	float						m_x=0;			// �r���{�[�h�̈ʒu���W
	float						m_y=0;					
	float						m_z=0;
	float						m_XSize=100.0f;		// �r���{�[�h�̂w�T�C�Y
	float						m_YSize=100.0f;		// �r���{�[�h�̂x�T�C�Y
	XMFLOAT3*                   m_Targetpos = nullptr;    //�ǔ���̈ړ���

	XMFLOAT3                    m_interval;  //�z�[�~���O���̌��_����̋���

	uint32_t                    m_life = 60; //HP
	uint32_t                    m_Maxlife = 90;
	EFFECTSTATUS                m_sts;    //�X�e�[�^�X
	EFFECTLIST                  m_type;

	bool                        m_isBillBolard;    //�r���{�[�h���g�p���邩
	bool                        m_isAnimation;     //�A�j���[�V������
	bool                        m_isLoop;          //�A�j���[�V���������[�v���邩
	bool                        m_isHorming;
	int                     m_animationDelay;    //�A�j���[�V�����̒x���x
	int                     m_delaycount;    //�A�j���[�V�����x���J�E���^�[
	DirectX::XMFLOAT4			m_Color= DirectX::XMFLOAT4(1,1,1,1);	// ���_�J���[�l
	ID3D11ShaderResourceView*   m_srv = nullptr;			// Shader Resourceview�e�N�X�`��
	ID3D11BlendState*			m_pBlendStateSrcAlpha = nullptr;
	ID3D11BlendState*			m_pBlendStateOne = nullptr;
	ID3D11BlendState*			m_pBlendStateDefault = nullptr;
	ID3D11VertexShader*			m_pVertexShader = nullptr;	// ���_�V�F�[�_�[���ꕨ
	ID3D11PixelShader*			m_pPixelShader = nullptr;	// �s�N�Z���V�F�[�_�[���ꕨ
	ID3D11InputLayout*			m_pVertexLayout = nullptr;	// ���_�t�H�[�}�b�g��`
	ID3D11Resource*				m_res = nullptr;

	DirectX::XMFLOAT2		m_uv[4];				// �e�N�X�`�����W

private:
	// �r���{�[�h�̒��_���W���v�Z
	void CalcVertex();
	// �r���{�[�h�p�̍s��𐶐�
	void CalcBillBoardMatrix(const DirectX::XMFLOAT4X4& cameramat);
	// �`��
	void Draw();
	// �\�[�X�A���t�@��ݒ肷��
	void SetBlendStateSrcAlpha();

	// �u�����h�X�e�[�g�𐶐�����
	void CreateBlendStateSrcAlpha();
	// ���Z������ݒ肷��
	void SetBlendStateOne();
	// �u�����h�X�e�[�g�i���Z�����j�𐶐�����
	void CreateBlendStateOne();
	// �f�t�H���g�̃u�����h�X�e�[�g��ݒ肷��
	void SetBlendStateDefault();
	// �f�t�H���g�̃u�����h�X�e�[�g�𐶐�����
	void CreateBlendStateDefault();

	//���_�o�b�t�@���폜
	void clearVBuffer()
	{

	}

public:
	uint32_t                    m_cutU;    //���̕�����
	uint32_t                    m_cutV;    //�c�̕�����
	uint32_t                    m_animationNo = 0;    //�A�j���[�V�����ԍ�
	ID3D11Buffer* m_vbuffer = nullptr;		// ���_�o�b�t�@
	MyVertex				m_Vertex[4];			// �r���{�[�h�̒��_���W

	//UV���W���Z�b�g
	void SetUV(DirectX::XMFLOAT2 uv[]) {
		m_uv[0] = uv[0];
		m_uv[1] = uv[1];
		m_uv[2] = uv[2];
		m_uv[3] = uv[3];
		CalcVertex();
	}
	//�A�j���[�V�����ݒ�
	void SetAnimation(bool isAnimaiton,bool isLoop)
	{
		m_isAnimation = isAnimaiton;
		m_isLoop = isLoop;
	}

	void SetAnimationNo(int AnimNo)
	{
		m_animationNo = AnimNo;
	}
	//���f���Z�b�g
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
	//���C�t��ݒ肷��
	void SetLife(int hp)
	{
		m_life = hp;
	}
	//�X�e�[�^�X��ݒ�
	void SetStatus(EFFECTSTATUS sts)
	{
		m_sts = sts;
	}
	//�r���{�[�h�Ƃ��Ĉ�������ݒ�
	void SetBillBoard(bool trigger)
	{
		m_isBillBolard = trigger;
	}

	//�r���{�[�h�Ƃ��Ĉ�������ݒ�
	void SetHorming(bool trigger)
	{
		m_isHorming = trigger;
	}

	//�C���^�[�o����ݒ�
	void SetInterval(XMFLOAT3 pos)
	{
	     m_interval = pos;
	}

	void SetTargetP(XMFLOAT3* targetp)
	{
		if (m_Targetpos != targetp) {
			m_animationNo = 0;
			m_Targetpos = targetp;
		}
	}

	void DeleteTargetP(void)
	{
		m_Targetpos = nullptr;
		m_animationNo = 0;
	}

	void Uninit(void)
	{
		if (m_vbuffer != nullptr) {
			m_vbuffer->Release();
			m_vbuffer = nullptr;
		}
	}

	//���C�t��n��
	uint32_t GetLife()
	{
		return m_life;
	}
	//�ő僉�C�t��n��
	uint32_t GetMaxLife()
	{
		return m_Maxlife;
	}
	EFFECTLIST GetType()
	{
		return m_type;
	}
	//�X�e�[�^�X��n��
    EFFECTSTATUS GetStatus()
	{
	 return m_sts;
	}

	XMFLOAT3* GetTargetPos()
	{
		return m_Targetpos;
	}

	//�r���{�[�h���ǂ�����n��
	bool GetBillBoard()
	{
	    return m_isBillBolard;
	}
	CBillBoard():m_x(0),m_y(0),m_z(0),m_srv(nullptr){

	}

	bool Init(float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color){
		m_x = x;
		m_y = y;
		m_z = z;
		m_XSize = xsize;
		m_YSize = ysize;
		m_Color = color;

		m_sts = EFFECTSTATUS::LIVE;

		// �f�o�C�X�擾
		ID3D11Device* dev = GetDX11Device();
		// �f�o�C�X�R���e�L�X�g�擾
		ID3D11DeviceContext* devcontext = GetDX11DeviceContext();

		// ���_�f�[�^�̒�`
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		unsigned int numElements = ARRAYSIZE(layout);

		// ���_�V�F�[�_�[�I�u�W�F�N�g�𐶐��A�����ɒ��_���C�A�E�g������
		bool sts = CreateVertexShader(dev,
			"shader/vsbillboard.hlsl",
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
			dev,							// �f�o�C�X�I�u�W�F�N�g
			"shader/psbillboard.hlsl", 
			"main", 
			"ps_4_0",
			&m_pPixelShader);

		if (!sts) {
			MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
			return false;
		}

		CalcVertex();						// �r���{�[�h�p�̒��_�f�[�^�쐬	

		CreateBlendStateSrcAlpha();			// �A���t�@�u�����f�B���O�p�u�����h�X�e�[�g����
		CreateBlendStateOne();				// ���Z�����p�̃u�����h�X�e�[�g����
		CreateBlendStateDefault();			// �f�t�H���g�̃u�����h�X�e�[�g����

		return true;
	}

	//�X�V����
	void Update()
	{
		//�t�F�[�h����
		//XMFLOAT4 color = m_Color;
		//color.w = m_life / m_Maxlife;
		//SetColor(color);
		
		//�A�j���[�V�����̏ꍇ
		m_delaycount++;

		static int SEdelay = 0;
		static bool TargetComplate = false;
		SEdelay++;
		//�v���O���X�o�[�̏ꍇ
		if (m_delaycount >= m_animationDelay) {
			m_delaycount = 0;
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
				SetUV(uv);

				//�z�[�~���O����ꍇ
				if (m_isHorming && m_Targetpos != nullptr)
				{
					m_x = m_Targetpos->x + m_interval.x;
					m_y = m_Targetpos->y + m_interval.y;
					m_z = m_Targetpos->z + m_interval.z;
				}

				//�A�j���[�V�����J�E���g��i�߂�
				if (m_type != EFFECTLIST::PROGRESSBAR_CIRCLE) {
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
						m_sts = EFFECTSTATUS::DEAD;
					}
				}
			}
		}
	}

	void Dispose() {
		/*if (m_vbuffer != nullptr) {
			m_vbuffer->Release();
			m_vbuffer = nullptr;
		}*/

		//if (m_srv != nullptr) {
		//	m_srv->Release();
		//	m_srv = nullptr;
		//}

		//if (m_pBlendStateOne != nullptr) {
		//	m_pBlendStateOne->Release();
		//	m_pBlendStateOne = nullptr;
		//}

		//if (m_pBlendStateSrcAlpha != nullptr) {
		//	m_pBlendStateSrcAlpha->Release();
		//	m_pBlendStateSrcAlpha = nullptr;
		//}

		//if (m_pBlendStateDefault != nullptr) {
		//	m_pBlendStateDefault->Release();
		//	m_pBlendStateDefault = nullptr;
		//}

		//if (m_pPixelShader != nullptr) {
		//	m_pPixelShader->Release();
		//	m_pPixelShader = nullptr;
		//}

		//if (m_pVertexShader != nullptr) {
		//	m_pVertexShader->Release();
		//	m_pVertexShader = nullptr;
		//}

		//if (m_pVertexLayout != nullptr) {
		//	m_pVertexLayout->Release();
		//	m_pVertexLayout = nullptr;
		//}
	}

	// �f�X�g���N�^
	virtual ~CBillBoard(){
		Dispose();
	}

	// �t�u���W���Z�b�g����
	void SetUV(float u[],float v[]){
		m_Vertex[0].tu = u[0];
		m_Vertex[0].tv = v[0];

		m_Vertex[1].tu = u[1];
		m_Vertex[1].tv = v[1];

		m_Vertex[2].tu = u[2];
		m_Vertex[2].tv = v[2];

		m_Vertex[3].tu = u[3];
		m_Vertex[3].tv = v[3];

		CalcVertex();						// �r���{�[�h�p�̒��_�f�[�^�쐬	
	}

	// �ʒu���w��
	void SetPosition(float x,float y,float z);

	// �r���{�[�h��`��
	void DrawBillBoard(const DirectX::XMFLOAT4X4& cameramat);

	// �r���{�[�h�`����Z����
	void DrawBillBoardAdd(const DirectX::XMFLOAT4X4& cameramat);

	// �r���{�[�h��Z���𒆐S�ɂ��ĉ�]�����ĕ`��
	void DrawRotateBillBoard(const DirectX::XMFLOAT4X4 &cameramat, float radian);
	
	// �T�C�Y���Z�b�g
	void SetSize(float x, float y);

	// �J���[���Z�b�g
	void SetColor(DirectX::XMFLOAT4 col);

	//UV�J�b�g�����Z�b�g
	void SetCutUV(uint16_t CutU, uint16_t CutV)
	{
		m_cutU = CutU;
		m_cutV = CutV;
	}

	//�A�j���[�V�����x����ݒ�
	void SetAnimationDelay(int Delay)
	{
		m_animationDelay = Delay;
		m_delaycount = 0;
	}


	void SetType(EFFECTLIST type)
	{
		m_type = type;
	}

	//	�e�N�X�`���ǂݍ���
	bool LoadTexTure(const char* filename);

};