//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#include "cbase.h"
#include "c_point_camera.h"
#include "toolframework/itoolframework.h"
#include "toolframework_client.h"
#include "tier1/KeyValues.h"

#if defined( HL2_CLIENT_DLL ) || defined( CSTRIKE_DLL )
#define USE_MONITORS
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "viewrender.h"
#include "view_shared.h"
#include "vprof.h"
#include "materialsystem/itexture.h"
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_PointCamera, DT_PointCamera, CPointCamera )
	RecvPropFloat( RECVINFO( m_FOV ) ), 
	RecvPropFloat( RECVINFO( m_ResolutionHeight ) ),
	RecvPropFloat( RECVINFO(m_ResolutionWidth) ),
	RecvPropInt( RECVINFO( m_bFogEnable ) ),
	RecvPropInt( RECVINFO( m_FogColor ) ),
	RecvPropFloat( RECVINFO( m_flFogStart ) ), 
	RecvPropFloat( RECVINFO( m_flFogEnd ) ), 
	RecvPropFloat( RECVINFO( m_flFogMaxDensity ) ), 
	RecvPropInt( RECVINFO( m_bActive ) ),
	RecvPropInt( RECVINFO( m_bUseScreenAspectRatio ) ),
#ifdef MAPBASE
	RecvPropInt( RECVINFO( m_iSkyMode ) ),
	RecvPropInt( RECVINFO( m_KeepRTTexture) )
#endif
END_RECV_TABLE()

C_EntityClassList<C_PointCamera> g_PointCameraList;
template<> C_PointCamera *C_EntityClassList<C_PointCamera>::m_pClassList = NULL;

C_PointCamera* GetPointCameraList()
{
	return g_PointCameraList.m_pClassList;
}




C_PointCamera::C_PointCamera()
{
	m_bActive = false;
	m_bFogEnable = false;

	g_PointCameraList.Insert( this );
}

C_PointCamera::~C_PointCamera()
{
	g_PointCameraList.Remove( this );
}

bool C_PointCamera::ShouldDraw()
{
	return false;
}

float C_PointCamera::GetFOV()
{
	return m_FOV;
}

Vector2D C_PointCamera::GetResolution() const
{
	return Vector2D(m_ResolutionHeight,m_ResolutionWidth);
}

bool C_PointCamera::IsFogEnabled()
{
	return m_bFogEnable;
}

void C_PointCamera::GetFogColor( unsigned char &r, unsigned char &g, unsigned char &b )
{
	r = m_FogColor.r;
	g = m_FogColor.g;
	b = m_FogColor.b;
}

float C_PointCamera::GetFogStart()
{
	return m_flFogStart;
}

float C_PointCamera::GetFogEnd()
{
	return m_flFogEnd;
}

float C_PointCamera::GetFogMaxDensity()
{
	return m_flFogMaxDensity;
}

bool C_PointCamera::IsActive()
{
	return m_bActive;
}

bool C_PointCamera::KeepRTTexture() const
{
	return m_KeepRTTexture;
}



void C_PointCamera::GetToolRecordingState( KeyValues *msg )
{
	BaseClass::GetToolRecordingState( msg );

	unsigned char r, g, b;
	static MonitorRecordingState_t state;
	state.m_bActive = IsActive() && !IsDormant();
	state.m_flFOV = GetFOV();
	state.m_bFogEnabled = IsFogEnabled();
	state.m_flFogStart = GetFogStart();
	state.m_flFogEnd = GetFogEnd();
	GetFogColor( r, g, b );
	state.m_FogColor.SetColor( r, g, b, 255 );
					  
	msg->SetPtr( "monitor", &state );
}


#ifdef MAPBASE
IMPLEMENT_CLIENTCLASS_DT( C_PointCameraOrtho, DT_PointCameraOrtho, CPointCameraOrtho )
	RecvPropInt( RECVINFO( m_bOrtho ) ),
	RecvPropArray( RecvPropFloat( RECVINFO( m_OrthoDimensions[0] ) ), m_OrthoDimensions ),
END_RECV_TABLE()
#endif

