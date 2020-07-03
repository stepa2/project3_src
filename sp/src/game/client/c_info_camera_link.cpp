#include "cbase.h"
#include "c_info_camera_link.h"

#include "tier0/memdbgon.h"

#ifdef MAPBASE

IMPLEMENT_CLIENTCLASS_DT(C_InfoCameraLink, DT_InfoCameraLink, CInfoCameraLink)
	RecvPropEHandle(RECVINFO(m_hCamera)),
	RecvPropEHandle(RECVINFO(m_hTargetEntity))
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( info_camera_link, C_InfoCameraLink );



static class Shared
{
public:
	CUtlMap<C_BaseEntity*, C_InfoCameraLink*> _linksOf;
	
	static bool C_BaseEntity_Ptr_LessFunc(C_BaseEntity* const& left, C_BaseEntity* const& right)
	{
		return left < right;
	}
	
	Shared(): _linksOf(C_BaseEntity_Ptr_LessFunc) {}
} _shared;





C_InfoCameraLink::~C_InfoCameraLink()
{
	_shared._linksOf.Remove(GetTarget());
}

void C_InfoCameraLink::Spawn()
{
//	if (m_hTargetEntity.Get() == nullptr) return;
	
//	ushort index = _linksOf.Find(m_hTargetEntity);
//	if (index != _linksOf.InvalidIndex()) return;

//	_linksOf.Insert(m_hTargetEntity, this);

	BaseClass::Spawn();
}

void C_InfoCameraLink::Link()
{
	C_BaseEntity* target = m_hTargetEntity;
	if (target == nullptr) return;
	
	ushort index = _shared._linksOf.Find(target);
	if (index != _shared._linksOf.InvalidIndex()) return;

	_shared._linksOf.Insert(target, this);
}

void C_InfoCameraLink::PostDataUpdate(DataUpdateType_t updateType)
{
	Link();
	BaseClass::PostDataUpdate(updateType);
}

C_InfoCameraLink* GetLinkOf(C_BaseEntity* entity)
{
	Assert(entity != nullptr);
	
	ushort index = _shared._linksOf.Find(entity);
	if(index == _shared._linksOf.InvalidIndex()) return nullptr;

	return _shared._linksOf.Element(index);
}
#endif
