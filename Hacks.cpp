#include "stdafx.h"
Createmove_Hacks CM_Hacks;
PaintTraverse_Hacks PT_Hacks;
Draw_Model_Exec_Hacks DMEHacks;
player_info_t* Info = new player_info_t;

CBaseEntity* CBaseEntity::GetObserverTarget()
{
	DWORD entityData = *(DWORD*)((DWORD)this + 0x3380);
	return Interfaces.pEntList->GetClientEntity(entityData & 0xFFF);
}

void SpecList()
{
	CBaseEntity* pLocal = Hacks.LocalPlayer;

	RECT scrn = Interfaces.pSurface->GetViewport();
	int ayy = 0;

	// Loop through all active entitys
	for (int i = 0; i < Interfaces.pEntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		CBaseEntity *pEntity = Interfaces.pEntList->GetClientEntity(i);
		player_info_t pinfo;

		// The entity isn't some laggy peice of shit or something
		if (pEntity &&  pEntity != pLocal)
		{
			if (Interfaces.pEngine->GetPlayerInfo(i, &pinfo) && !pEntity->isAlive() && !pEntity->IsDormant())
			{
				HANDLE obs = pEntity->GetObserverTarget();

				if (obs)
				{
					CBaseEntity *pTarget = Interfaces.pEntList->GetClientEntityFromHandle(obs);
					player_info_t pinfo2;
					if (pTarget)
					{
						if (Interfaces.pEngine->GetPlayerInfo(pTarget->GetIndex(), &pinfo2))
						{
							static float rainbow;
							rainbow += 0.005f;
							if (rainbow > 1.f) rainbow = 0.f;

							char buf[255]; sprintf_s(buf, "%s spectate %s", pinfo.m_szPlayerName, pinfo2.m_szPlayerName);
							RECT TextSize = Interfaces.pSurface->GetTextSize2(Hacks.Font_ESP, buf);
							Interfaces.pSurface->Clear(scrn.right - 260, (scrn.bottom / 2) + (16 * ayy), 260, 16, Color(10, 10, 10, 255));
							Interfaces.pSurface->Text2(scrn.right - TextSize.right - 4, (scrn.bottom / 2) + (16 * ayy), pTarget->GetIndex() == pLocal->GetIndex() ? Color::FromHSB(rainbow, 1.f, 1.f) : Color(0, 150, 0, 255), Hacks.Font_ESP, buf);
							ayy++;
						}
					}
				}
			}
		}
	}

	Interfaces.pSurface->Outline(scrn.right - 261, (scrn.bottom / 2) - 1, 262, (16 * ayy) + 2, Color(0, 150, 0, 255));
	Interfaces.pSurface->Outline(scrn.right - 260, (scrn.bottom / 2), 260, (16 * ayy), Color(0, 150, 0, 255));
}

void MsgFunc_ServerRankRevealAll()
{
	using tServerRankRevealAllFn = bool(__cdecl*)(int*);
	static tServerRankRevealAllFn ServerRankRevealAll = (tServerRankRevealAllFn)((PDWORD)Utils.PatternSearch("client.dll", (PBYTE)"\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxx????x????", NULL, NULL));
	int fArray[3] = { 0,0,0 };

	ServerRankRevealAll(fArray);
}

#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"

#define TEXTURE_GROUP_MODEL							"Model textures"

/*
NOTE:

TRY TO KEEP AMOUNT OF .CPP FILES TO A LOW AMOUND, OR IT TAKES A LONG TIME TO COMPILE

TO DO THIS, WE ARE KEEPING MOST FEATURES IN ONE .CPP SO WE DONT HAVE TO HAVE A .CPP
FOR EVERY FEATURE.

AIMBOTS HAVE THERE OWN CPP THOUGH.

*/


extern void dolegitshit();

#pragma region Create Move

//majmun18
bool bhopping = false;
void Createmove_Hacks::BunnyHop()
{
	bhopping = false;
	if ((Hacks.CurrentCmd->buttons & IN_JUMP))
	{
		bhopping = true;
		if (!(Hacks.LocalPlayer->GetFlags() & FL_ONGROUND))
			Hacks.CurrentCmd->buttons &= ~IN_JUMP;
		else
		{
			if (Settings.GetSetting(Tab_Ragebot, Ragebot_Duck == 2))
				Hacks.CurrentCmd->buttons &= ~IN_DUCK;
			UnLagNextTick = true;
			Hacks.SendPacket = true;
		}
	}
}

void Createmove_Hacks::AutoStrafer()
{
	if (bhopping) {
		if (Hacks.LocalPlayer->GetVecVelocity().Length2D() == 0 && (Hacks.CurrentCmd->forwardmove == 0 && Hacks.CurrentCmd->sidemove == 0)) {
			Hacks.CurrentCmd->forwardmove = 450.f;
		}
		else if (Hacks.CurrentCmd->forwardmove == 0 && Hacks.CurrentCmd->sidemove == 0) {
			if (Hacks.CurrentCmd->mousedx > 0 || Hacks.CurrentCmd->mousedx < -0) {
				Hacks.CurrentCmd->sidemove = Hacks.CurrentCmd->mousedx < 0.f ? -450.f : 450.f;
			}
			else {
				float temp1 = 5850.f / Hacks.LocalPlayer->GetVecVelocity().Length2D();
				Hacks.CurrentCmd->forwardmove = temp1;
				if (Hacks.CurrentCmd->forwardmove > 450)
					Hacks.CurrentCmd->forwardmove = 450;
				float temp = 450 - (temp1 - Hacks.CurrentCmd->forwardmove);
				if (temp < 450)
					temp = 450;
				Hacks.CurrentCmd->sidemove = (Hacks.CurrentCmd->command_number % 2) == 0 ? -temp : temp;
			}
		}
	}
}

float moveangle(Vector angles) {
	if (angles.x == 0 || angles.y == 0)
		return 0;
	float rise = angles.x;
	float run = angles.y;
	float value = rise / run;
	float theta = atan(value);
	theta = RAD2DEG(theta) + 90;
	return theta;
}
bool Createmove_Hacks::CircleStrafer(Vector &angles) {
	if (Settings.GetSetting(Tab_Misc, Misc_CircleStrafe_Enabled)) {
		static int add = 0;
		if (Settings.GetSetting(Tab_Misc, Misc_CircleStrafe_Mode)) {
			if (angles.y - add > 360)
				add -= 360;
			static bool shouldspin = false;
			static bool enabled = false;
			if (GetAsyncKeyState((int)Settings.GetSetting(Tab_Misc, Misc_CircleStrafe_Key))) {
				enabled = true;
			}
			else {
				enabled = false;
			}
			if (enabled) {
				shouldspin = true;
			}
			if (shouldspin) {
				Vector Dir;
				g_Math.angleVectors(Vector(0, add, 0), Dir);
				Dir *= 8218;
				Ray_t ray;
				CTraceWorldOnly filter;
				trace_t trace;
				ray.Init(Hacks.LocalPlayer->GetEyePosition(), Hacks.LocalPlayer->GetVecOrigin() + Dir);
				Interfaces.pTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);
				auto temp = 3.4f / ((trace.endpos - Hacks.LocalPlayer->GetVecOrigin()).Length() / 100.f);
				if (Hacks.LocalPlayer->GetVecVelocity().Length2D() > 415) {
					if (temp < 3.4f)
						temp = 3.4f;
				}
				else if (Hacks.LocalPlayer->GetVecVelocity().Length2D() > 200) {
					if (temp < 6.8f)
						temp = 6.8f;
				}
				else {
					if (temp < 10.2f)
						temp = 10.2f;
				}
				if (enabled) {
					add += temp;
					Hacks.CurrentCmd->sidemove = -450;
				}
				else {
					if (angles.y - add < temp) {
						add = angles.y;
						shouldspin = false;
					}
					else
						add += temp;
				}
				angles.y = add;
			}
			else {
				add = angles.y;
				return false;
			}
		}
		else {
			if (angles.y - add > 360)
				add -= 360;
			static bool shouldspin = false;
			static bool enabled = false;
			if (GetAsyncKeyState((int)Settings.GetSetting(Tab_Misc, Misc_CircleStrafe_Key))) {
				enabled = true;
			}
			else {
				enabled = false;
			}
			if (enabled) {
				shouldspin = true;
			}
			if (shouldspin) {
				Vector Dir;
				g_Math.angleVectors(Vector(0, add, 0), Dir);
				Dir *= 8218;
				Ray_t ray;
				CTraceWorldOnly filter;
				trace_t trace;
				ray.Init(Hacks.LocalPlayer->GetEyePosition(), Hacks.LocalPlayer->GetVecOrigin() + Dir);
				Interfaces.pTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);
				auto temp = 3.4f / ((trace.endpos - Hacks.LocalPlayer->GetVecOrigin()).Length() / 100.f);
				if (temp < 3.4f)
					temp = 3.4f;
				if (enabled) {
					add += temp;
					Hacks.CurrentCmd->sidemove = -450;
				}
				else {
					if (angles.y - add < temp) {
						add = angles.y;
						shouldspin = false;
					}
					else
						add += temp;
				}
				angles.y = add;
			}
			else {
				add = angles.y;
				return false;
			}
		}
	}
}

void Airstuck() {
	static bool enabled = false;
	static bool check = false;
	if (Settings.GetSetting(Tab_Misc, Misc_Airstuck_Enabled)) {
		if (GetAsyncKeyState((int)Settings.GetSetting(Tab_Misc, Misc_Airstuck_Key))) {
			if (!check) {
				enabled = !enabled;
				check = true;
			}
		}
		else {
			check = false;
		}
		if (enabled) {
			float ServerTime = Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->interval_per_tick;
			float NextPrimaryAttack = Hacks.LocalWeapon->NextPrimaryAttack();

			bool Shooting = false;
			if (NextPrimaryAttack > ServerTime)
				Shooting = true;


			if (!Shooting) {
				Hacks.CurrentCmd->tick_count = 16777216;
			}
		}
	}
}
void SetClanTag(const char* tag, const char* name)//190% paste
{

	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)(Utils.PatternSearch("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx", NULL, NULL))));

	pSetClanTag(tag, name);

}

