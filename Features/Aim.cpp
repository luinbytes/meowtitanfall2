#include "Aim.hpp"
#include "Vars.hpp"
#include <math.h>
#include <cmath>

bool AimIsVisible(Vector Start, Vector End, CBaseEntity* Local, CBaseEntity* Entity)
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

int GetTargetEntity(CUserCmd* Cmd, CBaseEntity* LocalPlayer)
{
	int Target = -1;
	float BestTarget = {};
	float AimFoV = Vars::FieldOfView;
	Vector HitboxPosition;
	auto CameraPos = LocalPlayer->GetCameraPos();

	if (Vars::RageMode)
		AimFoV = 360.f;

	for (int i = 1; i < EntityList->GetHighestEntityIndex(); i++)
	{
		auto TargetEntity = EntityList->GetClientEntity(i);

		if (!TargetEntity || TargetEntity->GetLifeState() != LIFE_ALIVE || TargetEntity->GetTeam() == LocalPlayer->GetTeam())
			continue;

		if (TargetEntity->GetHealth() <= 0)
			continue;

		auto Tmp = *(int*)((uintptr_t)TargetEntity + 0x4b0);

		if (!Tmp)
			continue;

		auto Name = TargetEntity->GetSignifierName();

		if (Name[0] != 'p')
			continue;

		if (!TargetEntity->GetHitboxPosition(HitboxPosition, 3))
			continue;

		auto Visible = AimIsVisible(CameraPos, HitboxPosition, LocalPlayer, TargetEntity);

		if (Visible)
		{
			auto Angle = Math::CalcAngle(CameraPos, HitboxPosition);
			auto FoV = Math::CalcFov(Cmd->worldViewAngles, Angle);

			if (FoV < AimFoV)
			{
				AimFoV = BestTarget;
				Target = i;
			}
		}
	}

	return Target;
}

Vector CalculateAngleWithViewAngles(Vector source, Vector destination, Vector view_angles)
{
	Vector delta = source - destination;
	Vector angles;

	angles.x = RAD2DEG(std::atanf(delta.z / std::hypotf(delta.x, delta.y))) - view_angles.x;
	angles.y = RAD2DEG(std::atanf(delta.y / delta.x)) - view_angles.y;
	angles.z = 0.0f;

	if (delta.x >= 0.0)
	{
		angles.y += 180.0f;
	}

	return angles;
	//return ((destination - source) - view_angles).normalized();
}

void Aim::Run(CUserCmd* Cmd)
{
	if (!Vars::RageMode) {
		if (!GetAsyncKeyState(Vars::AimbotKey)) {
			Aim::CurrentTarget = -1;
			return;
		}
	}

	CBaseEntity* LocalPlayer = BaseEntity->GetLocalPlayer();

	if (!LocalPlayer || LocalPlayer->GetLifeState() != LIFE_ALIVE) {
		Aim::CurrentTarget = -1;
		return;
	}

	auto Camera = LocalPlayer->GetCameraPos();
	auto WeaponPunch = LocalPlayer->GetPunchAngles();

	auto TargetIndex = GetTargetEntity(Cmd, LocalPlayer);

	if (TargetIndex == -1) {
		Aim::CurrentTarget = -1;
		return;
	}

	Aim::CurrentTarget = TargetIndex;

	auto Target = EntityList->GetClientEntity(TargetIndex);

	//lol
	if (!Vars::TargetTitans) {
		if (Target->GetHealth() > 120) {
			Aim::CurrentTarget = -1;
			return;
		}
	}

	Vector AimPoint;
	if (!Target->GetHitboxPosition(AimPoint, 3)) {
		Aim::CurrentTarget = -1;
		return;
	}

	Vector AimAngle = CalculateAngleWithViewAngles(Camera, AimPoint, Cmd->worldViewAngles);
	AimAngle -= WeaponPunch;
	AimAngle = Math::ClampAngles(AimAngle);

	if (Vars::Smoothness > 0.0f && !Vars::RageMode) {
		AimAngle /= Vars::Smoothness;
	}

	if (Vars::RageMode && Vars::AutoShoot)
		Cmd->buttons |= IN_ATTACK;


	if (Vars::SilentAim)
		Cmd->attackangles += AimAngle;
	else
		Cmd->worldViewAngles += AimAngle;
}