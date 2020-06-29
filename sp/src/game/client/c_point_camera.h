//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_POINTCAMERA_H
#define C_POINTCAMERA_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseentity.h"
#include "basetypes.h"


class C_PointCamera : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_PointCamera, C_BaseEntity );
	DECLARE_CLIENTCLASS();

public:
	C_PointCamera();
	~C_PointCamera();

	bool IsActive();
	
	// C_BaseEntity.
	virtual bool	ShouldDraw();

	float			GetFOV();
	bool			IsFogEnabled();
	void			GetFogColor( unsigned char &r, unsigned char &g, unsigned char &b );
	float			GetFogStart();
	float			GetFogMaxDensity();
	float			GetFogEnd();
	bool			UseScreenAspectRatio() const { return m_bUseScreenAspectRatio; }
	void			PostDataUpdate(DataUpdateType_t updateType) override;
#ifdef MAPBASE
	virtual bool	IsOrtho() const { return false; }
	virtual void	GetOrthoDimensions(float &up, float &dn, float &lf, float &rt) const {}

	SkyboxVisibility_t	SkyMode() { return m_iSkyMode; }

	ITexture*		GetRTTexture() {return m_RenderTargetTexture;}
#endif

	virtual void	GetToolRecordingState( KeyValues *msg );

	bool KeepRTTexture() const;

private:
	void CreateRTTexture();
	void ReleaseRTTexture();

private:
	float m_FOV;
	int m_ResolutionHeight;
	int m_ResolutionWidth;
	bool m_bFogEnable;
	color32 m_FogColor;
	float m_flFogStart;
	float m_flFogEnd;
	float m_flFogMaxDensity;
	bool m_bActive;
	bool m_bPrevActive;
	bool m_bUseScreenAspectRatio;
#ifdef MAPBASE
	bool m_KeepRTTexture;
	SkyboxVisibility_t m_iSkyMode;
	CTextureReference m_RenderTargetTexture;
#endif

public:
	C_PointCamera	*m_pNext;
};

#ifdef MAPBASE
class C_PointCameraOrtho : public C_PointCamera
{
public:
	DECLARE_CLASS( C_PointCameraOrtho, C_PointCamera );
	DECLARE_CLIENTCLASS();

public:
	bool			IsOrtho() const { return m_bOrtho; }
	void			GetOrthoDimensions( float &up, float &dn, float &lf, float &rt ) const
	{
		up = m_OrthoDimensions[0], dn = m_OrthoDimensions[1];
		lf = m_OrthoDimensions[2], rt = m_OrthoDimensions[3];
	}

private:
	bool m_bOrtho;
	float m_OrthoDimensions[4];
};
#endif

C_PointCamera *GetPointCameraList();

class CViewRender;

bool DrawCamera(C_PointCamera* camera, int cameraNum, const CViewSetup& viewSetup, CViewRender* viewRender, C_BasePlayer* player);


#endif // C_POINTCAMERA_H