void Clan_Tag() {

	if (Settings.GetSetting(Tab_Misc, Misc_Clan_Active)) {
		static int counter = 0;

		int value = Settings.GetSetting(Tab_Misc, Misc_Clan_Index);
		switch (value)
		{
		case 0:
		case 1: SetClanTag("splashgang", "pTitelow"); break;
		case 2: SetClanTag("JesusCry \r", "pTitelow"); break;
			//case 2: SetClanTag("[VALV\xE1\xB4\xB1]", "Valve"); break;

		case 3:
		{
			if (Hacks.CurrentCmd->buttons & IN_ATTACK)
				SetClanTag("Aiming", "pTitelow");
			else if (Hacks.CurrentCmd->buttons & IN_JUMP)
				SetClanTag("Bhopping", "pTitelow");
			else if (Hacks.CurrentCmd->buttons & IN_DUCK)
				SetClanTag("Getting Low", "pTitelow");
			else
				SetClanTag("JesusCry", "pTitelow");
		}
		break;

		case 4:
		{

			static int motion = 0;
			int ServerTime = (float)Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->interval_per_tick;

			if (counter % 48 == 0)
				motion++;
			int value = ServerTime % 50;
			switch (value) {
			case 0:SetClanTag("J/", "pTitelow"); break;
			case 1:SetClanTag("J\\", "pTitelow"); break;
			case 2:SetClanTag("Je", "pTitelow"); break;
			case 3:SetClanTag("Je/", "pTitelow"); break;
			case 4:SetClanTag("Je\\", "pTitelow"); break;
			case 5:SetClanTag("Jes", "pTitelow"); break;
			case 6:SetClanTag("Jes/", "pTitelow"); break;
			case 7:SetClanTag("Jes\\", "pTitelow"); break;
			case 8:SetClanTag("Jesu", "pTitelow"); break;
			case 9:SetClanTag("Jesu/", "pTitelow"); break;
			case 10:SetClanTag("Jesu\\", "pTitelow"); break;
			case 11:SetClanTag("Jesus", "pTitelow"); break;
			case 12:SetClanTag("Jesus/", "pTitelow"); break;
			case 13:SetClanTag("Jesus\\", "pTitelow"); break;
			case 14:SetClanTag("JesusC", "pTitelow"); break;
			case 15:SetClanTag("JesusC/", "pTitelow"); break;
			case 16:SetClanTag("JesusC\\", "pTitelow"); break;
			case 17:SetClanTag("JesusCr", "pTitelow"); break;
			case 18:SetClanTag("JesusCr/", "pTitelow"); break;
			case 19:SetClanTag("JesusCr\\", "pTitelow"); break;
			case 20:SetClanTag("JesusCry", "pTitelow"); break;
			case 21:SetClanTag("JesusCry/", "pTitelow"); break;
			case 22:SetClanTag("JesusCry\\", "pTitelow"); break;
			}
			counter++;
		}
		break;

		case 5:
		{
			static int motion = 0;
			if (counter % 48 == 0)
				motion++;
			int ServerTime = (float)Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->interval_per_tick;

			int value = ServerTime % 8;
			switch (value) {
			case 0:SetClanTag("\r JesusCry", "pTitelow"); break;
			case 1:SetClanTag("JesusCry \r", "pTitelow"); break;
			case 2:SetClanTag("\r JesusCry", "pTitelow"); break;
			case 3:SetClanTag("JesusCry \r", "pTitelow"); break;
			case 4:SetClanTag("\r JesusCry", "pTitelow"); break;
			case 5:SetClanTag("JesusCry \r", "pTitelow"); break;
			case 6:SetClanTag("\r JesusCry", "pTitelow"); break;
			case 7:SetClanTag("JesusCry \r", "pTitelow"); break;
			}
			counter++;
		}
		break;

		case 6:SetClanTag("Jesus$Cry", "Jesus$Cry"); break;
		}
	}
}
void Spamer() {
	int switchval = Settings.GetSetting(Tab_Misc, Misc_Spam_Index);
	stringstream message;
	message << "say";
	int static counter = 1;

	switch (switchval) {
	case 0:
	case 1: {
		int value = counter % 3;
		switch (value) {
		case 1: message << " vk.com/kekcwhite Hack present Best MM RAGE / Legit"; break;
		case 2: message << " Ohh if you read this message so you suck my big hack dick"; break;
		case 3: message << " You do not want to suck?"; break;
		case 4: message << " Ohh , no problem !!! Buy our Hack and do not suck!!! "; break;
		case 5: message << " vk.com/kekcwhite"; break;
		case 6: message << " vk.com/kekcwhite"; break;
		case 7: message << " vk.com/kekcwhite"; break;
		case 8: message << " We are waiting for the elite."; break;

		}

	}break;
	case 2:
	{
		int value = counter % 100 + 1;
		switch (value) {
		case 0:
		case 01: message << " Hello man "; break;
		case 02: message << " if you read this message you SUCK MY HACK DICK"; break;
		case 03: message << " I see you suck"; break;
		case 04: message << " Dont worry"; break;
		case 05: message << " If you want Boost RANK or will winner on HVH"; break;
		case 06: message << " Buy OUR HACK"; break;
		case 07: message << " JesusCry - BEST Rage/LEGIT hack in the world (Not comfimed)"; break;
		case 8: message << " Ohhh you dont know where buy this HACK(((("; break;
		case 9: message << " Okey , in vk.com/kekcwhite"; break;
		case 10: message << " vk.com/kekcwhite"; break;
		case 11: message << " vk.com/kekcwhite"; break;
		case 12: message << " vk.com/kekcwhite"; break;
		case 13: message << " if you dont want be A FUCKING CHEATER and want UP RANK"; break;
		case 14: message << " Ohh , +rep , You CAN BUY BOOST IN PARTY"; break;
		case 15: message << " vk.com/kekcwhite"; break;
		case 16: message << " vk.com/kekcwhite"; break;
		case 17: message << " DONT CRY BUY OUR HACK"; break;
		case 18: message << " vk.com/kekcwhite"; break;

		}
	}break;
	case 3:
	{
		static int lasti = 0;
		bool hasone = false;
		if (lasti > 98)
			lasti = 0;
		CBaseEntity * Player = Hacks.LocalPlayer;
		for (int i = lasti + 1; i < 99; i++) {
			Player = Interfaces.pEntList->GetClientEntity(i);
			if (!Player) continue;
			if (!Player->isAlive()
				|| Player->GetHealth() <= 0
				|| Player->IsDormant()
				|| (Player->GetTeam() == Hacks.LocalPlayer->GetTeam())
				|| Player == Hacks.LocalPlayer
				|| Player->HasGunGameImmunity())continue;
			if (!Interfaces.pEntList->GetClientEntity(i)) continue;

			Interfaces.pEngine->GetPlayerInfo(i, Info);
			char * name = Info->m_szPlayerName;
			char * place = Player->LastPlaceName();
			int health = Player->GetHealth();
			message << " " << name << " is " << place << " with " << health << "HP";
			lasti = i;
			hasone = true;
			break;
		}
		if (!hasone) {
			for (int i = 0; i < 99; i++) {
				Player = Interfaces.pEntList->GetClientEntity(i);
				if (!Player) continue;
				if (!Player->isAlive()
					|| Player->GetHealth() <= 0
					|| Player->IsDormant()
					|| (Player->GetTeam() == Hacks.LocalPlayer->GetTeam())
					|| Player == Hacks.LocalPlayer
					|| Player->HasGunGameImmunity())continue;
				if (!Interfaces.pEntList->GetClientEntity(i)) continue;

				Interfaces.pEngine->GetPlayerInfo(i, Info);
				char * name = Info->m_szPlayerName;
				char * place = Player->LastPlaceName();
				int health = Player->GetHealth();
				message << " " << name << " is " << place << " with " << health << "HP";
				lasti = i;
				hasone = true;
				break;
			}

		}

	}break;
	case 4:
	{
		static int lasti = 0;
		bool hasone = false;
		if (lasti > 98)
			lasti = 0;
		CBaseEntity * Player = Hacks.LocalPlayer;
		for (int i = lasti + 1; i < 99; i++) {
			Player = Interfaces.pEntList->GetClientEntity(i);
			if (!Player) continue;
			if (!Player->isAlive()
				|| Player->GetHealth() <= 0
				|| Player->IsDormant()
				|| (Player->GetTeam() != Hacks.LocalPlayer->GetTeam())
				|| Player == Hacks.LocalPlayer
				|| Player->HasGunGameImmunity())continue;
			if (!Interfaces.pEntList->GetClientEntity(i)) continue;

			Interfaces.pEngine->GetPlayerInfo(i, Info);
			char * name = Info->m_szPlayerName;
			char * place = Player->LastPlaceName();
			int health = Player->GetHealth();
			message << " " << name << " is " << place << " with " << health << "HP";
			lasti = i;
			hasone = true;
			break;
		}
		if (!hasone) {
			for (int i = 0; i < 99; i++) {
				Player = Interfaces.pEntList->GetClientEntity(i);
				if (!Player) continue;
				if (!Player->isAlive()
					|| Player->GetHealth() <= 0
					|| Player->IsDormant()
					|| (Player->GetTeam() == Hacks.LocalPlayer->GetTeam())
					|| Player == Hacks.LocalPlayer
					|| Player->HasGunGameImmunity())continue;
				if (!Interfaces.pEntList->GetClientEntity(i)) continue;

				Interfaces.pEngine->GetPlayerInfo(i, Info);
				char * name = Info->m_szFriendsName;
				char * place = Player->LastPlaceName();
				int health = Player->GetHealth();
				message << " " << name << " is " << place << " with " << health << "HP";
				lasti = i;
				hasone = true;
				break;
			}

		}

	}break;
	case 5:
	{
		int value = counter % 100 + 1;
		switch (value) {
		case 0: 
		case 01: message << " vk.com/kekcwhite"; break;
		case 02: message << " JesusCry"; break;

		}
	}break;
	}
	counter++;
	string str(message.str());
	char * send = new char[str.length() + 1];
	std::strcpy(send, str.c_str());

	if (send != "say") {
		Interfaces.pEngine->ClientCmd_Unrestricted(send, 0);

	}
	
}


