#include	"ModelMgr.h"
#include "dx11util.h"
#include <DirectXTex.h>

void ModelMgr::CreateShaderResourceViewFromFile(ID3D11ShaderResourceView* m_pTexture, const wchar_t* FileName)
{
	ID3D11Device* m_pdevice = CDirectXGraphics::GetInstance()->GetDXDevice();

    char ms[100];
    char* extension = strstr(ms, ".");

        TexMetadata meta;
        GetMetadataFromTGAFile(FileName, meta);

        std::unique_ptr<ScratchImage> image(new ScratchImage);
        HRESULT hr = LoadFromTGAFile(FileName, &meta, *image);
        hr = CreateShaderResourceView(m_pdevice, image->GetImages(), image->GetImageCount(), meta, &m_pTexture);
}
