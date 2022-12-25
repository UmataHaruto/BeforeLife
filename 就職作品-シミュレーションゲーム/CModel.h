#pragma once
#include	<d3d11.h>
#include	<vector>
#include	"ModelData.h"
#include	"animationdataassimp.h"

enum SELECT_SHADER_TYPE
{
	SELECT_SHADER_TYPE_NORMAL,
	SELECT_SHADER_TYPE_GATHERING,
	SELECT_SHADER_TYPE_CREATE,
	SELECT_SHADER_TYPE_CANCEL,
	SELECT_SHADER_TYPE_RANGE,

	SELECT_SHADER_TYPE_MAX,
	SELECT_SHADER_TYPE_NONE = -1
};

class CModel {
private:
	enum FILETYPE {
		eDATFILE,
		eASSIMPFILE
	};


	FILETYPE				m_filetype = eASSIMPFILE;		// �t�@�C���^�C�v
	ModelData				m_assimpfile;					// assimpfile
	ID3D11VertexShader* m_pVertexShader = nullptr;		// ���_�V�F�[�_�[���ꕨ
	ID3D11PixelShader* m_pPixelShader = nullptr;		// �s�N�Z���V�F�[�_�[���ꕨ
	ID3D11PixelShader* m_pSelectPixelShader[SELECT_SHADER_TYPE_MAX];    //�Z���N�g���V�F�[�_�[���ꕨ
	ID3D11InputLayout* m_pVertexLayout = nullptr;		// ���_�t�H�[�}�b�g��`

	ID3D11ShaderResourceView* m_texSRV = nullptr;			// �e�N�X�`���r�q�u
	//�Z���N�g���
	SELECT_SHADER_TYPE m_selecttype = SELECT_SHADER_TYPE_NONE;
public:
	std::vector<AnimationDataAssimp*> m_animationcontainer;
	unsigned int	m_AnimFileIdx = 0;

	bool Init(const char* filename, const char* vsfile, const char* psfile, std::string texfolder);
	void Uninit();
	void Update(
		unsigned int animno,
		const XMFLOAT4X4& mtxworld);
	void Draw(XMFLOAT4X4& mtxworld);
	void DrawShadow(XMFLOAT4X4& mtxworld,ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in);
	void ChangeColor(XMFLOAT4 color);
	unsigned int GetAnimationNum() {	// �A�j���[�V�����������擾

		return m_animationcontainer[m_AnimFileIdx]->GetScene()->mNumAnimations;
	}

	//Change Select type
	void ChangeSelectType(SELECT_SHADER_TYPE type)
	{
		m_selecttype = type;
	}
	SELECT_SHADER_TYPE GetSelectType()
	{
		return m_selecttype;
	}

	void ChangeAnimFileIdx() {
		m_AnimFileIdx++;
		if (m_AnimFileIdx >= m_animationcontainer.size()) {
			m_AnimFileIdx = 0;
		}
	}



	// �A�j���[�V���������[�h����
	bool LoadAnimation(const char* filename) {
		AnimationDataAssimp* animdata;

		animdata = new AnimationDataAssimp();
		bool sts = animdata->Init(filename);
		if (sts) {
			m_animationcontainer.push_back(animdata);
			return true;
		}
		else {
			MessageBox(nullptr, "Load Animation error", "error", MB_OK);
			return false;
		}
	}

	ModelData& GetModelData() {
		return m_assimpfile;
	}

	//void DrawOBB() {
	//	m_assimpfile.DrawOBB();
	//}

	//void GetOBBList(std::vector<COBB*>& obblist) {

	//	// ���b�V���f�[�^�擾
	//	const std::vector<Mesh>& meshes = m_assimpfile.GetMeshes();

	//	// OBB�S���擾
	//	for (int i = 0; i < meshes.size(); i++) {
	//		const Mesh& m = meshes[i];
	//		for (auto data : m.m_obbvectorcontainer) {
	//			obblist.push_back(data);
	//		}
	//	}
	//}
};
