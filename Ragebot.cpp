#include "stdafx.h"

CRageBot Ragebot;

static bool start = false;
static bool switch123 = false;

void CRageBot::FastLargeNormalise(Vector &angle)
{
	if ((Settings.GetSetting(Tab_Misc, Misc_Teleport_Enable)) && !(Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust)))
		return;

	if (!std::isfinite(angle.x)) { angle.x = 0; }
	if (!std::isfinite(angle.y)) { angle.y = 0; }

	while (angle.y <= -1800000) angle.y += 3600000;
	while (angle.y > 1800000) angle.y -= 3600000;
	while (angle.x <= -1800000) angle.x += 3600000;
	while (angle.x > 1800000) angle.x -= 3600000;

	while (angle.y <= -18000) angle.y += 36000;
	while (angle.y > 18000) angle.y -= 36000;
	while (angle.x <= -18000) angle.x += 36000;
	while (angle.x > 18000) angle.x -= 36000;

	while (angle.y <= -180) angle.y += 360;
	while (angle.y > 180) angle.y -= 360;
	while (angle.x <= -180) angle.x += 360;
	while (angle.x > 180) angle.x -= 360;

	if (angle.x > 89.0) angle.x = 89;
	if (angle.x < -89.0) angle.x = -89;
	if (angle.y < -180) angle.y = -179.999;
	if (angle.y > 180) angle.y = 179.999;

	angle.z = 0;
}
void CRageBot::FastLargeNormaliseMove(Vector &angle)
{
	if ((Settings.GetSetting(Tab_Misc, Misc_Teleport_Enable)) && !(Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust)))
		return;

	if (!std::isfinite(angle.x)) { angle.x = 0; }
	if (!std::isfinite(angle.y)) { angle.y = 0; }

	while (angle.y <= -1800000) angle.y += 3600000;
	while (angle.y > 1800000) angle.y -= 3600000;
	while (angle.x <= -1800000) angle.x += 3600000;
	while (angle.x > 1800000) angle.x -= 3600000;

	while (angle.y <= -18000) angle.y += 36000;
	while (angle.y > 18000) angle.y -= 36000;
	while (angle.x <= -18000) angle.x += 36000;
	while (angle.x > 18000) angle.x -= 36000;

	while (angle.y <= -180) angle.y += 360;
	while (angle.y > 180) angle.y -= 360;
	while (angle.x < -180) angle.x += 360;
	while (angle.x > 180) angle.x -= 360;

	if (angle.y < -180) angle.y = -179.999;
	if (angle.y > 180) angle.y = 179.999;

	angle.z = 0;
}

Vector GetHitboxPOS_Ent(CBaseEntity* Player, int HitboxID)
{
	if (!Player)
		return Vector(0, 0, 0);

	matrix3x4 matrix[128];
	if (!Player->SetupBones(matrix, 128, 0x00000100, GetTickCount64())) return Vector(0, 0, 0);
	const model_t* mod = Player->GetModel();
	if (!mod) return Vector(0, 0, 0);
	studiohdr_t* hdr = Interfaces.g_pModelInfo->GetStudioModel(mod);
	if (!hdr) return Vector(0, 0, 0);
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	if (!set) return Vector(0, 0, 0);
	mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
	if (!hitbox) return Vector(0, 0, 0);
	Vector vMin, vMax, vCenter, sCenter;
	g_Math.VectorTransform2(hitbox->bbmin, matrix[hitbox->bone], vMin);
	g_Math.VectorTransform2(hitbox->bbmax, matrix[hitbox->bone], vMax);
	vCenter = (vMin + vMax) *0.5f;
	return vCenter;
}

struct Hitbox
{
	Hitbox(void)
	{
		hitbox = -1;
	}

	Hitbox(int newHitBox)
	{
		hitbox = newHitBox;
	}

	int  hitbox;
	Vector points[9];
};
bool GetHitbox(CPlayer* Player, Hitbox *box)
{
	if (!Player)
		return false;
	if (!Player->Entity)
		return false;

	matrix3x4 matrix[128];
	if (!Player->Entity->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
		return false;
	const model_t* mod = Player->Entity->GetModel();
	if (!mod)
		return false;
	studiohdr_t* hdr = Interfaces.g_pModelInfo->GetStudioModel(mod);
	if (!hdr)
		return false;
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	if (!set)
		return false;
	mstudiobbox_t* hitbox = set->pHitbox(box->hitbox);
	if (!hitbox)
		return false;

	// center and all the corners of the hitbox hehe
	Vector points[] = { ((hitbox->bbmin + hitbox->bbmax) * .5f),
		Vector(hitbox->bbmin.x, hitbox->bbmin.y, hitbox->bbmin.z),
		Vector(hitbox->bbmin.x, hitbox->bbmax.y, hitbox->bbmin.z),
		Vector(hitbox->bbmax.x, hitbox->bbmax.y, hitbox->bbmin.z),
		Vector(hitbox->bbmax.x, hitbox->bbmin.y, hitbox->bbmin.z),
		Vector(hitbox->bbmax.x, hitbox->bbmax.y, hitbox->bbmax.z),
		Vector(hitbox->bbmin.x, hitbox->bbmax.y, hitbox->bbmax.z),
		Vector(hitbox->bbmin.x, hitbox->bbmin.y, hitbox->bbmax.z),
		Vector(hitbox->bbmax.x, hitbox->bbmin.y, hitbox->bbmax.z) };

	for (int index = 0; index <= 8; ++index)
	{
		g_Math.VectorTransform2(points[index], matrix[hitbox->bone], box->points[index]);
	}

	return true;
}
void dynamictimer() {
	float seconds = 0.12;
	static clock_t start_t = clock();
	double curTime = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (curTime >= seconds) {
		switch123 = !switch123;
	}
}
float DamageMultiplyer(int iHitGroup)
{//I think this is the right values, can't test bc no pc Skel test for me please! :) X :* 
	switch (iHitGroup)
	{
	case HITGROUP_GENERIC:
		return 0.5f;
	case HITGROUP_HEAD:
		return 2.0f;
	case HITGROUP_CHEST:
		return 0.5f;
	case HITGROUP_STOMACH:
		return 0.75f;
	case HITGROUP_LEFTARM:
		return 0.5f;
	case HITGROUP_RIGHTARM:
		return 0.5f;
	case HITGROUP_LEFTLEG:
		return 0.375f;
	case HITGROUP_RIGHTLEG:
		return 0.375f;
	case HITGROUP_GEAR:
		return 0.5f;
	default:
		return 1.0f;

	}

	return 1.0f;
}

bool bulletime() {
	float flServerTime = (float)Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->interval_per_tick;
	float flNextPrimaryAttack = Hacks.LocalWeapon->NextPrimaryAttack();

	bool BulletTime = true;

	if (flNextPrimaryAttack > flServerTime)
		BulletTime = false;
	return BulletTime;
}
Vector GetHitboxPOS(CPlayer* Player, int HitboxID)
{
	if (!Player)
		return Vector(0, 0, 0);
	if (!Player->Entity)
		return Vector(0, 0, 0);


	matrix3x4 matrix[128];
	if (!Player->Entity->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
		return Vector(0, 0, 0);
	const model_t* mod = Player->Entity->GetModel();
	if (!mod)
		return Vector(0, 0, 0);
	studiohdr_t* hdr = Interfaces.g_pModelInfo->GetStudioModel(mod);
	if (!hdr)
		return Vector(0, 0, 0);
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	if (!set)
		return Vector(0, 0, 0);
	mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
	if (!hitbox)
		return Vector(0, 0, 0);
	Vector
		vMin,
		vMax,
		vCenter,
		sCenter;

	g_Math.VectorTransform2(hitbox->bbmin, matrix[hitbox->bone], vMin);
	g_Math.VectorTransform2(hitbox->bbmax, matrix[hitbox->bone], vMax);
	vCenter = (vMin + vMax) *0.5f;
	int index = Player->Index;
	int autopsx = Settings.GetPlayer(index, Tab_Ragebot, Ragebot_AutoPSX);

	float pone = Settings.GetPlayer(index, Tab_Ragebot, Ragebot_PSX);

	if (Settings.GetSetting(Tab_Ragebot, Ragebot_Enable) && Player->Entity->GetTeam() != Hacks.LocalPlayer->GetTeam())
	{
		if (pone != 50) {
			float ptwoz = 50 - pone;
			float zval = (ptwoz*vMin.z + pone*vMax.z) / 50;
			vCenter.z = zval;
		}
	}
	return vCenter;
}
Vector GetHitboxPOSFull(CPlayer* Player, int HitboxID, float & bedamage)
{
	if (!Player)
		return Vector(0, 0, 0);
	if (!Player->Entity)
		return Vector(0, 0, 0);


	matrix3x4 matrix[128];
	if (!Player->Entity->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
		return Vector(0, 0, 0);
	const model_t* mod = Player->Entity->GetModel();
	if (!mod)
		return Vector(0, 0, 0);
	studiohdr_t* hdr = Interfaces.g_pModelInfo->GetStudioModel(mod);
	if (!hdr)
		return Vector(0, 0, 0);
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	if (!set)
		return Vector(0, 0, 0);
	mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
	if (!hitbox)
		return Vector(0, 0, 0);
	Vector
		vMin,
		vMax,
		vCenter,
		sCenter;

	g_Math.VectorTransform2(hitbox->bbmin, matrix[hitbox->bone], vMin);
	g_Math.VectorTransform2(hitbox->bbmax, matrix[hitbox->bone], vMax);
	vCenter = (vMin + vMax) *0.5f;
	int index = Player->Index;

	float pone = Settings.GetPlayer(index, Tab_Ragebot, Ragebot_PSX);
	int bestdamage = 0;

	if (Settings.GetSetting(Tab_Ragebot, Ragebot_Enable) && Player->Entity->GetTeam() != Hacks.LocalPlayer->GetTeam())
	{
		if (pone != 50) {
			float ptwoz = 50 - pone;
			float zval = (ptwoz*vMin.z + pone*vMax.z) / 50;
			vCenter.z = zval;
		}
		float damage = 0;
		Ragebot.AutoWall_ScanPos(vCenter, HitboxID, damage);
		bestdamage = damage;
		if (damage <= 0) {
			for (int i = 0; i < 5; i++) {
				Vector target = vCenter;
				switch (i) {
				case 4: target = vMin; break;
				case 1: target = vMax; break;
				case 2: target.x = vMin.x; target.y = vMax.y; break;
				case 3: target.y = vMin.y; target.x = vMax.x; break;
				case 0: target = (vMin + vMax)*0.5; break;

				}
				target.z = vCenter.z;
				Ragebot.AutoWall_ScanPos(target, HitboxID, damage);
				if (damage > bestdamage)
				{
					vCenter = target;
					bestdamage = damage;
				}

			}

			if (bestdamage <= 0) {
				for (int i = 0; i < 10; i++) {
					Vector target = vCenter;
					switch (i) {
					case 0: target = vMin; target.z = vMin.z;  break;
					case 1: target = vMax; target.z = vMin.z; break;
					case 2: target.x = vMin.x; target.y = vMax.y; target.z = vMin.z; break;
					case 3: target.y = vMin.y; target.x = vMax.x; target.z = vMin.z; break;
					case 4: target = (vMin + vMax)*0.5; target.z = vMin.z; break;

					case 5: target = vMin; target.z = vMax.z; break;
					case 6: target = vMax; target.z = vMax.z; break;
					case 7: target.x = vMin.x; target.y = vMax.y; target.z = vMax.z; break;
					case 8: target.y = vMin.y; target.x = vMax.x; target.z = vMax.z; break;
					case 9: target = (vMin + vMax)*0.5; target.z = vMax.z; break;

					}
					Ragebot.AutoWall_ScanPos(target, HitboxID, damage);
					if (damage > bestdamage)
					{
						vCenter = target;
						bestdamage = damage;
					}

				}

			}
		}
	}
	bedamage = bestdamage;
	return vCenter;
}

void ScaleDamage(int hitgroup, CBaseEntity *enemy, float weapon_armor_ratio, float &current_damage)
{
	current_damage *= DamageMultiplyer(hitgroup);

	if (enemy->GetArmor() > 0)
	{
		if (hitgroup == HITGROUP_HEAD)
		{
			if (enemy->HasHelmet())
				current_damage *= (weapon_armor_ratio);
		}
		else
		{
			current_damage *= (weapon_armor_ratio);
		}
	}
}
float RandomFloat(float flLow, float flHigh) {
	double min = flLow;
	double difference = flHigh - min;
	int randomiser = difference * 100000;
	float valuee = rand() % randomiser;
	difference += (valuee / 100000);
	return difference;
}
void CompansateSpread() {
	for (int i = 0; i < 256; ++i)
	{

		float fRand1 = RandomFloat(0.f, 1.f);
		float fRandPi1 = RandomFloat(0.f, 2.f * (float)PI);
		float fRand2 = RandomFloat(0.f, 1.f);
		float fRandPi2 = RandomFloat(0.f, 2.f * (float)PI);

		float fRandInaccuracy = fRand1 * Hacks.LocalWeapon->GetInaccuracy();

		float fRandSpread = fRand2 * i;

		float fSpreadX = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
		float fSpreadY = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;
		cout << i << " | " << fSpreadX << " | " << fSpreadY << endl;
	}

}
/*
bool CompansateSpread()
{
int iTracesHit = 0;

for (int i = 0; i < 256; ++i)
{
Vector vEnd;
//g_NoSpread.FixSpread(i, pCommand, pWeapon, vIn, vEnd);
g_Math.angleVectors(vEnd, vEnd);
vEnd = vEnd * pWeapon->GetCSWpnData()->fGetRange() + vStart;

trace_t pTrace; TraceLine(vStart, vEnd, 0x4600400B, pLocalBaseEntity, &pTrace);

if (pTrace.m_pEnt != pBaseEntity)
continue;

if (pTrace.hitgroup != iHitGroup)
continue;

++iTracesHit;
}

if (iTracesHit / 256.f >= fMinChance / 100.f)
return true;

return false;
}

*/

bool HandleBulletPen(CCSWeaponInfo *wpn_data, FireBulletData &data)
{

	surfacedata_t *enter_surface_data = Interfaces.pPhysProps->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;
	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow(wpn_data->GetRangeModifier(), (data.trace_length * 0.002));
	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))data.penetrate_count = 0;
	if (data.penetrate_count <= 0)return false;
	Vector dummy;
	trace_t trace_exit;
	if (!TraceToExit(dummy, data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit)) return false;
	surfacedata_t *exit_surface_data = Interfaces.pPhysProps->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;
	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;
	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71)) { combined_penetration_modifier = 3.0f; final_damage_modifier = 0.05f; }
	else { combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f; }
	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)combined_penetration_modifier = 2.0f;
	}
	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->GetPen()) * 1.25f);
	float thickness = VectorLength(trace_exit.endpos - data.enter_trace.endpos);
	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;
	float lost_damage = fmaxf(0.0f, v35 + thickness);
	if (lost_damage > data.current_damage)return false;
	if (lost_damage >= 0.0f)data.current_damage -= lost_damage;
	if (data.current_damage < 1.0f) return false;
	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool FireSimulatedBullet(FireBulletData &data, CBaseEntity *Local, CBaseCombatWeapon* weapon)
{
	data.penetrate_count = 4; // Max Amount Of Penitration
	data.trace_length = 0.0f; // wow what a meme
	CCSWeaponInfo* Weapon_Info = weapon->GetCSWpnData(); // Get Weapon Info
	data.current_damage = (float)Weapon_Info->GetDamage();// Set Damage Memes
	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = Weapon_Info->GetRange() - data.trace_length;
		Vector End_Point = data.src + data.direction * data.trace_length_remaining;
		UTIL_TraceLine(data.src, End_Point, 0x4600400B, Local, 0, &data.enter_trace);
		UTIL_ClipTraceToPlayers(data.src, End_Point * 40.f, 0x4600400B, &data.filter, &data.enter_trace);
		if (data.enter_trace.fraction == 1.0f) break;
		if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0) && (Local->GetTeam() != data.enter_trace.m_pEnt->GetTeam()))
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= pow(Weapon_Info->GetRangeModifier(), data.trace_length * 0.002);
			ScaleDamage(data.enter_trace.hitgroup, data.enter_trace.m_pEnt, Weapon_Info->GetArmorRatio(), data.current_damage);
			return true;
		}
		if (!HandleBulletPen(Weapon_Info, data)) break;
	}
	return false;
}

