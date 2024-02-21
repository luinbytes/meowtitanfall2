#include "Renderer.hpp"

void Renderer::Init()
{
	tahoma = MatSystemSurface->CreateFont();
	if (!MatSystemSurface->SetFontGlyphSet(tahoma, "Tahoma", 12, 500, 0, 0, FONTFLAG_DROPSHADOW)) {
		printf("failed to create font");
	}
}

void Renderer::DrawRect(int x, int y, int w, int h, Color color)
{
	MatSystemSurface->DrawSetColorRGBA(color.r, color.g, color.b, color.a);
	MatSystemSurface->DrawFilledRect(x, y, x + w, y + h);
}

void Renderer::DrawOutlinedRect(int x, int y, int w, int h, Color color)
{
	MatSystemSurface->DrawSetColorRGBA(color.r, color.g, color.b, color.a);
	MatSystemSurface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Renderer::DrawLine(int x0, int y0, int x1, int y1, Color color)
{
	MatSystemSurface->DrawSetColorRGBA(color.r, color.g, color.b, color.a);
	MatSystemSurface->DrawLine(x0, y0, x1, y1);
}

void Renderer::DrawString(unsigned long font, int x, int y, Color color, const char* str, ...)
{
	MatSystemSurface->DrawColoredTextWrapper(font, x, y, color.r, color.g, color.b, color.a, str);
}

std::pair<int, int> Renderer::GetTextSize(unsigned long font, std::string text) {
	std::wstring a(text.begin(), text.end());
	const wchar_t* wstr = a.c_str();
	static int w, h;

	MatSystemSurface->GetTextSize(font, wstr, &w, &h);
	return std::make_pair(w, h);
}