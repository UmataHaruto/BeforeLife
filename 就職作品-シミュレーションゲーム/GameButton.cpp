#include "GameButton.h"
#include "DX11util.h"
#include <tchar.h>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "Application.h"
#include "CDirectInput.h"
#include "VillagerMgr.h"
#include "ResourceManager.h"
#include "ModelMgr.h"
#include "XAudio2.h"
#include "myimgui.h"
#include <thread>
#include "mousepostoworld.h"
#include "BuildingMgr.h"
#include "Souko.h"
#include "Resource.h"
#include "Road.h"
#include "RouteSearch.h"

GameButton::GameButton()
{
}

GameButton::~GameButton()
{
}

void GameButton::Init()
{
	//�v���r���[���f����ݒ�
	std::unique_ptr<Resource> resource = std::make_unique<Resource>();
	Resource::Data data;
	data.amount = 1;
	data.Endurance = 100;
	data.EnduranceMax = 100;
	data.Hardness = 100;
	data.pos = XMFLOAT3(0, 0, 0);
	data.type =ItemType::ORE_COAL;
	resource->Init(data,MODELID::SMALLHOUSE);
	resource->SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::SMALLHOUSE)].modelname));
	resource->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CREATE);
	modelpreview = std::move(resource);

	m_shadow_parameter.LightPos[0] = -300;
	m_shadow_parameter.LightPos[1] = 900;
	m_shadow_parameter.LightPos[2] = 300;


	m_shadow_parameter.Aspect = 1.0f;
	m_shadow_parameter.farclip = 20000;
	m_shadow_parameter.nearclip = 10;
	m_shadow_parameter.Fov = 300;

	m_Game_HoverButton = GAMEBUTTON_NONE;

	m_io = &ImGui::GetIO();
	std::unique_ptr<bool>m_windowActivate = std::make_unique<bool>(true);



	//�{�^��������(�g���K�[)
	for (int i = 0; i < GAMEBUTTON_MAX; i++)
	{
		m_TriggerButton[i] = false;
	}

	for (int i = 0; i < GAMEBUTTON_MAX; i++)
	{
		m_RadioButton[i] = false;
	}

	for (int i = 0; i < SELECTBUTTON_MAX; i++)
	{
		m_Select_RadioButton[i] = false;
	}
	m_RadioButton[GAMEBUTTON_PLAY_00] = true;
	m_Select_RadioButton[SELECTBUTTON_MOUSE] = true;

	//�摜�\��
	ID3D11Device* device = CDirectXGraphics::GetInstance()->GetDXDevice();
	ID3D11DeviceContext* device11Context = GetDX11DeviceContext();

	//SRV����
	//�E�B���h�E
	CreatetSRVfromFile("assets/sprite/UI/Window.png", device, device11Context, &m_windowback_texture_view);
	//�J�[�\���I��͈�
	CreatetSRVfromFile("assets/sprite/UI/MouseSelectArea.png", device, device11Context, &m_selectarea_texture_view);

	//�V�X�e���{�^��
	CreatetSRVfromFile("assets/sprite/UI/SystemButton_Action.png", device, device11Context, &m_system_action_texture_view);
	CreatetSRVfromFile("assets/sprite/UI/ActionPriorityText.png", device, device11Context, &m_action_priority_texture);

	//�Q�[���{�^��
	CreatetSRVfromFile("assets/sprite/UI/PoseButton_00.png", device, device11Context, &m_texture_view[GAMEBUTTON_STOP]);
	CreatetSRVfromFile("assets/sprite/UI/PlayButton_00.png", device, device11Context, &m_texture_view[GAMEBUTTON_PLAY_00]);
	CreatetSRVfromFile("assets/sprite/UI/PlayButton_01.png", device, device11Context, &m_texture_view[GAMEBUTTON_PLAY_01]);
	CreatetSRVfromFile("assets/sprite/UI/PlayButton_02.png", device, device11Context, &m_texture_view[GAMEBUTTON_PLAY_02]);
	//�Z���N�g�{�^��
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_Mouse.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_MOUSE]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_ALL.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_ALL]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_Cut.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_CUT]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_Mine.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_MINE]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_COLLECT.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_COLLECT]);
	CreatetSRVfromFile("assets/sprite/UI/SelectButton_CANCCEL.png", device, device11Context, &m_select_texture_view[SELECTBUTTON_CANCEL]);

	//���z�{�^��
	CreatetSRVfromFile("assets/sprite/UI/BuildButton_House.png", device, device11Context, &m_build_texture_view[BUILDBUTTON_HOUSE]);
	CreatetSRVfromFile("assets/sprite/UI/BuildButton_Souko.png", device, device11Context, &m_build_texture_view[BUILDBUTTON_SOUKO]);
	CreatetSRVfromFile("assets/sprite/UI/BuildButton_Road.png", device, device11Context, &m_build_texture_view[BUILDBUTTON_ROAD]);

	//���z_�n�E�X�{�^��
	CreatetSRVfromFile("assets/sprite/UI/HouseIcon.png", device, device11Context, &m_build_house_texture_view[HOUSE_SMALL]);
	//���z_�q�Ƀ{�^��
	CreatetSRVfromFile("assets/sprite/UI/SoukoIcon.png", device, device11Context, &m_build_souko_texture_view[SOUKO_SMALL]);
	//���z_���H�{�^��
	CreatetSRVfromFile("assets/sprite/UI/Icon_Road_Dirt.png", device, device11Context, &m_build_road_texture_view[ROAD_DIRT]);

	//�A�C�e���A�C�R��
	CreatetSRVfromFile("assets/sprite/UI/Resource/Wood.png", device, device11Context, &m_resource_preview_texture[(int)ItemType::WOOD]);
	CreatetSRVfromFile("assets/sprite/UI/Resource/Coal.png", device, device11Context, &m_resource_preview_texture[(int)ItemType::ORE_COAL]);
	CreatetSRVfromFile("assets/sprite/UI/Resource/ironore.png", device, device11Context, &m_resource_preview_texture[(int)ItemType::ORE_IRON]);
	CreatetSRVfromFile("assets/sprite/UI/Resource/Goldore.png", device, device11Context, &m_resource_preview_texture[(int)ItemType::ORE_GOLD]);
	CreatetSRVfromFile("assets/sprite/UI/Resource/Iron.png", device, device11Context, &m_resource_preview_texture[(int)ItemType::IRON]);
	CreatetSRVfromFile("assets/sprite/UI/Resource/Gold.png", device, device11Context, &m_resource_preview_texture[(int)ItemType::GOLD]);

	//�p�[�e�B�[���X�g
	CreatetSRVfromFile("assets/sprite/UI/PartyList/Bar_Plane.png", device, device11Context, &m_party_list_texture[(int)PartyListType::BAR_BACK]);
	CreatetSRVfromFile("assets/sprite/UI/PartyList/Bar_Frame.png", device, device11Context, &m_party_list_texture[(int)PartyListType::BAR_FRAME]);
	CreatetSRVfromFile("assets/sprite/UI/PartyList/Bar_Health.png", device, device11Context, &m_party_list_texture[(int)PartyListType::BAR_HELTH]);
	CreatetSRVfromFile("assets/sprite/UI/PartyList/NameBar.png", device, device11Context, &m_party_list_texture[(int)PartyListType::BAR_NAME]);
	CreatetSRVfromFile("assets/sprite/UI/PartyList/Bar_Stamina.png", device, device11Context, &m_party_list_texture[(int)PartyListType::BAR_STAMINA]);
	CreatetSRVfromFile("assets/sprite/UI/PartyList/Emote.png", device, device11Context, &m_party_list_texture[(int)PartyListType::ICON_EMOTE]);

	//�����o�b�t�@
	ID3D11Device* dev;
	dev = CDirectXGraphics::GetInstance()->GetDXDevice();

	// �R���X�^���g�o�b�t�@�쐬
	bool sts = CreateConstantBuffer(
		dev,				// �f�o�C�X
		sizeof(XMFLOAT4),		// �T�C�Y
		&cb_Ambient);			// �R���X�^���g�o�b�t�@7
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(constant buffer Ambient) error", "Error", MB_OK);
	}
	// �R���X�^���g�o�b�t�@�쐬
	sts = CreateConstantBuffer(
		dev,				// �f�o�C�X
		sizeof(XMFLOAT4),		// �T�C�Y
		&cb_time);			// �R���X�^���g�o�b�t�@9
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(constant buffer Time) error", "Error", MB_OK);
	}

	// �R���X�^���g�o�b�t�@�쐬
	sts = CreateConstantBuffer(
		dev,				// �f�o�C�X
		sizeof(XMFLOAT4),		// �T�C�Y
		&cb_HairColor);			// �R���X�^���g�o�b�t�@8
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(constant bufferHairColor) error", "Error", MB_OK);
	}

}

