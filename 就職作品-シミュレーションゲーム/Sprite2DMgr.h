#pragma once
#include	"gameobject.h"
#include	"CModel.h"
#include	"dx11mathutil.h"
#include	"CDirectInput.h"
#include    "CBillBoard.h"
#include    "quad2d.h"
#include    "Window.h"
#include    "Resource.h"

#include	<wrl/client.h>
#include	<unordered_map>

class Sprite2DMgr{

	struct EFFECT_PATH
	{
		EFFECTLIST id;

		const char* texturepath;
		bool isAnimation;
		bool isLoop;
		bool isHorming;
		uint16_t cutU;
		uint16_t cutV;
		int AnimationDelay;
	};

	struct UI_PATH
	{
		UILIST id;

		const char* texturepath;
		bool isAnimation;
		bool isLoop;
		uint16_t cutU;
		uint16_t cutV;
		int AnimationDelay;
	};

	struct ProgressData
	{
		CBillBoard m_bar;
		CBillBoard m_targetimage;

		Resource::Data* m_data;
	};
private:
	Sprite2DMgr() {
	}
	//�G�t�F�N�g�e���v���[�g�i�[�x�N�^�[
	std::vector<EFFECT_PATH>g_EffectTemplate = {
	{EFFECTLIST::TARGETCIRCLE,"assets/sprite/TargetCircle.png",true,false,false,6,2,1},
	{EFFECTLIST::PROGRESSBAR_CIRCLE,"assets/sprite/Progressbar_Circle.png",true,false,false,10,6,1},
	{EFFECTLIST::WOOD,"assets/sprite/UI/Resource/Wood.png",false,false,false,1,1,1},
	{EFFECTLIST::ORE_COAL,"assets/sprite/UI/Resource/Coal.png",false,false,false,1,1,1},
	{EFFECTLIST::ORE_IRON,"assets/sprite/UI/Resource/ironore.png",false,false,false,1,1,1},
	{EFFECTLIST::ORE_GOLD,"assets/sprite/UI/Resource/Goldore.png",false,false,false,1,1,1},
	{EFFECTLIST::IRON,"assets/sprite/UI/Resource/Iron.png",false,false,false,1,1,1},
	{EFFECTLIST::GOLD,"assets/sprite/UI/Resource/Gold.png",false,false,false,1,1,1},
	};

	//UI�e���v���[�g�i�[�x�N�^�[
	std::vector<UI_PATH>g_UITemplate = {
		
		{UILIST::BLACKBACK_START ,"assets/sprite/UI/BlackBack.png",false,false,1,1,1},
		{UILIST::BLACKBACK_END ,"assets/sprite/UI/BlackBack.png",false,false,1,1,1},
		{UILIST::ICON_MOUSE ,"assets/sprite/UI/MouseIcon.png",false,false,1,1,1},

	};


	//���f���f�[�^�i�[
	std::vector<std::shared_ptr<CBillBoard>>g_modelhash;

	//UI�f�[�^�i�[
	std::vector<std::shared_ptr<Quad2D>>g_uihash;
public:
	//�G�t�F�N�g�i�[�x�N�^�[
	std::vector<CBillBoard>g_effects = {};
	std::vector<Quad2D>g_UserInterfaces = {};
	std::vector<Window*>g_Windows = {};
	//�v���O���X�o�[�f�[�^�i�[
	std::vector<ProgressData>g_bars = {};


	static Sprite2DMgr& GetInstance() {
		static Sprite2DMgr Instance;
		return Instance;
	}



	//������
	bool Init();

	//�`��
	void Draw(XMFLOAT4X4 camera_mtx);

	//�X�V
	void Update();

	//�폜
	void ClearEffect();
	void ClearUi();
	void ClearWindow();

	//�I������
	void Finalize();

	//UI���쐬
	void CreateUI(UILIST type, float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color);

	//�E�B���h�E���쐬
	void CreateWindowSprite(DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 Size, DirectX::XMFLOAT4 color, bool CancelKey, const char* TexturePath);

	//�^�[�Q�b�g�����Z�b�g
	void ResetTarget();

	XMFLOAT3* GetTarget();

	//�G�̃~�j�}�b�v�A�C�R���쐬
	void CreateEnemyMinimap(float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color,XMFLOAT3* EnemyPos);

	//UI�A�j���[�V�������쐬
	void CreateAnimation(UILIST type, float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color);

	//�v���O���X�o�[����
	void CreateCircleProgressBar(Resource::Data* resource,float xsize,float ysize);
	//�G�t�F�N�g���쐬
	void CreateEffect(EFFECTLIST type,float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color);

	//�A�j���[�V�����G�t�F�N�g���쐬
	void CreateAnimation(EFFECTLIST type,float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color,XMFLOAT3* ptarget);

	//�^�[�Q�b�g�͈͓����𔻒�
	bool TargetSelect(XMFLOAT3* EnemyPos);

	//�^�[�Q�b�g���ő�܂ŒB���Ă��邩
	bool CheckFullTarget();

	//�v���C���[���~�j�}�b�v�ɕ\��
	void MiniMapPlayer(XMFLOAT3* PlayerPos,XMFLOAT4X4* PlayerMtx);

	//�G���~�j�}�b�v�ɕ\��
	void MiniMapEnemy();

	//���e�B�N����\���X�V
	void ReticleUpdate(XMFLOAT3* PlayerPos, XMFLOAT4X4* PlayerMtx);

	//���f���̃L�[���擾
	std::shared_ptr<CBillBoard> GetModelPtr(EFFECTLIST type) {
		// ���݂��邩���m���߂�
		return GetInstance().g_modelhash[(int)type];
	}

	//���f���̃L�[���擾
	std::shared_ptr<Quad2D> GetUIPtr(UILIST type) {
		// ���݂��邩���m���߂�
		return GetInstance().g_uihash[(int)type];
	}
};