void FixHitbox(Hitbox* box)
{
	box->points[0].z += 10;
	for (int index = 0; index <= 8; ++index)
	{
		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;
		ray.Init(box->points[index], box->points[0]);
		Interfaces.pTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

		box->points[index] = tr.endpos;
	}
}
Vector pointaim = Vector(0, 0, 0);//
Vector CRageBot::GetBestPoint(CPlayer* Player, int HitboxID) {
	Vector aimpoint = GetHitboxPOS_Ent(Player->Entity, HitboxID);
	Vector point = GetHitboxPOS_Ent(Player->Entity, HitboxID);
	float maindamage = 0;
	int index = Player->Index;
	int autopsx = Settings.GetPlayer(index, Tab_Ragebot, Ragebot_AutoPSX);
	if (autopsx == 0 || autopsx == 1) {
		aimpoint = GetHitboxPOS(Player, HitboxID);
	}

	else if (autopsx == 2) {
		float damage = 0;
		float damage2 = 0;

		Vector point1 = GetHitboxPOSFull(Target, HitboxID, damage);
		Vector point2 = pointScanEdge(Target, HitboxID, damage2);

		if (damage2 > damage) {
			point1 = point2;
			damage = damage2;
		}
		if (damage > 0) {
			maindamage = damage;
			aimpoint = point1;

		}
		if (maindamage <= 0) {
			aimpoint = GetHitboxPOS(Player, HitboxID);
		}

	}

	else if (autopsx == 3) {
		float damage = 0;
		aimpoint = pointScanEdge(Player, HitboxID, damage);
		if (damage == 0) {
			aimpoint = GetHitboxPOS(Player, HitboxID);

		}

	}
	pointaim = aimpoint;

	return aimpoint;

}

Vector CRageBot::GetHitboxPOSBOXER(CPlayer* Player, int HitboxID)
{
	if (!Player)
		return Vector(0, 0, 0);
	if (!Player->Entity)
		return Vector(0, 0, 0);

	Vector best = Vector(0, 0, 0);
	Hitbox box = Hitbox(HitboxID);
	if (!GetHitbox(Player, &box))
		return best;
	FixHitbox(&box);

	float damage = 0;
	float bestdamage = 0;
	for (int index = 0; index <= 8; ++index)
	{
		FireBulletData Bullet_Data = FireBulletData(Hacks.LocalPlayer->GetEyePosition());
		Bullet_Data.filter.pSkip = Hacks.LocalPlayer;
		Vector Angle = Vector(0, 0, 0);
		CalcAngle(Bullet_Data.src, box.points[index], Angle);
		g_Math.angleVectors(Angle, Bullet_Data.direction);

		if (FireSimulatedBullet(Bullet_Data, Hacks.LocalPlayer, Hacks.LocalWeapon))
			damage = Bullet_Data.current_damage;
		else
			damage = -1;

		if (damage > bestdamage)
		{
			best = box.points[index];
			bestdamage = damage;
		}
	}
	return best;
}

