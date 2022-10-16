#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <assimp/cimport.h>
#include <unordered_map>
#include <vector>
#include "animationdataassimp.h"
#include "Shader.h"
#include "memory.h"
#include "ModelData.h"
#include "vertexproto.h"
#include "DX11Settransform.h"
#include "CDirectxGraphics.h"

const int INTERPOLATENUM = 1;			// 補間数

std::vector<Texture> ModelData::loadMaterialTextures(
	aiMaterial* mtrl,
	aiTextureType type,
	std::string typeName,
	const aiScene* scene)
{
	std::vector<Texture> textures;		// このマテリアルに関連づいたDIFFUSEテクスチャのリスト
	ID3D11Device* dev;
	ID3D11DeviceContext* devcon;

	dev = CDirectXGraphics::GetInstance()->GetDXDevice();
	devcon = CDirectXGraphics::GetInstance()->GetImmediateContext();

	// マテリアルからテクスチャ個数を取得し(基本は1個)ループする
	for (unsigned int i = 0; i < mtrl->GetTextureCount(type); i++)
	{
		aiString str;

		// マテリアルからｉ番目のテクスチャファイル名を取得する
		mtrl->GetTexture(type, i, &str);

		// もし既にロードされたテクスチャであればロードをスキップする
		bool skip = false;

		// ロード済みテクスチャ数分ループする
		for (unsigned int j = 0; j < m_texturesloaded.size(); j++)
		{
			// ファイル名が同じであったら読み込まない
			if (std::strcmp(m_texturesloaded[j].path.c_str(), str.C_Str()) == 0)
			{
				// 読み込み済みのテクスチャ情報をDIFFUSEテクスチャのリストにセット
				textures.push_back(m_texturesloaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // まだ読み込まれていなかった場合
			Texture tex;

			std::string filename = std::string(str.C_Str());
			filename = ExtractFileName(filename, '\\');		// ファイル名を取得
			filename = m_directory + filename;				// リソースディレクトリ＋ファイル名

			bool sts = CreatetSRVfromFile(
				filename.c_str(),
				dev, devcon, &tex.texture);
			if (!sts) {
				MessageBox(nullptr, "Texture couldn't be loaded", "Error!", MB_ICONERROR | MB_OK);
			}

			tex.type = typeName;
			tex.path = str.C_Str();
			// テクスチャ情報をDIFFUSEテクスチャのリストにセット
			textures.push_back(tex);
			this->m_texturesloaded.push_back(tex);	// このモデルに関連づいたテクスチャリストにセット
		}
	}

	return textures;
}

ModelData::ModelData()
{
}

ModelData::~ModelData()
{

	Exit();
}

void ModelData::CreateBone(aiNode* node)
{
	BONE bone;

	/*
	struct BONE
	{
	//std::string Name;
	aiMatrix4x4 Matrix;						// 初期配置行列
	aiMatrix4x4 AnimationMatrix;			// ボーン行列
	aiMatrix4x4 OffsetMatrix;				// ボーンオフセット行列
	};

	std::map<std::string, BONE> m_Bone;		//ボーンデータ（名前で参照）
	*/

	// ノード名をキーにしてボーン情報を保存
	m_Bone[node->mName.C_Str()] = bone;

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		CreateBone(node->mChildren[n]);
	}
}

bool ModelData::Load(std::string resourcefolder,
	std::string filename)
{
	bool sts = m_assimpscene.Init(filename);
	if (!sts) {
		MessageBox(nullptr, "ModelData load error", "error", MB_OK);
		return false;
	}

	m_directory = resourcefolder;		// このモデルのテクスチャが存在するディレクトリ

	// ボーンを生成する
	CreateBone(m_assimpscene.GetScene()->mRootNode);

	// ボーンの配列位置を格納する
	unsigned int num = 0;
	for (auto& data : m_Bone) {
		data.second.idx = num;
		num++;
	}

	// aiノードを解析する
	processNode(m_assimpscene.GetScene()->mRootNode, m_assimpscene.GetScene());

	// ボーン行列格納用の定数バッファを生成する

	ID3D11Device* device;
	device = CDirectXGraphics::GetInstance()->GetDXDevice();

	sts = CreateConstantBufferWrite(device, sizeof(ConstantBufferBoneMatrix), &m_constantbufferbonematrix);
	if (!sts) {
		MessageBox(nullptr, "constant buffer create(bonematrix) fail", "error", MB_OK);
	}

	return true;
}

void ModelData::Draw(ID3D11DeviceContext* devcon, XMFLOAT4X4& mtxworld)
{
	// アニメーションデータを持っているか？
	if (m_assimpscene.HasAnimation())
	{	// ボーン行列を定数バッファに反映させる
		UpdateBoneMatrixConstantBuffer();
	}

	for (int i = 0; i < m_meshes.size(); i++)
	{
		// ワールド変換行列
		DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, mtxworld);
		// 定数バッファセット処理
		m_meshes[i].Draw(devcon);
	}
}

