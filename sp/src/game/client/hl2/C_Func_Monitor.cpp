//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_FuncMonitor : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_FuncMonitor, C_BaseEntity );
	DECLARE_CLIENTCLASS();

// C_BaseEntity.
public:
	void Spawn() override { BaseClass::Spawn(); }
	virtual bool	ShouldDraw();
};

IMPLEMENT_CLIENTCLASS_DT( C_FuncMonitor, DT_FuncMonitor, CFuncMonitor )
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( func_monitor, C_FuncMonitor );

bool C_FuncMonitor::ShouldDraw()
{
	return true;
}
