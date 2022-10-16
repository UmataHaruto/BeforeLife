#include	<DirectXMath.h>
#include	<wrl/client.h>
#include	"dx11util.h"
#include	"2dsystem.h"
#include	"shader.h"

using Microsoft::WRL::ComPtr;

ComPtr<ID3D11Buffer> g_CBViewPort;
ComPtr<ID3D11Buffer> g_CBWorld;

bool Init2D() {

	ID3D11Device* device;
	ID3D11DeviceContext* devicecontext;

	// �f�o�C�X�擾
	device = GetDX11Device();
	devicecontext = GetDX11DeviceContext();


	// �萔�o�b�t�@�𐶐�
	bool sts = CreateConstantBufferWrite(device, sizeof(ConstantBufferViewPort), &g_CBViewPort);
	if (!sts) {
		MessageBox(nullptr, "CreateConstantBuffer error viewport", "error", MB_OK);
		return false;
	}

	// �萔�o�b�t�@�𐶐�
	sts = CreateConstantBuffer(device, sizeof(ConstantBufferWorld), &g_CBWorld);
	if (!sts) {
		MessageBox(nullptr, "CreateConstantBuffer error world", "error", MB_OK);
		return false;
	}

	// �萔�o�b�t�@�X�V
	ConstantBufferViewPort cb;
	cb.ViewportHeight[0] = DX11GetScreenHeight();// DirectXGraphics::GetInstance()->GetViewPortHeight();
	cb.ViewportWidth[0] = DX11GetScreenWidth();

	D3D11_MAPPED_SUBRESOURCE pData;
	HRESULT hr = devicecontext->Map(g_CBViewPort.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		memcpy_s(pData.pData, pData.RowPitch, (void*)&cb,sizeof(ConstantBufferViewPort));
		devicecontext->Unmap(g_CBViewPort.Get(), 0);
	}

	// �萔�o�b�t�@���f�o�t�փZ�b�g
	devicecontext->VSSetConstantBuffers(5, 1, g_CBViewPort.GetAddressOf());

	return false;
}

void Exit2D() {

}