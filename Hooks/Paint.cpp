#include "Paint.hpp"
#include "../Renderer/Renderer.hpp"
#include "../Features/ESP.hpp"
#include "../Features/Vars.hpp"
#include "../Features/Menu/Framework.hpp"
#include "../Features/Menu/zgui/zgui.hh"

void line(int x, int y, int x2, int y2, zgui::color c) noexcept
{
	Renderer::DrawLine(x, y, x2, y2, Color(c.r, c.g, c.b, c.a));
	/* draw line using your renderer */
}

void rect(int x, int y, int x2, int y2, zgui::color c) noexcept
{
	Renderer::DrawOutlinedRect(x, y, x2, y2, Color(c.r, c.g, c.b, c.a));
	/* draw outlined rectangle using your renderer */
}

void filled_rect(int x, int y, int x2, int y2, zgui::color c) noexcept
{
	Renderer::DrawRect(x, y, x2, y2, Color(c.r, c.g, c.b, c.a));
	/* draw filled rectangle using your renderer */
}

void text(int x, int y, zgui::color color, int font, bool center, const char* text) noexcept
{
	if (center) {
		auto [width, height] = Renderer::GetTextSize(font, text);
		Renderer::DrawString(font, x - width / 2.f, y, Color(color.r, color.g, color.b, color.a), text);
	}
	else {
		Renderer::DrawString(font, x, y, Color(color.r, color.g, color.b, color.a), text);
	}
	/* draw text using your renderer */
}

void get_text_size(unsigned long font, const char* text, int& width, int& height) noexcept
{
	auto [x, y] = Renderer::GetTextSize(font, text);
	width = x;
	height = y;
	/* calculate text size here */
}

float get_frametime() noexcept
{
	return Globals->frametime;
	/* return frametime */
}


static bool MenuOpen = false;
Color Enabled(bool Variable) { return Variable ? Color(65, 255, 65, 255) : Color(255, 65, 65, 255); }

PaintTraverseFn OriginalPaintTraverse = nullptr;
__int64 __fastcall HookedPaintTraverse(void* thisptr, __int64 panel, bool paintPopups, bool allowForce)
{
	auto Ret = OriginalPaintTraverse(thisptr, panel, paintPopups, allowForce);
	
	auto SetKeyboardInputEnabled = [&](void* ptr, uint64_t panelId, bool enabled) -> void {
		Memory::CallVirtual<31, void>(ptr, panelId, enabled);
	};

	auto SetMouseInputEnabled = [&](void* ptr, uint64_t panelId, bool enabled) -> void {
		Memory::CallVirtual<32, void>(ptr, panelId, enabled);
	};

	static bool FOVCircle = false;

	if (panel == MatSystemSurface->GetEmbeddedPanel())
	{
		MatSystemSurface->PushMakeCurrent(panel, true);

		zgui::functions.draw_line = line;
		zgui::functions.draw_rect = rect;
		zgui::functions.draw_filled_rect = filled_rect;
		zgui::functions.draw_text = text;
		zgui::functions.get_text_size = get_text_size;
		zgui::functions.get_frametime = get_frametime;

		zgui::poll_input("Titanfall 2");

		SetMouseInputEnabled(thisptr, panel, !zgui::globals::window_ctx.opened);
		SetKeyboardInputEnabled(thisptr, panel, !zgui::globals::window_ctx.opened);

		if (zgui::begin_window("meow hack", { 500, 350 }, tahoma, zgui::zgui_window_flags_no_ontoggle_animation)) {
			zgui::begin_groupbox("aimbot", { (468 / 2.f) - 5, 311 });
			{
				zgui::checkbox("enable#aimbot", Vars::Aimbot);
				zgui::key_bind("aimbot key", Vars::AimbotKey);
				zgui::checkbox("silent aim", Vars::SilentAim);
				zgui::checkbox("target titans", Vars::TargetTitans);
				zgui::checkbox("show field of view", FOVCircle);
				zgui::slider_int("field of view", 0, 150, Vars::FieldOfView);
				zgui::slider_float("smoothness", 0.f, 50.f, Vars::Smoothness);
				zgui::dummy();
				zgui::checkbox("rage bot", Vars::RageMode);
				zgui::checkbox("auto shoot", Vars::AutoShoot);
			}
			zgui::end_groupbox();

			zgui::next_column(468 / 2.f, 0);

			zgui::begin_groupbox("visuals", { (468 / 2.f), 311 });
			{
				zgui::checkbox("enable#esp", Vars::ESP);
				zgui::checkbox("name", Vars::Name);
				zgui::checkbox("box", Vars::Box);
				zgui::checkbox("health", Vars::Health);
				zgui::checkbox("head dot", Vars::HeadDot);
				zgui::checkbox("flags", Vars::Flags);
			}
			zgui::end_groupbox();

			zgui::end_window();
		}

		/*auto [x, y] = Renderer::GetTextSize(tahoma, "Cock and ball torture");

		Renderer::DrawRect(10, 10, x, y, Color(255, 0, 0, 255));
		Renderer::DrawString(tahoma, 10, 10, Color(255, 255, 255, 255), "Cock and ball torture");

		int w, h;
		MatSystemSurface->GetScreenSize(&w, &h);

		Framework::Begin();
		{
			auto tab = Framework::AddTabs({ "aimbot", "visuals", "exploits" });
		}*/

		if (FOVCircle) {
			Renderer::DrawString(tahoma, 15, 15, Color(255, 0, 0, 255), "theres no fov circle yet LMFAO");
			//Renderer::circle	
		}

		if (ClientState->SignonState == SIGNONSTATE_FULL)
			ESP::Run();

		MatSystemSurface->PopMakeCurrent(panel);
	}

	return Ret;
}