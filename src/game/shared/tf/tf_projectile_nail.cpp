//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Nail
//
//=============================================================================
#include "cbase.h"
#include "tf_projectile_nail.h"
#include "tf_gamerules.h"

#ifdef CLIENT_DLL
#include "c_basetempentity.h"
#include "c_te_legacytempents.h"
#include "c_te_effect_dispatch.h"
#include "input.h"
#include "c_tf_player.h"
#include "cliententitylist.h"
#endif

//=============================================================================
//
// TF Syringe Projectile functions (Server specific).
//
#define SYRINGE_MODEL				"models/weapons/w_models/w_syringe_proj.mdl"
#define SYRINGE_DISPATCH_EFFECT		"ClientProjectile_Syringe"
#define SYRINGE_GRAVITY	0.3f

LINK_ENTITY_TO_CLASS( tf_projectile_syringe, CTFProjectile_Syringe );
PRECACHE_REGISTER( tf_projectile_syringe );

short g_sModelIndexSyringe;
void PrecacheSyringe( void *pUser )
{
	g_sModelIndexSyringe = modelinfo->GetModelIndex( SYRINGE_MODEL );
}

PRECACHE_REGISTER_FN( PrecacheSyringe );

CTFProjectile_Syringe::CTFProjectile_Syringe()
{
}

CTFProjectile_Syringe::~CTFProjectile_Syringe()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Syringe::GetProjectileModelName( void )
{
	return SYRINGE_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Syringe::GetGravity( void )
{
	return SYRINGE_GRAVITY;
}

// Nail projectile used by the Nailgun

#define NAILGUN_NAIL_MODEL				"models/weapons/w_models/w_nail.mdl"
#define NAILGUN_NAIL_DISPATCH_EFFECT	"ClientProjectile_Nail"
#define NAILGUN_NAIL_GRAVITY	0.3f

LINK_ENTITY_TO_CLASS( tf_projectile_nail, CTFProjectile_Nail );
PRECACHE_REGISTER( tf_projectile_nail );

short g_sModelIndexNail;
void PrecacheNail( void *pUser )
{
	g_sModelIndexNail = modelinfo->GetModelIndex( NAILGUN_NAIL_MODEL );
}

PRECACHE_REGISTER_FN( PrecacheNail );

CTFProjectile_Nail::CTFProjectile_Nail()
{
}

CTFProjectile_Nail::~CTFProjectile_Nail()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Nail::GetProjectileModelName( void )
{
	return NAILGUN_NAIL_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Nail::GetGravity( void )
{
	// TODO: Maybe it would be a better idea to have some kind of a keyvalue in the weapon file
	// to easily change the nail gravity?
	return NAILGUN_NAIL_GRAVITY;
}



// Tranqualizer syringe (same as medic's syringe, except with a different gravity and particle effect

#define TRANQDART_MODEL				"models/weapons/w_models/w_dart.mdl"
#define TRANQDART_DISPATCH_EFFECT	"ClientProjectile_TranqDart"
#define TRANQDART_GRAVITY	0.0f

LINK_ENTITY_TO_CLASS( tf_projectile_dart, CTFProjectile_Dart );
PRECACHE_REGISTER( tf_projectile_dart );

short g_sModelIndexTranqDart;
void PrecacheTranqDart( void *pUser )
{
	g_sModelIndexTranqDart = modelinfo->GetModelIndex( TRANQDART_MODEL );
}

PRECACHE_REGISTER_FN( PrecacheTranqDart );


CTFProjectile_Dart::CTFProjectile_Dart()
{
}

CTFProjectile_Dart::~CTFProjectile_Dart()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Dart::GetProjectileModelName( void )
{
	return TRANQDART_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Dart::GetGravity( void )
{
	return TRANQDART_GRAVITY;
}