Vector CRageBot::GetHitboxPOSScan(CPlayer* Player, int HitboxID, Vector ps)
{
	if (!Player)
		return Vector(0, 0, 0);
	if (!Player->Entity)
		return Vector(0, 0, 0);

	matrix3x4 matrix[128];
	if (!Player->Entity->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
		return Vector(0, 0, 0);
	const model_t* mod = Player->Entity->GetModel();
	if (!mod)
		return Vector(0, 0, 0);
	studiohdr_t* hdr = Interfaces.g_pModelInfo->GetStudioModel(mod);
	if (!hdr)
		return Vector(0, 0, 0);
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	if (!set)
		return Vector(0, 0, 0);
	mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
	if (!hitbox)
		return Vector(0, 0, 0);
	Vector
		vMin,
		vMax,
		vCenter,
		sCenter;

	g_Math.VectorTransform2(hitbox->bbmin, matrix[hitbox->bone], vMin);
	g_Math.VectorTransform2(hitbox->bbmax, matrix[hitbox->bone], vMax);
	vCenter = (vMin + vMax) *0.5f;
	{
		Vector idealps = Vector(50, 50, 50);

		if (ps.x == -1) {
			float damage = 0;
			float bestdamage = 0;//Target->Override_Multibox.Index
			for (int z = 0; z < 11; z++)
			{
				for (int x = 0; x < 3; x++)
				{
					for (int y = 0; y < 3; y++)
					{

						Vector currentps;
						currentps.z = 10 * z;
						currentps.x = 50 * x;
						currentps.y = 50 * y;

						AutoWall_ScanPs(Player, Target_Bone, currentps, damage);

						if (damage > bestdamage)
						{
							idealps = currentps;
							bestdamage = damage;
						}
					}
				}

			}
		}
		else {
			idealps = ps;
		}

		float ponez = idealps.z;
		float ptwoz = 50 - ponez;
		float zval = (ptwoz*vMin.z + ponez*vMax.z) / 50;
		vCenter.z = zval;

		float ponex = idealps.x;
		float ptwox = 50 - ponex;
		float xval = (ptwox*vMin.x + ponex*vMax.x) / 50;
		vCenter.x = xval;

		float poney = idealps.y;
		float ptwoy = 50 - poney;
		float yval = (ptwoy*vMin.y + ponez*vMax.y) / 50;
		vCenter.y = yval;

	}
	return vCenter;
}

float FovToPlayer(Vector ViewOffSet, Vector View, CPlayer* pEntity, int aHitBox)
{
	CONST FLOAT MaxDegrees = 180.0f;
	Vector Angles = View;
	Vector Origin = ViewOffSet;
	Vector Delta(0, 0, 0);
	Vector Forward(0, 0, 0);
	g_Math.angleVectors(Angles, Forward);
	Vector AimPos = GetHitboxPOS(pEntity, aHitBox);
	VectorSubtract(AimPos, Origin, Delta);
	g_Math.Normalize(Delta, Delta);
	FLOAT DotProduct = Forward.Dot(Delta);
	float FOV(acos(DotProduct) * (MaxDegrees / PI));
	if (FOV < 0) FOV - (FOV * 2);
	return FOV;
}

float FovToPlayer_Ent(Vector ViewOffSet, Vector View, CBaseEntity* pEntity, int aHitBox)
{
	CONST FLOAT MaxDegrees = 180.0f;
	Vector Angles = View;
	Vector Origin = ViewOffSet;
	Vector Delta(0, 0, 0);
	Vector Forward(0, 0, 0);
	g_Math.angleVectors(Angles, Forward);
	Vector AimPos = GetHitboxPOS_Ent(pEntity, aHitBox);
	VectorSubtract(AimPos, Origin, Delta);
	g_Math.Normalize(Delta, Delta);
	FLOAT DotProduct = Forward.Dot(Delta);
	float FOV(acos(DotProduct) * (MaxDegrees / PI));
	if (FOV < 0) FOV - (FOV * 2);
	return FOV;
}

void CalcAngle(Vector src, Vector dst, Vector &angles)
{
	Vector delta = src - dst;
	double hyp = delta.Length2D();
	angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angles.x = (atan(delta.z / hyp) * 57.295779513082f);
	if (delta.x >= 0.0) angles.y += 180.0f;
}
float distance(Vector source, Vector pos) {
	Vector dist;
	dist.x = abs(source.x - pos.x);
	dist.y = abs(source.y - pos.y);
	float xx = dist.x*dist.x + dist.y*dist.y;
	float distance = sqrt(xx);
	return distance;
}
float fovtoplayer(Vector source, Vector Target) {
	Vector ang;
	CalcAngle(source, Target, ang);
	float fov = abs(Hacks.CurrentCmd->viewangles.y - ang.y);
	return fov;
}
float hitchance() {
	float hitchance = 101;
	if (!Hacks.LocalWeapon) return 0;
	if (Settings.GetSetting(Tab_Ragebot, Ragebot_Hitchance_Amount) != 0) {//Inaccuracy method
		float inaccuracy = Hacks.LocalWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;

	}
	return hitchance;
}
extern int bullets;
bool CRageBot::smartaim() {
	if (Settings.GetSetting(Tab_Ragebot, Ragebot_SA) == 0)
		return false;

	bool should = false;
	static int bulletstart = bullets;
	static bool first = true;
	static CPlayer *currenttarget = Target;
	if (currenttarget != Target) {
		first = true;

	}
	if (Hacks.CurrentCmd->buttons & IN_ATTACK) {
		if (first) {
			bulletstart = bullets;
			first = false;
			currenttarget = Target;
		}
		if ((bullets - bulletstart) >= Settings.GetSetting(Tab_Ragebot, Ragebot_SA)) {
			should = true;
		}
	}
	else {
		first = true;
	}



	return should;
}
bool CRageBot::IsVisableExact(CPlayer* Target, int boneID)
{
	trace_t Trace;
	Vector start = Hacks.LocalPlayer->GetEyePosition();
	Vector end = GetHitboxPOS(Target, boneID);
	UTIL_TraceLine(start, end, MASK_SOLID, Hacks.LocalPlayer, 0, &Trace);
	if (Trace.fraction == 1.0f) return true;
	return false;
}

bool shouldauto = false;
bool CRageBot::pointshitable(CPlayer* Player) {
	float dmamage = 0;
	if (!Player->Entity)
		return false;
	AutoWall_ScanPos(GetHitboxPOS_Ent(Player->Entity, (int)CSGOHitboxID::Head), (int)CSGOHitboxID::Head, dmamage);
	if (dmamage > 0)
		return true;
	AutoWall_ScanPos(GetHitboxPOS_Ent(Player->Entity, (int)CSGOHitboxID::LeftFoot), (int)CSGOHitboxID::LeftFoot, dmamage);
	if (dmamage > 0)
		return true;
	AutoWall_ScanPos(GetHitboxPOS_Ent(Player->Entity, (int)CSGOHitboxID::RightFoot), (int)CSGOHitboxID::RightFoot, dmamage);
	if (dmamage > 0)
		return true;
	AutoWall_ScanPos(GetHitboxPOS_Ent(Player->Entity, (int)CSGOHitboxID::LeftHand), (int)CSGOHitboxID::LeftHand, dmamage);
	if (dmamage > 0)
		return true;
	AutoWall_ScanPos(GetHitboxPOS_Ent(Player->Entity, (int)CSGOHitboxID::RightHand), (int)CSGOHitboxID::RightHand, dmamage);
	if (dmamage > 0)
		return true;
	return false;
}
void CRageBot::NewMove() {
	CPlayer* Player;
	float disttoplayer = 100000000;
	Any_Targets = false;
	shouldauto = false;
	static int lastindex = 0;
	bool triedfirst = false;
	for (int i = 0; i < 99; i++)
	{
		if (!bulletime() && Settings.GetSetting(Tab_Ragebot, Ragebot_SaveFps))continue;
		if (Settings.GetSetting(Tab_Ragebot, Ragebot_SaveFps) && !triedfirst) i = lastindex;
		triedfirst = true;
		Player = Playerlist.Get_Player_By_Index(i);
		if (Player->Is_Empty) continue;
		if (!Player->Entity) continue;
		if (!Interfaces.pEntList->GetClientEntity(Player->Index)) continue;


		if (Ragebot_IsGoodPlayer(Player))
		{
			Vector aimpointpos = GetHitboxPOS(Player, 0);

			if (true)
			{
				Vector source = Hacks.LocalPlayer->GetEyePosition();
				Vector pos = Player->Entity->GetEyePosition();
				float tempdist = 10000;
				if (Settings.GetSetting(Tab_Ragebot, Ragebot_Selection) == 2) {
					tempdist = distance(source, pos);
				}
				else
				{
					tempdist = fovtoplayer(source, pos);
				}

				if (disttoplayer > tempdist)
				{
					if (!pointshitable(Player) && Settings.GetSetting(Tab_Ragebot, Ragebot_SaveFps))continue;
					bool newtarget = false;
					Expected_Damage_PerEnt = HitScan(Player, aimpointpos);
					float hp = Player->Entity->GetHealth();
					if (Target_Bone_PerEnt > -1 && (Expected_Damage_PerEnt >= (hp*(Settings.GetSetting(Tab_Ragebot, Ragebot_MinDam) / 100)))) {
						//This shit breaks shit but it fixes shit so shit is shit.
						if (IsPosVisable(aimpointpos) || IsVisable(Player, Target_Bone_PerEnt) || Expected_Damage_PerEnt > 0) {
							if (Settings.GetSetting(Tab_Ragebot, Ragebot_Hitchance_Amount) * 2 <= hitchance() || (Settings.GetSetting(Tab_Ragebot, Ragebot_Hitchance_Amount) == 0)) {
								Target = Player;
								disttoplayer = tempdist;
								Target_Bone = Target_Bone_PerEnt;
								Any_Targets = true;
								if (Target->priority)
									break;
								if (Settings.GetSetting(Tab_Ragebot, Ragebot_SaveFps)) {
									lastindex = i;
									break;
								}
							}
							else {
								shouldauto = true;
							}
						}
					}
				}

			}

		}
		if (Settings.GetSetting(Tab_Ragebot, Ragebot_SaveFps) && lastindex == i && triedfirst) {
			lastindex = 0;
			i = 0;
		}
	}
	if (shouldauto) {
		if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_AutoDuck)) {
			Hacks.CurrentCmd->buttons |= IN_DUCK;
		}

		/* Auto Scope */
		if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_Autoscope))
		{
			if (Hacks.LocalWeapon->isSniper())
			{
				if (!Hacks.LocalPlayer->m_bIsScoped())
				{
					Hacks.CurrentCmd->buttons &= ~IN_ATTACK;
					Hacks.CurrentCmd->buttons |= IN_ATTACK2;

				}
			}
		}
	}
	Aimbot_Angle = Hacks.CurrentCmd->viewangles;
	Antiaim_Angle = Hacks.CurrentCmd->viewangles;
	if (Hacks.LocalWeapon->IsNade())
		return;
	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_AntiAimKnife)) {
		if (Hacks.LocalWeapon->IsKnife() || (Hacks.LocalWeapon->IsMiscWeapon()))
			return;
	}
	if (!(Hacks.LocalWeapon->IsKnife() || (Hacks.LocalWeapon->IsMiscWeapon()))) {
		bool aimbotting = false;
		if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_Aimbot))
			aimbotting = DoNewAimbot();
		if (!aimbotting) {
			if (Settings.GetSetting(Tab_Ragebot, Ragebot_NoRecoil)) {
				auto punchAngles = Hacks.LocalPlayer->GetPunchAngle() * 2.0f;
				if (punchAngles.x != 0.0f || punchAngles.y != 0) {
					static Vector Last_Puch_Angle = Vector(0, 0, 0);
					Hacks.CurrentCmd->viewangles += Last_Puch_Angle;
					Hacks.CurrentCmd->viewangles -= punchAngles;
					Last_Puch_Angle = punchAngles;
				}
			}
		}
	}
	Aimbot_Angle = Hacks.CurrentCmd->viewangles;
	bool shouldshoot = true;

	if (Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount) != 0)
		Aimbot_Angle = DoAimSpeedLimit(LastAngle, Aimbot_Angle, shouldshoot);

	Hacks.CurrentCmd->viewangles = Aimbot_Angle;

	if (!Settings.GetBoolSetting(Tab_Ragebot, Ragebot_Silent_Aim))
		Interfaces.pEngine->SetViewAngles(Hacks.CurrentCmd->viewangles);


	Hacks.CurrentCmd->viewangles = Antiaim_Angle;

	DoAntiAim();
	if (!(Settings.GetBoolSetting(Tab_Ragebot, Ragebot_AntiAimKnife))) {
		if (Hacks.LocalWeapon->IsKnife() || (Hacks.LocalWeapon->IsMiscWeapon()))
			return;
	}
	Antiaim_Angle = Hacks.CurrentCmd->viewangles;
	bool noonecares = true;
	if (Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount) != 0)
		Antiaim_Angle = DoAimSpeedLimit(LastAngle, Antiaim_Angle, noonecares);

	Hacks.CurrentCmd->viewangles = Antiaim_Angle;

	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_pSilent)) Do_pSilent(Aimbot_Angle, Antiaim_Angle);
	else if (Hacks.CurrentCmd->buttons & IN_ATTACK) Hacks.CurrentCmd->viewangles = Aimbot_Angle;
	else Hacks.CurrentCmd->viewangles = Antiaim_Angle;

	if (Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount) != 0)