void Chat_Spamer() {
	if (Settings.GetSetting(Tab_Misc, Misc_Spam_Active)) {
		static int delay = 1;

		int modval = (Settings.GetSetting(Tab_Misc, Misc_Spam_Speed) + 1) * 10;
		if (delay % modval == 0) {
			Spamer();
		}
		delay++;
	}
}
void Name_Stealer()
{
	Clan_Tag();
	Chat_Spamer();

	if (Settings.GetBoolSetting(Tab_Misc, Misc_NameStealer))
	{
		stringstream NewName;
		int Index = Settings.GetSetting(Tab_Misc, Misc_NameStealer_Index);

		switch (Index)
		{
		case 1: NewName << "JesusCry"; break;
		case 2: NewName << "SKEET.CC"; break;
		case 3: NewName << " "; break;
		case 4: NewName << "AIMWARE.NET"; break;
		case 5: NewName << "ABitSmarter"; break;
		case 6: NewName << "\r PU SUCKS!"; break;
		case 7: NewName << "Jesus$Cry"; break;
		case 8: NewName << "SK4M$WARE"; break;
		case 9: NewName << "OWNED YOU AND ALL"; break;
		}

		int ran = rand() % 2;
		for (int i = 0; i < ran; i++)
		{
			NewName << " ";
		}
		if (Settings.GetSetting(Tab_Misc, Misc_NameStealer_Invis)) {
			NewName << " \r";
		}
		ConVar* Name = Interfaces.g_ICVars->FindVar("name");
		if (Name)
		{
			*(int*)((DWORD)&Name->fnChangeCallback + 0xC) = 0;
			Name->SetValue(NewName.str().c_str());
		}
	}
}
void Createmove_Hacks::CorrectMovement(Vector vOldAngles, CInput::CUserCmd* pCmd, Vector Viewangs)
{
	Vector vMove(pCmd->forwardmove, pCmd->sidemove, pCmd->upmove);
	float flSpeed = sqrt(vMove.x * vMove.x + vMove.y * vMove.y), flYaw;
	Vector vMove2;
	g_Math.VectorAngles3D(vMove, vMove2);

	flYaw = DEG2RAD(Viewangs.y - vOldAngles.y + vMove2.y);
	pCmd->forwardmove = cos(flYaw) * flSpeed;
	pCmd->sidemove = sin(flYaw) * flSpeed;

	if (Viewangs.x < -90.f || Viewangs.x > 90.f)
		pCmd->forwardmove = -pCmd->forwardmove;
}
extern Vector GetHitboxPOS(CPlayer* Enemy, int HitboxID);

bool anyAlive()
{
	for (INT i = 0; i <= Interfaces.pEntList->GetHighestEntityIndex(); i++)
	{
		CBaseEntity* pEntity = (CBaseEntity*)Interfaces.pEntList->GetClientEntity(i);
		if (pEntity == NULL)
			continue;
		if (!pEntity->isAlive())
			continue;
		if (pEntity == Hacks.LocalPlayer)
			continue;
		if (pEntity->IsDormant())
			continue;
		if (pEntity->GetTeam() == Hacks.LocalPlayer->GetTeam())
			continue;
		player_info_t info;
		if (!(Interfaces.pEngine->GetPlayerInfo(pEntity->GetIndex(), &info)))
			continue;
		return true;
	}
	return false;
}

