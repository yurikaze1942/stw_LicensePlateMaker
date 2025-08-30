#pragma once

struct PlateItem
{
public:
	PlateItem(String _label, String _path, Texture _preview)
		: Label(_label)
		, Path(_path)
		, Preview(_preview)
	{};

	String Label;
	String Path;
	Texture Preview;
};

class Editor
{
public:
	/// @brief コンストラクター
	Editor();

	~Editor();

	/// @brief ロジック更新
	void Update();

	/// @brief 描画
	void Draw();

private:

	/// @brief 18x9のグリッドを描画する
	void GridDraw();

	/// @brief 色選択メニューを描画する
	void ColorMenuDraw();

	/// @brief 色数値入力GUI
	/// @param _text
	/// @param _label 
	/// @param _colorValue 
	/// @param _x 
	/// @param _y 
	/// @return 
	int ColorValueDraw(TextEditState& _text, String _label, int _colorValue, int _x, int _y);

	/// @brief プレートタイプや保存ボタンなどのメニュー描画
	void UtilMenuDraw();

	void ProgressDraw();

	/// @brief パーツ作成
	void Create();

	/// @brief ウィンドウサイズが変えられると呼ぶ
	void OnWindowSizeChange();

	DotInfo* GetDot(int x, int y);

	inline Size GetWindowSize()
	{
		return Window::GetState().virtualSize;
	}

	const int WIDTH, HEIGHT, OFFSET_X, OFFSET_Y;
	const float DOTS_WIDTH;
	DotInfo* dots;

	int dotSize;
	Line centerLine;
	bool isFrameDraw;
	
	Size prevSize;

	Color currentColor;
	Color backgroundColor;

	TextEditState
		drawR,
		drawG,
		drawB,
		backR,
		backG,
		backB;

	ListBoxState plateTypeList;

	Array<PlateItem> plateItems;

	Rect
		dotProgressBar,
		dotProgressBack,
		byteProgressBar,
		byteProgressBack;

	bool isGen;

	Creator gen;
	std::thread task;
	Font font;

	//todo このクラスで色塗りをする
	//     保存ボタンを押すとエクスポート用のクラスでmeshバイナリーの色を置き換える
	//     置き換えるのは ( [x][y]C9FF )
	//     例えば 0101C9FF で左上のドット
    //     非同期ループとかで順番にバイナリーを見ていって、一致する並びがあれば置き換え

};