//	DoAimSpeedLimit();

	if (!shouldshoot)
		Hacks.CurrentCmd->buttons &= ~IN_ATTACK;

	LastAngle = Hacks.CurrentCmd->viewangles;
}












/*void CRageBot::Move()
{
CPlayer* Player;
Expected_Damage = Settings.GetSetting( Tab_Ragebot, Ragebot_MinDam);
float fovtoplayer = 300;
Any_Targets = false;

for (int i = 0; i < 99; i++)
{
Player = Playerlist.Get_Player_By_Index(i);

if (Player->Is_Empty) continue;
if (!Player->Entity) continue;
if (!Interfaces.pEntList->GetClientEntity(Player->Index)) continue;


if (Ragebot_IsGoodPlayer(Player))
{
Vector aimpointpos;
Expected_Damage_PerEnt = HitScan(Player, aimpointpos);

if (Settings.GetSetting(Tab_Ragebot, Ragebot_Selection) == 0 || Settings.GetSetting(Tab_Ragebot, Ragebot_Selection) == 1) {//damage

if (Target_Bone_PerEnt > -1)
{

if (Expected_Damage_PerEnt > Expected_Damage)
{
Target = Player;
Expected_Damage = Expected_Damage_PerEnt;
Target_Bone = Target_Bone_PerEnt;
Any_Targets = true;
if (Target->Priority.Enabled)
break;
}
}
}
else if (Settings.GetSetting(Tab_Ragebot, Ragebot_Selection) == 2) {//fov
if (Target_Bone_PerEnt > -1)
{//me->GetEyePosition(), Hacks.CurrentCmd->viewangles, Player, LeBestBone
Vector tempang;
tempang.y = (Hacks.LocalPlayer->GetEyePosition(), Hacks.CurrentCmd->viewangles, Player, Target_Bone_PerEnt);
if (fovtoplayer > tempang.y)
{
if (Expected_Damage_PerEnt > Expected_Damage){
if (Settings.GetSetting(Tab_Ragebot, Ragebot_MinDam) == 0){
if (IsVisable(Player, Target_Bone_PerEnt)) {

Target = Player;
fovtoplayer = tempang.y;
Target_Bone = Target_Bone_PerEnt;
Any_Targets = true;
if (Target->Priority.Enabled)
break;
}
}else{
Target = Player;
fovtoplayer = tempang.y;
Target_Bone = Target_Bone_PerEnt;
Any_Targets = true;
if (Target->Priority.Enabled)
break;

}
}
}
}

}
else if (Settings.GetSetting(Tab_Ragebot, Ragebot_Selection) == 3) {//distance - not done
if (Target_Bone_PerEnt > -1)
{//me->GetEyePosition(), Hacks.CurrentCmd->viewangles, Player, LeBestBone
Vector tempang;
tempang.y = (Hacks.LocalPlayer->GetEyePosition(), Hacks.CurrentCmd->viewangles, Player, Target_Bone_PerEnt);
if (fovtoplayer > tempang.y)
{
if (Expected_Damage_PerEnt > Expected_Damage) {
if (Settings.GetSetting(Tab_Ragebot, Ragebot_MinDam) == 0) {
if (IsVisable(Player, Target_Bone_PerEnt)) {

Target = Player;
fovtoplayer = tempang.y;
Target_Bone = Target_Bone_PerEnt;
Any_Targets = true;
if (Target->Priority.Enabled)
break;
}
}
else {
Target = Player;
fovtoplayer = tempang.y;
Target_Bone = Target_Bone_PerEnt;
Any_Targets = true;
if (Target->Priority.Enabled)
break;

}
}
}
}
}
}

}
Aimbot_Angle = Hacks.CurrentCmd->viewangles;
Antiaim_Angle = Hacks.CurrentCmd->viewangles;

if (Hacks.LocalWeapon->IsMiscWeapon() || Hacks.LocalWeapon->IsKnife())
return;

if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_Aimbot))
DoAimbot();

if (!Settings.GetBoolSetting(Tab_Ragebot, Ragebot_Silent_Aim))
Interfaces.pEngine->SetViewAngles(Hacks.CurrentCmd->viewangles);
Aimbot_Angle = Hacks.CurrentCmd->viewangles;

Hacks.CurrentCmd->viewangles = Antiaim_Angle;

DoAntiAim();
Antiaim_Angle = Hacks.CurrentCmd->viewangles;

if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_pSilent)) Do_pSilent(Aimbot_Angle, Antiaim_Angle);
else if (Hacks.CurrentCmd->buttons & IN_ATTACK) Hacks.CurrentCmd->viewangles = Aimbot_Angle;
else Hacks.CurrentCmd->viewangles = Antiaim_Angle;


LastAngle = Hacks.CurrentCmd->viewangles;
}*/
extern bool infakelag;
void CRageBot::Do_pSilent(Vector AimbotAng, Vector Antiaimang)
{

	float flServerTime = (float)Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->interval_per_tick;
	float flNextPrimaryAttack = Hacks.LocalWeapon->NextPrimaryAttack();

	bool BulletTime = true;

	if (flNextPrimaryAttack > flServerTime)
		BulletTime = false;

	if (Hacks.CurrentCmd->buttons & IN_ATTACK && BulletTime && !CM_Hacks.UnLagNextTick)
	{
		Hacks.CurrentCmd->viewangles = AimbotAng;

		Hacks.SendPacket = false;
		CM_Hacks.UnLagNextTick = true;
	}
	else
	{
		Hacks.CurrentCmd->buttons &= ~IN_ATTACK;
		if (CM_Hacks.UnLagNextTick)
		{
			CM_Hacks.UnLagNextTick = false;

			Hacks.SendPacket = true;
		}
		Hacks.CurrentCmd->viewangles = Antiaimang;
	}
}

Vector CRageBot::DoAimSpeedLimit(Vector Last, Vector angle, bool &shoot)
{
	shoot = true;
	//X Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit)
	if (Last.x < angle.x - Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount)) {
		angle.x = Last.x + Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount);
		shoot = false;
	}
	else if (Last.x > angle.x + Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount)) {
		angle.x = Last.x - Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount);
		shoot = false;

	}
	//Y
	if (Last.y < (angle.y - Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount))) {
		angle.y = Last.y + Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount);
		shoot = false;
	}
	else if (Last.y >(angle.y + Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount))) {
		angle.y = Last.y - Settings.GetSetting(Tab_Ragebot, Ragebot_Speed_Limit_Amount);
		shoot = false;

	}

	return angle;
}
bool CRageBot::DoNewAimbot()
{
	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_On_Key_Press) && !(GetAsyncKeyState(Settings.GetSetting(Tab_Ragebot, Rabebot_On_Key_Button))))
	{
		return false;
	}

	if (!Any_Targets) return false;
	if (Target->Is_Empty) return false;
	Vector dst = GetHitboxPOS(Target, Target_Bone);
	dst = GetBestPoint(Target, Target_Bone);


	CalcAngle(Hacks.LocalPlayer->GetEyePosition(),
		dst,
		Hacks.CurrentCmd->viewangles);

	FastLargeNormalise(Hacks.CurrentCmd->viewangles);

	/* Recoil Controll */
	auto punchAngles = Hacks.LocalPlayer->GetPunchAngle() * 2.0f;
	if (punchAngles.x != 0.0f || punchAngles.y != 0)
	{
		Hacks.CurrentCmd->viewangles -= punchAngles;
	}
	if (Settings.GetSetting(Tab_Ragebot, Ragebot_AutoShoot)) {
		Hacks.CurrentCmd->buttons |= IN_ATTACK;
	}
	/* Auto Reload */
	if (Hacks.LocalWeapon->ammo() == 0)
	{
		Hacks.CurrentCmd->buttons &= ~IN_ATTACK;
		Hacks.CurrentCmd->buttons |= IN_RELOAD;
	}

	/* Auto Duck */
	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_AutoDuck)
		&& (Hacks.CurrentCmd->buttons & IN_ATTACK))
		Hacks.CurrentCmd->buttons |= IN_DUCK;


	/* Auto Scope */
	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_Autoscope))
	{
		if (Hacks.LocalWeapon->isSniper())
		{
			if (!Hacks.LocalPlayer->m_bIsScoped())
			{
				if (Hacks.CurrentCmd->buttons & IN_ATTACK)
				{
					Hacks.CurrentCmd->buttons &= ~IN_ATTACK;
					Hacks.CurrentCmd->buttons |= IN_ATTACK2;
				}
			}
		}
	}
	return true;




}

void CRageBot::DoAimbot()
{
	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_On_Key_Press)
		&& !(GetAsyncKeyState(Settings.GetBoolSetting(Tab_Ragebot, Rabebot_On_Key_Button))))
	{
		return;
	}

	if (!Any_Targets) return;
	if (Target->Is_Empty) return;
	int index = Target->Index;
	Vector dst = GetHitboxPOS(Target, Target_Bone);
	if (Settings.GetPlayer(index, Tab_Ragebot, Ragebot_AutoPSX) == 2)
		dst = GetHitboxPOSScan(Target, Target_Bone, Vector(-1, 50, 50));
	else if (Settings.GetPlayer(index, Tab_Ragebot, Ragebot_AutoPSX) == 3)
		dst = GetHitboxPOSBOXER(Target, Target_Bone);

	if (dst == Vector(0, 0, 0))
		return;

	CalcAngle(Hacks.LocalPlayer->GetEyePosition(),
		dst,
		Hacks.CurrentCmd->viewangles);

	FastLargeNormalise(Hacks.CurrentCmd->viewangles);



	Hacks.CurrentCmd->buttons |= IN_ATTACK;

	/* Auto Reload */
	if (Hacks.LocalWeapon->ammo() == 0)
	{
		Hacks.CurrentCmd->buttons &= ~IN_ATTACK;
		Hacks.CurrentCmd->buttons |= IN_RELOAD;
	}

	/* Auto Duck */
	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_AutoDuck)
		&& ((Hacks.CurrentCmd->buttons & IN_ATTACK)))
		if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_AutoDuck)) {
			Hacks.CurrentCmd->buttons |= IN_DUCK;
		}


	/* Auto Scope */
	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_Autoscope))
	{
		if (Hacks.LocalWeapon->isSniper())
		{
			if (!Hacks.LocalPlayer->m_bIsScoped())
			{
				if (Hacks.CurrentCmd->buttons & IN_ATTACK)
				{
					Hacks.CurrentCmd->buttons &= ~IN_ATTACK;
					Hacks.CurrentCmd->buttons |= IN_ATTACK2;
				}
			}
		}
	}





}

