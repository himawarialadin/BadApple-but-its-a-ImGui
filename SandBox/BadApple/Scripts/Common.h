#pragma once
#include <Math/Vector2.h>
#include <Math/Color.h>
#include <string>
#include <vector>
#include <array>

class IMode;
struct ColoredNode;
struct QuadTreeNode;

using ModeBuildFunc = IMode * (*)();
using ModeBuildFuncTable = std::vector<std::pair<ModeBuildFunc, std::string_view>>;

//階層ごとの空間の数
template<size_t N>
struct QuadSpaces
{
	static constexpr std::array<uint64_t, N + 1> Spaces = []()
		{
			//spaces[n] = {4^(n + 1) - 1} / 3

			std::array<uint64_t, N + 1> arr{};
			arr[0] = 0; // 最初の階層は空間がないので0
			for (int i = 0; i < N; ++i)
			{
				uint64_t power = 1ULL << (2 * (i + 1));
				arr[i + 1] = (power - 1) / 3;
			}
			return arr;
		}();
};

/*!
@brief 四分木のノードのIDと深度から色を取得する
*/
Math::Color QuadTreeColorsToColor(QuadTreeNode const& node);

/*!
	@brief 指定した矩形に合わせてノードの表示位置とサイズを計算する
	@param node 対象のノード
	@param rectMin 矩形の最小座標
	@param rectMax 矩形の最大座標
	@param[out] outPositon 出力されるノードの表示位置
	@param[out] outSize 出力されるノードの表示サイズ
*/
void CalculateNodeDisplaySize(QuadTreeNode const& node, Math::Vector2 const& size, Math::Vector2 const& rectMin, Math::Vector2 const& rectMax, Math::Vector2* outPositon, Math::Vector2* outSize);

/*!
	@brief ある矩形いっぱいにアスペクト比を変更せずに画像を表示するためのサイズを計算する
	@param rectWidth 矩形の幅
	@param rectHeight 矩形の高さ
	@param imageWidth 画像の幅
	@param imageHeight 画像の高さ
	@return 表示サイズ
*/
inline Math::Vector2 CalculateDisplaySize(float rectWidth, float rectHeight, float imageWidth, float imageHeight)
{
	float aspectRect = rectWidth / rectHeight;
	float aspectImage = imageWidth / imageHeight;

	float displayWidth, displayHeight;

	if (aspectImage > aspectRect)
	{
		// Scale based on width
		displayWidth = rectWidth;
		displayHeight = imageHeight * (rectWidth / imageWidth);
	}
	else
	{
		// Scale based on height
		displayWidth = imageWidth * (rectHeight / imageHeight);
		displayHeight = rectHeight;
	}

	return { displayWidth, displayHeight };
}