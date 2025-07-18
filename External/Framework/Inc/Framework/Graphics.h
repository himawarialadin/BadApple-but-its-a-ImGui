#include <d3d11.h>
#include <Math/Color.h>


class DX11Graphics
{
public:
	inline static DX11Graphics* Instance = nullptr;

public:
	DX11Graphics(HWND hwnd);
	~DX11Graphics();

public:
	void Render();
	void Present();

	void SetClearColor(Math::Color const& color) { g_ClearColor = color; }
	Math::Color const& GetClearColor() const { return g_ClearColor; }

	void Resize(UINT width, UINT height);

	ID3D11Device*					GetDevice() const { return g_pd3dDevice; }
	ID3D11DeviceContext*		GetContext() const { return g_pd3dDeviceContext; }
	ID3D11RenderTargetView* GetMainRTV() const { return g_mainRenderTargetView; }
private:
	void CleanUpRenderTarget();
	void CreateRenderTarget();

private:
	HWND									g_hWnd								= nullptr;
	ID3D11Device*						g_pd3dDevice					= nullptr;
	ID3D11DeviceContext*			g_pd3dDeviceContext			= nullptr;
	IDXGISwapChain*					g_pSwapChain					= nullptr;
	ID3D11RenderTargetView*		g_mainRenderTargetView	= nullptr;

	bool										g_SwapChainOccluded	= false;
	UINT										g_ResizeWidth = 0, g_ResizeHeight = 0;
	Math::Color							g_ClearColor = Math::Color(0.45f, 0.55f, 0.60f, 0.00f);
};