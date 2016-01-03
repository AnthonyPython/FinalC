//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//

#ifndef ECON_ENTITY_H
#define ECON_ENTITY_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"

#if defined( CLIENT_DLL )
#define CEconEntity C_EconEntity
#endif

#include "ihasattributes.h"
#include "econ_itemview.h"
#include "attribute_manager.h"

struct wearableanimplayback_t
{
	int iStub;
};

class CEconItemDefinition;

//-----------------------------------------------------------------------------
// Purpose: BaseCombatWeapon is derived from this in live tf2.
//			The only actual use for it I've found so far is for the c_model
//			activity translation. Need to do some more research into this.
//-----------------------------------------------------------------------------
class CEconEntity : public CBaseAnimating, public IHasAttributes
{
	DECLARE_CLASS( CEconEntity, CBaseAnimating );
	DECLARE_NETWORKCLASS();

#ifdef CLIENT_DLL
	DECLARE_PREDICTABLE();
#endif

public:
	CEconEntity();
	~CEconEntity();

#ifdef CLIENT_DLL
	virtual void OnPreDataChanged( DataUpdateType_t );
	virtual void OnDataChanged( DataUpdateType_t );
#endif

	virtual int TranslateViewmodelHandActivity( int iActivity ) { return iActivity; }

	virtual void PlayAnimForPlaybackEvent(wearableanimplayback_t iPlayback) {};

	virtual void SetItem( CEconItemView &newItem );
	CEconItemView *GetItem();
	virtual bool HasItemDefinition() const;
	virtual int GetItemID();

	virtual void GiveTo( CBaseEntity *pEntity );

	virtual CAttributeManager *GetAttributeManager() { return &m_AttributeManager; }
	virtual CAttributeContainer *GetAttributeContainer() { return &m_AttributeManager; }
	virtual CBaseEntity *GetAttributeOwner() { return NULL; }
	virtual void ReapplyProvision( void );

	virtual void UpdateOnRemove( void );

protected:
	EHANDLE m_hOldOwner;
	CEconItemView m_Item;

private:
	CAttributeContainer m_AttributeManager;
};

#endif