void Createmove_Hacks::MovePacket(CInput* thisptr, void* _EAX, int sequence_number, float input_sample_frametime, bool active)
{
	if (Interfaces.pEngine->IsConnected() && Interfaces.pEngine->IsInGame())
	{
#pragma region Get User Cmd
		CInput::CUserCmd* cmdlist = *(CInput::CUserCmd**)((DWORD)Interfaces.pInput + 0xEC);
		CInput::CUserCmd* cmd = &cmdlist[sequence_number % 150];
		CInput::CVerifiedUserCmd* verifiedCmdList = *(CInput::CVerifiedUserCmd**)((DWORD)Interfaces.pInput + 0xF0);
		CInput::CVerifiedUserCmd* VerifiedCmd = &verifiedCmdList[sequence_number % 150];
#pragma endregion

#pragma region Sort Out Hack Manger
		Hacks.CurrentCmd = cmd;
		Name_Stealer();

		Vector oldAngles = Hacks.CurrentCmd->viewangles;
		if (!Hacks.LocalPlayer)return;
		Hacks.LocalWeapon = Hacks.LocalPlayer->GetActiveBaseCombatWeapon();
		if (!Hacks.LocalWeapon) return;
#pragma endregion
		Vector OrigAng = Hacks.CurrentCmd->viewangles;
#pragma region Do Hacks
		if (Hacks.LocalPlayer)
		{

			if (Hacks.LocalPlayer->isAlive())
			{
				bool alive = anyAlive();


				Settings.weaponconfigs();

				// Had to comment this out since you didn't push code for snakestrafer. 
				/*
				if (Settings.GetSetting(Tab_Misc, Misc_CircleStrafe_Mode))
				SnakeStrafer(OrigAng);
				*/

				if (!CircleStrafer(OrigAng)) {
					if (Settings.GetBoolSetting(Tab_Misc, Misc_Auto_Strafer))
						AutoStrafer();
				}

				if (Settings.GetBoolSetting(Tab_Misc, Misc_Bunny_Hop))
					BunnyHop();

				if (!(Settings.GetSetting(Tab_Ragebot, Ragebot_AntiAimTargets))) {
					alive = true;
				}

				if (alive)
				{
#pragma region Prediction
					// Can Do Prediction Because We Have The Move Helper, If Not Pattern Search It And Try Again Next Packet
					if (Interfaces.g_pMoveHelper)
					{
						int TickBase = Hacks.LocalPlayer->GetTickBase(); // Get The Tick Base
						int Old_Time = Interfaces.pGlobalVars->curtime; // Get Current Time (Client Not Server)
						int Old_Frame_Time = Interfaces.pGlobalVars->frametime; // Get Frame Time (Client)

						Interfaces.pGlobalVars->curtime = TickBase * Interfaces.pGlobalVars->interval_per_tick; // Set Time To Server Time
						Interfaces.pGlobalVars->frametime = Interfaces.pGlobalVars->interval_per_tick; // Set Framerate To Tick Rate

						CMoveData Move_Data; // Make A Move Data
						memset(&Move_Data, 0, sizeof(Move_Data));
						bool Ducked = false;
						if (Hacks.CurrentCmd->buttons & IN_DUCK)
							Ducked = true;
						int duvkval = Settings.GetSetting(Tab_Ragebot, Ragebot_Duck);
						switch (duvkval) {
						case 2:
							Hacks.CurrentCmd->buttons &= ~IN_DUCK;
							break;
						}
						Interfaces.g_pMoveHelper->SetHost(Hacks.LocalPlayer); // Set Myself To Get Predicted
						Interfaces.g_pPred->SetupMove(Hacks.LocalPlayer, Hacks.CurrentCmd, Interfaces.g_pMoveHelper, &Move_Data); // Setup Prediction
						Interfaces.g_pGameMovement->ProcessMovement(Hacks.LocalPlayer, &Move_Data); // Process Prediction
						Interfaces.g_pPred->FinishMove(Hacks.LocalPlayer, Hacks.CurrentCmd, &Move_Data); // Finish Prediction
						Interfaces.g_pGameMovement->DecayPunchAngles();
						Interfaces.g_pMoveHelper->SetHost(NULL); // Remove Self From Move Helper
						if (Ducked)
							Hacks.CurrentCmd->buttons |= IN_DUCK;

						Interfaces.pGlobalVars->curtime = Old_Time;  // Reset Times To Correct Time
						Interfaces.pGlobalVars->frametime = Old_Frame_Time; // Reset Frame Time To Correct Time

						CBaseCombatWeapon* pWeapon = Hacks.LocalPlayer->GetActiveBaseCombatWeapon();
						if (pWeapon)
							pWeapon->UpdateAccuracyPenalty();
					}
					else
					{
						// if we dont have a pointer to Move Helper, Find One.
						Interfaces.g_pMoveHelper = **(IMoveHelper***)(Utils.PatternSearch("client.dll", (PBYTE)"\x8B\x0D\x00\x00\x00\x00\x8B\x45\x00\x51\x8B\xD4\x89\x02\x8B\x01", "xx????xx?xxxxxxx", NULL, NULL) + 2);
					}
#pragma endregion Player And Others Prediction


					if (Settings.GetBoolSetting(Tab_Ragebot, Ragebot_Enable))
						Ragebot.NewMove();


					int duvkval = Settings.GetSetting(Tab_Ragebot, Ragebot_Duck);
					switch (duvkval) {
					case 2:
						if (Hacks.CurrentCmd->buttons & IN_ATTACK)
							Hacks.CurrentCmd->buttons &= ~IN_DUCK;
						break;
					}
				}
				if (Settings.GetSetting(Tab_LegitBot, Legit_Activate) || Settings.GetBoolSetting(Tab_LegitBot, Trigger_Active)) {
					if (Settings.GetSetting(Tab_LegitBot, Legit_ME))
						Legitbot.Move();
					else
						dolegitshit();
				}

				//if(Settings.GetSetting(Tab_LegitBot, Old_Legit_Activate))
				//dolegitshit();

				Airstuck();

				/* AutoPistol */
				if (Settings.GetSetting(Tab_LegitBot, Legit_AutoPistol)) {
					if ((Hacks.CurrentCmd->buttons & IN_ATTACK) && (Hacks.LocalWeapon->NextPrimaryAttack()
						- ((float)Hacks.LocalPlayer->GetTickBase()
							* Interfaces.pGlobalVars->interval_per_tick) > 0) && Hacks.LocalWeapon->isPistoleW()) Hacks.CurrentCmd->buttons &= ~IN_ATTACK;
				}

				/* Custom Sky */
				if (Settings.GetSetting(Tab_Misc, Misc_CustomSky)) {
					for (MaterialHandle_t i = Interfaces.pMaterialSystem->FirstMaterial(); i != Interfaces.pMaterialSystem->InvalidMaterial(); i = Interfaces.pMaterialSystem->NextMaterial(i))
					{
						IMaterial *pMaterial = Interfaces.pMaterialSystem->GetMaterial(i);

						if (!pMaterial)
							continue;

						if (strstr(pMaterial->GetTextureGroupName(), "SkyBox textures"))
						{
							pMaterial->ColorModulate(Settings.GetSetting(Tab_Other, Other_Misc_Sky_Red) / 255,
								Settings.GetSetting(Tab_Other, Other_Misc_Sky_Green) / 255,
								Settings.GetSetting(Tab_Other, Other_Misc_Sky_Blue) / 255);
						}
					}
				}





				/* No Post Processing */
				void nopost();
				ConVar* postprocess = Interfaces.g_ICVars->FindVar("mat_postprocess_enable");
				{
					if (Settings.GetSetting(Tab_Misc, Misc_PostProcessing)) {
						if (postprocess)
						{
							postprocess->SetValue(0);
						}
					}
					else {
						if (postprocess)
						{
							postprocess->SetValue(1);
						}
					}
				}





				/* Custom Walls */
				if (Settings.GetSetting(Tab_Misc, Misc_AsusWalls)) {
					{
						for (MaterialHandle_t i = Interfaces.pMaterialSystem->FirstMaterial(); i != Interfaces.pMaterialSystem->InvalidMaterial(); i = Interfaces.pMaterialSystem->NextMaterial(i))
						{
							IMaterial *pMaterial = Interfaces.pMaterialSystem->GetMaterial(i);

							if (!pMaterial)
								continue;

							if (strstr(pMaterial->GetTextureGroupName(), "World")) {
								pMaterial->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Misc_CustomWalls_Alpha) / 255);
								pMaterial->ColorModulate(Settings.GetSetting(Tab_Other, Other_Misc_CustomWalls_Red) / 255,
									Settings.GetSetting(Tab_Other, Other_Misc_CustomWalls_Green) / 255,
									Settings.GetSetting(Tab_Other, Other_Misc_CustomWalls_Blue) / 255);
							}

						}
					}
				}


				void bypass();
				ConVar* cheats = Interfaces.g_ICVars->FindVar("sv_cheats");
				{
					if (Settings.GetSetting(Tab_Misc, Misc_Bypass)) {
						if (cheats)
						{
							cheats->SetValue(1);
						}
					}
					else {
						if (cheats)
						{
							cheats->SetValue(0);
						}
					}
				}



				if (Settings.GetSetting(Tab_Misc, Misc_EnableFOV)) {

					int viewmodelfovvalue = Settings.GetSetting(Tab_Misc, Misc_FovValue);

					ConVar* viewmodelfovcvar = Interfaces.g_ICVars->FindVar("viewmodel_fov");
					*(int*)((DWORD)&viewmodelfovcvar->fnChangeCallback + 0xC) = NULL;
					viewmodelfovcvar->SetValue(viewmodelfovvalue);

				}
				
			


				float asusvalue = Settings.GetSetting(Tab_Misc, Misc_AsusWallsValue);
				float max1f = 1.f;
				
				if (Settings.GetBoolSetting(Tab_Misc, Misc_AsusWalls1))
				{
					for (MaterialHandle_t i = Interfaces.pMaterialSystem->FirstMaterial(); i != Interfaces.pMaterialSystem->InvalidMaterial(); i = Interfaces.pMaterialSystem->NextMaterial(i))
					{
						IMaterial *pMaterial = Interfaces.pMaterialSystem->GetMaterial(i);

						if (!pMaterial)
							continue;

						if (strstr(pMaterial->GetTextureGroupName(), "World")) {
							pMaterial->AlphaModulate(asusvalue);
							pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							pMaterial->ColorModulate(max1f, max1f, max1f);
						}

					}
				}





				if (Settings.GetBoolSetting(Tab_Misc, Misc_WireframeWalls))
				{
					for (MaterialHandle_t i = Interfaces.pMaterialSystem->FirstMaterial(); i != Interfaces.pMaterialSystem->InvalidMaterial(); i = Interfaces.pMaterialSystem->NextMaterial(i))
					{
						IMaterial *pMaterial = Interfaces.pMaterialSystem->GetMaterial(i);

						if (!pMaterial)
							continue;

						if (strstr(pMaterial->GetTextureGroupName(), "World")) {
							pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
							pMaterial->ColorModulate(max1f, max1f, max1f);
						}

					}
				}





				if (Settings.GetSetting(Tab_Misc, Misc_ViewEnable)) {

					int viewX = Settings.GetSetting(Tab_Misc, Misc_ViewX);

					ConVar* viewXcvar = Interfaces.g_ICVars->FindVar("viewmodel_offset_x");
					*(int*)((DWORD)&viewXcvar->fnChangeCallback + 0xC) = NULL;
					viewXcvar->SetValue(viewX);


					int viewY = Settings.GetSetting(Tab_Misc, Misc_ViewY);

					ConVar* viewYcvar = Interfaces.g_ICVars->FindVar("viewmodel_offset_y");
					*(int*)((DWORD)&viewYcvar->fnChangeCallback + 0xC) = NULL;
					viewYcvar->SetValue(viewY);


					int viewZ = Settings.GetSetting(Tab_Misc, Misc_ViewZ);

					ConVar* viewZcvar = Interfaces.g_ICVars->FindVar("viewmodel_offset_z");
					*(int*)((DWORD)&viewZcvar->fnChangeCallback + 0xC) = NULL;
					viewZcvar->SetValue(viewZ);

				}




				if (Settings.GetSetting(Tab_Misc, Misc_HJitter)) {
					ConVar* hands = Interfaces.g_ICVars->FindVar("cl_righthand");

					static bool H_Jitter;
					if (H_Jitter) {
						hands->SetValue(1);
					}
					else
					{
						hands->SetValue(0);
					}
					H_Jitter = !H_Jitter;
				}


				if (cmd->buttons & IN_SCORE && Settings.GetSetting(Tab_Misc, Misc_RevRanks))
				{
					int fArray[3] = { 0,0,0 };
					MsgFunc_ServerRankRevealAll();
				}



#pragma region AntiUntrust
				if (Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust))
				{
					Ragebot.FastLargeNormalise(Hacks.CurrentCmd->viewangles);
					while (Hacks.CurrentCmd->viewangles.y < -180.0f) Hacks.CurrentCmd->viewangles.y += 360.0f;
					while (Hacks.CurrentCmd->viewangles.y > 180.0f) Hacks.CurrentCmd->viewangles.y -= 360.0f;
					if (Hacks.CurrentCmd->viewangles.x > 89.0f) Hacks.CurrentCmd->viewangles.x = 89.0f;
					if (Hacks.CurrentCmd->viewangles.x < -89.0f) Hacks.CurrentCmd->viewangles.x = -89.0f;
					Hacks.CurrentCmd->viewangles.z = 0;
				}
#pragma endregion 
				Vector simplifiedang = Hacks.CurrentCmd->viewangles;
				Vector simplifiedorg = OrigAng;
				if (!Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust))
				{

					Ragebot.FastLargeNormaliseMove(simplifiedang);
					while (simplifiedang.y < -180.0f) simplifiedang.y += 360.0f;
					while (simplifiedang.y > 180.0f) simplifiedang.y -= 360.0f;

					Ragebot.FastLargeNormaliseMove(simplifiedorg);
					while (simplifiedorg.y < -180.0f) simplifiedorg.y += 360.0f;
					while (simplifiedorg.y > 180.0f) simplifiedorg.y -= 360.0f;

				}
				CorrectMovement(simplifiedorg, Hacks.CurrentCmd, simplifiedang);
				Hacks.LastAngles = Hacks.CurrentCmd->viewangles;
				if (!Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust) && alive) {
					//nothing
				}
				if (Settings.GetSetting(Tab_Misc, Misc_Anti_Untrust))
				{
					Ragebot.FastLargeNormalise(Hacks.CurrentCmd->viewangles);
					while (Hacks.CurrentCmd->viewangles.y < -180.0f) Hacks.CurrentCmd->viewangles.y += 360.0f;
					while (Hacks.CurrentCmd->viewangles.y > 180.0f) Hacks.CurrentCmd->viewangles.y -= 360.0f;
					if (Hacks.CurrentCmd->viewangles.x > 89.0f) Hacks.CurrentCmd->viewangles.x = 89.0f;
					if (Hacks.CurrentCmd->viewangles.x < -89.0f) Hacks.CurrentCmd->viewangles.x = -89.0f;
					Hacks.CurrentCmd->viewangles.z = 0;
				}


			}

		}
#pragma endregion

