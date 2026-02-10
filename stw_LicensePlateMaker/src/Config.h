#pragma once

class Config
{
public:

	int Width = 18;
	int Height = 9;

	int ListType = 0;

	bool IsSizeChanged = false;

	void ChangeSize(int _type)
	{
		ListType = _type;
		IsSizeChanged = true;

		switch (_type)
		{
		case 0:
			Width = 18;
			Height = 9;
			break;

		case 1:
			Width = 29;
			Height = 7;
			break;

		default:
			break;
		}
	};
};
