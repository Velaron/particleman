/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
// pm_defs.h
#if !defined( PM_DEFSH )
#define PM_DEFSH
#pragma once

#include "archtypes.h"     // DAL
#define	MAX_PHYSENTS 600 		  // Must have room for all entities in the world.
#define MAX_MOVEENTS 64
#define	MAX_CLIP_PLANES	5

#define PM_NORMAL			0x00000000
#define PM_STUDIO_IGNORE	0x00000001		// Skip studio models
#define PM_STUDIO_BOX		0x00000002		// Use boxes for non-complex studio models (even in traceline)
#define PM_GLASS_IGNORE		0x00000004		// Ignore entities with non-normal rendermode
#define PM_WORLD_ONLY		0x00000008		// Only trace against the world

// Values for flags parameter of PM_TraceLine
#define PM_TRACELINE_PHYSENTSONLY	0
#define PM_TRACELINE_ANYVISIBLE		1


// PM_PlayerTrace results.
#include "pmtrace.h"

#if !defined ( USERCMD_H )
#include "usercmd.h"
#endif

// physent_t
typedef struct physent_s
{
	char			name[32];             // Name of model, or "player" or "world".
	int				player;
	Vector			origin;               // Model's origin in world coordinates.
	model_s*		model;		          // only for bsp models
	model_s*		studiomodel;		  // SOLID_BBOX, but studio clip intersections.
	Vector			mins, maxs;	          // only for non-bsp models
	int				info;		          // For client or server to use to identify (index into edicts or cl_entities)
	Vector			angles;               // rotated entities need this info for hull testing to work.

	int				solid;				  // Triggers and func_door type WATER brushes are SOLID_NOT
	int				skin;                 // BSP Contents for such things like fun_door water brushes.
	int				rendermode;			  // So we can ignore glass
	
	// Complex collision detection.
	float			frame;
	int				sequence;
	byte			controller[4];
	byte			blending[2];

	int				movetype;
	int				takedamage;
	int				blooddecal;
	int				team;
	int				classnumber;

	// For mods
	int				iuser1;
	int				iuser2;
	int				iuser3;
	int				iuser4;
	float			fuser1;
	float			fuser2;
	float			fuser3;
	float			fuser4;
	Vector			vuser1;
	Vector			vuser2;
	Vector			vuser3;
	Vector			vuser4;
} physent_t;

#endif