void GameButton::Update()
{
	ImGuiIO& io = ImGui::GetIO();
	modelpreview->SetPos(m_mouseworldpos);
	modelpreview->Update();

	//�f�o�b�O���j���[
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_F1))
	{
		m_debug = 1 - m_debug;
	}

	//�{�^���C�x���g
	if (m_RadioButton[GAMEBUTTON_STOP])
	{
		Application::Instance()->GAME_SPEED = 0;
	}
	if (m_RadioButton[GAMEBUTTON_PLAY_00])
	{
		Application::Instance()->GAME_SPEED = 1.0f;
	}	
	if (m_RadioButton[GAMEBUTTON_PLAY_01])
	{
		Application::Instance()->GAME_SPEED = 3;

	}
	if (m_RadioButton[GAMEBUTTON_PLAY_02])
	{
		//�����l�T
		Application::Instance()->GAME_SPEED = 30;
	}
	//���Ԍo�߃o�b�t�@���X�V
	static float time = 0;
	time += io.DeltaTime;

	ID3D11DeviceContext* devcontext;
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	devcontext->UpdateSubresource(cb_time,
		0,
		nullptr,
		&time,
		0, 0);

	// �R���X�^���g�o�b�t�@4����3���W�X�^�փZ�b�g�i���_�V�F�[�_�[�p�j
	devcontext->VSSetConstantBuffers(9, 1, &cb_time);
	// �R���X�^���g�o�b�t�@4����3���W�X�^�փZ�b�g(�s�N�Z���V�F�[�_�[�p)
	devcontext->PSSetConstantBuffers(9, 1, &cb_time);
}

