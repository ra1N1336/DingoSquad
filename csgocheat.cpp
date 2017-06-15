#include "stdafx.h"
#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iphlpapi.h>
#include "hwutils.h"
#include "XorStr.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <shlwapi.h>
#pragma comment (lib, "shell32.lib ")

#pragma comment(lib, "iphlpapi.lib")

bool Unhook_Cheat = false;
extern void savesets();
extern std::string settingstostring();

void Unhook()
{

	Unhook_Cheat = true;

}

void SaveSettings()
{
	settingstostring();
}

void LoadSettings()
{
	savesets();
}

void ResetWorld()
{
	// call reset world here or smth
}
DWORD GetHWID()
{
	UCHAR
		szFileSys[255],
		szVolNameBuff[255];

	DWORD
		dwMFL,
		dwSysFlags,
		dwSerial;

	GetVolumeInformation("C:\\", (LPTSTR)szVolNameBuff, 255, &dwSerial, &dwMFL, &dwSysFlags, (LPTSTR)szFileSys, 255);
	return dwSerial;
}

DWORD WINAPI CheatMain(LPVOID lpThreadParameter)
{
	//std::string str = GetHWID(); 
	//DWORD m_dwIP = std::strtoul(str.c_str(), NULL, 16); 

	//if (GetHWID() != m_dwIP) 
	//ExitProcess(0); 
	//setlocale(LC_ALL, "Russian");
	//MessageBox(0, "All credits to Ba3e1uH", "JesusCry = TOP HACK", MB_OK);
	//Sleep(500);
	//MessageBox(0, "Owned by vk.com/kekcwhite", "Tomas", MB_OK);
	//system("explorer vk.com/kekcwhite");
#ifdef DEBUG 
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	SetConsoleTitle("CSGO");
#endif 

#ifdef DEBUG 
	Interfaces.InitialiseSDK();
	CNetvarManager::Instance()->CreateDatabase();
	//CNetvarManager::Instance()->Dump("cheat_netvars.txt"); 
	/* Do Hooks */
	jakeschecks();
	Hacks.Hook();
	for (ClientClass* pClass = Interfaces.pClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
			RecvTable* pClassTable = pClass->m_pRecvTable;
			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];
				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;



				break;
			}

			break;
		}
	}
#else 
	if (DoesHWIDMatch() && jakeschecks()) {
		Interfaces.InitialiseSDK();

		CNetvarManager::Instance()->CreateDatabase();
		//CNetvarManager::Instance()->Dump("cheat_netvars.txt"); 
		/* Do Hooks */
		Hacks.Hook();
		for (ClientClass* pClass = Interfaces.pClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
			if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
				// Search for the 'm_nModelIndex' property. 
				RecvTable* pClassTable = pClass->m_pRecvTable;

				for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
					RecvProp* pProp = &pClassTable->m_pProps[nIndex];

					if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
						continue;



					break;
				}

				break;
			}
		}
	}
	else {
		FreeConsole();
		FreeLibraryAndExitThread(hMod, 0);
	}

#endif 

	while (!Unhook_Cheat && !Settings.GetSetting(Tab_Misc, Misc_Unhook))
	{

#ifndef DEBUG 
		//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)jakeschecks, 0, 0, 0); 

		if (!DoesHWIDMatch()) {
			Unhook_Cheat = true;
			Settings.SetSetting(Tab_Misc, Misc_Unhook, true);
		}
#endif 
		Sleep(100);
	}

	for (ClientClass* pClass = Interfaces.pClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
			// Search for the 'm_nModelIndex' property. 
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;

				

				break;
			}

			break;
		}
	}

	Hacks.UnHook();
	Interfaces.pEngine->ClientCmd_Unrestricted("cl_mouseenable 1", 0);
#ifdef DEBUG 
	cout << ired << "-= CHEAT UNLOADED =-" << endl;
#endif 

#ifdef DEBUG 
	FreeConsole();
#endif 

	/* UNLOAD DLL FROM GAME + EXIT THIS THREAD */
	Sleep(5000); // Sleep So Hooks Can Finish... 
#ifdef DEBUG 
	cout << blue << "-= CHEAT UNLOADED =-" << endl;
#endif 
	FreeLibraryAndExitThread(hMod, 0);
#ifdef DEBUG 
	cout << iyellow << "-= CHEAT UNLOADED =-" << endl;
#endif 

	return TRUE;
}