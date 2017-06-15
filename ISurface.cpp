#include "stdafx.h"

RECT ISurface::GetViewport()
{
	RECT Viewport = { 0, 0, 0, 0 };
	int w, h;
	Interfaces.pEngine->GetScreenSize(w, h);
	Viewport.right = w; Viewport.bottom = h;
	return Viewport;
}

RECT ISurface::GetTextSize2(DWORD font, const char* text)
{
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

void ISurface::Clear(int x, int y, int w, int h, Color color)
{
	Interfaces.pSurface->DrawSetColor(color);
	Interfaces.pSurface->DrawFilledRect(x, y, x + w, y + h);
}

void ISurface::Text2(int x, int y, Color color, DWORD font, const char* text)
{
	size_t origsize = strlen(text) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

	Interfaces.pSurface->DrawSetTextFont(font);

	Interfaces.pSurface->DrawSetTextColor(color);
	Interfaces.pSurface->DrawSetTextPos(x, y);
	Interfaces.pSurface->DrawPrintText(wcstring, wcslen(wcstring));
	return;
}

void ISurface::Text2(int x, int y, Color color, DWORD font, const wchar_t* text)
{
	Interfaces.pSurface->DrawSetTextFont(font);
	Interfaces.pSurface->DrawSetTextColor(color);
	Interfaces.pSurface->DrawSetTextPos(x, y);
	Interfaces.pSurface->DrawPrintText(text, wcslen(text));
}

void ISurface::Outline(int x, int y, int w, int h, Color color)
{
	Interfaces.pSurface->DrawSetColor(color);
	Interfaces.pSurface->DrawOutlinedRect(x, y, x + w, y + h);
}