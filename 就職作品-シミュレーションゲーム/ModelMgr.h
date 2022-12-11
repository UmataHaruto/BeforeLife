#pragma once
#include	<memory>
#include	<wrl/client.h>
#include	<unordered_map>
#include	"CModel.h"
#include    <vector>
#include	"dx11mathutil.h"
#include    <locale.h>

#define ALIGN16 _declspec(align(16))

using Microsoft::WRL::ComPtr;

//���f��ID
enum class MODELID {
	PLAYER,
	HAIR_00,
	PICKAXE,
	AXE,
	BASKET,
	SKYBOX,
	GRASS,
	CONIFER00,
	ORE_IRON,
	SMALLHOUSE,
	TERRAIN,
	TERRAIN_COLLISION,
	SMALLSOUKO,
	CREATE_PLANE,
	CREATE_WOOD,
	CREATE_COAL,
	CREATE_IRON,
	CREATE_GOLD,
	ROUTE_PREVIEW,
	ROAD_DIRT,
};

//���f���f�[�^���X�g
struct ModelDataList
{
	MODELID id;
	std::string modelname;
	std::string vsfilename;
	std::string psfilename;
	std::string texfolder;
};

struct SIMPLESHADER_CONSTANT_BUFFER
{
	XMMATRIX mWVP;//���[���h�A�r���[�A�ˉe�̍����ϊ��s��
	XMMATRIX mW;//���[���h�s��
	ALIGN16 XMFLOAT3 vEyePos;//�e���_���猩���A���_�̈ʒu
	ALIGN16 float fMinDistance;
	ALIGN16 float fMaxDistance;
	ALIGN16 int iMaxDevide;
	ALIGN16 XMFLOAT4 LightDir;
	ALIGN16 XMFLOAT4 WaveMove;
};

struct MY_MATERIAL
{
	CHAR szName[111];
	XMFLOAT4 Ka;//�A���r�G���g
	XMFLOAT4 Kd;//�f�B�t���[�Y
	XMFLOAT4 Ks;//�X�y�L�����[
	CHAR szTextureName[110];//�e�N�X�`���[�t�@�C����
};

class ModelMgr {
private:
	ModelMgr(){
	}
	std::unordered_map<std::string, std::unique_ptr<CModel>> m_modelhashmap;
	//�}�e���A�����
	MY_MATERIAL m_Material;
public:
	ModelMgr(const ModelMgr&) = delete;
	ModelMgr& operator=(const ModelMgr&) = delete;
	ModelMgr(ModelMgr&&) = delete;
	ModelMgr& operator=(ModelMgr&&) = delete;

	std::vector<ModelDataList> g_modellist = {
	{MODELID::PLAYER,"assets/ModelData/male_adult/male_adult.fbx","shader/vsoneskin.fx","shader/ps.fx","assets/ModelData/male_adult/"},
	{(const MODELID)MODELID::HAIR_00,"assets/ModelData/male_adult/Hair/Hair_00.obj","shader/vs_hair.hlsl","shader/ps_hair.hlsl","assets/ModelData/male_adult/Hair/"},
	{(const MODELID)MODELID::PICKAXE,"assets/ModelData/Tools/Pickaxe/Pickaxe.obj","shader/vs.fx","shader/ps.fx","assets/ModelData/Tools/Pickaxe/"},
	{(const MODELID)MODELID::AXE,"assets/ModelData/Tools/Axe/Axe.obj","shader/vs.fx","shader/ps.fx","assets/ModelData/Tools/Axe/"},
	{(const MODELID)MODELID::BASKET,"assets/ModelData/Tools/Basket/Basket.obj","shader/vs.fx","shader/ps.fx","assets/ModelData/Tools/Basket/"},
	{(const MODELID)MODELID::SKYBOX,"assets/skydome/Dome_X502.x","shader/vs.fx","shader/BillBoard.hlsl","assets/skydome/"},
	{(const MODELID)MODELID::GRASS,"assets/Modeldata/Block/GrassBlock.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/Block/"},
	{(const MODELID)MODELID::CONIFER00,"assets/Modeldata/tree/conifer00/conifer01.fbx","shader/vsoneskin.fx","shader/ps.fx","assets/Modeldata/tree/conifer00/"},
	{(const MODELID)MODELID::ORE_IRON,"assets/Modeldata/Ore/Normal/Ore_Normal.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/Ore/Normal/"},
	{(const MODELID)MODELID::SMALLHOUSE,"assets/Modeldata/House/House_Small.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/House/"},
	{(const MODELID)MODELID::TERRAIN,"assets/Modeldata/terrain/terrain.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/terrain/"},
	{(const MODELID)MODELID::TERRAIN,"assets/Modeldata/terrain/terrainColision.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/terrain/"},
	{(const MODELID)MODELID::SMALLSOUKO,"assets/Modeldata/Souko/Souko.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/Souko/"},
	{(const MODELID)MODELID::CREATE_WOOD,"assets/Modeldata/Create/CreatePlane.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/Create/"},
	{(const MODELID)MODELID::CREATE_WOOD,"assets/Modeldata/Create/CreateWood.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/Create/"},
	{(const MODELID)MODELID::CREATE_COAL,"assets/Modeldata/Create/CreateCoalOre.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/Create/"},
	{(const MODELID)MODELID::CREATE_IRON,"assets/Modeldata/Create/CreateIronOre.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/Create/"},
	{(const MODELID)MODELID::CREATE_GOLD,"assets/Modeldata/Create/CreateGoldOre.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/Create/"},
	{(const MODELID)MODELID::ROUTE_PREVIEW,"assets/Modeldata/RoutePreview.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/"},
	{(const MODELID)MODELID::ROAD_DIRT,"assets/Modeldata/Block/Road_Dirt.obj","shader/vs.fx","shader/ps.fx","assets/Modeldata/Block/"},

	};

