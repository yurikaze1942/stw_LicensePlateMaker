#include "stdafx.h"

Creator::Creator(DotInfo* _dots, int _xSize, int _ySize)
	: dots(_dots)
	, xSize(_xSize)
	, ySize(_ySize)
{

}

std::thread Creator::Generate()
{
	DotMaxStep = xSize * ySize;
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

void Creator::ReplaceColor(Color _color, int _x, int _y)
{
	ByteCurrentStep = 0;
	file.clear();
	file.seekg(0, std::ios::beg);
	file.seekp(0, std::ios::beg);
	char c[4];
	Color2Binary(_color, c);

	char target[4] = { (char)_x,(char)_y,(char)0xC9,(char)0xFF };

	char buf[4];

	while (!file.eof())
	{
		file.seekg(ByteCurrentStep);
		file.read(buf, 4);

		auto isFind = true;
		for(int i=0; i<4;i++)
			if (buf[i] != target[i])
			{
				isFind = false;
				break;
			}

		if (isFind)
		{
			file.seekp(ByteCurrentStep,std::ios::beg);
			file.write(c, 4);
			file.flush();
		}

		ByteCurrentStep++;

		//if (step >= 4 || ByteCurrentStep + 100 > ByteMaxStep)
		//	break;
	}
}

void Creator::GenerateWork()
{
	
	std::filesystem::copy("res\\template\\", "res\\work\\", std::filesystem::copy_options::overwrite_existing);
	std::filesystem::copy("res\\mesh\\lp_18_15d.mesh", "res\\work\\lp.mesh", std::filesystem::copy_options::overwrite_existing);
	ByteMaxStep = std::filesystem::file_size("res\\work\\lp.mesh");
	file = std::fstream("res\\work\\lp.mesh", std::ios::binary | std::ios::ate | std::ios::in | std::ios::out);

	

	for (int x = 0; x < xSize; x++)
	{
		for (int y = 0; y < ySize; y++)
		{
			int index = (xSize * y) + x;
			ReplaceColor(dots[index].Color, x, y);
			DotCurrentStep++;
		}
	}
	file.close();

	openSaveFileDialog();
}

void Creator::openSaveFileDialog()
{
	TextReader txr{ U"res\\GamePath.txt" };
	String path;
	txr.readLine(path);
	txr.close();
	path += U"\\rom\\data\\components\\";

	auto fil = FileFilter();
	fil.name = U"binファイル";
	fil.patterns = { U"bin" };
	Optional<FilePath> res = Dialog::SaveFile({ fil }, path, U"ファイルを保存", U"LicensePlate.bin");
	if (!res.has_value())
		return;

	system("start /wait res\\work\\_build.bat");

	std::filesystem::copy("res\\work\\LicensePlate.bin", (*res).toWstr().c_str(), std::filesystem::copy_options::overwrite_existing);
}

