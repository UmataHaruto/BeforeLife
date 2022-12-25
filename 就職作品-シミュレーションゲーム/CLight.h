#pragma once
#include	<DirectXMath.h>
#include	"memory.h"
#include	"Shader.h"
#include	"CDirectxGraphics.h"
#include	"dx11mathutil.h"

class CLight {
	struct ConstantBufferLight {
		DirectX::XMFLOAT4 LightDirection;
		DirectX::XMFLOAT4 EyePos;
		DirectX::XMFLOAT4 invLightDirection;
		DirectX::XMFLOAT4 invEyePos;
	};

	enum class LightType {
		DIRECTIONAL,
		POINT,
		SPOT
	};
	LightType			m_type;
	DirectX::XMFLOAT3	m_eyepos;
	DirectX::XMFLOAT4	m_lightpos;		// w=0の時は方向 w=1の時は位置
	ID3D11Buffer* m_pConstantBufferLight = nullptr;
	DirectX::XMFLOAT4X4 m_invworldmtx;		// ワールド変換行列の逆行列
public:
	bool Init(DirectX::XMFLOAT3 eyepos, DirectX::XMFLOAT4 lightpos) {
		m_lightpos = lightpos;
		m_eyepos = eyepos;
		m_type = LightType::DIRECTIONAL;

		ID3D11Device* dev;
		dev = CDirectXGraphics::GetInstance()->GetDXDevice();

		// コンスタントバッファ作成
		bool sts = CreateConstantBuffer(
			dev,				// デバイス
			sizeof(ConstantBufferLight),		// サイズ
			&m_pConstantBufferLight);			// コンスタントバッファ４
		if (!sts) {
			MessageBox(NULL, "CreateBuffer(constant buffer Light) error", "Error", MB_OK);
			return false;
		}

		Update();

		return true;
	}

	// ワールド変換行列を指定して
	void SetMtxWorld(const DirectX::XMFLOAT4X4& worldmtx) {

		// ワールド変換行列の逆行列を求める
		DX11MtxInverse(m_invworldmtx, worldmtx);

		Update();
	}

	void Update() {
		ConstantBufferLight		cb;

		cb.EyePos.w = 1.0;
		cb.EyePos.x = m_eyepos.x;
		cb.EyePos.y = m_eyepos.y;
		cb.EyePos.z = m_eyepos.z;

		cb.LightDirection.x = m_lightpos.x;
		cb.LightDirection.y = m_lightpos.y;
		cb.LightDirection.z = m_lightpos.z;
		cb.LightDirection.w = m_lightpos.w;

		// 視点位置に逆行列を掛けて変換する
		DirectX::XMFLOAT3 inveye;
		DX11Vec3MulMatrix(inveye, m_eyepos, m_invworldmtx);

		// 光の方向に逆行列を掛けて変換する
		DirectX::XMFLOAT3 invlight;
		DirectX::XMFLOAT3 lightdir;
		lightdir.x = m_lightpos.x;
		lightdir.y = m_lightpos.y;
		lightdir.z = m_lightpos.z;
		DX11Vec3MulMatrix(invlight, lightdir, m_invworldmtx);

		cb.invEyePos.x = inveye.x;
		cb.invEyePos.y = inveye.y;
		cb.invEyePos.z = inveye.z;
		cb.invEyePos.w = 1.0;

		cb.invLightDirection.x = invlight.x;
		cb.invLightDirection.y = invlight.y;
		cb.invLightDirection.z = invlight.z;
		cb.invLightDirection.w = 0.0f;

		ID3D11DeviceContext* devcontext;
		devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

		devcontext->UpdateSubresource(m_pConstantBufferLight,
			0,
			nullptr,
			&cb,
			0, 0);

		// コンスタントバッファ4をｂ3レジスタへセット（頂点シェーダー用）
		devcontext->VSSetConstantBuffers(4, 1, &m_pConstantBufferLight);
		// コンスタントバッファ4をｂ3レジスタへセット(ピクセルシェーダー用)
		devcontext->PSSetConstantBuffers(4, 1, &m_pConstantBufferLight);
	}

	void Uninit() {
		if (m_pConstantBufferLight) {
			m_pConstantBufferLight->Release();
			m_pConstantBufferLight = nullptr;
		}
	}

	void SetEyePos(DirectX::XMFLOAT3 eyepos) {
		m_eyepos = eyepos;
	}

	void SetLightPos(DirectX::XMFLOAT4 lightpos) {
		m_lightpos = lightpos;
	}
};