void ModelData::DrawOBB() {
	// OBB描画
	for (int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].DrawOBB(m_Bone);
	}
}

void ModelData::UpdateOBB(const XMFLOAT4X4& mtxworld) {
	// メッシュのOBB更新
	for (int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].UpdateOBB(m_Bone, mtxworld);
	}
}

// メッシュの解析
Mesh ModelData::processMesh(aiMesh* mesh, const aiScene* scene, int meshidx)
{
	std::vector<Vertex> vertices;			// 頂点
	std::vector<unsigned int> indices;		// 面の構成情報
	std::vector<Texture> textures;			// テクスチャ

	// 頂点情報を取得
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.m_Pos.x = mesh->mVertices[i].x;
		vertex.m_Pos.y = mesh->mVertices[i].y;
		vertex.m_Pos.z = mesh->mVertices[i].z;

		// 法線ベクトルが存在するか？
		if (mesh->HasNormals()) {
			vertex.m_Normal.x = mesh->mNormals[i].x;
			vertex.m_Normal.y = mesh->mNormals[i].y;
			vertex.m_Normal.z = mesh->mNormals[i].z;
		}
		else {
			vertex.m_Normal.x = 0.0f;
			vertex.m_Normal.y = 0.0f;
			vertex.m_Normal.z = 0.0f;
		}

		// テクスチャ座標（０番目）が存在するか？
		if (mesh->HasTextureCoords(0)) {
			vertex.m_Tex.x = mesh->mTextureCoords[0][i].x;
			vertex.m_Tex.y = mesh->mTextureCoords[0][i].y;
		}

		vertex.m_BoneNum = 0;

		for (unsigned int b = 0; b < 4; b++)
		{
			vertex.m_BoneIndex[b] = -1;
			vertex.m_BoneName[b] = "";
			vertex.m_BoneWeight[b] = 0.0f;
		}

		vertices.push_back(vertex);
	}

	//ボーンデータ初期化
	for (unsigned int b = 0; b < mesh->mNumBones; b++)
	{
		// メッシュに関連づいてるボーン情報を取得
		aiBone* bone = mesh->mBones[b];

		// ボーンオフセット行列を取得
		m_Bone[bone->mName.C_Str()].OffsetMatrix = bone->mOffsetMatrix;

		// ボーンに関連づいている頂点を選び､ウェイト値をセットする
		for (unsigned int widx = 0; widx < bone->mNumWeights; widx++)
		{
			aiVertexWeight weight = bone->mWeights[widx];

			unsigned int vidx = weight.mVertexId;			// このウエイトに関連づいてる頂点idx

			// メッシュの中の何番目か
			vertices[vidx].m_BoneWeight[vertices[vidx].m_BoneNum] = weight.mWeight;
			vertices[vidx].m_BoneName[vertices[vidx].m_BoneNum] = bone->mName.C_Str();
			// 該当するボーン名のインデックス値をセット
			vertices[vidx].m_BoneIndex[vertices[vidx].m_BoneNum] = m_Bone[bone->mName.C_Str()].idx;

			vertices[vidx].m_BoneNum++;

			assert(vertices[vidx].m_BoneNum <= 4);
		}
	}

	// テクスチャ情報を取得する
	if (mesh->mMaterialIndex >= 0)
	{
		// このメッシュのマテリアルインデックス値を取得する
		int	mtrlidx = mesh->mMaterialIndex;

		// シーンからマテリアルデータを取得する
		aiMaterial* material = scene->mMaterials[mtrlidx];

		// このマテリアルに関連づいたテクスチャを取り出す
		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);

		// このメッシュで使用しているテクスチャを保存
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	// 面の構成情報を取得
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	return Mesh(vertices, indices, textures);
}

void ModelData::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix)
{
	BONE* bone = &m_Bone[node->mName.C_Str()];

	//マトリクスの乗算順番に注意
	//（親ボーンとの相対位置を示す行列＊ボーン行列＊ボーンオフセット行列）
	aiMatrix4x4 worldMatrix;
	// 
	/*
	TReal a1, a2, a3, a4;   // assimp 行列は、行優先
	TReal b1, b2, b3, b4;
	TReal c1, c2, c3, c4;
	TReal d1, d2, d3, d4;
	*/

	worldMatrix = matrix;
	worldMatrix *= bone->AnimationMatrix;

	bone->Matrix = worldMatrix;
	bone->Matrix *= bone->OffsetMatrix;

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		UpdateBoneMatrix(node->mChildren[n], worldMatrix);
	}
}

