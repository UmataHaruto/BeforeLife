#include "Stage.h"
#include "dx11mathutil.h"
#include "myimgui.h"
#include "CCamera.h"
#include "ModelMgr.h"

void Stage::Init()
{
	//�C���X�^���V���O�`�揉����
	blocks.InitiInstancing(
		MAP_WIDTH * MAP_HEIGHT * MAP_DEPTH,
		"assets/Modeldata/Block/GrassBlock.obj",
		"shader/vsinstance.fx",
		"shader/ps_shadowmap.fx",
		"assets/Modeldata/Block/"
	);
	// �s�񏉊���
	DX11MtxIdentity(shadow_mtx);
	//���̃��f��
	CModel* model = new CModel();
	*model = *ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::TERRAIN_COLLISION)].modelname);
	blocks_shadow = model;

	shadow_mtx._41 = 550;
	shadow_mtx._42 = 10;
	shadow_mtx._43 = -550;

}

void Stage::Update()
{
	XMVECTOR Eye = XMVectorSet(
		CCamera::GetInstance()->GetEye().x,
		CCamera::GetInstance()->GetEye().y,
		CCamera::GetInstance()->GetEye().z,
		0.0f);

	XMVECTOR At = XMVectorSet(
		CCamera::GetInstance()->GetLookat().x,
		CCamera::GetInstance()->GetLookat().y,
		CCamera::GetInstance()->GetLookat().z,
		0.0f);

	XMVECTOR Up = XMVectorSet(
		CCamera::GetInstance()->GetUp().x,
		CCamera::GetInstance()->GetUp().y,
		CCamera::GetInstance()->GetUp().z,
		0.0f);

	XMFLOAT4X4 pro = CCamera::GetInstance()->GetProjectionMatrix();
	XMMATRIX projection = XMLoadFloat4x4(&pro);
	XMMATRIX View = XMMatrixLookAtLH(Eye, At, Up);
	View = XMMatrixMultiply(View, projection);

	XMFLOAT4X4 ans;
	XMStoreFloat4x4(&ans, View);

	blocks_pos_world.clear();
	//�`�搔�J�E���^
	int instance_num = 0;
	//�X�e�[�W�̕`��
	for (int z = 0; z < MAP_DEPTH; z++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				XMFLOAT4X4 mtxtrans;
				XMFLOAT3 trans = { (y * 12.5f) - 700,(z * -12.5f),(x * -12.5f) + 700 };

				//���f���`��
				//��ʓ��ɂ�����̂����`�悵�Ȃ�
				Field[z][x][y].SetPos(trans);
				switch (Field[z][x][y].GetBlockId())
				{
				case BLOCKTYPE::EMPTY:

					break;

				case BLOCKTYPE::GRASS:
					if (IsInFrustum(Field[z][x][y].GetPos(), ans))
						blocks_pos_world.push_back(Field[z][x][y].GetMtx());
					instance_num++;
					break;
				}

			}
		}
	}
	blocks.InstanceUpdate(blocks_pos_world);
}

void Stage::Draw()
{
	XMVECTOR Eye = XMVectorSet(
		CCamera::GetInstance()->GetEye().x,
		CCamera::GetInstance()->GetEye().y,
		CCamera::GetInstance()->GetEye().z,
		0.0f);

	XMVECTOR At = XMVectorSet(
		CCamera::GetInstance()->GetLookat().x,
		CCamera::GetInstance()->GetLookat().y,
		CCamera::GetInstance()->GetLookat().z,
		0.0f);

	XMVECTOR Up = XMVectorSet(
		CCamera::GetInstance()->GetUp().x,
		CCamera::GetInstance()->GetUp().y,
		CCamera::GetInstance()->GetUp().z,
		0.0f);

	XMFLOAT4X4 pro = CCamera::GetInstance()->GetProjectionMatrix();
	XMMATRIX projection = XMLoadFloat4x4(&pro);
	XMMATRIX View = XMMatrixLookAtLH(Eye, At, Up);
	View = XMMatrixMultiply(View, projection);

	XMFLOAT4X4 ans;
	XMStoreFloat4x4(&ans,View);
	////�X�e�[�W�̕`��
	//for (int z = 0; z < MAP_DEPTH; z++) {
	//	for (int y = 0; y < MAP_HEIGHT; y++){
	//		for (int x = 0; x < MAP_WIDTH; x++) {
	//			XMFLOAT4X4 mtxtrans;
	//			XMFLOAT4X4 mtxrottrans;
	//			XMFLOAT3 trans = { (y * 12.5f) - 700,(z * -12.5f),(x * -12.5f) + 700 };

	//			DX11MtxTranslation(trans, mtxtrans);
	//			//���f���`��
	//			//��ʓ��ɂ�����̂����`�悵�Ȃ�
	//			Field[z][x][y].SetPos(trans);
	//			switch (Field[z][x][y].GetBlockId())
	//			{
	//			case BLOCKTYPE::EMPTY:

	//				break;

	//			case BLOCKTYPE::GRASS:
	//				if(IsInFrustum(Field[z][x][y].GetPos(),ans))
	//					Field[z][x][y].Draw();
	//				break;
	//			}

	//		}
	//	}
	//}
	
	blocks.DrawInstance(blocks_pos_world.size());
	blocks_shadow->Draw(shadow_mtx);
}