void GameButton::Draw()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();

	//���l�E�B���h�E���J���Ă��邩
	static bool villagerpropaty = false;
	//�d���D��x�E�B���h�E���J���Ă��邩
	static bool workpriority = false;
	static int edit_select = 0;
	static int resource_select = 0;

	//�{�^���Ƀ}�E�X�I�[�o�[�����ۂɈ�x����SE���Đ�
	static int clickbutton = -1;
	static int clickbutton_old= -1;

	if (clickbutton != -1 && clickbutton_old != clickbutton)
	{
		clickbutton_old = clickbutton;
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/Select_01.wav");
	}

	//�E�B���h�E�ݒ�
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoBackground;
	if (!m_debug) window_flags |= ImGuiWindowFlags_MenuBar;

	ImGui::Begin(u8"�������", m_windowActivate, window_flags);

	//���j���[
	if (ImGui::BeginMenuBar())
	{
		ImGui::SetWindowFontScale(0.3);

		if (ImGui::BeginMenu(u8"�t�@�C��"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(u8"���l"))
		{
			ImGui::MenuItem(u8"�ڍ�", NULL, &villagerpropaty);

			//�L�����N�^�[���𒊏o
			std::string firstname;
			std::string lastname;
			std::string margename[100];

			const char* name[100];
			//�X�|�[�����W
			static float spawnpos[3];
			static float mood = 50;
			static float hitpoint_max = 100;
			static float hitpoint = 50;
			static float stamina_max = 100;
			static float stamina = 50;


			for (int i = 0; i < VillagerMgr::GetInstance().m_villager.size(); i++)
			{
				firstname = VillagerMgr::GetInstance().m_villager[i]->GetName(NameGenerator::MALE).c_str();
				lastname = VillagerMgr::GetInstance().m_villager[i]->GetName(NameGenerator::FAMILLY).c_str();
				margename[i] = firstname + lastname;

				name[i] = margename[i].c_str();
			}
			ImGui::Combo(u8"���l", &edit_select, name, VillagerMgr::GetInstance().m_villager.size());
			ImGui::SameLine();
			//���l�ǉ�
			if (ImGui::Button("+"))
			{
				Player::Data pdata;
				pdata.pos = XMFLOAT3(spawnpos[0], spawnpos[1], spawnpos[2]);
				pdata.firstname = NameGenerator::GetInstance().CreateName(NameGenerator::MALE);
				pdata.lastname = NameGenerator::GetInstance().CreateName(NameGenerator::FAMILLY);
				pdata.mood = mood;
				pdata.hp_max = hitpoint_max;
				pdata.hp = hitpoint;
				pdata.stamina = stamina;
				pdata.stamina_max = stamina_max;
				VillagerMgr::GetInstance().CreateVillager(pdata);
			}
			//�I�����ڍ폜
			ImGui::SameLine();
			if (ImGui::Button("-") && edit_select != 0)
			{
				if (VillagerMgr::GetInstance().m_villager[edit_select] != nullptr &&
					edit_select < VillagerMgr::GetInstance().m_villager.size()) {
					VillagerMgr::GetInstance().m_villager[edit_select]->Finalize();
					VillagerMgr::GetInstance().m_villager.erase(VillagerMgr::GetInstance().m_villager.begin() + edit_select);
					edit_select -= 1;
				}
			}
			ImGui::DragFloat3(u8"�X�|�[�����W", spawnpos, 1, -1500, 1500);
			ImGui::DragFloat(u8"�@��", &mood, 1, 1, 100);
			ImGui::DragFloat(u8"�ő�HP", &hitpoint_max, 1, 1, 1000);
			ImGui::DragFloat(u8"HP", &hitpoint, 1, 1, 1000);
			ImGui::DragFloat(u8"�ő�X�^�~�i", &stamina_max, 1, 1, 1000);
			ImGui::DragFloat(u8"�X�^�~�i", &stamina, 1, 1, 1000);


			static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

			static bool alpha_preview = true;
			static bool alpha_half_preview = true;
			static bool drag_and_drop = true;
			static bool options_menu = true;
			static bool hdr = false;

			ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
			ImGui::ColorEdit4("Hair Color", (float*)&color, ImGuiColorEditFlags_DisplayRGB | misc_flags);

			XMFLOAT4 xm_color = XMFLOAT4(color.x,color.y,color.z,color.w);

			//���F�̕ύX
			VillagerMgr::GetInstance().m_villager[edit_select]->SetHairColor(xm_color);

			ImGui::EndMenu();
		}
		ImGui::SetWindowFontScale(0.3);
		//���f���^�u
		if (ImGui::BeginMenu(u8"���f��"))
		{
			Resource::Data pdata;
			//�X�|�[�����W
			static float spawnpos[3] = { 500,0,-500 };
			static int endurance = 100;
			static int endurancemax = 100;
			static int hardness = 0;
			static int amount = 1;

			static int model_select = 0;
			static int modeltype_select = 1;

			const char* name[500];
			const char* modelname[100];
			ImGui::SetWindowFontScale(1);

			std::string resource_name[500];
			//�����̒��o
			for (int i = 0; i < ResourceManager::GetInstance().m_resources.size(); i++)
			{
				resource_name[i] = std::string("TREE");
				resource_name[i] = resource_name[i] + std::to_string(i);
				name[i] = resource_name[i].c_str();

				if (i >= 499)break;
			}

			ImGui::Combo(u8"�������X�g", &resource_select, name, ResourceManager::GetInstance().m_resources.size());
			//���f�����X�g
			for (int i = 0; i < ModelMgr::GetInstance().g_modellist.size(); i++)
			{
				modelname[i] = ModelMgr::GetInstance().g_modellist[i].modelname.c_str();
			}
			ImGui::Combo(u8"���f��", &model_select, modelname, ModelMgr::GetInstance().g_modellist.size());
			ImGui::SameLine();

			//���l�ǉ�
			if (ImGui::Button("+"))
			{
				pdata.type = (ItemType)modeltype_select;
				pdata.pos = XMFLOAT3(spawnpos[0], spawnpos[1], spawnpos[2]);
				pdata.Endurance = endurance;
				pdata.EnduranceMax = endurancemax;
				pdata.Hardness = hardness;
				pdata.amount = amount;
				ResourceManager::GetInstance().CreateResource(pdata, (MODELID)model_select);
			}
			//�I�����ڍ폜
			ImGui::SameLine();
			if (ImGui::Button("-") && resource_select != 0)
			{
				if (ResourceManager::GetInstance().m_resources[resource_select] != nullptr &&
					resource_select < ResourceManager::GetInstance().m_resources.size()) {
					ResourceManager::GetInstance().m_resources.erase(ResourceManager::GetInstance().m_resources.begin() + resource_select);
					resource_select -= 1;
				}
			}
			const char* modeltype[(int)ItemType::ITEM_MAX] = {
				"WOOD",
				 "ORE_COAL",
				 "ORE_IRON",
				 "ORE_GOLD",
				 "IRON",
				 "GOLD",
				 "HERB"
			};
			ImGui::Combo(u8"���f���^�C�v", &modeltype_select, modeltype, (int)ItemType::ITEM_MAX);
			ImGui::DragFloat3(u8"�X�|�[�����W", spawnpos, 1, -1500, 1500);
			ImGui::SliderInt(u8"�ϋv�x", &endurance, 0, 100);
			ImGui::SliderInt(u8"�ő�ϋv�x", &endurancemax, 0, 100);
			ImGui::SliderInt(u8"�d�x", &hardness, 0, 5);
			ImGui::SliderInt(u8"�ܗL��(1������)", &amount, 0, 100);
			ImGui::EndMenu();
		}
		//�T�E���h�^�u
		if (ImGui::BeginMenu(u8"�T�E���h"))
		{
			SoundMgr::GetInstance().g_soundlist;
			static int sound_select = 0;

			const char* name[SOUND_MAX];
			const char* modelname[SOUND_MAX];

			std::string resource_name[SOUND_MAX];

			//�����̒��o
			for (int i = 0; i < SoundMgr::GetInstance().g_soundlist.size(); i++)
			{
				name[i] = SoundMgr::GetInstance().g_soundlist[i].filename.c_str();
			}

			ImGui::Combo(u8"�T�E���h���X�g", &sound_select, name, SoundMgr::GetInstance().g_soundlist.size());
			if (ImGui::Button(u8"�Đ�"))
			{
				SoundMgr::GetInstance().XA_Play(SoundMgr::GetInstance().g_soundlist[sound_select].filename);
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"��~"))
			{
				SoundMgr::GetInstance().XA_Stop(SoundMgr::GetInstance().g_soundlist[sound_select].filename);
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"�S��~"))
			{
				SoundMgr::GetInstance().XA_StopAll();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(u8"���C�e�B���O"))
		{
			ImGui::SliderFloat3(u8"���C�g���W",m_shadow_parameter.LightPos, 1, 3000);
			ImGui::SliderFloat(u8"�A�X�y�N�g��", &m_shadow_parameter.Aspect, 1, 1000);
			ImGui::SliderFloat(u8"�j�A�N���b�v", &m_shadow_parameter.nearclip, 1, 1000);
			ImGui::SliderFloat(u8"�t�@�[�N���b�v", &m_shadow_parameter.farclip, 1, 50000);
			ImGui::SliderFloat(u8"FOV", &m_shadow_parameter.Fov, 1, 300);
			ImGui::SliderInt(u8"�t���[�����[�g", &Application::FPS, 1, 300);


			static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

			static bool alpha_preview = true;
			static bool alpha_half_preview = true;
			static bool drag_and_drop = true;
			static bool options_menu = true;
			static bool hdr = false;

			ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
			ImGui::ColorEdit4(u8"����Color", (float*)&color, ImGuiColorEditFlags_DisplayRGB | misc_flags);

			XMFLOAT4 cb;
			cb.x = color.x;
			cb.y = color.y;
			cb.z = color.z;
			cb.w = color.w;

			ID3D11DeviceContext* devcontext;
			devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

			devcontext->UpdateSubresource(cb_Ambient,
				0,
				nullptr,
				&cb,
				0, 0);

			// �R���X�^���g�o�b�t�@4����3���W�X�^�փZ�b�g�i���_�V�F�[�_�[�p�j
			devcontext->VSSetConstantBuffers(7, 1, &cb_Ambient);
			// �R���X�^���g�o�b�t�@4����3���W�X�^�փZ�b�g(�s�N�Z���V�F�[�_�[�p)
			devcontext->PSSetConstantBuffers(7, 1, &cb_Ambient);

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();

	}

	m_style = &ImGui::GetStyle();
	m_text_color = m_style->Colors;

	//�e�N�X�`���ݒ�
	m_pos = ImGui::GetCursorScreenPos();
	m_size = ImVec2(32.0f, 32.0f);                         // Size of the image we want to make visible
	//UV�ݒ�
	for (int i = 0; i < GAMEBUTTON_MAX; i++) {
		m_game_uv_min[i] = ImVec2(0.0f, 0.0f);                 // Top-left
		m_game_uv_max[i] = ImVec2(1.00f, 0.25f);                 // Lower-right
	}
	for (int i = 0; i < SELECTBUTTON_MAX; i++) {
		m_select_uv_min[i] = ImVec2(0.0f, 0.0f);                 // Top-left
		m_select_uv_max[i] = ImVec2(1.00f, 0.25f);                 // Lower-right
	}
	for (int i = 0; i < BUILDBUTTON_MAX; i++) {
		m_build_uv_min[i] = ImVec2(0.0f, 0.0f);                 // Top-left
		m_build_uv_max[i] = ImVec2(1.00f, 0.25f);                 // Lower-right
	}
	for (int i = 0; i < HOUSEBUTTON_MAX; i++) {
		m_build_house_uv_min[i] = ImVec2(0.0f, 0.0f);                 // Top-left
		m_build_house_uv_max[i] = ImVec2(1.00f, 0.25f);                 // Lower-right
	}
	for (int i = 0; i < SOUKOBUTTON_MAX; i++) {
		m_build_souko_uv_min[i] = ImVec2(0.0f, 0.0f);                 // Top-left
		m_build_souko_uv_max[i] = ImVec2(1.00f, 0.25f);                 // Lower-right
	}
	for (int i = 0; i < ROADBUTTON_MAX; i++) {
		m_build_road_uv_min[i] = ImVec2(0.0f, 0.0f);                 // Top-left
		m_build_road_uv_max[i] = ImVec2(1.00f, 0.25f);                 // Lower-right
	}
	m_tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	m_border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 50% opaque white

	m_bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);             // Black background
		//�w�i�F
	m_text_color[ImGuiCol_WindowBg] = ImVec4(0.01f, 0.37f, 0.56f, 0.90f);

	m_style->WindowRounding = 1.5f;
	m_style->AntiAliasedFill = true;
	m_style->AntiAliasedLines = true;

	//�z�o�[�{�^��������ꍇUV�ω�
	if (m_Game_HoverButton != -1) {
		m_game_uv_min[m_Game_HoverButton] = ImVec2(0.0f, 0.5f);
		m_game_uv_max[m_Game_HoverButton] = ImVec2(1.0f, 0.75f);
	}

	//�Z���N�g�{�^��UV�ύX
	if (m_Select_HoverButton != -1) {
		m_select_uv_min[m_Select_HoverButton] = ImVec2(0.0f, 0.5f);
		m_select_uv_max[m_Select_HoverButton] = ImVec2(1.0f, 0.75f);
	}
	//Build�{�^��UV�ύX
	if (m_Build_HoverButton != -1) {
		m_build_uv_min[m_Build_HoverButton] = ImVec2(0.0f, 0.5f);
		m_build_uv_max[m_Build_HoverButton] = ImVec2(1.0f, 0.75f);
	}
	//�n�E�X�{�^��UV�ύX
	if (m_Build_House_HoverButton != -1) {
		m_build_house_uv_min[m_Build_House_HoverButton] = ImVec2(0.0f, 0.5f);
		m_build_house_uv_max[m_Build_House_HoverButton] = ImVec2(1.0f, 0.75f);
	}

	//�q�Ƀ{�^��UV�ύX
	if (m_Build_Souko_HoverButton != -1) {
		m_build_souko_uv_min[m_Build_Souko_HoverButton] = ImVec2(0.0f, 0.5f);
		m_build_souko_uv_max[m_Build_Souko_HoverButton] = ImVec2(1.0f, 0.75f);
	}
	//���H�{�^��UV�ύX
	if (m_Build_Road_HoverButton != -1) {
		m_build_road_uv_min[m_Build_Road_HoverButton] = ImVec2(0.0f, 0.5f);
		m_build_road_uv_max[m_Build_Road_HoverButton] = ImVec2(1.0f, 0.75f);
	}

	//�Q�[���{�^����puv
	for (int i = 0; i < GAMEBUTTON_MAX; i++)
	{
		if (m_TriggerButton[i])
		{
			m_game_uv_min[i].y += 0.25;
			m_game_uv_max[i].y += 0.25;
		}
		if (m_RadioButton[i])
		{
			m_game_uv_min[i].y += 0.25;
			m_game_uv_max[i].y += 0.25;
		}
	}

	//�Z���N�g�{�^����puv
	for (int i = 0; i < SELECTBUTTON_MAX; i++)
	{
		if (m_Select_RadioButton[i])
		{
			m_select_uv_min[i].y += 0.25;
			m_select_uv_max[i].y += 0.25;
		}
	}

	//���z�{�^����puv
	for (int i = 0; i < BUILDBUTTON_MAX; i++)
	{
		if (m_Build_RadioButton[i])
		{
			m_build_uv_min[i].y += 0.25;
			m_build_uv_max[i].y += 0.25;
		}
	}

	//���z_�n�E�X�{�^����puv
	for (int i = 0; i < HOUSEBUTTON_MAX; i++)
	{
		if (m_Build_House_RadioButton[i])
		{
			m_build_house_uv_min[i].y += 0.25;
			m_build_house_uv_max[i].y += 0.25;
		}
	}

	//���z_�q�Ƀ{�^����puv
	for (int i = 0; i < SOUKOBUTTON_MAX; i++)
	{
		if (m_Build_Souko_RadioButton[i])
		{
			m_build_souko_uv_min[i].y += 0.25;
			m_build_souko_uv_max[i].y += 0.25;
		}
	}

	//���z_���H�{�^����puv
	for (int i = 0; i < ROADBUTTON_MAX; i++)
	{
		if (m_Build_Road_RadioButton[i])
		{
			m_build_road_uv_min[i].y += 0.25;
			m_build_road_uv_max[i].y += 0.25;
		}
	}
	//�z�o�[���Z�b�g
	m_Game_HoverButton = GAMEBUTTON_NONE;
	m_Select_HoverButton = SELECTBUTTON_NONE;
	m_Build_HoverButton = BUILDBUTTON_NONE;
	m_Build_House_HoverButton = HOUSEBUTTON_NONE;
	m_Build_Souko_HoverButton = SOUKOBUTTON_NONE;
	m_Build_Road_HoverButton = ROADBUTTON_NONE;


	//�V�X�e���{�^��-------------------------------------------------
	//�A�N�V�����D��x�{�^��
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.3f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.3f));

	ImGui::SetCursorPos(ImVec2(10, 30));
	if (!workpriority)
	{
		if (ImGui::ImageButton(m_system_action_texture_view, ImVec2(30, 30), ImVec2(0, 0), ImVec2(1, 0.25), 0, ImVec4(1, 1, 1, 0), ImVec4(1, 1, 1, 1)))
		{
			SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
			workpriority = 1 - workpriority;
		}
	}
	else
	{
		if (ImGui::ImageButton(m_system_action_texture_view, ImVec2(30, 30), ImVec2(0, 0.5), ImVec2(1, 0.75), 0, ImVec4(1, 1, 1, 0), ImVec4(1, 1, 1, 1)))
		{
			SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
			workpriority = 1 - workpriority;
		}
	}

	ImGui::PopStyleColor(3);

	//�E�B���h�E�X�V,�P�t���[���̂ݕ`���؂�
	{
		static int window_update_cnt = 0;
		static bool window_update_flg = false;
		if (!workpriority && window_update_flg)
		{
			window_update_cnt++;
		}
		//�X�N���[���p�ɃE�B���h�E���X�V
		if (workpriority || window_update_cnt > 1)
		{
			workpriority = true;
			window_update_cnt = 0;
			window_update_flg = false;
			ImGui::SetNextWindowPos(ImVec2(200, 30), ImGuiCond_::ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_::ImGuiCond_Always);

			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoBackground;
			window_flags |= ImGuiWindowFlags_NoScrollbar;
			static int characternum = VillagerMgr::GetInstance().m_villager.size();

			if (characternum != VillagerMgr::GetInstance().m_villager.size())
			{
				characternum = VillagerMgr::GetInstance().m_villager.size();
				workpriority = false;
				window_update_flg = true;
			}
			characternum = VillagerMgr::GetInstance().m_villager.size();

			std::unique_ptr<bool> active = std::make_unique<bool>();
			ImGui::SetWindowFontScale(0.2);
			ImGui::Begin(u8"�A�N�V����", active.get(), window_flags);

			//�E�B���h�E�摜
			ImGui::SetCursorPos(ImVec2(0, 0));
			ImGui::Image(m_windowback_texture_view, ImVec2(500, 50 + (VillagerMgr::GetInstance().m_villager.size() * 30)), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);

			ImGui::SetCursorPos(ImVec2(153, 0));
			ImGui::Image(m_action_priority_texture, ImVec2(200, 66), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);

			ImGui::SetCursorPos(ImVec2(10, 0));
			ImGui::TextColored(ImVec4(1, 1, 1, 1),u8"�A�N�V�����D��x");

			//���O�\��
			for (int i = 0; i < VillagerMgr::GetInstance().m_villager.size(); i++) {
				//���O
				std::string name = VillagerMgr::GetInstance().m_villager[i]->GetName(NameGenerator::NAMETYPE::FAMILLY) + VillagerMgr::GetInstance().m_villager[i]->GetName(NameGenerator::NAMETYPE::MALE);
				ImGui::SetCursorPos(ImVec2(10, 40 + (i * 30)));
				ImGui::TextColored(ImVec4(1, 1, 1, 1), name.c_str());

				//�D��x�ύX
				for (int j = 0; j < VillagerMgr::GetInstance().m_villager[i]->GetWorkPriority().size(); j++)
				{
					ImGui::SetCursorPos(ImVec2(153 + (j * 26.5), 40 + (i * 30)));
					//�D��x�𕶎���ɕϊ�
					std::string priority;
					for (int cnt = 0; cnt < (i + 1) * (j + 1); cnt++)
					{
						priority = priority + "         " + std::to_string((i + 1)) + std::to_string((j + 1));
					}
					ImGui::SetWindowFontScale(0.3);
					bool ret = ImGui::Button(priority.c_str(), ImVec2(20, 20));
					if (ret)
					{
						SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
						if (VillagerMgr::GetInstance().m_villager[i]->GetWorkPriority()[j].priority < 5) {
							VillagerMgr::GetInstance().m_villager[i]->SetWorkPriority(j, VillagerMgr::GetInstance().m_villager[i]->GetWorkPriority()[j].priority + 1);
						}
						else
						{
							VillagerMgr::GetInstance().m_villager[i]->SetWorkPriority(j,1);
						}
					}
					ImGui::SetCursorPos(ImVec2(158 + (j * 26.5), 40 + (i * 30)));
					ImVec4 priority_color = {1,1,1,1};

					switch (VillagerMgr::GetInstance().m_villager[i]->GetWorkPriority()[j].priority)
					{
					case 1:
						priority_color = { 0.8,1,0.8,1 };
						break;

					case 2:
						priority_color = { 0.6,1,0.6,1 };
						break;

					case 3:
						priority_color = { 0.4,1,0.4,1 };

						break;

					case 4:
						priority_color = { 0.2,1,0.2,1 };
						break;

					case 5:
						priority_color = { 0,1,0,1 };

						break;

					default:
						break;
					}

					ImGui::TextColored(priority_color,std::to_string(VillagerMgr::GetInstance().m_villager[i]->GetWorkPriority()[j].priority).c_str());

					ImGui::SetWindowFontScale(0.3);
				}

			}

			ImGui::SetWindowFontScale(0.3);

			ImGui::End();
		}
	}
	//---------------------------------------------------------------
	//�����ꂽ��
	static int PushNum = 0;
	ImGui::SetCursorPos(ImVec2(950,5));
	//�Q�[���{�^��---------------------------------------------------
	//STOP�{�^��
	if (ImGui::ImageButton(m_texture_view[GAMEBUTTON_STOP], m_size, m_game_uv_min[GAMEBUTTON_STOP], m_game_uv_max[GAMEBUTTON_STOP], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearGameRadio();
		m_RadioButton[GAMEBUTTON_STOP] = 1 - m_RadioButton[GAMEBUTTON_STOP];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 3;
		m_Game_HoverButton = GAMEBUTTON_STOP;
	}
	ImGui::SetCursorPos(ImVec2(980,5));

	//PLAY00�{�^��
	if (ImGui::ImageButton(m_texture_view[GAMEBUTTON_PLAY_00], m_size, m_game_uv_min[GAMEBUTTON_PLAY_00], m_game_uv_max[GAMEBUTTON_PLAY_00], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearGameRadio();
		m_RadioButton[GAMEBUTTON_PLAY_00] = 1 - m_RadioButton[GAMEBUTTON_PLAY_00];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 4;
		m_Game_HoverButton = GAMEBUTTON_PLAY_00;
	}
	ImGui::SetCursorPos(ImVec2(1010, 5));
	//PLAY01�{�^��
	if (ImGui::ImageButton(m_texture_view[GAMEBUTTON_PLAY_01], m_size, m_game_uv_min[GAMEBUTTON_PLAY_01], m_game_uv_max[GAMEBUTTON_PLAY_01], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearGameRadio();
		m_RadioButton[GAMEBUTTON_PLAY_01] = 1 - m_RadioButton[GAMEBUTTON_PLAY_01];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 5;
		m_Game_HoverButton = GAMEBUTTON_PLAY_01;
	}
	ImGui::SetCursorPos(ImVec2(1040, 5));
	//PLAY02�{�^��
	if (ImGui::ImageButton(m_texture_view[GAMEBUTTON_PLAY_02], m_size, m_game_uv_min[GAMEBUTTON_PLAY_02], m_game_uv_max[GAMEBUTTON_PLAY_02], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearGameRadio();
		m_RadioButton[GAMEBUTTON_PLAY_02] = 1 - m_RadioButton[GAMEBUTTON_PLAY_02];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 6;
		m_Game_HoverButton = GAMEBUTTON_PLAY_02;
	}
	//�Q�[���{�^��---------------------------------------------------

	//�Z���N�g�{�^��---------------------------------------------------
	//�w�i�F�폜
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));


	//�}�E�X�{�^��
	ImGui::SetCursorPos(ImVec2(1010, 610));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_MOUSE], ImVec2(60,60), m_select_uv_min[SELECTBUTTON_MOUSE], m_select_uv_max[SELECTBUTTON_MOUSE], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_MOUSE] = 1 - m_Select_RadioButton[SELECTBUTTON_MOUSE];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 7;
		m_Select_HoverButton = SELECTBUTTON_MOUSE;
	}

	//ALL�{�^��
	ImGui::SetCursorPos(ImVec2(1045, 645));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_ALL], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_ALL], m_select_uv_max[SELECTBUTTON_ALL], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_ALL] = 1 - m_Select_RadioButton[SELECTBUTTON_ALL];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 8;
		m_Select_HoverButton = SELECTBUTTON_ALL;
	}

	//���̃{�^��
	ImGui::SetCursorPos(ImVec2(1115, 645));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_CUT], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_CUT], m_select_uv_max[SELECTBUTTON_CUT], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_CUT] = 1 - m_Select_RadioButton[SELECTBUTTON_CUT];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 9;
		m_Select_HoverButton = SELECTBUTTON_CUT;
	}

	//�̌@�{�^��
	ImGui::SetCursorPos(ImVec2(1080, 610));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_MINE], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_MINE], m_select_uv_max[SELECTBUTTON_MINE], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_MINE] = 1 - m_Select_RadioButton[SELECTBUTTON_MINE];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 10;
		m_Select_HoverButton = SELECTBUTTON_MINE;
	}

	//�̏W�{�^��
	ImGui::SetCursorPos(ImVec2(1150, 610));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_COLLECT], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_COLLECT], m_select_uv_max[SELECTBUTTON_COLLECT], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_COLLECT] = 1 - m_Select_RadioButton[SELECTBUTTON_COLLECT];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 11;
		m_Select_HoverButton = SELECTBUTTON_COLLECT;
	}

	//�L�����Z���{�^��
	ImGui::SetCursorPos(ImVec2(1185, 645));
	if (ImGui::ImageButton(m_select_texture_view[SELECTBUTTON_CANCEL], ImVec2(60, 60), m_select_uv_min[SELECTBUTTON_CANCEL], m_select_uv_max[SELECTBUTTON_CANCEL], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		ClearSelectRadio();
		m_Select_RadioButton[SELECTBUTTON_CANCEL] = 1 - m_Select_RadioButton[SELECTBUTTON_CANCEL];
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 12;
		m_Select_HoverButton = SELECTBUTTON_CANCEL;
	}

	ImGui::PopStyleColor(3);
	//�Z���N�g�{�^��---------------------------------------------------

	//���z�{�^��---------------------------------------------------
	ImGui::SetCursorPos(ImVec2(0, 645));
	if (ImGui::ImageButton(m_build_texture_view[BUILDBUTTON_HOUSE], ImVec2(60, 60), m_build_uv_min[BUILDBUTTON_HOUSE], m_build_uv_max[BUILDBUTTON_HOUSE], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		bool temp = m_Build_RadioButton[BUILDBUTTON_HOUSE];
		ClearBuildRadio();
		m_Build_RadioButton[BUILDBUTTON_HOUSE] = 1 - temp;
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 13;
		bool temp = m_Build_RadioButton[BUILDBUTTON_HOUSE];
		m_Build_HoverButton = BUILDBUTTON_HOUSE;
	}

	ImGui::SetCursorPos(ImVec2(70, 645));
	if (ImGui::ImageButton(m_build_texture_view[BUILDBUTTON_SOUKO], ImVec2(60, 60), m_build_uv_min[BUILDBUTTON_SOUKO], m_build_uv_max[BUILDBUTTON_SOUKO], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		bool temp = m_Build_RadioButton[BUILDBUTTON_SOUKO];
		ClearBuildRadio();
		m_Build_RadioButton[BUILDBUTTON_SOUKO] = 1 - temp;
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 14;
		m_Build_HoverButton = BUILDBUTTON_SOUKO;
	}

	ImGui::SetCursorPos(ImVec2(140, 645));
	if (ImGui::ImageButton(m_build_texture_view[BUILDBUTTON_ROAD], ImVec2(60, 60), m_build_uv_min[BUILDBUTTON_ROAD], m_build_uv_max[BUILDBUTTON_ROAD], 0, m_border_col, m_tint_col))
	{
		SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
		bool temp = m_Build_RadioButton[BUILDBUTTON_ROAD];
		ClearBuildRadio();
		m_Build_RadioButton[BUILDBUTTON_ROAD] = 1 - temp;
	}
	//�z�o�[�C�x���g
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		clickbutton = 15;
		m_Build_HoverButton = BUILDBUTTON_ROAD;
	}
	//���z�{�^��---------------------------------------------------

	modelpreviewflg = false;

	//���z�E�B���h�E---------------------------------------------------
	if (m_Build_RadioButton[BUILDBUTTON_HOUSE])
	{
		ImGui::SetNextWindowPos(ImVec2(0, 550));
		ImGui::SetNextWindowSize(ImVec2(400, 100));
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoBackground;

		std::unique_ptr<bool> windowactive = std::make_unique<bool>(false);

		ImGui::Begin(u8"�Z��",windowactive.get(), window_flags);

		XMFLOAT3 g_nerp;
		XMFLOAT3 g_farp;

		//�E�B���h�E�摜
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(m_windowback_texture_view, ImVec2(400, 100), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);

		SearchMousePoint(g_nerp, g_farp);
		ImGui::SetCursorPos(ImVec2(10,15));
		//SMALLHOUSE�{�^��
		if (ImGui::ImageButton(m_build_house_texture_view[HOUSE_SMALL], ImVec2(64.0f, 64.0f), m_build_house_uv_min[HOUSE_SMALL], m_build_house_uv_max[HOUSE_SMALL], 0, m_border_col, m_tint_col))
		{
			SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
			modelpreview->SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::SMALLHOUSE)].modelname));
			modelpreview->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CREATE);
			modelpreview->InitColision();

			bool temp = m_Build_House_RadioButton[HOUSE_SMALL];
			ClearHouseRadio();
			m_Build_House_RadioButton[HOUSE_SMALL] = 1 - temp;
		}
		//�z�o�[�C�x���g
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
		{
			clickbutton = 16;
			m_Build_House_HoverButton = HOUSE_SMALL;
		}
		
		HouseButtonType currentselect = HOUSEBUTTON_NONE;
		MODELID createmodel;

		//���ݑI�𒆂̃I�u�W�F�N�g
		for (int i = 0; i < HOUSEBUTTON_MAX; i++)
		{
			if (m_Build_House_RadioButton[i])
			{
				currentselect = (HouseButtonType)i;
			}
		}
		switch (currentselect)
		{
		case GameButton::HOUSE_SMALL:
			createmodel = MODELID::SMALLHOUSE;
			break;

		case GameButton::HOUSEBUTTON_MAX:
			break;

		case GameButton::HOUSEBUTTON_NONE:
			break;

		default:
			break;
		}
		if (currentselect != HOUSEBUTTON_NONE) {
			modelpreviewflg = true;
		}

		//���̍\�����ƃq�b�g���Ă��Ȃ���
		bool hit = false;
		modelpreview->GetOBB()->SetBoxColor(false);
		modelpreview->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CREATE);

		std::vector<COBB> obbs = BuildingMgr::GetInstance().GetAllObb();

		for (int i = 0; i < obbs.size(); i++)
		{
			if (modelpreview->GetOBB()->Collision(obbs[i]))
			{
				hit = true;
			}
		}

		if (hit)
		{
			modelpreview->GetOBB()->SetBoxColor(true);
			modelpreview->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CANCEL);
		}

		//�N���b�N�����z������
		if (io.MouseClicked[0] && modelpreviewflg == true) {
			if (io.MousePos.y > 50 && io.MousePos.y < 550) {
				
				if(!hit)
				{
					SoundMgr::GetInstance().XA_Play("assets/sound/SE/Build_00.wav");
					House::Data initdata;
					initdata.pos = m_mouseworldpos;
					initdata.type = currentselect;
					BuildingMgr::GetInstance().CreateHouse(initdata, createmodel);
					RouteSearch::GetInstance().InitStageCollider();

				}
			}
		}

		ImGui::End();
	}

	if (m_Build_RadioButton[BUILDBUTTON_SOUKO])
	{
		MODELID createmodel;

		ImGui::SetNextWindowPos(ImVec2(0, 550));
		ImGui::SetNextWindowSize(ImVec2(400, 100));
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoBackground;

		std::unique_ptr<bool> windowactive = std::make_unique<bool>(false);

		ImGui::Begin(u8"�q��",windowactive.get(), window_flags);
		XMFLOAT3 g_nerp;
		XMFLOAT3 g_farp;
		//�E�B���h�E�摜
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(m_windowback_texture_view, ImVec2(400, 100), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);

		SearchMousePoint(g_nerp, g_farp);
		ImGui::SetCursorPos(ImVec2(10, 15));
		//SMALLHOUSE�{�^��
		if (ImGui::ImageButton(m_build_souko_texture_view[SOUKO_SMALL], ImVec2(64.0f, 64.0f), m_build_souko_uv_min[SOUKO_SMALL], m_build_souko_uv_max[SOUKO_SMALL], 0, m_border_col, m_tint_col))
		{
			SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
			modelpreview->SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::SMALLSOUKO)].modelname));
			modelpreview->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CREATE);
			modelpreview->InitColision();
			bool temp = m_Build_Souko_RadioButton[SOUKO_SMALL];
			ClearSoukoRadio();
			m_Build_Souko_RadioButton[SOUKO_SMALL] = 1 - temp;
		}
		//�z�o�[�C�x���g
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
		{
			clickbutton = 17;
			m_Build_Souko_HoverButton = SOUKO_SMALL;
		}

		SoukoButtonType currentselect = SOUKOBUTTON_NONE;

		//���ݑI�𒆂̃I�u�W�F�N�g
		for (int i = 0; i < SOUKOBUTTON_MAX; i++)
		{
			if (m_Build_Souko_RadioButton[i])
			{
				currentselect = (SoukoButtonType)i;
			}
		}
		//�e�ݒ�
		Souko::Data initdata;

		switch (currentselect)
		{
		case GameButton::SOUKO_SMALL:
			initdata.store_max = 200;
			createmodel = MODELID::SMALLSOUKO;
			break;

		case GameButton::SOUKOBUTTON_MAX:
			break;

		case GameButton::SOUKOBUTTON_NONE:
			break;

		default:
			break;
		}
		if (currentselect != SOUKOBUTTON_NONE) {
			modelpreviewflg = true;
		}

		//���̍\�����ƃq�b�g���Ă��Ȃ���
		bool hit = false;
		modelpreview->GetOBB()->SetBoxColor(false);
		modelpreview->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CREATE);

		std::vector<COBB> obbs = BuildingMgr::GetInstance().GetAllObb();

		for (int i = 0; i < obbs.size(); i++)
		{
			if (modelpreview->GetOBB()->Collision(obbs[i]))
			{
				hit = true;
			}
		}

		if (hit)
		{
			modelpreview->GetOBB()->SetBoxColor(true);
			modelpreview->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CANCEL);
		}

		//�N���b�N�����z������
		if (io.MouseClicked[0] && modelpreviewflg == true) {
			if (io.MousePos.y > 50 && io.MousePos.y < 550) {

				if (!hit)
				{
					SoundMgr::GetInstance().XA_Play("assets/sound/SE/Build_00.wav");
					initdata.pos = m_mouseworldpos;
					initdata.type = currentselect;
					BuildingMgr::GetInstance().CreateSouko(initdata, createmodel);
					RouteSearch::GetInstance().InitStageCollider();
				}
			}
		}
		ImGui::End();
	}
	//���H����
	if (m_Build_RadioButton[BUILDBUTTON_ROAD])
	{
		ImGui::SetNextWindowPos(ImVec2(0, 550));
		ImGui::SetNextWindowSize(ImVec2(400, 100));
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoBackground;

		std::unique_ptr<bool> windowactive = std::make_unique<bool>(false);

		ImGui::Begin(u8"���H",windowactive.get(), window_flags);

		XMFLOAT3 g_nerp;
		XMFLOAT3 g_farp;

		//�E�B���h�E�摜
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(m_windowback_texture_view, ImVec2(400, 100), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);

		SearchMousePoint(g_nerp, g_farp);
		ImGui::SetCursorPos(ImVec2(10, 15));
		//DIRT�{�^��
		if (ImGui::ImageButton(m_build_road_texture_view[ROAD_DIRT], ImVec2(64.0f, 64.0f), m_build_road_uv_min[ROAD_DIRT], m_build_road_uv_max[ROAD_DIRT], 0, m_border_col, m_tint_col))
		{
			SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");
			modelpreview->SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::ROAD_DIRT)].modelname));
			modelpreview->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CREATE);
			modelpreview->InitColision();

			bool temp = m_Build_Road_RadioButton[ROAD_DIRT];
			ClearHouseRadio();
			m_Build_Road_RadioButton[ROAD_DIRT] = 1 - temp;
		}
		//�z�o�[�C�x���g
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
		{
			clickbutton = 18;
			m_Build_Road_HoverButton = ROAD_DIRT;
		}

		RoadButtonType currentselect = ROADBUTTON_NONE;
		MODELID createmodel;

		//���ݑI�𒆂̃I�u�W�F�N�g
		for (int i = 0; i < ROADBUTTON_MAX; i++)
		{
			if (m_Build_Road_RadioButton[i])
			{
				currentselect = (RoadButtonType)i;
			}
		}
		switch (currentselect)
		{
		case GameButton::ROAD_DIRT:
			createmodel = MODELID::ROAD_DIRT;
			break;

		case GameButton::ROADBUTTON_MAX:
			break;

		case GameButton::ROADBUTTON_NONE:
			break;

		default:
			break;
		}
		if (currentselect != ROADBUTTON_NONE) {
			modelpreviewflg = true;
		}

		//���̍\�����ƃq�b�g���Ă��Ȃ���
		bool hit = false;
		modelpreview->GetOBB()->SetBoxColor(false);
		modelpreview->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CREATE);

		std::vector<COBB> obbs = BuildingMgr::GetInstance().GetAllObb();

		for (int i = 0; i < obbs.size(); i++)
		{
			if (modelpreview->GetOBB()->Collision(obbs[i]))
			{
				hit = true;
			}
		}

		if (hit)
		{
			modelpreview->GetOBB()->SetBoxColor(true);
			modelpreview->GetModel()->ChangeSelectType(SELECT_SHADER_TYPE::SELECT_SHADER_TYPE_CANCEL);
		}
		static XMFLOAT3 dragstartpos;
		static bool isdrag = false;
		//�N���b�N�����z������
		if (io.MouseClicked[0] && modelpreviewflg == true) {
			if (io.MousePos.y > 50 && io.MousePos.y < 550) {
				if (!hit)
				{
					isdrag = true;
					dragstartpos = m_mouseworldpos;
				}
			}
		}
		if (io.MouseReleased[0] && isdrag == true)
		{
			isdrag = false;
			//�Ώۃ}�X��ݒ�
			float x_min = dragstartpos.x;
			float y_min = fabs(dragstartpos.z);

			float x_max = m_mouseworldpos.x;
			float y_max = fabs(m_mouseworldpos.z);

			if (x_min > x_max)
			{
				float temp = x_max;
				x_max = x_min;
				x_min = temp;
			}

			if (y_min > y_max)
			{
				float temp = y_max;
				y_max = y_min;
				y_min = temp;
			}
			int x_min_idx = x_min / 12.5f;
			int y_min_idx = (y_min / 12.5f);

			int x_max_idx = x_max / 12.5f;
			int y_max_idx = (y_max / 12.5f);

			for (int i = y_min_idx - 1; i < y_max_idx; i++)
			{
				for (int j = x_min_idx - 1; j < x_max_idx; j++)
				{
					SoundMgr::GetInstance().XA_Play("assets/sound/SE/Build_00.wav");
					BuildingMgr::GetInstance().CreateRoad(XMFLOAT3(j * 12.5,m_mouseworldpos.y,i * -12.5), createmodel);
				}
			}
		}

		ImGui::End();
	}

