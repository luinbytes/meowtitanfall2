#pragma once
#include "../Include.hpp"

inline Font tahoma;

namespace Renderer
{
	void Init();
	void DrawRect(int x, int y, int w, int h, Color color);
	void DrawOutlinedRect(int x, int y, int w, int h, Color color);
	void DrawLine(int x0, int y0, int x1, int y1, Color color);
	void DrawString(unsigned long font, int x, int y, Color color, const char* str, ...);
	std::pair<int, int> GetTextSize(unsigned long font, std::string text);
}