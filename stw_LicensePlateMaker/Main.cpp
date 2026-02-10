# include <Siv3D.hpp> // Siv3D v0.6.15
#include "Config.h"

void Main()
{
	system("start /wait res\\GameDir.bat");
	TextReader txr{ U"res\\GamePath.txt" };
	String path;
	txr.readLine(path);
	txr.close();
	path += U"\\sdk\\component_mod_compiler.com";
	FileSystem::Copy(path, U"res\\template\\component_mod_compiler.com");
	
	Window::SetTitle(U"Stormworks License Plate Maker");
	Window::SetToggleFullscreenEnabled(false);
	Window::SetStyle(WindowStyle::Sizable);
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
	Window::ResizeActual({ 1280,720 }, Centering::No);

	std::unique_ptr<Config> config = std::make_unique<Config>();

	std::unique_ptr<Editor> edit = std::make_unique<Editor>(config.get());

	while (System::Update())
	{
		edit->Update();
		edit->Draw();

		if (config->IsSizeChanged)
		{
			config->IsSizeChanged = false;
			edit.reset();
			edit = std::make_unique<Editor>(config.get());
		}
	}

	edit.reset();
}
