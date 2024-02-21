#include "ESP.hpp"
#include "../Renderer/Renderer.hpp"
#include "Vars.hpp"
#include "Aim.hpp"

enum PlayerBones
{
	head = 12,
	neck = 9,
	right_shoulder = 39,
	right_elbow = 42,
	right_hand = 46,
	right_knee = 67,
	right_foot = 68,
	right_leg = 66,
	mid_spine = 6,
	pelvis = 5,
	left_shoulder = 15,
	left_elbow = 16,
	left_hand = 20,
	left_knee = 62,
	left_foot = 63,
	left_leg = 61
};

typedef float matrix_t[3][4];
const auto GetViewMatrix = []() -> float(*)[4]
{
	auto ViowRender = *(uintptr_t*)(ViewRenderPtr);

	if (!ViowRender)
		return nullptr;

	auto* const ViewMatrix = **(matrix_t**)(ViowRender + 0x12EF90);
	return ViewMatrix;
};

bool ESP::WorldToScreen(Vector Src, Vector& Dst)
{
	auto* const Matrix = GetViewMatrix();
	auto w = 0.0f;

	Dst[0] = Matrix[0][0] * Src[0] + Matrix[0][1] * Src[1] + Matrix[0][2] * Src[2] + Matrix[0][3];
	Dst[1] = Matrix[1][0] * Src[0] + Matrix[1][1] * Src[1] + Matrix[1][2] * Src[2] + Matrix[1][3];
	w = Matrix[3][0] * Src[0] + Matrix[3][1] * Src[1] + Matrix[3][2] * Src[2] + Matrix[3][3];

	if (w < 0.01f)
		return false;

	const auto InvW = 1.f / w;
	Dst[0] *= InvW;
	Dst[1] *= InvW;

	int ScreenWidth;
	int ScreenHeight;
	MatSystemSurface->GetScreenSize(&ScreenWidth, &ScreenHeight);

	auto x = ScreenWidth / 2.f;
	auto y = ScreenHeight / 2.f;

	x += .5f * Dst[0] * ScreenWidth + .5f;
	y -= .5f * Dst[1] * ScreenHeight + .5f;

	Dst[0] = x;
	Dst[1] = y;

	return true;
}

bool IsVisible(Vector Start, Vector End, CBaseEntity* Local, CBaseEntity* Entity)
{
	Ray_t RayData;
	RayData.Init(Start, End);

	trace_t Trace;
	memset(&Trace, 0, sizeof(trace_t));

	CTraceFilterCustom Filter;
	Filter.Entity = Local;
	EngineTrace->TraceRay(RayData, TRACE_MASK_SHOT, &Filter, &Trace);

	CBaseEntity* TraceEntity = (CBaseEntity*)Trace.m_pEnt;

	if (!TraceEntity || TraceEntity->GetHealth() <= 0 || TraceEntity == Local)
		return false;

	return TraceEntity == Entity;
}

