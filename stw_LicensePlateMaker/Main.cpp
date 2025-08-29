# include <Siv3D.hpp> // Siv3D v0.6.15

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

	auto edit = new Editor();

	while (System::Update())
	{
		edit->Update();
		edit->Draw();
	}
}
