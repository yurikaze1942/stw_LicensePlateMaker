#include "stdafx.h"

Creator::Creator(DotInfo* _dots, int _xSize, int _ySize, String _plate)
	: dots(_dots)
	, xSize(_xSize)
	, ySize(_ySize)
	, platePath(_plate)
	, ByteCurrentStep(0)
	, ByteMaxStep(0)
	, DotCurrentStep(0)
	, DotMaxStep(0)
{

}

std::thread Creator::Generate()
{
	DotMaxStep = xSize * ySize + 1;
	DotCurrentStep = 0;
	ByteMaxStep = 1;
	ByteCurrentStep = 0;

	return std::thread([&] {GenerateWork(); });
}

void Creator::Color2Binary(Color _color, char* _bytes)
{
	_bytes[0] = _color.r;
	_bytes[1] = _color.g;
	_bytes[2] = _color.b;
	_bytes[3] = 255;
}

void Creator::ReplaceColor(char* _meshData ,Color _color, int _x, int _y)
{
	ByteCurrentStep = 0;
	char c[4];
	Color2Binary(_color, c);

	char target[4] = { (char)_x,(char)_y,(char)0xC9,(char)0xFF };

	while (ByteCurrentStep < ByteMaxStep)
	{
		auto isFind = true;
		for (int i = 0; i < 4; i++)
			if (_meshData[ByteCurrentStep + i] != target[i])
			{
				isFind = false;
				break;
			}

		if (isFind)
		{
			for (int i = 0; i < 4; i++)
				_meshData[ByteCurrentStep + i] = c[i];

			ByteCurrentStep += 4;

			//file.seekp(ByteCurrentStep,std::ios::beg);
			//file.write(c, 4);
			//file.flush();
		}
		else
		{
			ByteCurrentStep++;
		}

		

		//if (step >= 4 || ByteCurrentStep + 100 > ByteMaxStep)
		//	break;
	}
}

void Creator::GenerateWork()
{
	auto path = getPathToSave();
	auto fileName = FileSystem::BaseName(path);

	FileSystem::RemoveContents(U"res\\work\\");

	std::filesystem::copy("res\\template\\", "res\\work\\", std::filesystem::copy_options::overwrite_existing);
	std::filesystem::copy(platePath.toWstr(), "res\\work\\lp.mesh", std::filesystem::copy_options::overwrite_existing);
	ByteMaxStep = std::filesystem::file_size("res\\work\\lp.mesh");
	auto ifs = std::ifstream("res\\work\\lp.mesh", std::ios::binary | std::ios::in);
	char* meshData = new char[ByteMaxStep+1];
	char buf;
	while (!ifs.eof())
	{
		ifs.seekg(ByteCurrentStep);
		ifs.read(&buf, 1);
		meshData[ByteCurrentStep] = buf;
		ByteCurrentStep++;
	}
	ifs.close();
	ByteCurrentStep = 0;
	DotCurrentStep++;

	for (int x = 0; x < xSize; x++)
	{
		for (int y = 0; y < ySize; y++)
		{
			int index = (xSize * y) + x;
			ReplaceColor(meshData, dots[index].Color, x, y);
			DotCurrentStep++;
		}
	}

	ByteCurrentStep = 0;

	auto ofs = std::ofstream("res\\work\\lp.mesh", std::ios::binary | std::ios::trunc | std::ios::out);
	for (unsigned int i = 0; i < ByteMaxStep; i++)
	{
		ofs.write(&meshData[i], 1);
		ByteCurrentStep++;
	}
	DotCurrentStep++;
	ofs.close();

	delete[] meshData;

	TextReader reader{ U"res\\work\\LicensePlate.xml" };
	String xml = reader.readAll();
	xml.replace(U"[ItemName]", fileName);
	reader.close();
	TextWriter writer{ U"res\\work\\LicensePlate.xml", OpenMode::Trunc };
	writer.write(xml);
	writer.close();

	std::filesystem::rename("res\\work\\LicensePlate.xml", (U"res\\work\\{}.xml"_fmt(fileName)).toWstr());

	//Grokありがとう =======

	STARTUPINFO si = {};
	PROCESS_INFORMATION pi = {};
	si.cb = sizeof(si);

	// 作業フォルダを明示的に指定
	String workDir = FileSystem::InitialDirectory() + U"res\\work";

	// コマンドライン（cmd.exe でラップ推奨）
	String cmdLine = U"cmd.exe /c component_mod_compiler.com {}.xml lp.mesh"_fmt(fileName);

	// フルパスの方が確実
	// String cmdLine = U"cmd.exe /c \"res\\work\\component_mod_compiler.com\" {} lp.mesh"_fmt(fileName);

	auto rv = CreateProcess(
		nullptr,                                      // lpApplicationName
		(wchar_t*)cmdLine.toWstr().c_str(),           // lpCommandLine（修正可能）
		nullptr, nullptr, false,
		0,                                            // CREATE_NO_WINDOW とか入れてもOK
		nullptr,
		workDir.toWstr().c_str(),                     // ← これ重要！
		&si,
		&pi
	);

	if (rv) {
		WaitForSingleObject(pi.hProcess, INFINITE);   // 完了待機
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else {
		DWORD err = GetLastError();
		System::MessageBoxOK(U"CreateProcess失敗: ErrorCode = {}"_fmt(err));
	}

	//Grok終わり =======

	std::filesystem::copy((U"res\\work\\{}.bin"_fmt(fileName)).toWstr(), path.toWstr(), std::filesystem::copy_options::overwrite_existing);
}

String Creator::getPathToSave()
{
	TextReader txr{ U"res\\GamePath.txt" };
	String path;
	txr.readLine(path);
	txr.close();
	path += U"\\rom\\data\\components\\";

	auto defaultName = U"LicensePlate_{:08x}.xml"_fmt(Random<int>(0, 0xfffffff));

	auto fil = FileFilter();
	fil.name = U"binファイル";
	fil.patterns = { U"bin" };
	Optional<FilePath> res = Dialog::SaveFile({ fil }, path, U"ファイルを保存", defaultName);
	if (!res.has_value())
		return path + defaultName;

	return res.value();
}