void RenderBoundingBox(CBaseEntity* Local, CBaseEntity* Entity, float Dist, int EntityIndex)
{
	auto AbsOrigin = Entity->GetAbsOrigin();
	Vector vecMin = AbsOrigin + Entity->GetMins();
	Vector vecMax = AbsOrigin + Entity->GetMaxs();

	Vector points[] =
	{
		{ vecMin[0], vecMin[1], vecMin[2] },
		{ vecMin[0], vecMax[1], vecMin[2] },
		{ vecMax[0], vecMax[1], vecMin[2] },
		{ vecMax[0], vecMin[1], vecMin[2] },
		{ vecMax[0], vecMax[1], vecMax[2] },
		{ vecMin[0], vecMax[1], vecMax[2] },
		{ vecMin[0], vecMin[1], vecMax[2] },
		{ vecMax[0], vecMin[1], vecMax[2] },
	};

	Vector flb, brt, blb, frt, frb, brb, blt, flt;

	if (!ESP::WorldToScreen(points[3], flb) ||
		!ESP::WorldToScreen(points[0], blb) ||
		!ESP::WorldToScreen(points[2], frb) ||
		!ESP::WorldToScreen(points[6], blt) ||
		!ESP::WorldToScreen(points[5], brt) ||
		!ESP::WorldToScreen(points[4], frt) ||
		!ESP::WorldToScreen(points[1], brb) ||
		!ESP::WorldToScreen(points[7], flt))
		return;

	Vector screen[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb[0];
	float top = flb[1];
	float right = flb[0];
	float bottom = flb[1];

	for (int i = 0; i < 8; i++)
	{
		if (left > screen[i][0])
			left = screen[i][0];
		if (top < screen[i][1])
			top = screen[i][1];
		if (right < screen[i][0])
			right = screen[i][0];
		if (bottom > screen[i][1])
			bottom = screen[i][1];
	}

	float x = left,
		y = bottom,
		w = right - left,
		h = top - bottom;

	int t = 1;

	Vector Head;
	Entity->GetHitboxPosition(Head, 3);

	Vector Out;
	if (Vars::HeadDot && ESP::WorldToScreen(Head, Out))
	{
		Renderer::DrawRect(Out.x, Out.y, 3, 3, Color(255, 0, 0, 255));
	}

	auto PlayerVisible = IsVisible(Local->GetCameraPos(), Head, Local, Entity);
	Color VisibleColor = PlayerVisible ? Color(255, 255, 255, 255) : Color(255, 0, 0, 255);
	Color TargetColor = EntityIndex == Aim::CurrentTarget ? Color(255, 100, 100, 255) : Color(255, 255, 255, 255);
	//Box
	if (Vars::Box) {
		Renderer::DrawOutlinedRect(x, y, w, h, TargetColor);
		Renderer::DrawOutlinedRect(x - t, y - t, w + t * 2, h + t * 2, Color(0, 0, 0, 200));
		Renderer::DrawOutlinedRect(x + t, y + t, w - t * 2, h - t * 2, Color(0, 0, 0, 200));
	}

	// Health
	if (Vars::Health) {
		Renderer::DrawRect(x - 6, y - 1, 4, h + 2, Color(0, 0, 0, 120));

		float health = (float)((float)Entity->GetHealth() / (float)(Entity->GetMaxHealth()));
		float height = h * health;

		Color HealthColor = Color((int)(255 * (1.f - health)), (int)(255 * health), 0, 255);
		Renderer::DrawRect(x - 5, y + (h - height), 2, height, HealthColor);
	}

	// Name
	if (Vars::Name) {
		auto [width, height] = Renderer::GetTextSize(tahoma, Entity->GetName());

		Renderer::DrawString(tahoma, x + w / 2.f - width / 2.f, y - height + 2.f, TargetColor, Entity->GetName());
	}

	if (Vars::Flags) {
		int Offset = 0;
		auto AddFlags = [&](std::string label) -> void {
			auto [width, height] = Renderer::GetTextSize(tahoma, label);
			Renderer::DrawString(tahoma, x + w + 2, y - 2 + Offset, TargetColor, label.c_str());
			Offset += height / 2.f;
		};

		AddFlags(std::to_string((int)(Dist)) + "M");
		AddFlags(std::to_string(Entity->GetHealth()) + "/" + std::to_string(Entity->GetMaxHealth()) + "HP");
		float Dist = AbsOrigin.DistTo(Local->GetAbsOrigin());
		if (Entity->GetHealth() > 120) {
			AddFlags("TITAN");
		}
		AddFlags(PlayerVisible ? "Visible" : "Invisible");
	}
}

void ESP::Run()
{
	auto Local = BaseEntity->GetLocalPlayer();

	if (!Local || Local->GetLifeState() != LIFE_ALIVE || Local->GetHealth() <= 0)
		return;

	int w, h;
	MatSystemSurface->GetScreenSize(&w, &h);

	for (int i = 1; i < EntityList->GetHighestEntityIndex(); i++)
	{
		auto Entity = EntityList->GetClientEntity(i);

		if (!Entity || Entity->GetLifeState() != LIFE_ALIVE || Entity->GetHealth() <= 0 || Entity->GetTeam() == Local->GetTeam() || Entity == Local)
			continue;

		auto Tmp = *(int*)((uintptr_t)Entity + 0x4b0);

		if (!Tmp)
			continue;

		auto Name = Entity->GetSignifierName();

		if (Name[0] == 'p') //player
		{
			Vector AbsOrigin = Entity->GetAbsOrigin();
			float Dist = AbsOrigin.DistTo(Local->GetAbsOrigin()) / METERS_TO_INCHES;

			if (Dist > 250.f)
				continue;

			RenderBoundingBox(Local, Entity, Dist, i);
		}
	}
}