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

//�K�w���Ƃ̋�Ԃ̐�
template<size_t N>
struct QuadSpaces
{
	static constexpr std::array<uint64_t, N + 1> Spaces = []()
		{
			//spaces[n] = {4^(n + 1) - 1} / 3

			std::array<uint64_t, N + 1> arr{};
			arr[0] = 0; // �ŏ��̊K�w�͋�Ԃ��Ȃ��̂�0
			for (int i = 0; i < N; ++i)
			{
				uint64_t power = 1ULL << (2 * (i + 1));
				arr[i + 1] = (power - 1) / 3;
			}
			return arr;
		}();
};

/*!
@brief �l���؂̃m�[�h��ID�Ɛ[�x����F���擾����
*/
Math::Color QuadTreeColorsToColor(QuadTreeNode const& node);

/*!
	@brief �w�肵����`�ɍ��킹�ăm�[�h�̕\���ʒu�ƃT�C�Y���v�Z����
	@param node �Ώۂ̃m�[�h
	@param rectMin ��`�̍ŏ����W
	@param rectMax ��`�̍ő���W
	@param[out] outPositon �o�͂����m�[�h�̕\���ʒu
	@param[out] outSize �o�͂����m�[�h�̕\���T�C�Y
*/
void CalculateNodeDisplaySize(QuadTreeNode const& node, Math::Vector2 const& size, Math::Vector2 const& rectMin, Math::Vector2 const& rectMax, Math::Vector2* outPositon, Math::Vector2* outSize);

/*!
	@brief �����`�����ς��ɃA�X�y�N�g���ύX�����ɉ摜��\�����邽�߂̃T�C�Y���v�Z����
	@param rectWidth ��`�̕�
	@param rectHeight ��`�̍���
	@param imageWidth �摜�̕�
	@param imageHeight �摜�̍���
	@return �\���T�C�Y
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