bool CRageBot::IsVisable(CPlayer* Target, int boneID)
{
	trace_t Trace;
	Vector start = Hacks.LocalPlayer->GetEyePosition();
	Vector end = GetHitboxPOS(Target, boneID);
	UTIL_TraceLine(start, end, MASK_SOLID, Hacks.LocalPlayer, 0, &Trace);
	if (Trace.m_pEnt == Target->Entity) return true;
	if (Trace.fraction == 1.0f) return true;
	return false;
}
bool CRageBot::IsPosVisable(Vector Target)
{
	trace_t Trace;
	Vector start = Hacks.LocalPlayer->GetEyePosition();
	Vector end = Target;
	UTIL_TraceLine(start, end, MASK_SOLID, Hacks.LocalPlayer, 0, &Trace);
	//if (Trace.m_pEnt == Target->Entity) return true;
	if (Trace.fraction == 1.0f) return true;
	return false;
}

void CRageBot::AutoWall_ScanPs(CPlayer* Target, int bone_ID, Vector ps, float& Damage)
{
	Vector Target_Point = GetHitboxPOSScan(Target, bone_ID, ps);
	FireBulletData Bullet_Data = FireBulletData(Hacks.LocalPlayer->GetEyePosition());
	Bullet_Data.filter.pSkip = Hacks.LocalPlayer;
	Vector Angle = Vector(0, 0, 0);
	CalcAngle(Bullet_Data.src, Target_Point, Angle);
	g_Math.angleVectors(Angle, Bullet_Data.direction);

	if (FireSimulatedBullet(Bullet_Data, Hacks.LocalPlayer, Hacks.LocalWeapon))
		Damage = Bullet_Data.current_damage;
	else
		Damage = -1;
}

void CRageBot::AutoWall_Scan(CPlayer* Target, int bone_ID, float& Damage)
{
	Vector Target_Point = GetHitboxPOS(Target, bone_ID);

	FireBulletData Bullet_Data = FireBulletData(Hacks.LocalPlayer->GetEyePosition());
	Bullet_Data.filter.pSkip = Hacks.LocalPlayer;
	Vector Angle = Vector(0, 0, 0);
	CalcAngle(Bullet_Data.src, Target_Point, Angle);
	g_Math.angleVectors(Angle, Bullet_Data.direction);

	if (FireSimulatedBullet(Bullet_Data, Hacks.LocalPlayer, Hacks.LocalWeapon))
		Damage = Bullet_Data.current_damage;
	else
		Damage = -1;
}

void CRageBot::AutoWall_ScanPos(Vector Target, int bone_ID, float& Damage)
{
	Vector Target_Point = Target;

	FireBulletData Bullet_Data = FireBulletData(Hacks.LocalPlayer->GetEyePosition());
	Bullet_Data.filter.pSkip = Hacks.LocalPlayer;
	Vector Angle = Vector(0, 0, 0);
	CalcAngle(Bullet_Data.src, Target_Point, Angle);
	g_Math.angleVectors(Angle, Bullet_Data.direction);

	if (FireSimulatedBullet(Bullet_Data, Hacks.LocalPlayer, Hacks.LocalWeapon))
		Damage = Bullet_Data.current_damage;
	else
		Damage = -1;
}

/*
Vector CRageBot::MultiBoxScan(CPlayer* Target, float& Damage) {
float damage = 0;
float bestdamage = 0;//Target->Override_Multibox.Index
float idealps = 50;
for (int z = 0; z < 11; z++)
{
for (int x = 0; x < 3; x++)
{
for (int y = 0; y < 3; y++)
{


float currentpsz = 10 * z;
float currentpsx = 50 * x;
float currentpsy = 50 * y;

AutoWall_ScanPs(Target, Target_Bone, currentpsx, damage);

if (damage > bestdamage)
{
idealps = currentpsx;
bestdamage = damage;
}
}
}

}

}
*/
Vector differHeadpoints[] = { Vector(0,0,0), Vector(0,0,1.5), Vector(0,0,3),Vector(0,0,4), Vector(0,0,-2), Vector(0,0,-4), Vector(0,0,4.8), Vector(0,0,5), Vector(0,0,5.4), Vector(0,3,0), Vector(3,0,0),Vector(-3,0,0), Vector(0,-3,0), Vector(0,2,4.2), Vector(0,-2,4.2), Vector(2,0,4.2), Vector(-2,0,4.2),  Vector(3.8,0,0), Vector(-3.8,0,0),Vector(0,3.6,0), Vector(0,-3.6,0),  Vector(0,0,0),Vector(0,0,0) };
Vector differChestpoints[] = { Vector(0,0,0), Vector(4,0,0), Vector(-4,0,0), Vector(0,-4,0), Vector(0,4,0) };
Vector differPelvispoints[] = { Vector(0,0,0), Vector(4,0,0), Vector(-4,0,0), Vector(0,-4,0), Vector(0,4,0) };
Vector differChestpointsDak[] = { Vector(0,0,0), Vector(2.5,2.5,0), Vector(-2.5,2.5,0), Vector(2.5,-2.5,0), Vector(-2.5,-2.5,0) };
Vector differPelvispointsDak[] = { Vector(0,0,0), Vector(2.5,2.5,0), Vector(-2.5,2.5,0), Vector(2.5,-2.5,0), Vector(-2.5,2.5,0) };
Vector differHeadpointsDak[] = { Vector(0,0,-4) , Vector(0,0,-2), Vector(0,0,0), Vector(0,0,1.5), Vector(0,0,3), };

int hitboxess[] = { (int)CSGOHitboxID::Head,(int)CSGOHitboxID::Neck, (int)CSGOHitboxID::Chest,(int)CSGOHitboxID::Pelvis,(int)CSGOHitboxID::LeftUpperArm,(int)CSGOHitboxID::RightUpperArm,(int)CSGOHitboxID::LeftThigh,	(int)CSGOHitboxID::RightThigh, (int)CSGOHitboxID::RightFoot, (int)CSGOHitboxID::LeftFoot,(int)CSGOHitboxID::RightHand,(int)CSGOHitboxID::LeftHand, (int)CSGOHitboxID::Head , (int)CSGOHitboxID::Head ,(int)CSGOHitboxID::Head , (int)CSGOHitboxID::Head };
Vector CRageBot::pointScanEdge(CPlayer* Player, int HitboxID, float &damage) {
	Vector aimpoint = Vector(0, 0, 0);
	Vector point = GetHitboxPOS_Ent(Player->Entity, HitboxID);
	float maindamage = 0;
	int repeats = 3;
	if (HitboxID == (int)CSGOHitboxID::Head)
		repeats = 22;
	if (HitboxID == (int)CSGOHitboxID::Chest)
		repeats = 4;
	if (HitboxID == (int)CSGOHitboxID::Pelvis)
		repeats = 4;
	Vector staticadd = Vector(0, 0, 0);
	for (int i = 0; i <= repeats; i++) {
		Vector addtion = aimpoint;
		int index = Player->Index;
		if (HitboxID == (int)CSGOHitboxID::Head)
			addtion = differHeadpoints[i];
		if (HitboxID == (int)CSGOHitboxID::Chest)
			addtion = differChestpoints[i];
		if (HitboxID == (int)CSGOHitboxID::Pelvis)
			addtion = differPelvispoints[i];


		Vector testpoint = point;
		testpoint = point + addtion;
		float indamage = 0;
		AutoWall_ScanPos(testpoint, HitboxID, indamage);
		if (maindamage < indamage) {
			aimpoint = testpoint;
			maindamage = indamage;
			damage = indamage;
			staticadd = addtion;
		}
	}
	if ((int)CSGOHitboxID::Head == HitboxID) {
		float chestaim = 0;
		AutoWall_Scan(Player, (int)CSGOHitboxID::UpperChest, chestaim);
		damage = maindamage;

		if ((int)maindamage >= (int)chestaim - 2 && (int)maindamage <= (int)chestaim + 2)
			maindamage = 0;
		if (maindamage <= 50 && Settings.GetSetting(Tab_Ragebot, Ragebot_Multibox) == 1 || maindamage <= 50 && Settings.GetSetting(Tab_Ragebot, Ragebot_Multibox) == 0) {
			maindamage = 0;
		}

	}
	if (maindamage <= 0) {
		damage = 0;
		aimpoint = GetHitboxPOS(Player, HitboxID);
	}
	return aimpoint;

}
Vector CRageBot::pointScanEdge2(CPlayer* Player, int HitboxID, float &damage) {
	Vector aimpoint = Vector(0, 0, 0);
	Vector point = GetHitboxPOS_Ent(Player->Entity, HitboxID);
	float maindamage = 0;
	int repeats = 3;
	if (HitboxID == (int)CSGOHitboxID::Head)
		repeats = 9;
	if (HitboxID == (int)CSGOHitboxID::Chest)
		repeats = 4;
	if (HitboxID == (int)CSGOHitboxID::Pelvis)
		repeats = 4;

	for (int i = 0; i <= repeats; i++) {
		Vector addtion = aimpoint;
		int index = Player->Index;
		if (Settings.GetPlayer(index, Tab_Ragebot, Ragebot_Multibox) == 5 && Player->Entity->GetVecVelocity().Length2D() != 0) {
			if (HitboxID == (int)CSGOHitboxID::Head) {
				addtion = differHeadpointsDak[i];
				repeats = 4;
			}
			if (HitboxID == (int)CSGOHitboxID::Chest)
				addtion = differChestpointsDak[i];
			if (HitboxID == (int)CSGOHitboxID::Pelvis)
				addtion = differPelvispointsDak[i];

		}
		else
		{
			if (HitboxID == (int)CSGOHitboxID::Head)
				addtion = differHeadpoints[i];
			if (HitboxID == (int)CSGOHitboxID::Chest)
				addtion = differChestpoints[i];
			if (HitboxID == (int)CSGOHitboxID::Pelvis)
				addtion = differPelvispoints[i];
		}
		Vector testpoint = point;
		testpoint = point + addtion;
		float indamage = 0;
		AutoWall_ScanPos(testpoint, HitboxID, indamage);
		if (maindamage < indamage) {
			aimpoint = testpoint;
			maindamage = indamage;
			damage = indamage;
		}
	}
	return aimpoint;

}

