#pragma once
#include <string_view>
#include <Math/Vector2Int.h>

struct ID3D11ShaderResourceView;
class TextureLoader
{
public:
	static void LoadTextureFromFile(std::string_view filename, ID3D11ShaderResourceView** srv, Math::Vector2Int* outSize = nullptr);
};