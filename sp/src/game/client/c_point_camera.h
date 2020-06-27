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

class CViewRender;
class C_PointCamera;

bool DrawCamera(C_PointCamera* camera, int cameraNum, const CViewSetup& viewSetup, CViewRender* viewRender, C_BasePlayer* player);

class C_PointCamera : public C_BaseEntity
{
	friend bool DrawCamera(C_PointCamera* camera, int cameraNum, const CViewSetup& viewSetup, CViewRender* viewRender, C_BasePlayer* player);
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
	Vector2D		GetResolution() const;
	bool			IsFogEnabled();
	void			GetFogColor( unsigned char &r, unsigned char &g, unsigned char &b );
	float			GetFogStart();
	float			GetFogMaxDensity();
	float			GetFogEnd();
	bool			UseScreenAspectRatio() const { return m_bUseScreenAspectRatio; }
#ifdef MAPBASE
	virtual bool	IsOrtho() const { return false; }
	virtual void	GetOrthoDimensions(float &up, float &dn, float &lf, float &rt) const {}

	SkyboxVisibility_t	SkyMode() { return m_iSkyMode; }
#endif

	virtual void	GetToolRecordingState( KeyValues *msg );

	bool KeepRTTexture() const;

private:
	float m_FOV;
	float m_ResolutionHeight;
	float m_ResolutionWidth;
	bool m_bFogEnable;
	color32 m_FogColor;
	float m_flFogStart;
	float m_flFogEnd;
	float m_flFogMaxDensity;
	bool m_bActive;
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


#endif // C_POINTCAMERA_H
