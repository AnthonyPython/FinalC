//========= Copyright � Valve LLC, All rights reserved. =======================
//
// Purpose:		
//
// $NoKeywords: $
//=============================================================================
#ifndef MERASMUS_THROWINGGRENADE_H
#define MERASMUS_THROWINGGRENADE_H
#ifdef _WIN32
#pragma once
#endif

#include "NextBotBehavior.h"
#include "merasmus.h"

class CMerasmusThrowingGrenade : public Action<CMerasmus>
{
	DECLARE_CLASS( CMerasmusThrowingGrenade, Action<CMerasmus> )
public:

	CMerasmusThrowingGrenade( CTFPlayer *pTarget );
	virtual ~CMerasmusThrowingGrenade() {}

	virtual char const *GetName( void ) const;

	virtual ActionResult<CMerasmus> OnStart( CMerasmus *me, Action<CMerasmus> *priorAction ) override;
	virtual ActionResult<CMerasmus> Update( CMerasmus *me, float dt ) override;
	virtual void OnEnd( CMerasmus *me, Action<CMerasmus> *newAction ) override;

private:
	CHandle<CTFPlayer> m_hVictim;
	CountdownTimer m_sequenceDuration;
	CountdownTimer m_actionDelay;
	PathFollower m_PathFollower;
};

#endif