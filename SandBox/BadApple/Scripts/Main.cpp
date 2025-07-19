#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImPlot/implot.h>

#include <Framework/ISandbox.h>
#include <Framework/Framework.h>
#include <Framework/Window.h>
#include <Framework/Timer.h>
#include <Framework/TextureLoader.h>

#include "Mode/ImPlotMode.h"
#include "Mode/RectangleMode.h"
#include "Mode/ImGuiWindowMode.h"

#include "Window/PlayerWindow.h"
#include "Window/SettingWindow.h"
#include "Window/VideoWindow.h"
#include "Window/FPSWindow.h"
#include "Window/StartupPopupWindow.h"
#include "Window/StatusCheckPopupWindow.h"

#include "AnimationPlayer.h"
#include "Constant.h"

#include <array>
#include <Utility/Singleton.h>
#include <Utility/JobSystem/JobSystem.h>


class SandBox : public ISandbox
{
	using Windows = std::vector<std::unique_ptr<IWindow>>;

private:
	ModeBuildFuncTable						m_modeBuilders;
	Windows										m_windows;
	
	std::unique_ptr<AnimationPlayer>	m_player				= nullptr;
	SettingWindow*								m_settingWindow	= nullptr;
	VideoWindow*								m_videoWindow		= nullptr;
	FPSWindow*									m_fpsWindow		= nullptr;
private:
	template<typename T>
	T* CreateWindows()
	{
		auto window = std::make_unique<T>();
		T* ptr = window.get();
		m_windows.emplace_back(std::move(window));
		return ptr;
	}
	

	void OnInit(HWND) override
	{
		ImPlot::CreateContext();
		m_player = std::make_unique<AnimationPlayer>();
		m_player->Initialize(VideoFileName, AudioFileName);

		//Modes
		m_modeBuilders = 
		{
			{ []()-> IMode* { return new ImGuiWindowMode; },	"ImGui Window Mode" },
			{ []()-> IMode* { return new RectangleMode; },			"Rectangle Mode" },
			{ []()-> IMode* { return new ImPlotMode; },				"ImPlot Mode" },
			// Add more modes here as needed
		};
		//Windows
		{
			auto playerWindow = CreateWindows<PlayerWindow>();
			playerWindow->SetPlayer(m_player.get());

			m_videoWindow = CreateWindows<VideoWindow>();
			m_videoWindow->Initialize(VideoFileName);
			m_videoWindow->SetAnimationPlayer(m_player.get());

			m_settingWindow = CreateWindows<SettingWindow>();
			m_settingWindow->SetModeBuilders(&m_modeBuilders);
			m_settingWindow->SetVideoWindow(m_videoWindow);
			m_settingWindow->SetAnimationPlayer(m_player.get());

			m_fpsWindow = CreateWindows<FPSWindow>();

			auto popupWindow = CreateWindows<StartupPopupWindow>();
			auto statusPopup = CreateWindows<StatusCheckPopupWindow>();
		}

		//Load ini
		ImGui::LoadIniSettingsFromDisk(WindowIniFileName.data());
		

		Window::GetInstance()->SetEnableAlphaComposing(true);
	}

	void OnUpdate(HWND) override
	{
		m_fpsWindow->CalculateFPS(Timer::DeltaTime());
		m_player->Update();

		auto& mode = m_settingWindow->GetMode();
		if (mode != nullptr)
		{
			mode->m_data.whiteNodes = &m_player->GetCurrentFrame();
			mode->m_data.size = m_player->GetFrameSize();
		}
	}

	void OnDrawImGui(HWND) override
	{
		auto& mode = m_settingWindow->GetMode();
		if (mode != nullptr)
			mode->Show();

		for(auto& window : m_windows)
			window->Show();

	}

};

int main()
{
	auto& jobSystem = Utility::Singleton::Register<Utility::JobSystem>();
	jobSystem.Initialize();

	SetDllDirectoryA(DllDirectory.data());

	Framework f;
	f.Run(new SandBox, L"Bad Apple!! but it's ImGui");
}