//�w�i�F�폜
	bool party_list_active;
	ImGui::SetNextWindowPos(ImVec2(0, 100));
	ImGui::SetNextWindowSize(ImVec2(200, 400));

	//�E�B���h�E�ݒ�
	ImGuiWindowFlags party_window_flags = 0;
	party_window_flags |= ImGuiWindowFlags_NoTitleBar;
	party_window_flags |= ImGuiWindowFlags_NoScrollbar;
	party_window_flags |= ImGuiWindowFlags_NoMove;
	party_window_flags |= ImGuiWindowFlags_NoResize;
	party_window_flags |= ImGuiWindowFlags_NoBackground;

	//�E�B���h�E�X�V,�P�t���[���̂ݕ`���؂�
	{
		static bool isdrow = true;
		static int window_update_cnt = 0;
		static bool window_update_flg = false;
		if (!isdrow && window_update_flg)
		{
			window_update_cnt++;
		}

		if (window_update_cnt > 1 || isdrow) {
			isdrow = true;
			window_update_flg = false;
			window_update_cnt = 0;
			ImGui::Begin(u8"�Z�l���X�g", &party_list_active, party_window_flags);

			static int characternum = VillagerMgr::GetInstance().m_villager.size();

			if (characternum != VillagerMgr::GetInstance().m_villager.size())
			{
				characternum = VillagerMgr::GetInstance().m_villager.size();
				isdrow = false;
				window_update_flg = true;
			}

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.3f));
			static int hover_party_list = -1;
			ImGui::SetWindowFontScale(0.3);
			for (int i = 0; i < VillagerMgr::GetInstance().m_villager.size(); i++) {
				//�Z�l���X�g���n
				//HP�䗦
				float ratio = VillagerMgr::GetInstance().m_villager[i]->GetHitpoint() / VillagerMgr::GetInstance().m_villager[i]->GetMaxHitpoint();
				if (ratio > 1.0f)
				{
					ratio = 1.0f;
				}
				else if (ratio < 0.0f)
				{
					ratio = 0;
				}
				ImGui::SetCursorPos(ImVec2(0, 10 + (i * 50)));

				if (ratio >= 1.0f)
				{
					if (hover_party_list == i || VillagerMgr::GetInstance().m_villager[i]->GetSelect()) {
						ImGui::ImageButton(m_party_list_texture[PartyListType::BAR_NAME], ImVec2(200, 50), ImVec2(0, 0.8), ImVec2(0, 1.0), 0, m_border_col, m_tint_col);
					}
					else
					{
						ImGui::ImageButton(m_party_list_texture[PartyListType::BAR_NAME], ImVec2(200, 50), ImVec2(0, 0), ImVec2(1.0, 0.2), 0, m_border_col, m_tint_col);
					}
				}
				else if (ratio >= 0.5f)
				{
					if (hover_party_list == i || VillagerMgr::GetInstance().m_villager[i]->GetSelect()) {
						ImGui::ImageButton(m_party_list_texture[PartyListType::BAR_NAME], ImVec2(200, 50), ImVec2(0, 0.8), ImVec2(0, 1.0), 0, m_border_col, m_tint_col);
					}
					else
					{
						ImGui::ImageButton(m_party_list_texture[PartyListType::BAR_NAME], ImVec2(200, 50), ImVec2(0, 0.2), ImVec2(1.0, 0.4), 0, m_border_col, m_tint_col);
					}
				}
				else if (ratio >= 0.3f)
				{
					if (hover_party_list == i || VillagerMgr::GetInstance().m_villager[i]->GetSelect()) {
						ImGui::ImageButton(m_party_list_texture[PartyListType::BAR_NAME], ImVec2(200, 50), ImVec2(0, 0.8), ImVec2(0, 1.0), 0, m_border_col, m_tint_col);
					}
					else
					{
						ImGui::ImageButton(m_party_list_texture[PartyListType::BAR_NAME], ImVec2(200, 50), ImVec2(0, 0.4), ImVec2(1.0, 0.6), 0, m_border_col, m_tint_col);
					}
				}
				else
				{
					if (hover_party_list == i || VillagerMgr::GetInstance().m_villager[i]->GetSelect()) {
						ImGui::ImageButton(m_party_list_texture[PartyListType::BAR_NAME], ImVec2(200, 50), ImVec2(0, 0.8), ImVec2(0, 1.0), 0, m_border_col, m_tint_col);
					}
					else
					{
						ImGui::ImageButton(m_party_list_texture[PartyListType::BAR_NAME], ImVec2(200, 50), ImVec2(0, 0.6), ImVec2(1.0, 0.8), 0, m_border_col, m_tint_col);
					}
				}
				//�z�o�[�C�x���g
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
				{
					clickbutton = 19;
					hover_party_list = i;
					if (ImGui::IsMouseClicked(0))
					{
						SoundMgr::GetInstance().XA_Play("assets/sound/SE/SelectClick00.wav");

						VillagerMgr::GetInstance().SelectOneVillager(i);
					}
				}
				if (io.MousePos.x > 200 || io.MousePos.y > 500 || io.MousePos.y < 100)
				{
					hover_party_list = -1;
				}
				//���O
				std::string name = VillagerMgr::GetInstance().m_villager[i]->GetName(NameGenerator::NAMETYPE::FAMILLY) + VillagerMgr::GetInstance().m_villager[i]->GetName(NameGenerator::NAMETYPE::MALE);
				ImGui::SetCursorPos(ImVec2(10, 10 + (i * 50)));
				ImGui::TextColored(ImVec4(1, 1, 1, 1), name.c_str());

				ImGui::SetCursorPos(ImVec2(10, 30 + (i * 50)));
				if (VillagerMgr::GetInstance().m_villager[i]->GetMood() > 80) {
					ImGui::Image(m_party_list_texture[PartyListType::ICON_EMOTE], ImVec2(25, 25), ImVec2(0, 0), ImVec2(1, 0.2), m_tint_col, m_border_col);
				}
				else if (VillagerMgr::GetInstance().m_villager[i]->GetMood() > 60)
				{
					ImGui::Image(m_party_list_texture[PartyListType::ICON_EMOTE], ImVec2(25, 25), ImVec2(0, 0.2), ImVec2(1, 0.4), m_tint_col, m_border_col);
				}
				else if (VillagerMgr::GetInstance().m_villager[i]->GetMood() > 40)
				{
					ImGui::Image(m_party_list_texture[PartyListType::ICON_EMOTE], ImVec2(25, 25), ImVec2(0, 0.4), ImVec2(1, 0.6), m_tint_col, m_border_col);
				}
				else if (VillagerMgr::GetInstance().m_villager[i]->GetMood() > 20)
				{
					ImGui::Image(m_party_list_texture[PartyListType::ICON_EMOTE], ImVec2(25, 25), ImVec2(0, 0.6), ImVec2(1, 0.8), m_tint_col, m_border_col);
				}
				else
				{
					ImGui::Image(m_party_list_texture[PartyListType::ICON_EMOTE], ImVec2(25, 25), ImVec2(0, 0.8), ImVec2(1, 1), m_tint_col, m_border_col);
				}
				//HP�Q�[�W
				ImGui::SetCursorPos(ImVec2(40, 30 + (i * 50)));
				ImGui::Image(m_party_list_texture[PartyListType::BAR_BACK], ImVec2(125, 10), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
				ImGui::SetCursorPos(ImVec2(40, 30 + (i * 50)));

				ImGui::Image(m_party_list_texture[PartyListType::BAR_HELTH], ImVec2(125 * ratio, 10), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
				ImGui::SetCursorPos(ImVec2(40, 30 + (i * 50)));
				ImGui::Image(m_party_list_texture[PartyListType::BAR_FRAME], ImVec2(125, 10), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);

				//�X�^�~�i�Q�[�W
				ImGui::SetCursorPos(ImVec2(40, 45 + (i * 50)));
				ImGui::Image(m_party_list_texture[PartyListType::BAR_BACK], ImVec2(125, 10), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
				ImGui::SetCursorPos(ImVec2(40, 45 + (i * 50)));

				ratio = VillagerMgr::GetInstance().m_villager[i]->GetStamina() / VillagerMgr::GetInstance().m_villager[i]->GetMaxStamina();

				ImGui::Image(m_party_list_texture[PartyListType::BAR_STAMINA], ImVec2(125 * ratio, 10), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
				ImGui::SetCursorPos(ImVec2(40, 45 + (i * 50)));
				ImGui::Image(m_party_list_texture[PartyListType::BAR_FRAME], ImVec2(125, 10), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
			}
			ImGui::SetWindowFontScale(0.3);
			ImGui::PopStyleColor(3);

			ImGui::End();
		}
	}
	//���ԕ\���E�B���h�E
	ImGui::SetCursorPos(ImVec2(1072, 5));
	ImGui::Image(m_windowback_texture_view,ImVec2(150,32),ImVec2(0,0),ImVec2(1,1),m_tint_col,m_border_col);
	 ImGui::GetID(m_texture_view);

	 ImGui::SetCursorPos(ImVec2(0, 50));
	////�v�b�V����
	//ImGui::Text(u8"�v�b�V����%d", PushNum);

	////������\���p
	std::string str;

	////�t���[�����[�g�\��
	ImGui::SetWindowFontScale(0.3);
	str = u8"����" + std::to_string((int)m_io->Framerate) + "FPS";

	ImGui::Text(str.c_str());
	//ImGui::Text(u8"���_��%d, �C���f�b�N�X��:%d (�|���S����%d)", m_io->MetricsRenderVertices, m_io->MetricsRenderIndices, m_io->MetricsRenderIndices / 3);
	//ImGui::Text(u8"���s�E�B���h�E��%d, ���蓖�Đ�:%d", m_io->MetricsRenderWindows, m_io->MetricsActiveAllocations);
	ImGui::SetCursorPos(ImVec2(1080, 5));
	ImGui::SetNextWindowContentSize(ImVec2(20,20));
	//time

	int hour = Application::Instance()->GAME_TIME / 60;
	int minutes = Application::Instance()->GAME_TIME % 60;
	ImGui::SetWindowFontScale(0.4);
	ImGui::Text(u8"%2d�� %2d��",hour,minutes);
	ImGui::SetWindowFontScale(0.3);

	//�͈͑I��\��
	static ImVec2 DragStartPos = ImVec2(0,0);

	//�I��͈̓X�^�[�g�n�_
	if (ImGui::IsMouseClicked(0)) {
		DragStartPos = ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	}
	if (ImGui::IsMouseDown(0))
	{
		ImGui::SetCursorPos(ImVec2(DragStartPos.x, DragStartPos.y));
		//�I���T�C�Y��ό`
		ImVec2 size;
		size.x = ImGui::GetMousePos().x - DragStartPos.x;
		size.y = ImGui::GetMousePos().y - DragStartPos.y;

		ImGui::Image(m_selectarea_texture_view,size, ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
	}
	//ImGui::Text(u8"�}�E�X�X�^�[�gPos:%f  %f",DragStartPos.x / Application::CLIENT_WIDTH,DragStartPos.y / Application::CLIENT_HEIGHT);

	//�f�ޕ\��
	ImGui::SetWindowFontScale(0.25);

	ImGui::SetCursorPos(ImVec2(1150, 80));
	ImGui::Image(m_resource_preview_texture[(int)ItemType::WOOD],ImVec2(35,35), ImVec2(0,0), ImVec2(1,1),m_tint_col,m_border_col);
	ImGui::SetCursorPos(ImVec2(1200, 82));
	ImGui::Text(std::to_string(ResourceManager::GetInstance().GetItem(ItemType::WOOD)).c_str());
	ImGui::SetCursorPos(ImVec2(1150, 110));
	ImGui::Image(m_resource_preview_texture[(int)ItemType::ORE_COAL], ImVec2(35, 35), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
	ImGui::SetCursorPos(ImVec2(1200, 114));
	ImGui::Text(std::to_string(ResourceManager::GetInstance().GetItem(ItemType::ORE_COAL)).c_str());
	ImGui::SetCursorPos(ImVec2(1150, 140));
	ImGui::Image(m_resource_preview_texture[(int)ItemType::ORE_IRON], ImVec2(35, 35), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
	ImGui::SetCursorPos(ImVec2(1200, 146));
	ImGui::Text(std::to_string(ResourceManager::GetInstance().GetItem(ItemType::ORE_IRON)).c_str());
	ImGui::SetCursorPos(ImVec2(1150, 170));
	ImGui::Image(m_resource_preview_texture[(int)ItemType::ORE_GOLD], ImVec2(35, 35), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
	ImGui::SetCursorPos(ImVec2(1200, 178));
	ImGui::Text(std::to_string(ResourceManager::GetInstance().GetItem(ItemType::ORE_GOLD)).c_str());
	ImGui::SetCursorPos(ImVec2(1150, 200));
	ImGui::Image(m_resource_preview_texture[(int)ItemType::IRON], ImVec2(35, 35), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
	ImGui::SetCursorPos(ImVec2(1200, 210));
	ImGui::Text(std::to_string(ResourceManager::GetInstance().GetItem(ItemType::IRON)).c_str());
	ImGui::SetCursorPos(ImVec2(1150, 230));
	ImGui::Image(m_resource_preview_texture[(int)ItemType::GOLD], ImVec2(35, 35), ImVec2(0, 0), ImVec2(1, 1), m_tint_col, m_border_col);
	ImGui::SetCursorPos(ImVec2(1200, 242));
	ImGui::Text(std::to_string(ResourceManager::GetInstance().GetItem(ItemType::GOLD)).c_str());

	ImGui::SetWindowFontScale(0.2);

	ImGui::End();
	//���l���I����Ԃ�
	//���l�E�B���h�E�̏ڍ�
	if (villagerpropaty) {
		//���l��I����Ԃ�
		VillagerMgr::GetInstance().m_villager[edit_select]->SetSelect(true);

		ImGui::SetNextWindowPos(ImVec2(750,50));
		ImGui::SetNextWindowSize(ImVec2(350, 350));
		ImGui::Begin(u8"�ڍ�");
		//���O�\��
		std::string first = VillagerMgr::GetInstance().m_villager[edit_select]->GetName(NameGenerator::NAMETYPE::MALE).c_str();
		std::string last = VillagerMgr::GetInstance().m_villager[edit_select]->GetName(NameGenerator::NAMETYPE::FAMILLY).c_str();
		ImGui::Text((first + last).c_str());

		//���W
		static float position[3];
		position[0] = VillagerMgr::GetInstance().m_villager[edit_select]->GetPos().x;
		position[1] = VillagerMgr::GetInstance().m_villager[edit_select]->GetPos().y;
		position[2] = VillagerMgr::GetInstance().m_villager[edit_select]->GetPos().z;

		ImGui::DragFloat3(u8"���W",position,1,0,1000);
		
		//VillagerMgr::GetInstance().m_villager[edit_select]->SetPos(XMFLOAT3(position[0],position[1],position[2]));

		//animation
		CModel* model = VillagerMgr::GetInstance().m_villager[edit_select]->GetModel();
		ImGui::Text(u8"�A�j���[�V������");
		ImGui::Text(model->m_animationcontainer[0]->GetScene()->mAnimations[(int)VillagerMgr::GetInstance().m_villager[edit_select]->GetAnimData().animno]->mName.C_Str());
		ImGui::End();
	}

	

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


}

void GameButton::PrevieModelDraw()
{
	if (m_mouseworldpos.y > -50 && m_mouseworldpos.y < 50 && modelpreviewflg == true)
	{
		modelpreview->Draw();
	}
}

void GameButton::UnInit()
{
}

void GameButton::ClearGameRadio()
{
	for (int i = 0; i < GAMEBUTTON_MAX; i++)
	{
		m_RadioButton[i] = false;
	}
}

void GameButton::ClearSelectRadio()
{
	for (int i = 0; i < SELECTBUTTON_MAX; i++)
	{
		m_Select_RadioButton[i] = false;
	}
}

void GameButton::ClearBuildRadio()
{
	for (int i = 0; i < BUILDBUTTON_MAX; i++)
	{
		m_Build_RadioButton[i] = false;
	}
}

void GameButton::ClearHouseRadio()
{
	for (int i = 0; i < HOUSEBUTTON_MAX; i++)
	{
		m_Build_House_RadioButton[i] = false;
	}
}

void GameButton::ClearSoukoRadio()
{
	for (int i = 0; i < SOUKOBUTTON_MAX; i++)
	{
		m_Build_Souko_RadioButton[i] = false;
	}

}

void GameButton::SetMouseWorld(XMFLOAT3 pos)
{
	m_mouseworldpos = pos;
}