void Stage::DrawShadow(ID3D11InputLayout* layout_in, ID3D11VertexShader* vs_in, ID3D11PixelShader* ps_in)
{
	XMVECTOR Eye = XMVectorSet(
		CCamera::GetInstance()->GetEye().x,
		CCamera::GetInstance()->GetEye().y,
		CCamera::GetInstance()->GetEye().z,
		0.0f);

	XMVECTOR At = XMVectorSet(
		CCamera::GetInstance()->GetLookat().x,
		CCamera::GetInstance()->GetLookat().y,
		CCamera::GetInstance()->GetLookat().z,
		0.0f);

	XMVECTOR Up = XMVectorSet(
		CCamera::GetInstance()->GetUp().x,
		CCamera::GetInstance()->GetUp().y,
		CCamera::GetInstance()->GetUp().z,
		0.0f);

	XMFLOAT4X4 pro = CCamera::GetInstance()->GetProjectionMatrix();
	XMMATRIX projection = XMLoadFloat4x4(&pro);
	XMMATRIX View = XMMatrixLookAtLH(Eye, At, Up);
	View = XMMatrixMultiply(View, projection);

	XMFLOAT4X4 ans;
	XMStoreFloat4x4(&ans, View);
	//�X�e�[�W�̕`��
	for (int z = 0; z < MAP_DEPTH; z++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				XMFLOAT4X4 mtxtrans;
				XMFLOAT4X4 mtxrottrans;
				XMFLOAT3 trans = { (y * 12.5f) - 700,(z * -12.5f),(x * -12.5f) + 700 };

				DX11MtxTranslation(trans, mtxtrans);
				//���f���`��
				//��ʓ��ɂ�����̂����`�悵�Ȃ�
				Field[z][x][y].SetPos(trans);
				switch (Field[z][x][y].GetBlockId())
				{
				case BLOCKTYPE::EMPTY:

					break;

				case BLOCKTYPE::GRASS:
					if (IsInFrustum(Field[z][x][y].GetPos(), ans))
						Field[z][x][y].DrawShadow(layout_in, vs_in, ps_in);
					break;
				}

			}
		}
	}
}



void Stage::SetField(int x, int y, int z, Block data)
{
	Field[x][y][z] = data;
}

void Stage::SetLimit(int time)
{
	//���E��999�𒴂��Ă��Ȃ���
	if (time <= 999) {
		TimeLimit = time;
	}
	else {
		TimeLimit = 999;
	}
}

void Stage::SetRecord(int Record)
{
	//���E��999999�𒴂��Ă��Ȃ���
	if (Record <= 999999) {
		this->Record = Record;
	}
	else {
		this->Record = 999999;
	}
}

void Stage::RoadStageData()
{
	FILE* fp;

	fp = fopen("assets/StageData/STAGE1.txt", "r");

	BLOCKTYPE type;

	for (int y = 0; y < MAP_DEPTH; y++) {
		for (int z = 0; z < MAP_HEIGHT; z++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				fscanf(fp, "%d,", &type);
				Block block;
				Vector3 pos = {y,z,x};
				block.Init(pos, type);
				Field[y][z][x] = block;
			}
		}
	}
	//�X�e�[�W���̏�����
	for (int z = 0; z < MAP_DEPTH; z++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				XMFLOAT3 trans = { (y * 12.5f) - 700,(z * -12.5f),(x * -12.5f) + 700 };

				//���f���`��
				//��ʓ��ɂ�����̂����`�悵�Ȃ�
				Field[x][y][z].SetPos(trans);

			}
		}
	}
fclose(fp);

}

bool Stage::SearchBlock(XMFLOAT3 mousepos,Block* output)
{
	//�z��ԍ�
	int x = mousepos.x / 12.5;
	int y = 0;
	int z = mousepos.z / 12.5;

	//�z��O�͏������Ȃ�
	if (x >= 0 && y >= 0 && z >= 0)
	{
		output = &Field[y][x][z];
		return true;
	}
	return false;
}

Block Stage::GetField(int x,int y,int z)
{
	return Field[x][y][z];
}

