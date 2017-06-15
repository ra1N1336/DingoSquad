#include "stdafx.h"


void NoSky()
{
	for (MaterialHandle_t i = Interfaces.pMaterialSystem->FirstMaterial(); i != Interfaces.pMaterialSystem->InvalidMaterial(); i = Interfaces.pMaterialSystem->NextMaterial(i))
	{
		IMaterial *pMaterial = Interfaces.pMaterialSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		if (strstr(pMaterial->GetTextureGroupName(), "SkyBox textures"))
		{
			pMaterial->ColorModulate(0, 0, 0);
		}
	}
}