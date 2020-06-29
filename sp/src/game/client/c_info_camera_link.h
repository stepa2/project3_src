#pragma once
#include "c_point_camera.h"

#ifdef MAPBASE

class C_InfoCameraLink: public C_BaseEntity
{
public:
	DECLARE_CLASS(C_InfoCameraLink, C_BaseEntity)
	DECLARE_CLIENTCLASS()

	~C_InfoCameraLink();

	void Spawn() override;
	
	
	C_PointCamera* GetCamera() const { return m_hCamera; }
	C_BaseEntity* GetTarget() const { return m_hTargetEntity; }

private:
	void PostDataUpdate(DataUpdateType_t updateType) override;

private:
	
	CHandle<C_PointCamera> m_hCamera = nullptr;
	EHANDLE m_hTargetEntity = nullptr;
};

// Returns C_InfoCameraLink that links given entity with camera or nullptr
C_InfoCameraLink* GetLinkOf(C_BaseEntity* entity);
#endif