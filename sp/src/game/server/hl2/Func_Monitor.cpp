//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "point_camera.h"
#include "modelentities.h"
#include "info_camera_link.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CFuncMonitor : public CFuncBrush
{
	DECLARE_DATADESC();
	DECLARE_CLASS( CFuncMonitor, CFuncBrush );
	DECLARE_SERVERCLASS();

public:
	void Spawn() override;
	void Activate() override;
	void UpdateOnRemove() override;
	int UpdateTransmitState() override
	{
		return SetTransmitState(FL_EDICT_PVSCHECK);
	}

private:
	void InputSetCamera(inputdata_t &inputdata);
	void SetCameraByName(const char *szName);
	void ReleaseCameraLink();

	CHandle<CInfoCameraLink> m_hInfoCameraLink;
};

// automatically hooks in the system's callbacks
BEGIN_DATADESC( CFuncMonitor )

	DEFINE_FIELD( m_hInfoCameraLink, FIELD_EHANDLE ),

	// Outputs
	DEFINE_INPUTFUNC( FIELD_STRING, "SetCamera", InputSetCamera ),

END_DATADESC()


LINK_ENTITY_TO_CLASS( func_monitor, CFuncMonitor );


IMPLEMENT_SERVERCLASS_ST( CFuncMonitor, DT_FuncMonitor )
END_SEND_TABLE()

void CFuncMonitor::Spawn()
{
	BaseClass::Spawn();
	m_hInfoCameraLink = CreateInfoCameraLink(this, nullptr);
}


//-----------------------------------------------------------------------------
// Purpose: Called after all entities have spawned and after a load game.
//-----------------------------------------------------------------------------
void CFuncMonitor::Activate()
{
 	BaseClass::Activate();
	SetCameraByName(STRING(m_target));
}

void CFuncMonitor::UpdateOnRemove()
{
	ReleaseCameraLink();
	BaseClass::UpdateOnRemove();
}


//-----------------------------------------------------------------------------
// Frees the camera.
//-----------------------------------------------------------------------------
void CFuncMonitor::ReleaseCameraLink()
{
	if ( m_hInfoCameraLink )
	{
		UTIL_Remove( m_hInfoCameraLink );
		m_hInfoCameraLink = nullptr;

		// Keep the target up-to-date for save/load
		m_target = NULL_STRING;
	}
}


//-----------------------------------------------------------------------------
// Sets camera 
//-----------------------------------------------------------------------------
void CFuncMonitor::SetCameraByName(const char *szName)
{
	m_hInfoCameraLink->SetCameraByName(szName);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncMonitor::InputSetCamera(inputdata_t &inputdata)
{
	SetCameraByName( inputdata.value.String() );
}
