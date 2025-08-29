#include "stdafx.h"

Editor::Editor()
	: WIDTH(18)
	, HEIGHT(9)
	, DOTS_WIDTH(0.7f)
	, OFFSET_X(5)
	, OFFSET_Y(5)

	, isGen(false)

	, drawR(U"0")
	, drawG(U"0")
	, drawB(U"0")
	, backR(U"255")
	, backG(U"255")
	, backB(U"255")
{
	prevSize = Window::DefaultClientSize;

	Scene::SetBackground(Color(220,248,254));

	dots = new DotInfo[WIDTH * HEIGHT];
	dotSize = (prevSize.x * DOTS_WIDTH) / WIDTH;
	for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			int index = (WIDTH * y) + x;
			dots[index] = DotInfo();
			dots[index].Rect = Rect(x * dotSize + OFFSET_X, y * dotSize + OFFSET_Y, dotSize);
			dots[index].Color = Color(255,255,255,255);
		}
	}

	auto x0 = dotSize * 9 + OFFSET_X;
	centerLine = Line(x0, 0, x0, dotSize * 9 + (OFFSET_Y*2));

	currentColor = Palette::Black;
	backgroundColor = Palette::White;

	isFrameDraw = true;

	font = Font( FontMethod::SDF, 64);
}

Editor::~Editor()
{
	delete[] dots;
}

void Editor::Update()
{
	auto wSize = GetWindowSize();
	if (prevSize.x != wSize.x || prevSize.y != wSize.y)
	{
		prevSize = wSize;
		OnWindowSizeChange();
	}

	if (isGen)
	{
		if (gen.DotCurrentStep == gen.DotMaxStep)
		{
			task.join();
			isGen = false;
		}
			

		return;
	}

	for (int i = 0; i < WIDTH * HEIGHT; i++)
	{
		if (dots[i].Rect.leftPressed())
		{
			dots[i].Color = currentColor;
		}
		else if (dots[i].Rect.rightPressed())
		{
			dots[i].Color = Palette::White;
		}
	}
}

void Editor::Draw()
{
	if (isGen)
	{
		ProgressDraw();

		return;
	}

	GridDraw();
	ColorMenuDraw();
	UtilMenuDraw();
}

void Editor::GridDraw()
{
	centerLine.draw(3, Palette::Black);
	for (int i = 0; i < WIDTH * HEIGHT; i++)
	{
		dots[i].Rect.draw(dots[i].Color);
		if (isFrameDraw)
		{
			Color c = dots[i].Color;
			int ave = (c.r + c.g + c.b) * 0.333f;
			if (ave < 150)
				c = Palette::White;
			else
				c = Palette::Black;
			dots[i].Rect.drawFrame(1, c);
		}
	}
}

void Editor::ColorMenuDraw()
{
	SimpleGUI::Headline(U"描画色 (左クリック)", { dotSize * WIDTH + OFFSET_X + 10,OFFSET_Y });
	HSV c = currentColor;
	SimpleGUI::ColorPicker(c, { dotSize * WIDTH + OFFSET_X + 10,OFFSET_Y + 45 });
	currentColor = c;
	currentColor.r = ColorValueDraw(drawR, U"R:", currentColor.r, dotSize * WIDTH + OFFSET_X + 165, OFFSET_Y + 45);
	currentColor.g = ColorValueDraw(drawG, U"G:", currentColor.g, dotSize * WIDTH + OFFSET_X + 165, OFFSET_Y + 85);
	currentColor.b = ColorValueDraw(drawB, U"B:", currentColor.b, dotSize * WIDTH + OFFSET_X + 165, OFFSET_Y + 125);

	SimpleGUI::Headline(U"背景色 (右クリック)", { dotSize * WIDTH + OFFSET_X + 10,OFFSET_Y + 170 });
	c = backgroundColor;
	SimpleGUI::ColorPicker(c, { dotSize * WIDTH + OFFSET_X + 10,OFFSET_Y + 215 });
	backgroundColor = c;
	backgroundColor.r = ColorValueDraw(backR, U"R:", backgroundColor.r, dotSize * WIDTH + OFFSET_X + 165, OFFSET_Y + 220);
	backgroundColor.g = ColorValueDraw(backG, U"G:", backgroundColor.g, dotSize * WIDTH + OFFSET_X + 165, OFFSET_Y + 255);
	backgroundColor.b = ColorValueDraw(backB, U"B:", backgroundColor.b, dotSize * WIDTH + OFFSET_X + 165, OFFSET_Y + 295);

}

int Editor::ColorValueDraw(TextEditState& _text, String _label, int _colorValue, int _x, int _y)
{
	if (_text.text.length() > 0 || _colorValue > 0)
		_text.text = Format(_colorValue);
	SimpleGUI::Headline(_label, { _x, _y });
	SimpleGUI::TextBox(_text, { _x + 40, _y + 1 }, 50, 3);
	if (_text.textChanged)
	{
		if (_text.text.length() == 0)
		{
			return 0;
		}

		for (int i = _text.text.length() - 1; i >= 0; i--)
			if (_text.text[i] < '0' || _text.text[i]>'9')
				_text.text = _text.text.removed_at(i);
		auto v = Parse<int>(_text.text);
		if (v < 0)v = 0;
		if (v > 255) v = 255;
		return v;
	}
	return _colorValue;
}

void Editor::UtilMenuDraw()
{
	SimpleGUI::CheckBox(isFrameDraw, U"枠線の描画", { dotSize * WIDTH + OFFSET_X + 10,OFFSET_Y + 340 });
	if (SimpleGUI::Button(U"保存", { prevSize.x - 100, prevSize.y - 50 }, 90))
	{
		Create();
	}
}

void Editor::ProgressDraw()
{
	font(U"Dot : {:>3}/{:>3} | {:.2f}%"_fmt(gen.DotCurrentStep, gen.DotMaxStep, ((float)gen.DotCurrentStep / gen.DotMaxStep) * 100)).draw(24, Arg::topLeft(10, 10), Palette::Black);
	font(U"Byte: {:>5}/{:>5} | {:.2f}%"_fmt(gen.ByteCurrentStep, gen.ByteMaxStep, ((double)gen.ByteCurrentStep / gen.ByteMaxStep) * 100)).draw(24, Arg::topLeft(10, 35), Palette::Black);
}

void Editor::Create()
{
	gen = Creator(dots, WIDTH, HEIGHT);
	task = gen.Generate();
	isGen = true;
}

void Editor::OnWindowSizeChange()
{
	dotSize = (prevSize.x * DOTS_WIDTH) / WIDTH;
	for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			int index = (WIDTH * y) + x;
			dots[index].Rect = Rect(x * dotSize + OFFSET_X, y * dotSize + OFFSET_Y, dotSize);
		}
	}

	auto x0 = dotSize * 9 + OFFSET_X;
	centerLine = Line(x0, 0, x0, dotSize * 9 + (OFFSET_Y * 2));
}

DotInfo* Editor::GetDot(int x, int y)
{
	return &dots[(WIDTH * y) + x];
}
