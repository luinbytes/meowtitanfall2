#include "Framework.hpp"
#include "../../Renderer/Renderer.hpp"

#include <windows.h>

POINT cursor;
POINT cursor_corrected;
bool should_drag;
bool should_move;

bool CursorInFrame(int x, int y, int w, int h) {
	GetCursorPos(&cursor);

	if (cursor.x > x && cursor.y > y && cursor.x < x + w && cursor.y < y + h)
		return true;

	return false;
}

void Framework::HandleMovement() {
	GetCursorPos(&cursor);

	if (GetAsyncKeyState(VK_LBUTTON) < 0 && CursorInFrame(Utils::x, Utils::y, Utils::w, Utils::header_height)) {
		should_drag = true;

		if (!should_move) {
			cursor_corrected.x = cursor.x - Utils::x;
			cursor_corrected.y = cursor.y - Utils::y;
			should_move = true;
		}
	}

	if (should_drag) {
		Utils::x = cursor.x - cursor_corrected.x;
		Utils::y = cursor.y - cursor_corrected.y;
	}

	if (GetAsyncKeyState(VK_LBUTTON) == 0) {
		should_drag = false;
		should_move = false;
	}
}

void Framework::Begin() {
	if (!Utils::opened)
		return;

	HandleMovement();

	// Background
	Renderer::DrawRect(Utils::x, Utils::y, Utils::w, Utils::h, Color(15, 15, 15, 255));
	// Header background
	Renderer::DrawRect(Utils::x, Utils::y, Utils::w, Utils::header_height, Color(25, 25, 25, 255));
	// Full menu outline
	Renderer::DrawOutlinedRect(Utils::x, Utils::y, Utils::w, Utils::h, Color(50, 50, 50, 255));
	// Darker menu outline
	Renderer::DrawOutlinedRect(Utils::x - 1, Utils::y - 1, Utils::w + 2, Utils::h + 2, Color(0, 0, 0, 255));
	// Header outline
	Renderer::DrawOutlinedRect(Utils::x, Utils::y, Utils::w, Utils::header_height, Color(50, 50, 50, 255));
	// Header outline top
	Renderer::DrawOutlinedRect(Utils::x, Utils::y, Utils::w, Utils::header_height - 1, Color(75, 75, 75, 255));
	// Header outline bottom
	Renderer::DrawOutlinedRect(Utils::x, Utils::y, Utils::w, Utils::header_height + 1, Color(75, 75, 75, 255));
	
	// Brighter menu outline
	// we call this here to overlap the header outlines LOL
	Renderer::DrawOutlinedRect(Utils::x + 1, Utils::y + 1, Utils::w - 2, Utils::h - 2, Color(75, 75, 75, 255));

	// Title text size
	auto [title_x, title_y] = Renderer::GetTextSize(tahoma, "titanfall 2 hake");
	// Title header offset
	int offset = Utils::header_height / 2.f - title_y / 2.f;
	// Title
	Renderer::DrawString(tahoma, Utils::x + offset, Utils::y + offset, Color(255, 255, 255, 255), "titanfall 2 hake");
}

int Framework::AddTabs(std::vector<std::string> tabs) {

	// Initial offset from the last rendered tab
	int offset = 0;
	auto tab = [&](std::string name, int x, int y, bool active) -> bool {
		auto [name_x, name_y] = Renderer::GetTextSize(tahoma, name);
		int w = name_x + 6;
		int h = 20;
		Renderer::DrawString(tahoma, x + w / 2.f - name_x / 2.f, y + h / 2.f - name_y / 2.f, active ? Color(255, 0, 100, 255) : Color(120, 120, 120, 255), name.c_str());
		
		offset += name_x + 6;
		return CursorInFrame(x, y, w, h) && GetAsyncKeyState(VK_LBUTTON) < 0;
	};

	for (int i = 0; i < tabs.size(); i++) {
		if (tab(tabs.at(i), Utils::x + 5 + offset, Utils::y + Utils::header_height, Utils::active_tab == i)) {
			Utils::active_tab = i;
		}
	}

	return Utils::active_tab;
}