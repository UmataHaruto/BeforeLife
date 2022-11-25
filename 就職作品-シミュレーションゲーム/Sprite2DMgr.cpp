#include "Sprite2DMgr.h"

#include	"drawaxis.h"
#include    "DX11util.h"
#include    "dx11mathutil.h"
#include    "CBillBoard.h"
#include    "CCamera.h"
#include "mystring.h"
#include "Setting.h"

bool Sprite2DMgr::Init() {
	//�G�t�F�N�g�̏�����
	for (auto& b : g_EffectTemplate)
	{
		std::shared_ptr<CBillBoard>	p(std::make_shared<CBillBoard>());
	    p->LoadTexTure(b.texturepath);

		p->Init(0, 0, 200, 100, 100, XMFLOAT4(1, 1, 1, 1));

		//�r���{�[�h�L����
		p->SetBillBoard(true);

		//�A�j���[�V�����ݒ�
		p->SetAnimation(b.isAnimation, b.isLoop);
		p->SetCutUV(b.cutU,b.cutV);
		p->SetType(b.id);
		p->SetHorming(b.isHorming);
		p->SetAnimationDelay(b.AnimationDelay);
		

		//�������C�t��ݒ�
		p->SetLife(200);

		//�r���{�[�h������
		p->SetPosition(0, 0, 0);
		p->SetSize(100,100);
		p->SetColor(XMFLOAT4(1, 1, 1, 1));

		//UV���Z�b�g
		XMFLOAT2 uv[4];
		uv[0].x = 0.0f;
		uv[0].y = 0.0f;
		uv[1].x = 1.0f;
		uv[1].y = 0.0f;
		uv[2].x = 0.0f;
		uv[2].y = 1.0f;
		uv[3].x = 1.0f;
		uv[3].y = 1.0f;

		p->SetUV(uv);

		// ���f�����X�g�Ɋi�[
		g_modelhash.push_back(p);
	}

	//UI�̏�����
	for (auto& b : g_UITemplate)
	{
		std::shared_ptr<Quad2D>	p(std::make_shared<Quad2D>());
		p->LoadTexture(b.texturepath);
		XMFLOAT2 uv[4];
		uv[0].x = 0.0f;
		uv[0].y = 0.0f;
		uv[1].x = 1.0f;
		uv[1].y = 0.0f;
		uv[2].x = 0.0f;
		uv[2].y = 1.0f;
		uv[3].x = 1.0f;
		uv[3].y = 1.0f;

		p->Init(100, 100, XMFLOAT4(1, 1, 1, 1),uv);

		p->SetPosition(100,100,0);
		p->SetStatus(UISTATUS::LIVE);
		p->SetType(b.id);

		//�A�j���[�V�����ݒ�
		p->SetAnimation(b.isAnimation, b.isLoop);
		p->SetCutUV(b.cutU, b.cutV);
		p->SetAnimationDelay(b.AnimationDelay);

		p->updateVertex(200,200,XMFLOAT4(1,1,1,1),uv);
		p->updateVbuffer();

		p->SetPosition(100, 100, 0);

		// ���f�����X�g�Ɋi�[
		g_uihash.push_back(p);
	}

	return true;
}

void Sprite2DMgr::Draw(XMFLOAT4X4 camera_mtx) {
	//�r���{�[�h�̕`��
	TurnOffZbuffer();
	for (int i = 0; i < g_effects.size(); i++) {

		g_effects[i].DrawBillBoard(camera_mtx);
	}
	//�v���O���X�o�[
	for (int i = 0; i < g_bars.size(); i++) {

		g_bars[i].m_bar.DrawBillBoard(camera_mtx);
		g_bars[i].m_targetimage.DrawBillBoard(camera_mtx);

	}
	//�E�B���h�E�̕`��
	for (int i = 0; i < g_Windows.size(); i++) {

		g_Windows[i]->Draw();
	}
	//UI�̕`��
	for (int i = 0; i < g_UserInterfaces.size(); i++) {

			g_UserInterfaces[i].Draw();
	}
	TurnOffZbuffer();
}