#pragma region Set Cmds
		VerifiedCmd->m_cmd = *Hacks.CurrentCmd;
		VerifiedCmd->m_crc = Hacks.CurrentCmd->GetChecksum();
#pragma endregion
	}
}
#pragma endregion Movement Related Hacks

#pragma region Paint Traverse



bool PaintTraverse_Hacks::PaintTraverse_IsGoodPlayer_ESP(CBaseEntity* Target)
{
	if (!Target)
		return false;

	if (Target == Hacks.LocalPlayer)
		return false;

	if (Target->IsDormant())
		return false;

	if (!Target->IsPlayer())
		return false;

	if (Target->GetHealth() == 0)
		return false;

	if (!Target->isAlive())
		return false;


	return true;
}
struct DHitbox
{
	DHitbox(void)
	{
		dhitbox = -1;
	}

	DHitbox(int newHitBox)
	{
		dhitbox = newHitBox;
	}

	int  dhitbox;
	Vector points[9];
};
bool GetHitbox(CBaseEntity* Player, DHitbox *box)
{
	if (!Player)
		return false;

	matrix3x4 matrix[128];
	if (!Player->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
		return false;
	const model_t* mod = Player->GetModel();
	if (!mod)
		return false;
	studiohdr_t* hdr = Interfaces.g_pModelInfo->GetStudioModel(mod);
	if (!hdr)
		return false;
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	if (!set)
		return false;
	mstudiobbox_t* hitbox = set->pHitbox(box->dhitbox);
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
void DFixHitbox(DHitbox* box)
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
extern Vector pointaim;
extern Vector Delta2Screen(Vector delta, int w, int h);
bool drawhitbox(CBaseEntity* Player) {

	Vector a;
	Vector b;

	if (!Player)
		return 0;

	matrix3x4 matrix[128];
	if (!Player->SetupBones(matrix, 128, 0x00000100, GetTickCount64())) return 0;
	const model_t* mod = Player->GetModel();
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
	Interfaces.g_pDebugOverlay->ScreenPosition(vMin, a);
	Interfaces.g_pDebugOverlay->ScreenPosition(vMax, b);

	Interfaces.pSurface->DrawSetColor(0, 190, 150, 255);
	Interfaces.pSurface->DrawFilledRect(a.x, a.y, a.x + 4, a.y + 4);
	Interfaces.pSurface->DrawSetColor(0, 9, 255, 255);
	Interfaces.pSurface->DrawFilledRect(b.x, b.y, b.x + 4, b.y + 4);


	return true;
}

void Draw_Corner_Box_Faster(ESP_Box Box, CBaseEntity* Target)
{
	int Line_Size = (Box.y - Box.h) / 8;
	int Line_Size2 = (Box.y - Box.h) / 6;

	//top inwards
	Interfaces.pSurface->DrawLine(Box.w, Box.y, Box.w + Line_Size, Box.y);
	Interfaces.pSurface->DrawLine(Box.x, Box.y, Box.x - Line_Size, Box.y);

	//top downwards
	Interfaces.pSurface->DrawLine(Box.x, Box.y, Box.x, Box.y - Line_Size);
	Interfaces.pSurface->DrawLine(Box.w, Box.y, Box.w, Box.y - Line_Size);

	//bottom inwards
	Interfaces.pSurface->DrawLine(Box.x, Box.h, Box.x - Line_Size, Box.h);
	Interfaces.pSurface->DrawLine(Box.w, Box.h, Box.w + Line_Size, Box.h);

	//bottom upwards
	Interfaces.pSurface->DrawLine(Box.x, Box.h, Box.x, Box.h + Line_Size);
	Interfaces.pSurface->DrawLine(Box.w, Box.h, Box.w, Box.h + Line_Size);

	//outlines
	Interfaces.pSurface->DrawSetColor(0, 0, 0, 200);

	//top inwards
	Interfaces.pSurface->DrawLine(Box.w, Box.y - 1, Box.w + Line_Size, Box.y - 1);
	Interfaces.pSurface->DrawLine(Box.x, Box.y - 1, Box.x - Line_Size, Box.y - 1);
	//inlines
	Interfaces.pSurface->DrawLine(Box.w - 1, Box.y + 1, Box.w + Line_Size, Box.y + 1);
	Interfaces.pSurface->DrawLine(Box.x + 1, Box.y + 1, Box.x - Line_Size, Box.y + 1);

	// top downwards
	Interfaces.pSurface->DrawLine(Box.x - 1, Box.y - 1, Box.x - 1, Box.y - Line_Size);
	Interfaces.pSurface->DrawLine(Box.w + 1, Box.y - 1, Box.w + 1, Box.y - Line_Size);
	//inlines
	Interfaces.pSurface->DrawLine(Box.x + 1, Box.y, Box.x + 1, Box.y - Line_Size);
	Interfaces.pSurface->DrawLine(Box.w - 1, Box.y, Box.w - 1, Box.y - Line_Size);

	//bottom inwards
	Interfaces.pSurface->DrawLine(Box.x, Box.h + 1, Box.x - Line_Size, Box.h + 1);
	Interfaces.pSurface->DrawLine(Box.w, Box.h + 1, Box.w + Line_Size, Box.h + 1);
	//inlines
	Interfaces.pSurface->DrawLine(Box.x + 1, Box.h - 1, Box.x - Line_Size, Box.h - 1);
	Interfaces.pSurface->DrawLine(Box.w - 1, Box.h - 1, Box.w + Line_Size, Box.h - 1);

	//bottom upwards
	Interfaces.pSurface->DrawLine(Box.x - 1, Box.h + 1, Box.x - 1, Box.h + Line_Size);
	Interfaces.pSurface->DrawLine(Box.w + 1, Box.h + 1, Box.w + 1, Box.h + Line_Size);
	//inlines
	Interfaces.pSurface->DrawLine(Box.x + 1, Box.h, Box.x + 1, Box.h + Line_Size);
	Interfaces.pSurface->DrawLine(Box.w - 1, Box.h, Box.w - 1, Box.h + Line_Size);

}

ESP_Box PaintTraverse_Hacks::Get_Box(CBaseEntity* Entity)
{
	ESP_Box result;
	Vector vEye = Entity->GetBonePos(6);
	Vector vPelvis = GetHitboxPOS_Ent(Entity, (int)CSGOHitboxID::Pelvis);
	Vector vFoot = GetHitboxPOS_Ent(Entity, (int)CSGOHitboxID::LeftFoot);

	Vector LvEye = vEye;
	if (vPelvis.z - vFoot.z > 30)
		LvEye.z -= 77;
	else
		LvEye.z -= 57;
	vEye += Vector(0, 0, 10);
	Vector ScreenTop, ScreenBottom;
	Interfaces.g_pDebugOverlay->ScreenPosition(vEye, ScreenTop);
	Interfaces.g_pDebugOverlay->ScreenPosition(LvEye, ScreenBottom);
	int height = ScreenBottom.y - ScreenTop.y;
	int Width = height / 2;
	ScreenTop.x += Width / 2;
	ScreenBottom.x -= Width / 2;
	result.x = ScreenBottom.x;
	result.y = ScreenTop.y;
	result.w = ScreenTop.x;
	result.h = ScreenBottom.y;
	return result;
}

bool PaintTraverse_Hacks::IsVisable(CBaseEntity* Target, int boneID)
{
	CBaseEntity* copy = Target;
	trace_t Trace;
	Vector start = Hacks.LocalPlayer->GetEyePosition();
	Vector end = GetHitboxPOS_Ent(Target, boneID);
	UTIL_TraceLine(start, end, MASK_SOLID, Hacks.LocalPlayer, 0, &Trace);
	if (Trace.m_pEnt == copy) return true;
	if (Trace.fraction == 1.0f) return true;
	return false;
}

float sepssub(Vector source, Vector pos) {
	Vector dist;
	dist.x = abs(source.x - pos.x);
	dist.y = abs(source.y - pos.y);
	float xx = dist.x*dist.x + dist.y*dist.y;
	float distance = sqrt(xx);
	return distance;
}

extern float distance(Vector source, Vector pos);

RECT GetTextSize(DWORD font, const char* text) {
	size_t origsize = strlen(text) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);
	RECT rect; int x, y;
	Interfaces.pSurface->GetTextSize(font, wcstring, x, y);
	rect.left = x; rect.bottom = y;
	rect.right = x;
	return rect;
}

void PaintTraverse_Hacks::DrawBox(CBaseEntity* Target, int r, int g, int b, int a)
{//
 //drawhitbox(Target);
	int Health = Target->GetHealth();
	int Armor = Target->GetArmorValue();
	ESP_Box	BOX = Get_Box(Target);
	int Line_Size = (BOX.y - BOX.h) / 8;
	int Line_Size2 = (BOX.y + BOX.h) / 8;
	if (Settings.GetSetting(Tab_Visuals, Vis_Box) != 0 || Settings.GetSetting(Tab_Visuals, Vis_Box) != 1)
	{
		Interfaces.pSurface->DrawSetColor(r, g, b, a);
		int swi = Settings.GetSetting(Tab_Visuals, Vis_Box);
		switch (swi) {
		case 2:
			Interfaces.pSurface->DrawSetColor(r, g, b, a);
			Interfaces.pSurface->DrawOutlinedRect(BOX.x, BOX.y, BOX.w, BOX.h);
			Interfaces.pSurface->DrawSetColor(0, 0, 0, 185);
			Interfaces.pSurface->DrawOutlinedRect(BOX.x - 1, BOX.y - 1, BOX.w + 1, BOX.h + 1);
			Interfaces.pSurface->DrawOutlinedRect(BOX.x + 1, BOX.y + 1, BOX.w - 1, BOX.h - 1);
			Interfaces.pSurface->DrawSetColor(r, g, b, a);
			break;
		case 3:
			Draw_Corner_Box_Faster(BOX, Target);
			break;
		}
	}
	if (Settings.GetSetting(Tab_Visuals, Vis_Health)) {
		if (Health > 100)
			Health = 100;
		int healthG = Health * 2.55;
		int healthR = 255 - healthG;
		Interfaces.pSurface->DrawSetColor(0, 0, 0, 150);
		Interfaces.pSurface->DrawOutlinedRect(BOX.x - 5, BOX.y - 1, BOX.x - 1, BOX.h + 1);
		Interfaces.pSurface->DrawOutlinedRect(BOX.w + 1, BOX.y - 1, BOX.w + 5, BOX.h + 1);

		double Filler = BOX.y - BOX.h;
		Filler /= 100;
		Filler *= Health;
		int pos = Filler + BOX.h;

		double Filler1 = BOX.y - BOX.h;
		Filler1 /= 100;
		Filler1 *= Armor;
		int pos1 = Filler1 + BOX.h;

		if (Health > 0) {
			Interfaces.pSurface->DrawSetColor(healthR, healthG, 0, 255);
			Interfaces.pSurface->DrawOutlinedRect(BOX.x - 4, pos, BOX.x - 2, BOX.h);
		}

		if (Armor > 0) {
			Interfaces.pSurface->DrawSetColor(80, 135, 190, 255);
			Interfaces.pSurface->DrawOutlinedRect(BOX.w + 2, pos1, BOX.w + 4, BOX.h);
		}
	}

	int aTextRed = Settings.GetSetting(Tab_Other, Other_TextRed);
	int aTextGreen = Settings.GetSetting(Tab_Other, Other_TextGreen);
	int aTextBlue = Settings.GetSetting(Tab_Other, Other_TextBlue);
	int aTextAlpha = Settings.GetSetting(Tab_Other, Other_TextAlpha);
	if (Settings.GetSetting(Tab_Visuals, Vis_Name))
	{
		Interfaces.pEngine->GetPlayerInfo(Target->GetIndex(), Info);
		RECT nameSize = GetTextSize(Hacks.Font_ESP, Info->m_szPlayerName);
		Interfaces.pSurface->DrawT(((BOX.x + BOX.w) / 2) - (nameSize.right / 2), BOX.y - 16, CColor(255, 255, 255, 255), Hacks.Font_ESP, false, Info->m_szPlayerName);
	}

	if (Settings.GetBoolSetting(Tab_Visuals, Vis_Weapon))
	{
		CBaseCombatWeapon* Weapon = Target->GetActiveBaseCombatWeapon();
		RECT textSize = GetTextSize(Hacks.Font_ESP, Weapon->GetWeaponName());
		Interfaces.pSurface->DrawT(((BOX.x + BOX.w) / 2) - (textSize.right / 2), BOX.h + 2, CColor(255, 255, 255, 255), Hacks.Font_ESP, false, Weapon->GetWeaponName());
	}
	if (Settings.GetSetting(Tab_Visuals, Vis_Background)) {
		int Health = Target->GetHealth();
		char hp[256];
		sprintf(hp, "%d", Health);
		if (Settings.GetSetting(Tab_Visuals, Vis_Health)) {
			Interfaces.pSurface->DrawT(BOX.w + 13, BOX.y, CColor(255, 255, 255, 255), Hacks.Font_ESP, false, hp);
			Interfaces.pSurface->DrawT(BOX.w + 33, BOX.y, CColor(0, 200, 255, 255), Hacks.Font_ESP, false, "HP");
		}
		else {
			Interfaces.pSurface->DrawT(BOX.w + 3, BOX.y, CColor(255, 255, 255, 255), Hacks.Font_ESP, false, hp);
			Interfaces.pSurface->DrawT(BOX.w + 23, BOX.y, CColor(0, 200, 255, 255), Hacks.Font_ESP, false, "HP");
		}
	}
	if (Settings.GetSetting(Tab_Visuals, Vis_AimLines)) {
		Interfaces.pSurface->DrawSetColor(r, g, b, a);
		int snapsize = (BOX.y - BOX.h) / 4;
		int width = 0;
		int height = 0;
		Interfaces.pEngine->GetScreenSize(width, height);
		//Shanxs said do from cross hair befor eit was height - 10
		Interfaces.pSurface->DrawLine(width / 2, height - 10, BOX.x - snapsize, BOX.h);
	}
	float dist = distance(Target->GetEyePosition(), Hacks.LocalPlayer->GetEyePosition());
}
void PaintTraverse_Hacks::DrawSkeleton(CBaseEntity* pEntity, CColor col)
{
	if (Settings.GetSetting(Tab_Visuals, Vis_Skel)) {
		studiohdr_t* pStudioHdr = Interfaces.g_pModelInfo->GetStudioModel(pEntity->GetModel());

		if (!pStudioHdr)
			return;

		Vector vParent, vChild, sParent, sChild;

		for (int j = 0; j < pStudioHdr->numbones; j++)
		{
			mstudiobone_t* pBone = pStudioHdr->GetBone(j);

			if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
			{

				vChild = pEntity->GetBonePos(j);
				vParent = pEntity->GetBonePos(pBone->parent);
				Interfaces.g_pDebugOverlay->ScreenPosition(vParent, sParent);
				Interfaces.g_pDebugOverlay->ScreenPosition(vChild, sChild);
				int red = 0;
				int green = 0;
				int blue = 0;
				int alpha = 0;
				col.GetColor(red, green, blue, alpha);
				Interfaces.pSurface->DrawSetColor(red, green, blue, alpha);
				Interfaces.pSurface->DrawLine(sParent[0], sParent[1], sChild[0], sChild[1]);
			}
		}
	}
}
bool cclassweapon(int val) {
	bool ret = false;
	if (val == (int)CSGOClassID::CAK47
		|| val >= 200 && val <= 250
		|| val == (int)CSGOClassID::CSCAR17
		|| val == (int)CSGOClassID::CDEagle)
		ret = true;
	return ret;
}

void PaintTraverse_Hacks::DrawESP()
{
	if (!Hacks.LocalPlayer) return;

	CBaseEntity* Target;
	int Max_Entitys = Interfaces.pEntList->GetHighestEntityIndex();

	/* Paint Shit */

	static DWORD dwDrawOnRadar = *(DWORD*)(DWORD)Utils.PatternSearch("client.dll", (PBYTE)"\xA1\x00\x00\x00\x00\x85\xC0\x74\x06\x05", "x????xxxxx", NULL, NULL);

	for (int i = 0; i < Max_Entitys; i++)
	{
		Target = Interfaces.pEntList->GetClientEntity(i);

		if (!Target)
			continue;

		ClientClass* cClass = (ClientClass*)Target->GetClientClass();

		if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
		{
			if (!PaintTraverse_IsGoodPlayer_ESP(Target))
				continue;
			if (false)//ingame radar
			{

				//BYTE* pDrawOnRadar = (BYTE*)((*(DWORD*)dwDrawOnRadar) + 0xD1D + i);
				// 1 = draw on radar
				//pDrawOnRadar = 1;
			}
			int Red;
			int Blue;
			int Green;
			int Alpha;
			bool seeable = IsVisable(Target, 0);
			if (Target->GetTeam() == Hacks.LocalPlayer->GetTeam()) {
				if (Settings.GetSetting(Tab_Visuals, Vis_Box_Team)) {
					if (Target->GetTeam() == 3) {
						if (seeable) {
							Red = Settings.GetSetting(Tab_Other, Other_Esp_CT_Visible_Red);
							Green = Settings.GetSetting(Tab_Other, Other_Esp_CT_Visible_Green);
							Blue = Settings.GetSetting(Tab_Other, Other_Esp_CT_Visible_Blue);
							Alpha = Settings.GetSetting(Tab_Other, Other_Esp_CT_Visible_Alpha);
							DrawBox(Target, Red, Green, Blue, Alpha);
							DrawSkeleton(Target, CColor(Red, Green, Blue, Alpha));
						}
						else {
							Red = Settings.GetSetting(Tab_Other, Other_Esp_CT_inVisible_Red);
							Green = Settings.GetSetting(Tab_Other, Other_Esp_CT_inVisible_Green);
							Blue = Settings.GetSetting(Tab_Other, Other_Esp_CT_inVisible_Blue);
							Alpha = Settings.GetSetting(Tab_Other, Other_Esp_CT_inVisible_Alpha);
							if (!Settings.GetSetting(Tab_Visuals, Vis_Box_Vis)) {
								DrawBox(Target, Red, Green, Blue, Alpha);
								DrawSkeleton(Target, CColor(Red, Green, Blue, Alpha));
							}
						}
					}
					else {
						if (seeable) {
							Red = Settings.GetSetting(Tab_Other, Other_Esp_T_Visible_Red);
							Green = Settings.GetSetting(Tab_Other, Other_Esp_T_Visible_Green);
							Blue = Settings.GetSetting(Tab_Other, Other_Esp_T_Visible_Blue);
							Alpha = Settings.GetSetting(Tab_Other, Other_Esp_T_Visible_Alpha);
							DrawBox(Target, Red, Green, Blue, Alpha);
							DrawSkeleton(Target, CColor(Red, Green, Blue, Alpha));
						}
						else {
							Red = Settings.GetSetting(Tab_Other, Other_Esp_T_inVisible_Red);
							Green = Settings.GetSetting(Tab_Other, Other_Esp_T_inVisible_Green);
							Blue = Settings.GetSetting(Tab_Other, Other_Esp_T_inVisible_Blue);
							Alpha = Settings.GetSetting(Tab_Other, Other_Esp_T_inVisible_Alpha);
							if (!Settings.GetSetting(Tab_Visuals, Vis_Box_Vis)) {
								DrawBox(Target, Red, Green, Blue, Alpha);
								DrawSkeleton(Target, CColor(Red, Green, Blue, Alpha));
							}
						}
					}
				}
			}
			else {
				if (Target->GetTeam() == 3) {
					if (seeable) {
						Red = Settings.GetSetting(Tab_Other, Other_Esp_CT_Visible_Red);
						Green = Settings.GetSetting(Tab_Other, Other_Esp_CT_Visible_Green);
						Blue = Settings.GetSetting(Tab_Other, Other_Esp_CT_Visible_Blue);
						Alpha = Settings.GetSetting(Tab_Other, Other_Esp_CT_Visible_Alpha);
						DrawBox(Target, Red, Green, Blue, Alpha);
						DrawSkeleton(Target, CColor(Red, Green, Blue, Alpha));
					}
					else {
						Red = Settings.GetSetting(Tab_Other, Other_Esp_CT_inVisible_Red);
						Green = Settings.GetSetting(Tab_Other, Other_Esp_CT_inVisible_Green);
						Blue = Settings.GetSetting(Tab_Other, Other_Esp_CT_inVisible_Blue);
						Alpha = Settings.GetSetting(Tab_Other, Other_Esp_CT_inVisible_Alpha);
						if (!Settings.GetSetting(Tab_Visuals, Vis_Box_Vis)) {
							DrawBox(Target, Red, Green, Blue, Alpha);
							DrawSkeleton(Target, CColor(Red, Green, Blue, Alpha));
						}
					}
				}
				else {
					if (seeable) {
						Red = Settings.GetSetting(Tab_Other, Other_Esp_T_Visible_Red);
						Green = Settings.GetSetting(Tab_Other, Other_Esp_T_Visible_Green);
						Blue = Settings.GetSetting(Tab_Other, Other_Esp_T_Visible_Blue);
						Alpha = Settings.GetSetting(Tab_Other, Other_Esp_T_Visible_Alpha);
						DrawBox(Target, Red, Green, Blue, Alpha);
						DrawSkeleton(Target, CColor(Red, Green, Blue, Alpha));
					}
					else {
						Red = Settings.GetSetting(Tab_Other, Other_Esp_T_inVisible_Red);
						Green = Settings.GetSetting(Tab_Other, Other_Esp_T_inVisible_Green);
						Blue = Settings.GetSetting(Tab_Other, Other_Esp_T_inVisible_Blue);
						Alpha = Settings.GetSetting(Tab_Other, Other_Esp_T_inVisible_Alpha);
						if (!Settings.GetSetting(Tab_Visuals, Vis_Box_Vis)) {
							DrawBox(Target, Red, Green, Blue, Alpha);
							DrawSkeleton(Target, CColor(Red, Green, Blue, Alpha));
						}
					}
				}
			}

		}
				else if (cclassweapon(cClass->m_ClassID) && Settings.GetSetting(Tab_Visuals, Vis_DropWeapon)) {
					Vector vOrig; Vector vScreen;
					CBaseEntity* ent = Interfaces.pEntList->GetClientEntity(i);
					vOrig = ent->GetAbsOrigin();
					{
						CBaseCombatWeapon* weapon = (CBaseCombatWeapon*)ent;
						CColor col = CColor(255, 255, 255, 255);
						Interfaces.g_pDebugOverlay->ScreenPosition(vOrig, vScreen);

						Interfaces.pSurface->DrawT(vScreen.x, vScreen.y, col, Hacks.Font_ESP, false, weapon->GetPName());

					}
				}
	}
	/* Glow Shit */
	CGlowObjectManager* GlowObjectManager = (CGlowObjectManager*)Interfaces.g_pGlowObjectManager;
	CGlowObjectManager::GlowObjectDefinition_t* glowEntity;
	if (Settings.GetSetting(Tab_Visuals, Vis_Glow)) {
		for (int i = 0; i < GlowObjectManager->size; i++)
		{
			glowEntity = &GlowObjectManager->m_GlowObjectDefinitions[i];
			CBaseEntity* Entity = glowEntity->getEntity();

			if (!Entity) continue;

			ClientClass* cClass = (ClientClass*)Entity->GetClientClass();

			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (!PaintTraverse_IsGoodPlayer_ESP(Entity))
					continue;

				int Red;
				int Blue;
				int Green;
				int Alpha;
				bool seeable = IsVisable(Entity, 0);

				if (Entity->GetTeam() == Hacks.LocalPlayer->GetTeam()) {
					if (!seeable) {

						Red = Settings.GetSetting(Tab_Other, Other_Glow_inVisible_Red);
						Green = Settings.GetSetting(Tab_Other, Other_Glow_inVisible_Green);
						Blue = Settings.GetSetting(Tab_Other, Other_Glow_inVisible_Blue);
						Alpha = Settings.GetSetting(Tab_Other, Other_Glow_inVisible_Alpha);
						if (!Settings.GetSetting(Tab_Visuals, Vis_Glow_Vis) && Settings.GetBoolSetting(Tab_Visuals, Vis_Glow_Team)) {
							glowEntity->set(CColor(Red, Green, Blue, Alpha));

						}

					}
					else {
						Red = Settings.GetSetting(Tab_Other, Other_Glow_Visible_Red);
						Green = Settings.GetSetting(Tab_Other, Other_Glow_Visible_Green);
						Blue = Settings.GetSetting(Tab_Other, Other_Glow_Visible_Blue);
						Alpha = Settings.GetSetting(Tab_Other, Other_Glow_Visible_Alpha);
						if (Settings.GetBoolSetting(Tab_Visuals, Vis_Glow_Team))
							glowEntity->set(CColor(Red, Green, Blue, Alpha));

					}

				}
				else {
					if (!seeable) {

						Red = Settings.GetSetting(Tab_Other, Other_Glow_inVisible_Red);
						Green = Settings.GetSetting(Tab_Other, Other_Glow_inVisible_Green);
						Blue = Settings.GetSetting(Tab_Other, Other_Glow_inVisible_Blue);
						Alpha = Settings.GetSetting(Tab_Other, Other_Glow_inVisible_Alpha);
						if (!Settings.GetSetting(Tab_Visuals, Vis_Glow_Vis)) {
							glowEntity->set(CColor(Red, Green, Blue, Alpha));

						}
					}
					else {
						Red = Settings.GetSetting(Tab_Other, Other_Glow_Visible_Red);
						Green = Settings.GetSetting(Tab_Other, Other_Glow_Visible_Green);
						Blue = Settings.GetSetting(Tab_Other, Other_Glow_Visible_Blue);
						Alpha = Settings.GetSetting(Tab_Other, Other_Glow_Visible_Alpha);
						glowEntity->set(CColor(Red, Green, Blue, Alpha));

					}

				}
			}
		}
	}

}


	

	


