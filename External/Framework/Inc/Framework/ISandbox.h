#pragma once
#include <Windows.h>

class ISandbox
{
public:
	virtual ~ISandbox() = default;

public:
	virtual void OnDrawImGui(HWND hwnd)	= 0;
	virtual void OnInit(HWND hwnd)				{};
	virtual void OnDestory(HWND hwnd)		{};
	virtual void OnUpdate(HWND hwnd)		{};
};

#define SB_FUNC_ONINIT void OnInit(HWND hwnd)
#define SB_FUNC_ONUPDATE void OnUpdate(HWND hwnd)
#define SB_FUNC_ONDRAWIMGUI void OnDrawImGui(HWND hwnd)

#define SANDBOX_CLASS(Name) \
class Name : public ISandbox \
{ \
public: \
	SB_FUNC_ONINIT				override; \
	SB_FUNC_ONUPDATE			override; \
	SB_FUNC_ONDRAWIMGUI	override; \
}