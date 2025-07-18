#include "Framework.h"
#include "Graphics.h"
#include "Window.h"
#include "MainDokingBuilder.h"
#include "ISandbox.h"
#include "ImGuiRenderer.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx12.h>
#include <ImGui/imgui_impl_win32.h>
#include <d3d12.h>
#include <dxgidebug.h>
#include <Utility/Singleton.h>
#include <Utility/JobSystem/JobSystem.h>
#include "Timer.h"
#include <memory>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


void Framework::Run(ISandbox* sandBox, std::wstring_view title)
{
	std::unique_ptr<Window>							window				= std::make_unique<Window>(title);
	std::unique_ptr<DX11Graphics>					graphics				= std::make_unique<DX11Graphics>(window->GetHWND());
	std::unique_ptr<DX11ImGuiRenderer>		imguiRenderer	= std::make_unique<DX11ImGuiRenderer>(window->GetHWND(), graphics->GetDevice(), graphics->GetContext());
	std::unique_ptr<MainDockingBuilder>			dockingBuilder	= std::make_unique<MainDockingBuilder>();

	window->SetCustomWndProc(ImGui_ImplWin32_WndProcHandler);
	window->Show();

	sandBox->OnInit(window->GetHWND());


	while (window->ProcessMessage())
	{
		Timer::Update();

		sandBox->OnUpdate(window->GetHWND());

		//ImGui new frame
		{
			imguiRenderer->NewFrame();
			dockingBuilder->Dock();
			sandBox->OnDrawImGui(window->GetHWND());
		}

		//Render
		{
			graphics->Render();
			imguiRenderer->Render(graphics->GetContext(), graphics->GetMainRTV());
			graphics->Present();
		}

	}

	sandBox->OnDestory(window->GetHWND());

	Utility::Internal::SingletonFinalizer::Finalize();
	delete sandBox;
}

void Framework::Exit()
{
	
	::PostMessage(Window::GetInstance()->GetHWND() , WM_QUIT, 0, 0);
}