void PaintTraverse_Hacks::DrawPaintTraverseHacks()
{
	Hacks.LocalPlayer = Interfaces.pEntList->GetClientEntity(Interfaces.pEngine->GetLocalPlayer());
	if (!Hacks.LocalPlayer) return;
	Hacks.LocalWeapon = Hacks.LocalPlayer->GetActiveBaseCombatWeapon();

	// Draw Cheats Here
	if (Settings.GetSetting(Tab_Visuals, Vis_Active)) {
		DrawESP();
		//bombESP();
	}
	if (Settings.GetSetting(Tab_Misc, Misc_RevSpec))
	{
		SpecList();
	}
}
#pragma endregion All Drawing Related Hacks



#pragma region Draw Model Execute

void Draw_Model_Exec_Hacks::InitKeyValues(KeyValues* keyValues, char* name)
{
	DWORD dwFunction = (DWORD)Utils.PatternSearch("client.dll", (PBYTE)"\x68\x00\x00\x00\x00\x8B\xC8\xE8\x00\x00\x00\x00\x89\x45\xFC\xEB\x07\xC7\x45\x00\x00\x00\x00\x00\x8B\x03\x56", "x????xxx????xxxxxxx?????xxx", NULL, NULL); dwFunction += 7; dwFunction = dwFunction + *reinterpret_cast<PDWORD_PTR>(dwFunction + 1) + 5;
	__asm
	{
		push name
		mov ecx, keyValues
		call dwFunction
	}
}

