#pragma once
class Creator
{
public:
	Creator() = default;
	Creator(DotInfo* _dots, int _xSize, int _ySize, String _plate);

	std::thread Generate();

	int DotMaxStep, DotCurrentStep;
	unsigned int ByteMaxStep, ByteCurrentStep;

private:
	void Color2Binary(Color _color, char* _bytes);

	void ReplaceColor(char* _meshData, Color _color, int _x, int _y);

	void GenerateWork();

	void openSaveFileDialog();

	DotInfo* dots;
	int xSize, ySize;
	String platePath;
};