void Sprite2DMgr::Update() {

	//�r���{�[�h�̍X�V
	{
		for (int i = 0; i < g_effects.size(); i++) {
			g_effects[i].SetLife(g_effects[i].GetLife() - 1);

			g_effects[i].Update();

			//�A�j���[�V������i�߂�
			if (g_effects[i].GetLife() > 0)
			{

			}
			//����ł���ꍇ
			else
			{
				g_effects[i].SetStatus(EFFECTSTATUS::DEAD);
			}
		}
		//����ł���G�t�F�N�g���폜
		auto it = g_effects.begin();
		while (it != g_effects.end())
		{
			if ((*it).GetStatus() == EFFECTSTATUS::DEAD)
			{
				(*it).Uninit();
				it = g_effects.erase(it);
			}
			else
			{
				++it;
			}
		}

	}
	//�v���O���X�o�[�̍X�V
	{
		for (int i = 0; i < g_bars.size(); i++) {

			//�A�j���[�V�����ԍ�=�p�[�Z���g
			g_bars[i].m_bar.m_animationNo = (g_bars[i].m_data->EnduranceMax - g_bars[i].m_data->Endurance) / 2;
			if (g_bars[i].m_bar.m_animationNo % 1 != 0)
			{
				g_bars[i].m_bar.m_animationNo -= 0.5;
			}
			g_bars[i].m_bar.Update();
			g_bars[i].m_targetimage.Update();

			//�A�j���[�V������i�߂�
			if (g_bars[i].m_data->Endurance > 0)
			{
			}
			//����ł���ꍇ
			else
			{
				g_bars[i].m_bar.SetStatus(EFFECTSTATUS::DEAD);
				g_bars[i].m_targetimage.SetStatus(EFFECTSTATUS::DEAD);

			}
		}
		//����ł���G�t�F�N�g���폜
		auto it = g_bars.begin();
		while (it != g_bars.end())
		{
			if ((*it).m_bar.GetStatus() == EFFECTSTATUS::DEAD)
			{
				(*it).m_bar.Uninit();
				(*it).m_targetimage.Uninit();

				it = g_bars.erase(it);
			}
			else
			{
				++it;
			}
		}

	}
	//UI�̍X�V
	{
		for (int i = 0; i < g_UserInterfaces.size(); i++) {
			g_UserInterfaces[i].Update();
		}
		//����ł���UI���폜
		auto it = g_UserInterfaces.begin();

		while (it != g_UserInterfaces.end())
		{
			if ((*it).GetStatus() == UISTATUS::DEAD)
			{
				it = g_UserInterfaces.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	//�E�B���h�E�̍X�V
	for (int i = 0; i < g_Windows.size(); i++) {
		g_Windows[i]->Update();

		//�X�V
		for (int j = 0; j < g_Windows[i]->m_Buttons.size(); j++) {
			g_Windows[i]->m_Buttons[j].Update();
			//�N���b�N����Ă���C�x���g�����s
			if (g_Windows[i]->m_Buttons[j].GetSts()) {
				switch (j)
				{
				case 0:
					g_Windows[i]->ClickEvent_00();
					break;
				case 1:
					g_Windows[i]->ClickEvent_01();
					break;
				case 2:
					g_Windows[i]->ClickEvent_02();
					break;
				case 3:
					g_Windows[i]->ClickEvent_03();
					break;
				case 4:
					g_Windows[i]->ClickEvent_04();
					break;
				case 5:
					g_Windows[i]->ClickEvent_05();
					break;
				case 6:
					g_Windows[i]->ClickEvent_06();
					break;
				case 7:
					g_Windows[i]->ClickEvent_07();
					break;
				case 8:
					g_Windows[i]->ClickEvent_08();
					break;
				case 9:
					g_Windows[i]->ClickEvent_09();
					break;
				default:
					break;
				}
			}
		}
	}
	//�E�B���h�E�̔j��
	{
		auto it = g_Windows.begin();
		while (it !=g_Windows.end())
		{
			//�j��t���O�������Ă���ꍇ
			if ((*it)->GetDestroy())
			{
				(*it)->UnInit();
				it = g_Windows.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void Sprite2DMgr::ClearEffect()
{
	g_effects.clear();
}

void Sprite2DMgr::ClearUi()
{
	g_UserInterfaces.clear();
}

void Sprite2DMgr::ClearWindow()
{
	g_Windows.clear();
}

void Sprite2DMgr::Finalize() {
}

void Sprite2DMgr::CreateUI(UILIST type, float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color)
{
	Quad2D board = *Sprite2DMgr::GetInstance().GetUIPtr(type);

	//�r���{�[�h������
	board.SetColor(color);
	board.SetScale(xsize, ysize, 0);
	board.SetPosition(x, y, z);
	board.SetType(type);

	//UV���Z�b�g
	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 1.0f;
	uv[3].x = 1.0f;
	uv[3].y = 1.0f;

	board.updateVbuffer();

	//�G�t�F�N�g���X�g�֊i�[
	g_UserInterfaces.push_back(board);
}

void Sprite2DMgr::CreateWindowSprite(DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 Size, DirectX::XMFLOAT4 color,bool CancelKey, const char* TexturePath)
{
	Setting* board = new Setting(Pos,Size,color,CancelKey,TexturePath);
	g_Windows.push_back(board);
}

void Sprite2DMgr::ResetTarget()
{
	
}
void Sprite2DMgr::CreateEnemyMinimap(float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color, XMFLOAT3* EnemyPos)
{
}

void Sprite2DMgr::CreateAnimation(UILIST type, float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color)
{
	Quad2D board = *Sprite2DMgr::GetInstance().GetUIPtr(type);

	//�r���{�[�h������
	board.SetScale(xsize, ysize, 0);
	board.SetPosition(x, y, z);

	//UV���Z�b�g
	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 1.0f;
	uv[3].x = 1.0f;
	uv[3].y = 1.0f;

	//�G�t�F�N�g���X�g�֊i�[
	g_UserInterfaces.push_back(board);
}

void Sprite2DMgr::CreateCircleProgressBar(Resource::Data* resource, float xsize, float ysize)
{
	if (resource != nullptr) {
		ProgressData data;
		//�o�[�𐶐�
		CBillBoard board = *Sprite2DMgr::GetInstance().GetModelPtr(EFFECTLIST::PROGRESSBAR_CIRCLE);
		//�C���[�W����
		CBillBoard image = *Sprite2DMgr::GetInstance().GetModelPtr((EFFECTLIST)((int)resource->type + 2));

		board.m_vbuffer = nullptr;
		image.m_vbuffer = nullptr;
		//�r���{�[�h�L����
		board.SetBillBoard(true);
		image.SetBillBoard(true);

		//�������C�t��ݒ�
		board.SetLife(90);
		image.SetLife(90);

		//�r���{�[�h������
		board.SetPosition(resource->pos.x, resource->pos.y + 30, resource->pos.z);
		board.SetSize(xsize, ysize);
		board.SetColor(XMFLOAT4(1, 1, 1, 1));
		board.SetAnimationNo(0);

		image.SetPosition(resource->pos.x, resource->pos.y + 30, resource->pos.z);
		image.SetSize(xsize - 10, ysize - 10);
		image.SetColor(XMFLOAT4(1, 1, 1, 1));
		image.SetAnimationNo(0);
		//UV���Z�b�g
		XMFLOAT2 uv[4];
		uv[0].x = 0.0f;
		uv[0].y = 0.0f;
		uv[1].x = 1.0f;
		uv[1].y = 0.0f;
		uv[2].x = 0.0f;
		uv[2].y = 1.0f;
		uv[3].x = 1.0f;
		uv[3].y = 1.0f;

		board.SetUV(uv);
		image.SetUV(uv);

		//�G�t�F�N�g���X�g�֊i�[
		data.m_bar = board;
		data.m_targetimage = image;

		//���_�o�b�t�@����
		ID3D11Device* dev;
		dev = GetDX11Device();

		ID3D11Buffer* vbuffer = nullptr;		// ���_�o�b�t�@
		ID3D11Buffer* Imagevbuffer = nullptr;		// ���_�o�b�t�@

		// ���_�o�b�t�@�쐬�i��ŕύX�\�j
		bool sts = CreateVertexBufferWrite(dev, sizeof(MyVertex), 4, board.m_Vertex, &vbuffer);
		if (!sts) {
			MessageBox(nullptr, "create vertex buffer error(CBillBoard)", "error", MB_OK);
		}

		data.m_bar.m_vbuffer = vbuffer;

		// ���_�o�b�t�@�쐬�i��ŕύX�\�j
		sts = CreateVertexBufferWrite(dev, sizeof(MyVertex), 4, image.m_Vertex, &Imagevbuffer);
		if (!sts) {
			MessageBox(nullptr, "create vertex buffer error(CBillBoard)", "error", MB_OK);
		}

		data.m_targetimage.m_vbuffer = Imagevbuffer;
		data.m_data = resource;

		//�i�[
		g_bars.push_back(data);
	}
}

void Sprite2DMgr::CreateEffect(EFFECTLIST type,float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color)
{
	CBillBoard board = *Sprite2DMgr::GetInstance().GetModelPtr(type);

	board.m_vbuffer = nullptr;

	//�r���{�[�h�L����
	board.SetBillBoard(true);

	//�������C�t��ݒ�
	board.SetLife(90);

	//�r���{�[�h������
	board.SetPosition(x,y,z);
	board.SetSize(xsize,ysize);
	board.SetColor(color);

	//UV���Z�b�g
	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 1.0f;
	uv[3].x = 1.0f;
	uv[3].y = 1.0f;

	board.SetUV(uv);

	//�G�t�F�N�g���X�g�֊i�[
	g_effects.push_back(board);

	//���_�o�b�t�@����
	ID3D11Device* dev;
	dev = GetDX11Device();

	ID3D11Buffer* vbuffer = nullptr;		// ���_�o�b�t�@

	// ���_�o�b�t�@�쐬�i��ŕύX�\�j
	bool sts = CreateVertexBufferWrite(dev, sizeof(MyVertex), 4, board.m_Vertex, &vbuffer);
	if (!sts) {
		MessageBox(nullptr, "create vertex buffer error(CBillBoard)", "error", MB_OK);
	}

	g_effects[g_effects.size()].m_vbuffer = vbuffer;
}

void Sprite2DMgr::CreateAnimation(EFFECTLIST type,float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color,XMFLOAT3* ptarget)
{
	CBillBoard board = *Sprite2DMgr::GetInstance().GetModelPtr(type);

	board.m_vbuffer = nullptr;

	//�r���{�[�h�L����
	board.SetBillBoard(true);

	//�������C�t��ݒ�
	board.SetLife(90);

	//�r���{�[�h������
	board.SetPosition(x, y, z);
	board.SetSize(xsize, ysize);
	board.SetColor(color);
	board.SetAnimationNo(0);
	board.SetTargetP(ptarget);

	//UV���Z�b�g
	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 1.0f;
	uv[3].x = 1.0f;
	uv[3].y = 1.0f;

	board.SetUV(uv);

	//�G�t�F�N�g���X�g�֊i�[
	g_effects.push_back(board);

	//���_�o�b�t�@����
	ID3D11Device* dev;
	dev = GetDX11Device();

	ID3D11Buffer* vbuffer = nullptr;		// ���_�o�b�t�@

	// ���_�o�b�t�@�쐬�i��ŕύX�\�j
	bool sts = CreateVertexBufferWrite(dev, sizeof(MyVertex), 4, board.m_Vertex, &vbuffer);
	if (!sts) {
		MessageBox(nullptr, "create vertex buffer error(CBillBoard)", "error", MB_OK);
	}

	g_effects[g_effects.size() - 1].m_vbuffer = vbuffer;
}

bool Sprite2DMgr::CheckFullTarget()
{
	return true;
}

float length(XMFLOAT3 v) {
	float l;
	l = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return l;
}

// ���K��
XMFLOAT3 normalize(XMFLOAT3 v) {

	float l = length(v);
	XMFLOAT3 t;
	t.x = v.x / l;
	t.y = v.y / l;
	t.z = v.z / l;

	return t;
}

void Sprite2DMgr::MiniMapPlayer(XMFLOAT3* PlayerPos,XMFLOAT4X4* PlayerMtx)
{

}

void Sprite2DMgr::MiniMapEnemy()
{
	
}

void Sprite2DMgr::ReticleUpdate(XMFLOAT3* PlayerPos,XMFLOAT4X4* PlayerMtx)
{
	
}