void Draw_Model_Exec_Hacks::LoadFromBuffer(KeyValues* keyValues, char const *resourceName, const char *pBuffer)
{
	DWORD dwFunction = (DWORD)Utils.PatternSearch("client.dll", (PBYTE)"\xE8\x00\x00\x00\x00\x8A\xD8\xFF\x15\x00\x00\x00\x00\x84\xDB", "x????xxxx????xx", NULL, NULL); dwFunction = dwFunction + *reinterpret_cast<PDWORD_PTR>(dwFunction + 1) + 5;
	__asm
	{
		push 0
		push 0
		push 0
		push pBuffer
		push resourceName
		mov ecx, keyValues
		call dwFunction
	}
}

IMaterial *Draw_Model_Exec_Hacks::Create_Material(bool Ignore, bool Lit, bool Wireframe)
{
	static int created = 0;
	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
	};

	char* baseType = (Lit == true ? "VertexLitGeneric" : "UnlitGeneric");
	char material[512];
	sprintf_s(material, sizeof(material), tmp, baseType, (Ignore) ? 1 : 0, (Wireframe) ? 1 : 0);

	char name[512];
	sprintf_s(name, sizeof(name), "#ayy_meme_%i.vmt", created); ++created;

	KeyValues* keyValues = (KeyValues*)malloc(sizeof(KeyValues));
	InitKeyValues(keyValues, baseType);
	LoadFromBuffer(keyValues, name, material);

	IMaterial *createdMaterial = Interfaces.pMaterialSystem->CreateMaterial(name, keyValues);
	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}