int CRageBot::HitScan(CPlayer* Target, Vector &point)
{
	point = Vector(0, 0, 0);
	std::vector<int> HitBoxesToScan;
	float damage = 0;
	int index = Target->Index;
	float bestdamage = 0;//Target->Override_Multibox.Index
	int switchval = Settings.GetPlayer(index, Tab_Ragebot, Ragebot_Multibox);
	int value = Settings.GetPlayer(index, Tab_Ragebot, Ragebot_Bone);
	int hitboxess[] = { (int)CSGOHitboxID::Head,(int)CSGOHitboxID::Neck, (int)CSGOHitboxID::Chest,(int)CSGOHitboxID::Pelvis,(int)CSGOHitboxID::LeftUpperArm,(int)CSGOHitboxID::RightUpperArm,(int)CSGOHitboxID::LeftThigh,	(int)CSGOHitboxID::RightThigh, (int)CSGOHitboxID::RightFoot, (int)CSGOHitboxID::LeftFoot,(int)CSGOHitboxID::RightHand,(int)CSGOHitboxID::LeftHand, (int)CSGOHitboxID::Stomach, (int)CSGOHitboxID::NeckLower , (int)CSGOHitboxID::LowerChest ,(int)CSGOHitboxID::UpperChest , (int)CSGOHitboxID::RightShin,  (int)CSGOHitboxID::LeftShin,(int)CSGOHitboxID::RightLowerArm, (int)CSGOHitboxID::LeftLowerArm , };
	int scantimes = 5;
	int hp = Target->Entity->GetHealth();

	int healthper = (hp*(Settings.GetSetting(Tab_Ragebot, Ragebot_MinDam)) / 100);
	int speed = Settings.GetPlayer(index, Tab_Ragebot, Ragebot_MultiboxSpeed);
	if (speed == 1 || speed == 0) {
		scantimes = 7;
	}
	if (speed == 2)
		scantimes = 4;
	if (speed == 3)
		scantimes = 13;
	if (speed == 4)
		scantimes = 20;

	bool sa = smartaim();
	if (sa) {
		float damage = 0;
		point = pointScanEdge(Target, (int)CSGOHitboxID::Pelvis, damage);
		if (damage > 0) {
			Target_Bone_PerEnt = (int)CSGOHitboxID::Pelvis;
			bestdamage = damage;
		}
		else {
			sa = false;
		}

	}
	if (sa) {
		switchval == 90;
	}
	switch (switchval) {
	case 0:
	case 1:
	{
		int hboxid = (int)CSGOHitboxID::Head;
		switch (value) {
		case 0:
		{
			hboxid = (int)CSGOHitboxID::Head;
		}
		break;
		case 1:
			hboxid = (int)CSGOHitboxID::Head;
			break;

		case 2:
			hboxid = (int)CSGOHitboxID::Neck;
			break;

		case 3:
			hboxid = (int)CSGOHitboxID::Chest;
			break;

		case 4:
			hboxid = (int)CSGOHitboxID::Pelvis;
			break;

		}
		if (speed == 4 && hboxid != (int)CSGOHitboxID::Neck) {
			point = pointScanEdge(Target, hboxid, damage);
			if (damage <= 0)
				AutoWall_ScanPos(GetHitboxPOS(Target, hboxid), hboxid, damage);
			Target_Bone_PerEnt = hboxid;
			if (damage <= 0 && IsVisable(Target, hboxid)) {
				damage = 4;
			}

		}
		else if (speed == 3 && hboxid != (int)CSGOHitboxID::Neck) {
			point = pointScanEdge2(Target, hboxid, damage);
			Target_Bone_PerEnt = hboxid;

		}
		else {
			Target_Bone_PerEnt = hboxid;
			AutoWall_Scan(Target, hboxid, damage);
		}
		bestdamage = damage;
		Target_Bone_PerEnt = hboxid;


	}
	break;
	case 2://Priority first
	{
		int hboxid = (int)CSGOHitboxID::Head;
		switch (value) {
		case 0:
			hboxid = (int)CSGOHitboxID::Head;
			break;
		case 1:
			hboxid = (int)CSGOHitboxID::Head;
			break;

		case 2:
			hboxid = (int)CSGOHitboxID::Neck;
			break;

		case 3:
			hboxid = (int)CSGOHitboxID::Chest;
			break;

		case 4:
			hboxid = (int)CSGOHitboxID::Pelvis;
			break;

		}
		Target_Bone_PerEnt = hboxid;
		point = pointScanEdge(Target, Target_Bone_PerEnt, damage);

		if (damage > 0) {
			bestdamage = damage;
		}

		else {

			for (int i = 0; i < scantimes; i++)
			{
				AutoWall_Scan(Target, hitboxess[i], damage);

				if (damage > bestdamage)
				{
					Target_Bone_PerEnt = hitboxess[i];
					bestdamage = damage;
				}
			}
			if (bestdamage <= 0 || bestdamage < healthper) {
				point = GetHitboxPOSFull(Target, (int)CSGOHitboxID::Pelvis, damage);
				if (damage > 0 && damage >= healthper) {
					Target_Bone_PerEnt = (int)CSGOHitboxID::Pelvis;
					bestdamage = damage;

				}

			}
		}

	}

	break;
Dynamic:
	case 3://Velocity
	{
		if (!Hacks.LocalWeapon)break;
		float spreadd = 1 / Hacks.LocalWeapon->GetInaccuracy();
		float spread = Hacks.LocalWeapon->GetInaccuracy();
		float velocity = Target->Entity->GetVecVelocity().Length2D();
		if (Hacks.LocalWeapon->isSniper()) {
			if (velocity >= 150) {
				if (spread <= 0.0025) {
					for (int i = 0; i < scantimes; i++)
					{

						AutoWall_Scan(Target, hitboxess[i], damage);
						float hp = Target->Entity->GetHealth();
						if (damage > bestdamage && (damage >= hp * 60 / 100))
						{
							Target_Bone_PerEnt = hitboxess[i];
							bestdamage = damage;
						}
					}
				}

			}
			else
			{
				AutoWall_Scan(Target, (int)CSGOHitboxID::Pelvis, damage);
				if (damage > 0) {
					Target_Bone_PerEnt = (int)CSGOHitboxID::Pelvis;
					bestdamage = damage;

				}

				if (bestdamage <= 0) {
					point = pointScanEdge(Target, (int)CSGOHitboxID::Pelvis, damage);
					if (damage > 0) {
						Target_Bone_PerEnt = (int)CSGOHitboxID::Pelvis;
						bestdamage = damage;

					}

					if (bestdamage <= 0) {
						point = pointScanEdge(Target, (int)CSGOHitboxID::Chest, damage);
						if (damage > 0) {
							Target_Bone_PerEnt = (int)CSGOHitboxID::Chest;
							bestdamage = damage;

						}

						if (bestdamage <= 0) {

							for (int i = 2; i < scantimes; i++)
							{
								AutoWall_Scan(Target, hitboxess[i], damage);
								if (hitboxess[i] == (int)CSGOHitboxID::NeckLower)
									damage = 0;

								if (damage > bestdamage)
								{
									Target_Bone_PerEnt = hitboxess[i];
									bestdamage = damage;
								}
							}
							if (bestdamage <= 0) {
								for (int i = 0; i < 3; i++)
								{
									AutoWall_Scan(Target, i, damage);

									if (damage > bestdamage)
									{
										Target_Bone_PerEnt = i;
										bestdamage = damage;
									}


								}
								if (bestdamage <= 0) {
									point = pointScanEdge(Target, 0, damage);
									if (damage > 0)
									{
										Target_Bone_PerEnt = 0;
										bestdamage = damage;
									}
								}

							}
						}
					}
				}
			}
		}
		else if ((int)Hacks.LocalWeapon->GetItemDefinitionIndex() == (int)WEAPON_DUALBERETTA) {
			if (spreadd > 55) {
				for (int i = 2; i > 0; i--)
				{

					AutoWall_Scan(Target, hitboxess[i], damage);
					float hp = Target->Entity->GetHealth();
					if (damage > bestdamage)
					{
						Target_Bone_PerEnt = hitboxess[i];
						bestdamage = damage;
					}
				}


			}

			if (bestdamage <= 0) {
				point = pointScanEdge(Target, (int)CSGOHitboxID::Chest, damage);
				if (damage != 0) {
					Target_Bone_PerEnt = (int)CSGOHitboxID::Chest;
					bestdamage = damage;

				}
				if (bestdamage <= 0) {
					point = pointScanEdge(Target, (int)CSGOHitboxID::Chest, damage);
					if (damage > 0)
					{
						Target_Bone_PerEnt = (int)CSGOHitboxID::Chest;
						bestdamage = damage;
					}
				}
				if (bestdamage <= 0) {
					{//Normal

						for (int i = 3; i < scantimes; i++)
						{
							AutoWall_Scan(Target, hitboxess[i], damage);

							if (damage > bestdamage)
							{
								Target_Bone_PerEnt = hitboxess[i];
								bestdamage = damage;
							}
						}
						if (bestdamage <= 0) {
							point = pointScanEdge(Target, 0, damage);
							if (damage > 0)
							{
								Target_Bone_PerEnt = 0;
								bestdamage = damage;
							}
						}

					}

				}
			}
		}
		else {
			for (int i = 3; i > 0; i--)
			{

				AutoWall_Scan(Target, hitboxess[i], damage);
				float hp = Target->Entity->GetHealth();
				if (damage > bestdamage)
				{
					Target_Bone_PerEnt = hitboxess[i];
					bestdamage = damage;
				}
			}
			if (bestdamage <= 0) {
				point = pointScanEdge(Target, 0, damage);
				if (damage > 0)
				{
					Target_Bone_PerEnt = 0;
					bestdamage = damage;
				}
				if (bestdamage <= 0) {
					AutoWall_Scan(Target, (int)CSGOHitboxID::Pelvis, damage);
					if (damage != 0) {
						Target_Bone_PerEnt = (int)CSGOHitboxID::Pelvis;
						bestdamage = damage;
					}

					if (damage <= 0) {
						point = pointScanEdge(Target, (int)CSGOHitboxID::Pelvis, damage);
						if (damage != 0) {
							Target_Bone_PerEnt = (int)CSGOHitboxID::Pelvis;
							bestdamage = damage;

						}
						if (damage <= 0) {
							point = pointScanEdge(Target, (int)CSGOHitboxID::Pelvis, damage);
							if (damage > 0)
							{
								Target_Bone_PerEnt = (int)CSGOHitboxID::Pelvis;
								bestdamage = damage;
							}

							if (damage <= 0) {
								point = pointScanEdge(Target, (int)CSGOHitboxID::Chest, damage);
								if (damage != 0) {
									Target_Bone_PerEnt = (int)CSGOHitboxID::Chest;
									bestdamage = damage;

								}
								if (damage <= 0) {
									point = pointScanEdge(Target, (int)CSGOHitboxID::Chest, damage);
									if (damage > 0)
									{
										Target_Bone_PerEnt = (int)CSGOHitboxID::Chest;
										bestdamage = damage;
									}

								}
								if (damage <= 0) {

									for (int i = 2; i < scantimes; i++)
									{
										AutoWall_Scan(Target, hitboxess[i], damage);
										if (hitboxess[i] == (int)CSGOHitboxID::NeckLower)
											damage = 0;

										if (damage > bestdamage)
										{
											Target_Bone_PerEnt = hitboxess[i];
											bestdamage = damage;
										}
									}
								}
							}
						}
					}

				}

			}


		}
	}
	break;
	}

	if (point == Vector(0, 0, 0))
		point = GetHitboxPOS(Target, Target_Bone_PerEnt);

	//

	return bestdamage;
}

