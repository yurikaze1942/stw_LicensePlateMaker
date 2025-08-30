#include "stdafx.h"

Creator::Creator(DotInfo* _dots, int _xSize, int _ySize, String _plate)
	: dots(_dots)
	, xSize(_xSize)
	, ySize(_ySize)
	, platePath(_plate)
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

	TextReader reader{ U"res\\work\\LicensePlate.xml" };
	String xml = reader.readAll();
	xml.replace(U"[ItemName]", FileSystem::BaseName(*res));
	reader.close();
	TextWriter writer{ U"res\\work\\LicensePlate.xml", OpenMode::Trunc };
	writer.write(xml);
	writer.close();

	system("start /wait res\\work\\_build.bat");

	std::filesystem::copy("res\\work\\LicensePlate.bin", (*res).toWstr().c_str(), std::filesystem::copy_options::overwrite_existing);
}

