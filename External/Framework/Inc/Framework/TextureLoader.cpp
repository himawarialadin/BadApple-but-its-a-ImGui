#include "TextureLoader.h"
#include "Graphics.h"
#include <d3d11.h>
#include <wrl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>
#include <format>
#include <dxgidebug.h>

void TextureLoader::LoadTextureFromFile(std::string_view filename, ID3D11ShaderResourceView** srv, Math::Vector2Int* outSize /*= nullptr*/)
{
	auto device = DX11Graphics::Instance->GetDevice();

	int width, height, channels;
	// RGBA形式で強制的に読み込む（4チャネル）
	stbi_uc* imageData = stbi_load(filename.data(), &width, &height, &channels, 4);
	if (!imageData)
		throw std::runtime_error(std::format("画像の読み込みに失敗: {}", filename));

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA subresource = {};
	subresource.pSysMem = imageData;
	subresource.SysMemPitch = width * 4;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	HRESULT hr = device->CreateTexture2D(&texDesc, &subresource, &texture);
	stbi_image_free(imageData);  // 読み込みデータは不要になるので即解放

	if (FAILED(hr))
		throw std::runtime_error(std::format("画像の読み込みに失敗: {}", filename));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, &*srv);
	if (FAILED(hr))
		throw std::runtime_error(std::format("画像の読み込みに失敗: {}", filename));

	if (outSize != nullptr)
	{
		outSize->x = width;
		outSize->y = height;
	}

	(*srv)->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(filename.size()), filename.data());
}