int last = 0;
int onladder = 0;
void CRageBot::DoAntiAim() {
	if (!Hacks.LocalPlayer)
		return;
	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_AntiAimWarmup)) {
		if (warmup)
			return;
	}
	dynamictimer();
	if (Hacks.CurrentCmd->buttons & IN_USE)
		return;
	if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_AntiAimLadders)) {
		if (Hacks.LocalPlayer->GetMoveType() == MOVETYPE_LADDER) {
			onladder = 2;
			return;
		}
		if (onladder > 0) {
			onladder--;
			return;
		}
	}



	if (Settings.GetBoolSetting(Tab_Ragebot, AntiAim_Enable))
	{
		int Settings_Y;
		int Settings_X = Settings.GetSetting(Tab_Ragebot, AntiAim_X);
		static int fakeangtimer = -1;
		if ((fakeangtimer < 2 || infakelag) && !(CM_Hacks.UnLagNextTick || Settings.GetSetting(Tab_Ragebot, AntiAim_FY) == 0 || Settings.GetSetting(Tab_Ragebot, AntiAim_FY) == 1)) {//Reall AA
			Hacks.SendPacket = false;
			Settings_Y = Settings.GetSetting(Tab_Ragebot, AntiAim_FY);
		}
		else {
			Settings_Y = Settings.GetSetting(Tab_Ragebot, AntiAim_Y);
			fakeangtimer = -1;
			Hacks.SendPacket = true;

		}
		fakeangtimer++;
		if (!Hacks.LocalWeapon) return;
		float flServerTime = (float)Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->interval_per_tick;
		float flNextPrimaryAttack = Hacks.LocalWeapon->NextPrimaryAttack();

		bool BulletTime = true;

		if (flNextPrimaryAttack > flServerTime)
			BulletTime = false;


		Vector views = Hacks.CurrentCmd->viewangles;
		Vector views1 = Hacks.CurrentCmd->viewangles;
		Vector totarget = Hacks.CurrentCmd->viewangles;
		CPlayer* Player = nullptr;
		for (int i = 0; i < 99; i++)
		{
			Player = Playerlist.Get_Player_By_Index(i);

			if (Player->Is_Empty) continue;
			if (!Player->Entity) continue;
			if (!Interfaces.pEntList->GetClientEntity(Player->Index)) continue;


			if (Ragebot_IsGoodPlayer(Player))
			{
				CalcAngle(Hacks.LocalPlayer->GetEyePosition(), GetHitboxPOS(Player, 0), totarget);
			}
		}
		if (Any_Targets)
		{
			bool nulltest = true;
			if (Target->Is_Empty) nulltest = false;;
			if (!Target) nulltest = false;
			if (!Interfaces.pEntList->GetClientEntity(Target->Index)) nulltest = false;;

			if (Ragebot_IsGoodPlayer(Target) && nulltest)
			{
				CalcAngle(Hacks.LocalPlayer->GetEyePosition(), GetHitboxPOS(Target, 0), totarget);
			}
		}

		if (Settings.GetSetting(Tab_Ragebot, AntiAim_AtTarget) == 2)
		{
			if (Player != nullptr)
			{
				views.y = totarget.y;
			}
		}
		static Vector Lastreal = views;
		static int Fast_Spin_Yaw = 0;
		static int Fast_Spin_Yaw2 = 0;
		static int Reversed_Spin_Yaw = -2;
		static int Reversed_Spin_Yaw2 = -2;
		static int Slow_Spin_Yaw = 0;
		static bool jitter = false;
		switch (Settings_X)
		{
		case 2://emotion 86
			views.x = 86.792510943;
			break;
		case 3: //down
			views.x = 81;
			break;
		case 4:// up
			views.x = -85.983222943432;
			break;
		case 5: // REVENGER EYE
		{
			static int counter = 0;
			static int motion = 0;
			int ServerTime = (float)Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->realtime;

			if (counter % 48 == 0)
				motion++;
			int value = ServerTime % 69;
			switch (value) {
			case 0:views.x = -57.443562f;
				if (Hacks.SendPacket)
					views.x = 70.23433211f; break;
			case 1:views.x = -70.453455f;
				if (Hacks.SendPacket)
					views.x = 0.43443f; break;
			case 2:views.x = 56.2123213245999f;
				if (Hacks.SendPacket)
					views.x = -30.32335533f; break;
			case 3:views.x = -88.21466777f;
				if (Hacks.SendPacket)
					views.x = 30.2435478999f; break;
			case 4:views.x = 61.5446433f;
				if (Hacks.SendPacket)
					views.x = -30.45666f; break;
			case 5:views.x = -61.4354666f;
				if (Hacks.SendPacket)
					views.x = 79.f; break;
			case 6:views.x = 50.3355569f;
				if (Hacks.SendPacket)
					views.x = -33.46335324324f; break;
			case 7:views.x = -83.3243243243f;
				if (Hacks.SendPacket)
					views.x = 73.54534543f; break;
			case 8:views.x = 66.567476756f;
				if (Hacks.SendPacket)
					views.x = -76.576575675756f; break;
			case 9:views.x = 82.43543543534f;
				if (Hacks.SendPacket)
					views.x = -72.345345345f; break;
			case 10:views.x = -62.4535435345f;
				if (Hacks.SendPacket)
					views.x = 32.4534545345f; break;
			case 11:views.x = 74.324234342f;
				if (Hacks.SendPacket)
					views.x = -74.324234234f; break;
			case 12:views.x = -44.98978797f;
				if (Hacks.SendPacket)
					views.x = 74.43434599f; break;
			case 13:views.x = 63.994599f;
				if (Hacks.SendPacket)
					views.x = -33.9999f; break;
			case 14:views.x = -63.99999999f;
				if (Hacks.SendPacket)
					views.x = 83.9999f; break;
			case 15:views.x = 76.6745635f;
				if (Hacks.SendPacket)
					views.x = -56.f; break;
			case 16:views.x = -32.54654645f;
				if (Hacks.SendPacket)
					views.x = 72.3343f; break;
			case 17:views.x = 85.456546456f;
				if (Hacks.SendPacket)
					views.x = -55.46546436f; break;
			case 18:views.x = -85.546456456f;
				if (Hacks.SendPacket)
					views.x = 70.99343f; break;
			case 19:views.x = 70.423432423f;
				if (Hacks.SendPacket)
					views.x = -50.654654f; break;
			case 20:views.x = -60.975466f;
				if (Hacks.SendPacket)
					views.x = 30.2242424f; break;
			case 21:views.x = 69.99394f;
				if (Hacks.SendPacket)
					views.x = -49.493493943f; break;
			case 22:views.x = -79.5464535455643f;
				if (Hacks.SendPacket)
					views.x = 69.945945945f; break;
			case 23:views.x = 78.32325325f;
				if (Hacks.SendPacket)
					views.x = -29.423432455f; break;
			case 24:views.x = -74.98897f;
				if (Hacks.SendPacket)
					views.x = 29.3223144f; break;
			case 25:views.x = 79.1554433f;
				if (Hacks.SendPacket)
					views.x = -79.2432423f; break;
			case 26:views.x = -79.23423423f;
				if (Hacks.SendPacket)
					views.x = 49.9978f; break;
			case 27:views.x = 88.9999f;
				if (Hacks.SendPacket)
					views.x = -58.323232411f; break;
			case 28:views.x = 58.939243294f;
				if (Hacks.SendPacket)
					views.x = -90.9995f; break;
			case 29:views.x = 81.9928393f;
				if (Hacks.SendPacket)
					views.x = -61.9999f; break;
			case 30:views.x = -91.2145423f;
				if (Hacks.SendPacket)
					views.x = 51.93948343f; break;
			}
			counter++;
		}
		break;

		case 6:// fakedown
			views.x = -70.148813372001;
			if (Hacks.SendPacket)
				views.x = 70.148813372001;
			break;
		case 7:// Float(BB)
			if (Hacks.LocalPlayer->GetFlags() & FL_ONGROUND) {
				views.x = 0;
				if (!Hacks.SendPacket)
					views.x = 21600;

			}
			else {//Should be 90 but 90 causes crash
				views.x = 89;
				if (!Hacks.SendPacket)
					views.x = 103;

			}

			break;
		case 8://BackFlip
		{
			views.x = 70.f;
			if (Hacks.SendPacket)
				views.x = 0.f;

			break;
		}
		case 9:
		{
			views.x = 84.792510943;
			if (Hacks.SendPacket)
				views.x = -85.983222943432;

			break;
		}
		case 10:
		{
			views.x = 0;
			break;
		}
		case 11:
		{
			float test = (Interfaces.pGlobalVars->realtime);
			float test2 = (Interfaces.pGlobalVars->tickcount);
			views.x = test;
			if (Hacks.SendPacket)
				views.x = test2;

			break;

		}
		case 12:
		{
			float test3 = (Interfaces.pGlobalVars->realtime);
			float test4 = (Interfaces.pGlobalVars->frametime);
			views.x = test3;
			if (Hacks.SendPacket)
				views.x = test4;

			break;
		}
		case 13:
		{
			{
				float test3 = (Interfaces.pGlobalVars->realtime * Interfaces.pGlobalVars->curtime);
				float test4 = (Interfaces.pGlobalVars->realtime * Interfaces.pGlobalVars->interval_per_tick);

				views.x = test3;
				if (Hacks.SendPacket)
					views.x = test4;

				break;
			}
		case 14:
			float realx = (Settings.GetSetting(Tab_Ragebot, Ragebot_Realx));
			float fakex = (Settings.GetSetting(Tab_Ragebot, Ragebot_Fakex));



			// + & +
			views.x = realx;
			if (Hacks.SendPacket)
				views.x = fakex;
			// + & -
			if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_MinusFakex))
			{
				views.x = realx;
				if (Hacks.SendPacket && Settings.GetBoolSetting(Tab_Ragebot, Ragebot_MinusFakex))
					views.x = -fakex;
			}
			// - & +
			if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_MinusRealx))
			{
				views.x = -realx;
				if (Hacks.SendPacket)
					views.x = fakex;
			}
			// - & -
			if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_MinusRealx))
			{
				views.x = -realx;
				if (Hacks.SendPacket && Settings.GetBoolSetting(Tab_Ragebot, Ragebot_MinusFakex))
					views.x = -fakex;
			}

		}
		break;
		case 15: // Ultra (Test by JesusCry)
		{
			static int counter = 0;
			static int motion = 0;
			int ServerTime = (float)Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->realtime;

			if (counter % 73 == 0)
				motion++;
			int value = ServerTime % 69;
			switch (value) {
			case 0:views.x = -2147483648.0f;
				if (Hacks.SendPacket)
					views.x = 2147483647.0f; break;
			case 1:views.x = -9223372036854775808.0f;
				if (Hacks.SendPacket)
					views.x = 9223372036854775807.0f; break;
			case 2:views.x = 56.2123213245999f;
				if (Hacks.SendPacket)
					views.x = -30.32335533f; break;
			case 3:views.x = -88.21466777f;
				if (Hacks.SendPacket)
					views.x = 30.2435478999f; break;
			case 4:views.x = 61.5446433f;
				if (Hacks.SendPacket)
					views.x = -30.45666f; break;
			case 5:views.x = -61.4354666f;
				if (Hacks.SendPacket)
					views.x = 79.f; break;
			case 6:views.x = 50.3355569f;
				if (Hacks.SendPacket)
					views.x = -33.46335324324f; break;
			case 7:views.x = -83.3243243243f;
				if (Hacks.SendPacket)
					views.x = 73.54534543f; break;
			case 8:views.x = 66.567476756f;
				if (Hacks.SendPacket)
					views.x = -76.576575675756f; break;
			case 9:views.x = 82.43543543534f;
				if (Hacks.SendPacket)
					views.x = -72.345345345f; break;
			case 10:views.x = -62.4535435345f;
				if (Hacks.SendPacket)
					views.x = 32.4534545345f; break;
			case 11:views.x = 74.324234342f;
				if (Hacks.SendPacket)
					views.x = -74.324234234f; break;
			case 12:views.x = -44.98978797f;
				if (Hacks.SendPacket)
					views.x = 74.43434599f; break;
			case 13:views.x = 63.994599f;
				if (Hacks.SendPacket)
					views.x = -33.9999f; break;
			case 14:views.x = -63.99999999f;
				if (Hacks.SendPacket)
					views.x = 83.9999f; break;
			case 15:views.x = 76.6745635f;
				if (Hacks.SendPacket)
					views.x = -56.f; break;
			case 16:views.x = -32.54654645f;
				if (Hacks.SendPacket)
					views.x = 72.3343f; break;
			case 17:views.x = 85.456546456f;
				if (Hacks.SendPacket)
					views.x = -55.46546436f; break;
			case 18:views.x = -85.546456456f;
				if (Hacks.SendPacket)
					views.x = 70.99343f; break;
			case 19:views.x = 70.423432423f;
				if (Hacks.SendPacket)
					views.x = -50.654654f; break;
			case 20:views.x = -60.975466f;
				if (Hacks.SendPacket)
					views.x = 30.2242424f; break;
			case 21:views.x = 69.99394f;
				if (Hacks.SendPacket)
					views.x = -49.493493943f; break;
			case 22:views.x = -79.5464535455643f;
				if (Hacks.SendPacket)
					views.x = 69.945945945f; break;
			case 23:views.x = 78.32325325f;
				if (Hacks.SendPacket)
					views.x = -29.423432455f; break;
			case 24:views.x = -74.98897f;
				if (Hacks.SendPacket)
					views.x = 29.3223144f; break;
			case 25:views.x = 79.1554433f;
				if (Hacks.SendPacket)
					views.x = -79.2432423f; break;
			case 26:views.x = -79.23423423f;
				if (Hacks.SendPacket)
					views.x = 49.9978f; break;
			case 27:views.x = 88.9999f;
				if (Hacks.SendPacket)
					views.x = -58.323232411f; break;
			case 28:views.x = 58.939243294f;
				if (Hacks.SendPacket)
					views.x = -90.9995f; break;
			case 29:views.x = 81.9928393f;
				if (Hacks.SendPacket)
					views.x = -61.9999f; break;
			case 30:views.x = -91.2145423f;
				if (Hacks.SendPacket)
					views.x = 51.93948343f; break;
			}
			counter++;
		}
		break;

		}
		//Yaw
		switch (Settings_Y)
		{
		case 2:
			//Backwards 
			views.y += 179.5748399;
			break;
		case 3:
			//Jitter
			views.y = -89.148813372001;
			if (Hacks.SendPacket)
				views.y = - 178.148813372001;
			break;
		case 4:
			//Left
			views.y -= 88.993353; //+ 360;
			break;
		case 5:
			//Right
			views.y = 160.993353; //+ 360;
			break;
		case 6:
			//Forward
			views.y += 1.009995;
			break;
		case 7://45 Jitter
		{
			bool aa1 = false;
			bool aa2 = false;
			bool aa3 = false;
			bool aa4 = false;
			float add = 0;

			if (totarget.y > 0 && totarget.y < 88.5532521) {
				aa1 = true;
			}
			else if (totarget.y < 0 && totarget.y > -88.9995022) {
				aa3 = true;
			}
			else if (totarget.y < -90 && totarget.y > -178.91234) {
				aa2 = true;
			}
			else if (totarget.y > 90 && totarget.y < 177.9901523322) {
				aa4 = true;
			}

			if (aa1 = true) {
				add = 135.34522342;
			}
			if (aa3 = true) {
				add = 43.98763635;
			}
			if (aa4 = true) {
				add = -42.122999;
			}
			if (aa2 = true) {
				add = -132.00998;
			}
			if (!Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust)) {
				if (!(Hacks.LocalPlayer->GetVecVelocity().Length2D() > 200)) {
					views.y = totarget.y;
				}
				else {
					views.y += add;
				}
			}
			else {
				views.y += add;
			}
		}
		break;

		case 8:
		{
			if (!Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust)) {
				if (Hacks.LocalPlayer->GetVecVelocity().Length2D() > 300)
				{
					views.y += 15.13423;
				}
				else {
					views.y -= 50.9999055523;
				}
			}
			else {
				if (Hacks.CurrentCmd->buttons & IN_ATTACK) {
					views.y += 150.656645363;
				}
				else
					views.y -= 147.4788437875;
			}
		}
		break;
		case 9://Sideways jitter
		{
			float add = 0;
			if (!Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust)) {
				if (switch123) {
					add = 87.2355999;
				}
				else {
					add = 42.46654664;
				}
			}
			else {
				if (switch123) {
					add = 87.456524556123;
				}
				else {
					add = 42.2355997663;
				}
			}
			if (!Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust)) {
				if (!(Hacks.LocalPlayer->GetVecVelocity().Length2D() > 200)) {
					views.y = totarget.y;
				}
				else {
					views.y += add;
				}
			}
			else {
				views.y += add;
			}
		}
		break;
		case 10:
		{
			float add = 0;
			if (!Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust)) {
				if (switch123) {
					add = -87.454322;
				}
				else {
					add = 132.6654;
				}
			}
			else {
				if (switch123) {
					add = -86.254211999;
				}
				else {
					add = 127.99955;
				}
			}
			if (!Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust)) {
				if (!(Hacks.LocalPlayer->GetVecVelocity().Length2D() > 200)) {
					views.y = totarget.y;
				}
				else {
					views.y += add;
				}
			}
			else {
				views.y += add;
			}
		}
		break;
		case 11:
			//Half Right
			views.y += 43.990665;
			break;
		case 12:
			//Half Left
			views.y -= 42.00990655;
			break;
		case 13:
			//Test
		{
			float CalculatedCurTime = (Interfaces.pGlobalVars->curtime * 2000);
			views.y = CalculatedCurTime;
			break;
		}
		case 14:
		{
			views.y = 36000180.0;
			break;
		}
		case 15:
		{ //Float(BB)
			views.y = 360000000;
			break;
		}
		case 16:
		{

			float realy = (Settings.GetSetting(Tab_Ragebot, Ragebot_Realy));

			views.y = realy;

			if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_MinusRealy))
				views.y = -realy;
			break;
		}

		case 17:
		{

			float fakey = (Settings.GetSetting(Tab_Ragebot, Ragebot_Fakey));

			views.y = fakey;

			if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_MinusFakey))
				views.y = -fakey;

			break;
		}
		case 18:
		{
			float meme = (Interfaces.pGlobalVars->realtime * Interfaces.pGlobalVars->tickcount);

			views.y = meme;
		}
		}

		if (!Hacks.SendPacket)
			Lastreal = views;

		if (Hacks.LocalPlayer->GetFlags() & FL_ONGROUND && !Hacks.SendPacket && Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust) && Settings.GetSetting(Tab_Ragebot, Ragebot_Duck) > 1) {
			views.y = 0;
		}
		Hacks.CurrentCmd->viewangles = views;
	}
}
float CRageBot::PelvisAng(CPlayer * Target) {
	if (!Target)
		return 0;

	matrix3x4 matrix[128];
	if (!Target->Entity->SetupBones(matrix, 128, 0x00000100, GetTickCount64())) return 0;
	const model_t* mod = Target->Entity->GetModel();
	if (!mod) return 0;
	studiohdr_t* hdr = Interfaces.g_pModelInfo->GetStudioModel(mod);
	if (!hdr) return 0;
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	if (!set) return 0;
	mstudiobbox_t* hitbox = set->pHitbox((int)CSGOHitboxID::Pelvis);
	if (!hitbox) return 0;
	Vector vMin, vMax, vCenter, sCenter;
	g_Math.VectorTransform2(hitbox->bbmin, matrix[hitbox->bone], vMin);
	g_Math.VectorTransform2(hitbox->bbmax, matrix[hitbox->bone], vMax);
	Vector angs = Vector(0, 0, 0);
	CalcAngle(vMin, vMax, angs);
	return angs.y;
}
bool CRageBot::Ragebot_IsGoodPlayer(CPlayer * Target)
{
	if (!Target->Entity) return false;

	if (!Target->Entity->isAlive()
		|| Target->Entity->GetHealth() <= 0
		|| Target->Entity->IsDormant()
		|| (Target->Entity->GetTeam() == Hacks.LocalPlayer->GetTeam())
		|| Target->Entity == Hacks.LocalPlayer
		|| Target->Entity->HasGunGameImmunity()
		)
		return false;

	if (Target->friendly)
		return false;


	return true;
}