void ModelData::Update(
	unsigned int animno,
	const XMFLOAT4X4& mtxworld,
	unsigned int animfileno,
	std::vector<AnimationDataAssimp*>& animationcontainer) {

	// 0番目のシーンを取り出し
	const aiScene* s = animationcontainer[animfileno]->GetScene();

	// アニメーションデータを持っているか？
	if (s->HasAnimations())
	{
		//アニメーションデータからボーンマトリクス算出
		aiAnimation* animation = s->mAnimations[animno];

		// ボーンの数だけループ
		for (unsigned int c = 0; c < animation->mNumChannels; c++)
		{
			aiNodeAnim* nodeAnim = animation->mChannels[c];

			// ボーン存在チェック(アニメーションにのみ存在するボーンがあった場合は無視する)
			auto itr = m_Bone.find(nodeAnim->mNodeName.C_Str());
			if (itr != m_Bone.end()) {
				BONE* bone = &m_Bone[nodeAnim->mNodeName.C_Str()];

				int f1, f2;

				f1 = m_Frame % nodeAnim->mNumRotationKeys;//簡易実装   
				aiQuaternion rot1 = nodeAnim->mRotationKeys[f1].mValue; // クオータニオン

				f1 = m_Frame % nodeAnim->mNumPositionKeys;//簡易実装
				aiVector3D pos1 = nodeAnim->mPositionKeys[f1].mValue;

				f2 = m_preFrame % nodeAnim->mNumRotationKeys;//簡易実装  
				aiQuaternion rot2 = nodeAnim->mRotationKeys[f2].mValue; // クオータニオン

				f2 = m_preFrame % nodeAnim->mNumPositionKeys;//簡易実装	
				aiVector3D pos2 = nodeAnim->mPositionKeys[f2].mValue;

				// 補間
				rot1.Interpolate(rot1, rot1, rot2, m_factor);
				pos1.x = pos1.x * (1.0f - m_factor) + pos2.x * (m_factor);
				pos1.y = pos1.y * (1.0f - m_factor) + pos2.y * (m_factor);
				pos1.z = pos1.z * (1.0f - m_factor) + pos2.z * (m_factor);

				bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot1, pos1);
			}
		}

		//再帰的にボーンマトリクスを更新
		UpdateBoneMatrix(m_assimpscene.GetScene()->mRootNode, aiMatrix4x4());

		// メッシュのOBB更新
		for (int i = 0; i < m_meshes.size(); i++)
		{
			m_meshes[i].UpdateOBB(m_Bone,mtxworld);
		}
	}

	if (m_cnt % INTERPOLATENUM == 0) {
		m_preFrame = m_Frame;
		m_Frame++;
		m_factor = 0;
	}

	m_factor = 1.0f / (float)(m_cnt % INTERPOLATENUM + 1);

	m_cnt++;
}

void ModelData::Exit()
{
	// テクスチャリソースを解放する
	for (auto tex : m_texturesloaded)
	{
		if (tex.texture != nullptr) {
			tex.texture->Release();
		}
	}

	// メッシュの解放
	for (int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Close();
	}

	// 定数バッファ解放
	if (m_constantbufferbonematrix) {
		m_constantbufferbonematrix->Release();
		m_constantbufferbonematrix = nullptr;
	}

	// assimp scene 解放
	m_assimpscene.Exit();

	// シーン(アニメーション用の)の解放
	for (auto s : m_sceneAnimContainer) {
		aiReleaseImport(s);
	}
}

// ノードの解析
void ModelData::processNode(aiNode* node, const aiScene* scene)
{
	// ノード内のメッシュの数分ループする
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		int meshindex = node->mMeshes[i];			// ノードのi番目メッシュのインデックスを取得
		aiMesh* mesh = scene->mMeshes[meshindex];	// シーンからメッシュ本体を取り出す

		m_meshes.push_back(this->processMesh(mesh, scene, meshindex));
	}

	// 子ノードについても解析
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

void ModelData::UpdateBoneMatrixConstantBuffer() {

	ConstantBufferBoneMatrix cb;
	DirectX::XMFLOAT4X4 mtx;

	// ボーン行列を定数バッファにセット
	unsigned int num = 0;
	for (auto data : m_Bone) {
		// aiMatrix4X4をXMFLOAT4X4へ変換(転置する)
		mtx = DX11MtxaiToDX(data.second.Matrix);

		// 転置する
		DX11MtxTranspose(mtx,mtx);

		cb.mBoneMatrix[num] = mtx;
		num++;
	}

	ID3D11DeviceContext* devicecontext;

	devicecontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	D3D11_MAPPED_SUBRESOURCE pData;

	// ボーン行列格納用定数バッファ更新
	HRESULT hr = devicecontext->Map(m_constantbufferbonematrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		memcpy_s(pData.pData, pData.RowPitch, (void*)&cb, sizeof(ConstantBufferBoneMatrix));
		devicecontext->Unmap(m_constantbufferbonematrix, 0);
	}

	// GPUへ定数バッファをセット
	devicecontext->VSSetConstantBuffers(5, 1, &m_constantbufferbonematrix);

}
