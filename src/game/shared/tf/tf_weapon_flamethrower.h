//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#ifndef TF_WEAPON_FLAMETHROWER_H
#define TF_WEAPON_FLAMETHROWER_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "tf_weaponbase_rocket.h"
#include "tf_weapon_grenade_pipebomb.h"

// Client specific.
#ifdef CLIENT_DLL
	#include "particlemgr.h"
	#include "particle_util.h"
	#include "particles_simple.h"
	#include "c_tf_projectile_rocket.h"

	#define CTFFlameThrower C_TFFlameThrower
	#define CTFFlameRocket C_TFFlameRocket
	#define CTFFlameEntity C_TFFlameEntity
#else
	#include "tf_projectile_rocket.h"
	#include "baseentity.h"
#endif


enum FlameThrowerState_t
{
	// Firing states.
	FT_STATE_IDLE = 0,
	FT_STATE_STARTFIRING,
	FT_STATE_FIRING,
	FT_STATE_AIRBLASTING
};

//=========================================================
// Flamethrower Weapon
//=========================================================
class CTFFlameThrower : public CTFWeaponBaseGun
{
	DECLARE_CLASS( CTFFlameThrower, CTFWeaponBaseGun );
public:
	DECLARE_DATADESC();
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFFlameThrower();
	~CTFFlameThrower();

	virtual void	Spawn( void );
	virtual void	Precache( void );

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_FLAMETHROWER; }

	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo );
	virtual void	ItemPostFrame( void );
	virtual void	PrimaryAttack();
	virtual void	SecondaryAttack();
	void			AirBlastAttack();
	void			FlameAttack(bool bice);

	virtual bool	Lower( void );
	virtual void	WeaponReset( void );

	virtual void	DestroySounds( void );

	Vector			GetVisualMuzzlePos();
	Vector			GetFlameOriginPos();

	int				GetBuffType( void ) const;

	virtual bool	Deploy( void );
	
	// Client specific.
#if defined( CLIENT_DLL )

	virtual void	OnDataChanged(DataUpdateType_t updateType);
	virtual void	UpdateOnRemove( void );
	virtual void	SetDormant( bool bDormant );

	virtual char const *GetFlameEffectInternal( void ) const;

	//	Start/stop flame sound and particle effects
	void			StartFlame();
	void			StopFlame( bool bAbrupt = false );

	void			RestartParticleEffect();	

	// constant pilot light sound
	void 			StartPilotLight();
	void 			StopPilotLight();

	void			UpdateParticleEffect( void );
	// Server specific.
#else
	virtual void	DeflectEntity( CBaseEntity *pEntity, CTFPlayer *pAttacker, Vector &vecDir );
	virtual void	DeflectPlayer( CTFPlayer *pVictim, CTFPlayer *pAttacker, Vector &vecDir );

	void			SetHitTarget( void );
	void			HitTargetThink( void );
#endif

	bool			CanAirBlast( void );
	bool			CanAirBlastDeflectProjectile( void );
	bool			CanAirBlastPushPlayers( void );
	bool			CanAirBlastPutOutTeammate( void );
	
	bool			HasMmmph(void);
	virtual void	ActivateMmmph(void);
	virtual bool	HasChargeBar( void );
	virtual const char* GetEffectLabelText( void ) { return "#TF_PyroRage"; }

protected:
	Vector GetMuzzlePosHelper( bool bVisualPos );
	CNetworkVar( int, m_iWeaponState );
	CNetworkVar( bool, m_bCritFire );
	CNetworkVar( bool, m_bHitTarget );

	float m_flStartFiringTime;
	float m_flNextPrimaryAttackAnim;

	int			m_iParticleWaterLevel;
	float		m_flAmmoUseRemainder;

#if defined( CLIENT_DLL )
	CSoundPatch	*m_pFiringStartSound;

	CSoundPatch	*m_pFiringLoop;
	bool		m_bFiringLoopCritical;

	CNewParticleEffect *m_pFlameEffect;
	EHANDLE		m_hFlameEffectHost;
	
	CNewParticleEffect *m_pChargeEffect;

	CSoundPatch *m_pPilotLightSound;

	bool m_bOldHitTarget;
	CSoundPatch *m_pHitTargetSound;
#else
	float m_flStopHitSoundTime;
#endif

	CTFFlameThrower( CTFFlameThrower const& );
};

//=============================================================================
#define	TF_FLAMETHROWER_ROCKET_DAMAGE				15
#define TF_FLAMETHROWER_ROCKET_BURN_RADIUS			198

// Generic flame rocket.
class CTFFlameRocket : public CTFProjectile_Rocket
{
	DECLARE_CLASS( CTFFlameRocket, CTFProjectile_Rocket );
public:
	DECLARE_NETWORKCLASS(); 

	virtual int		GetWeaponID( void ) const		{ return TF_WEAPON_FLAMETHROWER_ROCKET; }
	virtual float	GetRadius() { return TF_FLAMETHROWER_ROCKET_BURN_RADIUS; }

#ifdef GAME_DLL
	// Creation.
	static CTFFlameRocket *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL );	
	virtual void	Spawn();
	virtual void	Precache();
#else
	virtual const char *GetTrailParticleName( void ) { return "incendiaryrockettrail"; }
#endif
};

#if defined( GAME_DLL )
DECLARE_AUTO_LIST( ITFFlameEntityAutoList );
#endif

class CTFFlameEntity : public CBaseEntity
#ifdef GAME_DLL
	, public ITFFlameEntityAutoList
#endif
{
	DECLARE_CLASS( CTFFlameEntity, CBaseEntity );
public:
	DECLARE_NETWORKCLASS();
	DECLARE_DATADESC();

	virtual void Spawn( void );

#if defined( CLIENT_DLL )
	virtual void OnDataChanged( DataUpdateType_t updateType );

	virtual void ClientThink( void );
	CNewParticleEffect *m_pFlameEffect;
#endif
	
public:
#if defined( GAME_DLL )
	static CTFFlameEntity *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, int iDmgType, float flDmgAmount, bool bCritFromBehind );

	void FlameThink( void );
	void CheckCollision( CBaseEntity *pOther, bool *pbHitWorld );
	CBaseEntity *GetAttacker( void ) { return m_hAttacker.Get(); }
	bool IsBehindTarget( CBaseEntity *pVictim );

private:
	void OnCollide( CBaseEntity *pOther );
	void SetHitTarget( void );

	Vector						m_vecInitialPos;		// position the flame was fired from
	Vector						m_vecPrevPos;			// position from previous frame
	Vector						m_vecBaseVelocity;		// base velocity vector of the flame (ignoring rise effect)
	Vector						m_vecAttackerVelocity;	// velocity of attacking player at time flame was fired
	float						m_flTimeRemove;			// time at which the flame should be removed
	int							m_iDmgType;				// damage type
	float						m_flDmgAmount;			// amount of base damage
	CUtlVector<EHANDLE>			m_hEntitiesBurnt;		// list of entities this flame has burnt
	int							m_iAttackerTeam;		// team of attacking player
	CHandle<CTFFlameThrower>	m_hLauncher;			// weapon that fired this flame
	bool						m_bCritFromBehind;
#endif

	CNetworkHandle(CBaseEntity, m_hAttacker);			// attacking player
};



#endif // TF_WEAPON_FLAMETHROWER_H