bool Draw_Model_Exec_Hacks::Do_Chams(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	static IMaterial* Covered_Lit = Create_Material(true, true, false);
	static IMaterial* Visable_Lit = Create_Material(false, true, false);



	/* Making Materials Is Slow, So Make Sure They Are Defined As Static */
	//fix this line below i changed it
	//from this bool flat = Settings.GetSetting(Tab_Visuals, Cham_Flat);
	bool flat = Settings.GetSetting(Tab_Visuals, Cham_Flat);
	Covered_Lit->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, flat);
	Visable_Lit->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, flat);


	static IMaterial* MethyModel = Create_Material(true, true, true);
	static IMaterial* MethyWire = Create_Material(true, true, false);

	const char* Model_Name = Interfaces.g_pModelInfo->GetModelName((model_t*)pInfo.pModel);


	std::string modelName = Interfaces.g_pModelInfo->GetModelName(pInfo.pModel);
	if (modelName.find("arms") != std::string::npos && Settings.GetSetting(Tab_Misc, Misc_MethyHands))
	{
		//	IMaterial* Hands = Interfaces.pMaterialSystem->FindMaterial(Model_Name, (TEXTURE_GROUP_MODEL));
		//	Hands->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
		////	Hands->ColorModulate(255, 255, 255);
		//	Hands->SetMaterialVarFlag(MATERIAL_VAR_MODEL, true);
		//	Hands->ColorModulate(0, 0, 0);
		//	Interfaces.g_pModelRender->ForcedMaterialOverride(Hands);


		MethyWire->ColorModulate(Settings.GetSetting(Tab_Other, Other_Misc_Hands_Red) / 255,
			Settings.GetSetting(Tab_Other, Other_Misc_Hands_Green) / 255,
			Settings.GetSetting(Tab_Other, Other_Misc_Hands_Blue) / 255);

		MethyWire->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Misc_Hands_Alpha) / 255);

		MethyModel->ColorModulate(Settings.GetSetting(Tab_Other, Other_Misc_Wire_Red) / 255,
			Settings.GetSetting(Tab_Other, Other_Misc_Wire_Green) / 255,
			Settings.GetSetting(Tab_Other, Other_Misc_Wire_Blue) / 255);

		MethyModel->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Misc_Wire_Alpha) / 255);

		Interfaces.g_pModelRender->ForcedMaterialOverride(MethyWire, OVERRIDE_NORMAL);
		Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

		Interfaces.g_pModelRender->ForcedMaterialOverride(MethyModel, OVERRIDE_NORMAL);
		Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

		return true;

	}



	if (modelName.find("arms") != std::string::npos && Settings.GetSetting(Tab_Misc, Misc_ChamHands))
	{
		//	IMaterial* Hands = Interfaces.pMaterialSystem->FindMaterial(Model_Name, (TEXTURE_GROUP_MODEL));
		//	Hands->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
		////	Hands->ColorModulate(255, 255, 255);
		//	Hands->SetMaterialVarFlag(MATERIAL_VAR_MODEL, true);
		//	Hands->ColorModulate(0, 0, 0);
		//	Interfaces.g_pModelRender->ForcedMaterialOverride(Hands);


		MethyWire->ColorModulate(Settings.GetSetting(Tab_Other, Other_Misc_Hands_Red) / 255,
			Settings.GetSetting(Tab_Other, Other_Misc_Hands_Green) / 255,
			Settings.GetSetting(Tab_Other, Other_Misc_Hands_Blue) / 255);

		MethyWire->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Misc_Hands_Alpha) / 255);

		Interfaces.g_pModelRender->ForcedMaterialOverride(MethyWire, OVERRIDE_NORMAL);
		Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

		return true;

	}



	if (modelName.find("arms") != std::string::npos && Settings.GetSetting(Tab_Misc, Misc_WireHands))
	{
		//	IMaterial* Hands = Interfaces.pMaterialSystem->FindMaterial(Model_Name, (TEXTURE_GROUP_MODEL));
		//	Hands->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
		////	Hands->ColorModulate(255, 255, 255);
		//	Hands->SetMaterialVarFlag(MATERIAL_VAR_MODEL, true);
		//	Hands->ColorModulate(0, 0, 0);
		//	Interfaces.g_pModelRender->ForcedMaterialOverride(Hands);


		MethyModel->ColorModulate(Settings.GetSetting(Tab_Other, Other_Misc_Wire_Red) / 255,
			Settings.GetSetting(Tab_Other, Other_Misc_Wire_Green) / 255,
			Settings.GetSetting(Tab_Other, Other_Misc_Wire_Blue) / 255);

		MethyModel->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Misc_Wire_Alpha) / 255);

		Interfaces.g_pModelRender->ForcedMaterialOverride(MethyModel, OVERRIDE_NORMAL);
		Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

		return true;

	}



	if (strstr(Model_Name, ("flash")) && Settings.GetSetting(Tab_Misc, Misc_NoFlash))
	{
		IMaterial* flash = Interfaces.pMaterialSystem->FindMaterial("effects\\flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);
		IMaterial* flashWhite = Interfaces.pMaterialSystem->FindMaterial("effects\\flashbang_white", TEXTURE_GROUP_CLIENT_EFFECTS);
		flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		flashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		Interfaces.g_pModelRender->ForcedMaterialOverride(flash);
		Interfaces.g_pModelRender->ForcedMaterialOverride(flashWhite);
	}









	else if (strstr(Model_Name, "weapon") && Settings.GetSetting(Tab_Misc, Misc_Weapon) /*  */)
	{

		Visable_Lit->ColorModulate(Settings.GetSetting(Tab_Other, Other_Cham_Weapon_Red) / 255,
			Settings.GetSetting(Tab_Other, Other_Cham_Weapon_Green) / 255,
			Settings.GetSetting(Tab_Other, Other_Cham_Weapon_Blue) / 255);

		Visable_Lit->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Cham_Weapon_Alpha) / 255);

		Interfaces.g_pModelRender->ForcedMaterialOverride(Visable_Lit, OVERRIDE_NORMAL);
		Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

		return true;
	}



	if (Settings.GetSetting(Tab_Visuals, Cham_Active)) {
		CBaseEntity* Model_Entity = Interfaces.pEntList->GetClientEntity(pInfo.entity_index);
		const char* Model_Name = Interfaces.g_pModelInfo->GetModelName((model_t*)pInfo.pModel);

		if (!Model_Entity)
			return false;



		if (Model_Entity->IsPlayer() && Model_Entity != Hacks.LocalPlayer && strstr(Model_Name, "models/player") && true /*  */)
		{
			if (Model_Entity->GetTeam() != Hacks.LocalPlayer->GetTeam())
			{
				Covered_Lit->ColorModulate(
					Settings.GetSetting(Tab_Other, Other_Cham_Enemy_inVisible_Red) / 255,
					Settings.GetSetting(Tab_Other, Other_Cham_Enemy_inVisible_Green) / 255,
					Settings.GetSetting(Tab_Other, Other_Cham_Enemy_inVisible_Blue) / 255);
				Covered_Lit->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Cham_Enemy_inVisible_Alpha) / 255);

				Visable_Lit->ColorModulate(
					Settings.GetSetting(Tab_Other, Other_Cham_Enemy_Visible_Red) / 255,
					Settings.GetSetting(Tab_Other, Other_Cham_Enemy_Visible_Green) / 255,
					Settings.GetSetting(Tab_Other, Other_Cham_Enemy_Visible_Blue) / 255);
				Visable_Lit->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Cham_Enemy_Visible_Alpha) / 255);

			}
			else
			{
				Covered_Lit->ColorModulate(
					Settings.GetSetting(Tab_Other, Other_Cham_Team_inVisible_Red) / 255,
					Settings.GetSetting(Tab_Other, Other_Cham_Team_inVisible_Green) / 255,
					Settings.GetSetting(Tab_Other, Other_Cham_Team_inVisible_Blue) / 255);
				Covered_Lit->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Cham_Team_inVisible_Alpha) / 255);

				Visable_Lit->ColorModulate(
					Settings.GetSetting(Tab_Other, Other_Cham_Team_Visible_Red) / 255,
					Settings.GetSetting(Tab_Other, Other_Cham_Team_Visible_Green) / 255,
					Settings.GetSetting(Tab_Other, Other_Cham_Team_Visible_Blue) / 255);
				Visable_Lit->AlphaModulate(Settings.GetSetting(Tab_Other, Other_Cham_Team_Visible_Alpha) / 255);

			}


			if (!Model_Entity->isAlive())
			{
				Covered_Lit->ColorModulate(0.75f, 0.75f, 0.75f);
				Visable_Lit->ColorModulate(1.f, 1.f, 1.f);
			}

			if (Model_Entity->HasGunGameImmunity())
			{
				Covered_Lit->AlphaModulate(0.75f);
			}
			else
			{
				Covered_Lit->AlphaModulate(1.f);
			}

			if (Settings.GetSetting(Tab_Visuals, Cham_OnlyVis)) {
				Covered_Lit->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}
			else {
				Covered_Lit->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

			}
			if (Model_Entity->GetTeam() == Hacks.LocalPlayer->GetTeam()) {
				if (Settings.GetSetting(Tab_Visuals, Cham_ShowTeam)) {
					Interfaces.g_pModelRender->ForcedMaterialOverride(Covered_Lit, OVERRIDE_NORMAL);
					Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
					Interfaces.g_pModelRender->ForcedMaterialOverride(Visable_Lit, OVERRIDE_NORMAL);
					Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
				}
				else {
					Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

					Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

				}
			}
			else {
				Interfaces.g_pModelRender->ForcedMaterialOverride(Covered_Lit, OVERRIDE_NORMAL);
				Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
				Interfaces.g_pModelRender->ForcedMaterialOverride(Visable_Lit, OVERRIDE_NORMAL);
				Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

			}

			return true;
		}

	}
	return false;
































}




#pragma endregion