	static ModelMgr& GetInstance() {
		static ModelMgr Instance;
		return Instance;
	}

	virtual ~ModelMgr() {
		Finalize();
	}

	//�I������
	void Finalize() {
		for (auto itr = m_modelhashmap.begin(); itr != m_modelhashmap.end(); ++itr) {
			(itr->second)->Uninit();
		}
	}
	//���f����ǂݍ���(���f����,vs�t�@�C��,ps�t�@�C��,�e�N�X�`���t�@�C��)
	bool LoadModel(std::string modelfilename, std::string vsfilename,std::string psfilename,std::string texfilefolder) {

		std::unique_ptr<CModel>	p;

		p = std::make_unique<CModel>();

		ID3D11Device* pdevice = CDirectXGraphics::GetInstance()->GetDXDevice();
		ID3D11DeviceContext* m_DeviceContext = CDirectXGraphics::GetInstance()->GetImmediateContext();
		ID3D11DepthStencilView* m_pBackBuffer_DSTexDSV = CDirectXGraphics::GetInstance()->GetDepthStencilView();
		ID3D11Texture2D* m_pBackBuffer_DSTex;
		ID3D11RenderTargetView* m_pBackBuffer_TexRTV = CDirectXGraphics::GetInstance()->GetRenderTargetView();
		ID3D11ShaderResourceView* m_srv = nullptr;

		//�V�F�[�_�[Resource�r���[�̐���
		const char* multibyte = "Assets/Modeldata/WaterBump.bmp";
		wchar_t widechar[100];

		setlocale(LC_CTYPE, "jpn");

		mbstowcs(widechar, multibyte, 100);
		const wchar_t* texturePath = widechar;

		CreateShaderResourceViewFromFile(m_srv,texturePath);

		//�f�v�X�X�e���V���r���[�p�̃e�N�X�`���[���쐬
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = 700;
		descDepth.Height = 500;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		pdevice->CreateTexture2D(&descDepth, NULL, &m_pBackBuffer_DSTex);
		//���̃e�N�X�`���[�ɑ΂��f�v�X�X�e���V���r���[(DSV)���쐬
		pdevice->CreateDepthStencilView(m_pBackBuffer_DSTex, NULL, &m_pBackBuffer_DSTexDSV);

		//�����_�[�^�[�Q�b�g�r���[�Ɛ[�x�X�e���V���r���[���p�C�v���C���Ƀo�C���h
		m_DeviceContext->OMSetRenderTargets(1, &m_pBackBuffer_TexRTV, m_pBackBuffer_DSTexDSV);

		//�e�N�X�`���[�p�T���v���[�쐬
		D3D11_SAMPLER_DESC SamDesc;
		ID3D11SamplerState* m_pSamPoint = nullptr;//�e�N�X�`���[�̃T���v���[

		ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		SamDesc.MaxAnisotropy = 16;
		SamDesc.MinLOD = 0;
		SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
		pdevice->CreateSamplerState(&SamDesc, &m_pSamPoint);


		// ���f���ǂݍ���
		bool sts = p->Init(modelfilename.c_str(), vsfilename.c_str(), psfilename.c_str(), texfilefolder.c_str());
		if (!sts) {
			return false;
		}

		// unordered_map�R���e�i�Ɋi�[
		m_modelhashmap[modelfilename].swap(p);

		return true;
	}

	//���f���̃L�[���擾
	CModel* GetModelPtr(std::string key) {
		// ���݂��邩���m���߂�
		auto it = m_modelhashmap.find(key);
		if (it == m_modelhashmap.end()) {
			return nullptr;
		}
		return m_modelhashmap[key].get();
	}

	void CreateShaderResourceViewFromFile(ID3D11ShaderResourceView* m_pTexture, const wchar_t* FileName);

};