bool DrawCamera(C_PointCamera* camera, int cameraNum, const CViewSetup& viewSetup, CViewRender* viewRender, C_BasePlayer* player)
{
#ifdef USE_MONITORS
	VPROF_INCREMENT_COUNTER( "cameras rendered", 1 );

#ifdef MAPBASE
	ITexture* renderTarget = camera->m_RenderTargetTexture;
	if(renderTarget == nullptr) return false;
#else
	ITexture* renderTarget = GetCameraTexture();
#endif

	// Setup fog state for the camera.
	fogparams_t oldFogParams;
	float flOldZFar = 0.0f;

	bool fogEnabled = camera->IsFogEnabled();

	CViewSetup monitorView = viewSetup;

	fogparams_t *pFogParams = nullptr;

	if ( fogEnabled )
	{	
		if ( !player )
			return false;

		pFogParams = player->GetFogParams();

		// Save old fog data.
		oldFogParams = *pFogParams;
		flOldZFar = viewSetup.zFar;

		pFogParams->enable = true;
		pFogParams->start = camera->GetFogStart();
		pFogParams->end = camera->GetFogEnd();
		pFogParams->farz = camera->GetFogEnd();
		pFogParams->maxdensity = camera->GetFogMaxDensity();

		unsigned char r, g, b;
		camera->GetFogColor( r, g, b );
		pFogParams->colorPrimary.SetR( r );
		pFogParams->colorPrimary.SetG( g );
		pFogParams->colorPrimary.SetB( b );

		monitorView.zFar = camera->GetFogEnd();
	}

	monitorView.width = renderTarget->GetActualWidth();
	monitorView.height = renderTarget->GetActualHeight();
	monitorView.x = 0;
	monitorView.y = 0;
	monitorView.origin = camera->GetAbsOrigin();
	monitorView.angles = camera->GetAbsAngles();
	monitorView.fov = camera->GetFOV();
#ifdef MAPBASE
	if (camera->IsOrtho())
	{
		monitorView.m_bOrtho = true;
		camera->GetOrthoDimensions( monitorView.m_OrthoTop, monitorView.m_OrthoBottom,
			monitorView.m_OrthoLeft, monitorView.m_OrthoRight );
	}
	else
#endif
		monitorView.m_bOrtho = false;

	monitorView.m_flAspectRatio = camera->UseScreenAspectRatio() ? 0.0f : 1.0f;
	monitorView.m_bViewToProjectionOverride = false;

#ifdef MAPBASE
	// 
	// Monitor sky handling
	// 
	if ( camera->SkyMode() == SKYBOX_3DSKYBOX_VISIBLE )
	{
		int nClearFlags = (VIEW_CLEAR_DEPTH | VIEW_CLEAR_COLOR);
		bool bDrew3dSkybox = false;
		SkyboxVisibility_t nSkyMode = camera->SkyMode();

		Frustum frustum;
		render->Push3DView( monitorView, nClearFlags, renderTarget, (VPlane *)frustum );

		// if the 3d skybox world is drawn, then don't draw the normal skybox
		CSkyboxView *pSkyView = new CSkyboxView(viewRender);
		if ( ( bDrew3dSkybox = pSkyView->Setup( monitorView, &nClearFlags, &nSkyMode ) ) != false )
		{
			viewRender->AddViewToScene( pSkyView );
		}
		SafeRelease( pSkyView );

		viewRender->ViewDrawScene( bDrew3dSkybox, nSkyMode, monitorView, nClearFlags, VIEW_MONITOR );
 		render->PopView( frustum );
	}
	else
#endif
	{
		// @MULTICORE (toml 8/11/2006): this should be a renderer....
		Frustum frustum;
 		render->Push3DView( monitorView, VIEW_CLEAR_DEPTH | VIEW_CLEAR_COLOR, renderTarget, (VPlane *)frustum );
		viewRender->ViewDrawScene( false, SKYBOX_2DSKYBOX_VISIBLE, monitorView, 0, VIEW_MONITOR );
 		render->PopView( frustum );
	}

	// Reset the world fog parameters.
	if ( fogEnabled )
	{
		if ( pFogParams )
		{
			*pFogParams = oldFogParams;
		}
		monitorView.zFar = flOldZFar;
	}
#endif // USE_MONITORS
	return true;
}