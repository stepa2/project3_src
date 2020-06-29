//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef INFO_CAMERA_LINK_H
#define INFO_CAMERA_LINK_H

#include "baseentity.h"


class CPointCamera;

//-----------------------------------------------------------------------------
// Link between entities and cameras
//-----------------------------------------------------------------------------
class CInfoCameraLink : public CBaseEntity
{
	DECLARE_CLASS( CInfoCameraLink, CBaseEntity );
	DECLARE_SERVERCLASS();
 	DECLARE_DATADESC();

public:
	CInfoCameraLink();
	~CInfoCameraLink();

	void Activate() override;
	int UpdateTransmitState() override
	{
		return SetTransmitState(FL_EDICT_ALWAYS);
	}
	void SetCameraByName(const char *szName);
	
private:
	void InputSetCamera(inputdata_t &inputdata);
	void InputSetTargetEntity(inputdata_t&) = delete;


#ifdef MAPBASE
	CNetworkHandle(CPointCamera, m_hCamera);
	CNetworkHandle(CBaseEntity, m_hTargetEntity);
#else
	CHandle<CPointCamera> m_hCamera;
	EHANDLE m_hTargetEntity;
#endif
	string_t m_strPointCamera;

	friend CInfoCameraLink *CreateInfoCameraLink( CBaseEntity *pTarget, CPointCamera *pCamera );
	friend void PointCameraSetupVisibility( CBaseEntity *pPlayer, int area, unsigned char *pvs, int pvssize );
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CInfoCameraLink *CreateInfoCameraLink( CBaseEntity *pTarget, CPointCamera *pCamera );


//-----------------------------------------------------------------------------
// Sets up visibility 
//-----------------------------------------------------------------------------
void PointCameraSetupVisibility( CBaseEntity *pPlayer, int area, unsigned char *pvs, int pvssize );



#endif // INFO_CAMERA_LINK_H

