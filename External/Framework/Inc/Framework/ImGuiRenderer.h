#pragma once
#include <WindowsFwd.h>

struct ID3D11RenderTargetView;
struct ID3D11DeviceContext;
struct ID3D11Device;

class DX11ImGuiRenderer
{
public:
	DX11ImGuiRenderer(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* ctx);
	~DX11ImGuiRenderer();

public:
	void NewFrame();
	void Render(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* rtv);

private:

};