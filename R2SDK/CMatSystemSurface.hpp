#pragma once

struct Color
{
	Color() {}
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { this->r = r; this->g = g; this->b = b; this->a = a; }
	uint8_t r, g, b, a;
};

class VPanelWrapper
{
public:

};

inline VPanelWrapper* PanelWrapper;

using Font = uint32_t;

enum EFontFlags {
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,		// custom generated font - never fall back to asian compatibility mode
	FONTFLAG_BITMAP = 0x800,		// compiled bitmap font - no fallbacks
};

enum EFontDrawType : int {
	// Use the "additive" value from the scheme file
	FONT_DRAW_DEFAULT = 0,

	// Overrides
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

class CMatSystemSurface
{
public:
	uintptr_t GetEmbeddedPanel()
	{
		return Memory::CallVirtual<9, uintptr_t>(this);
	}

	void PushMakeCurrent(uintptr_t panel, bool useInSets)
	{
		return Memory::CallVirtual<11, void>(this, panel, useInSets);
	}

	void PopMakeCurrent(uintptr_t panel)
	{
		return Memory::CallVirtual<12, void>(this, panel);
	}

	void DrawSetColorRGBA(char r, char g, char b, int a)
	{
		return Memory::CallVirtual<14, void>(this, r, g, b, a);
	}	
	
	void DrawSetColor(Color color)
	{
		return Memory::CallVirtual<15, void>(this, color);
	}

	void DrawFilledRect(int x, int y, int w, int h)
	{
		return Memory::CallVirtual<16, void>(this, x, y, w, h);
	}
	
	void DrawOutlinedRect(int x, int y, int w, int h)
	{
		return Memory::CallVirtual<19, void>(this, x, y, w, h);
	}
		
	void DrawLine(int x, int y, int w, int h)
	{
		return Memory::CallVirtual<20, void>(this, x, y, w, h);
	}

	__int64 DrawColoredTextWrapper(int font, int x, int y, int r, int g, int b, int a, const char* fmt, ...)
	{
		return Memory::CallVirtual<171, __int64>(this, font, x, y, r, g, b, a, fmt);
	}

	int GetScreenSize(int* width, int* height)
	{
		return Memory::CallVirtual<48, int>(this, width, height);
	}

	void GetTextSize(Font font, const wchar_t* text, int* width, int* height) {
		return Memory::CallVirtual<86, void>(this, font, text, width, height);
	}

	Font CreateFont() {
		return Memory::CallVirtual<78, Font>(this);
	}

	bool SetFontGlyphSet(Font font, const char* windows_font_name, int tall, int weight, int blur, int scanlines, int flags, int range_min = 0, int range_max = 0) {
		return Memory::CallVirtual<79, bool>(this, font, windows_font_name, tall, weight, blur, scanlines, flags, range_min, range_max);
	}
};

inline CMatSystemSurface* MatSystemSurface;