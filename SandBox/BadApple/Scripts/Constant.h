#pragma once
#include <Math/Color.h>
#include <string>

static constexpr std::string_view DisplayWindowName		= "Bad Apple!!";
static constexpr std::string_view DllDirectory					= "./Plugins/";
static constexpr std::string_view WindowIniFileName		= "Assets/imgui.ini";
static constexpr std::string_view VideoFileName				= "Assets/BadApple.mp4";
static constexpr std::string_view AudioFileName				= "Assets/BadApple.wav";
static constexpr std::string_view FMOD_LogoFilename		= "Assets/FMOD_Logo.png";

//４分木の色分け
constexpr Math::Color QuadTreeColors[] =
{
	{ 0.42f, 0.45f, 0.50f, 1.0f },
	{ 0.43f, 0.91f, 0.72f, 1.0f },
	{ 0.95f, 0.65f, 0.69f, 1.0f },
	{ 0.99f, 0.83f, 0.30f, 1.0f },
};

constexpr uint64_t		QuadTreeMaxDepth					= 10;		//四分木空間の最大深度
constexpr float			QuadTreeColorParLevelScale		= 0.95f;	// 色のスケール
constexpr float			QuadTreeColorFrameThickness	= 0.5f;		// 色の枠線の太さ
