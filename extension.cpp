/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "CDetour/detours.h"
//#include <sourcehook.h>
#include "iplayerinfo.h"
#include <server_class.h>
#include <ispatialpartition.h>

class CBaseEntity;
struct variant_hax
{
	const char *pszValue;
};

struct inputdata_t
{
	// The entity that initially caused this chain of output events.
	CBaseEntity *pActivator;
	// The entity that fired this particular output.
	CBaseEntity *pCaller;
	// The data parameter for this output.
	variant_hax value;
	// The unique ID of the output that was fired.
	int nOutputID;
};

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

InputTestActivator g_Interface;
SMEXT_LINK(&g_Interface);

IGameConfig *g_pGameConf = NULL;
CDetour *g_pDetour_InputTestActivator = NULL;

DETOUR_DECL_MEMBER1(DETOUR_InputTestActivator, void, inputdata_t *, inputdata)
{
	if(!inputdata || !inputdata->pActivator || !inputdata->pCaller)
		return;

	//META_CONPRINTF("DETOUR_InputTestActivator Call!");
	DETOUR_MEMBER_CALL(DETOUR_InputTestActivator)(inputdata);
}

bool InputTestActivator::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	srand((unsigned int)time(NULL));

	char conf_error[255] = "";
	if (!gameconfs->LoadGameConfigFile("testactivator_fix.games", &g_pGameConf, conf_error, sizeof(conf_error)))
	{
		if(conf_error[0])
			snprintf(error, maxlength, "Could not read testactivator_fix.games.txt: %s", conf_error);

		return false;
	}

	CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);

	g_pDetour_InputTestActivator = DETOUR_CREATE_MEMBER(DETOUR_InputTestActivator, "CBaseFilter_InputTestActivator");
	if(g_pDetour_InputTestActivator == NULL)
	{
		snprintf(error, maxlength, "Could not create detour for CBaseFilter::InputTestActivator.");
		SDK_OnUnload();
		return false;
	}

	g_pDetour_InputTestActivator->EnableDetour();
	return true;
}

void InputTestActivator::SDK_OnUnload()
{
	if (g_pDetour_InputTestActivator != NULL)
	{
		g_pDetour_InputTestActivator->Destroy();
		g_pDetour_InputTestActivator = NULL;
	}
}

bool InputTestActivator::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	return true;
}