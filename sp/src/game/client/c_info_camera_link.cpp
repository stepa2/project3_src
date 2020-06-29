#include "cbase.h"
#include "c_info_camera_link.h"

#include "tier0/memdbgon.h"

#ifdef MAPBASE

IMPLEMENT_CLIENTCLASS_DT(C_InfoCameraLink, DT_InfoCameraLink, CInfoCameraLink)
	RecvPropEHandle(RECVINFO(m_hCamera)),
	RecvPropEHandle(RECVINFO(m_hTargetEntity))
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( info_camera_link, C_InfoCameraLink );



void PrintLink(C_BaseEntity* targetEnt,C_InfoCameraLink* linkEnt);

static class Shared
{
public:
	CUtlMap<C_BaseEntity*, C_InfoCameraLink*> _linksOf;
	
	static bool C_BaseEntity_Ptr_LessFunc(C_BaseEntity* const& left, C_BaseEntity* const& right)
	{
	return left < right;
	}
	
	Shared(): _linksOf(C_BaseEntity_Ptr_LessFunc)
	{
		FOR_EACH_MAP(_linksOf, i)
			PrintLink(_linksOf.Key(i),_linksOf.Element(i));
	}
} _shared;





C_InfoCameraLink::~C_InfoCameraLink()
{
	DevMsg("info_camera_link destructor\n");
	_shared._linksOf.Remove(GetTarget());
}

void C_InfoCameraLink::Spawn()
{
	DevMsg("info_camera_link Spawn()\n");
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

	Msg("%s\n",target->GetEntityName());
	
	ushort index = _shared._linksOf.Find(target);
	if (index != _shared._linksOf.InvalidIndex()) return;

	_shared._linksOf.Insert(target, this);
}

void C_InfoCameraLink::PostDataUpdate(DataUpdateType_t updateType)
{
	Link();
	BaseClass::PostDataUpdate(updateType);
}




/*
void C_InfoCameraLink::Activate()
{
	model_t const* model = m_hTargetEntity.Get()->GetModel();
	size_t materials_count = modelinfo->GetModelMaterialCount(model);
	IMaterial** materials = new IMaterial*[materials_count];
	modelinfo->GetModelMaterials(model, materials_count, materials);

	for (size_t i = 0; i < materials_count; i++)
	{
		IMaterial* material = materials[i];
		// It seems it is impossible to add proxy to a material in runtime
	}
	
	
}
*/

C_InfoCameraLink* GetLinkOf(C_BaseEntity* entity)
{
	Assert(entity != nullptr);
	
	ushort index = _shared._linksOf.Find(entity);
	if(index == _shared._linksOf.InvalidIndex()) return nullptr;

	return _shared._linksOf.Element(index);
}

void PrintLink(C_BaseEntity* targetEnt,C_InfoCameraLink* linkEnt)
{
	Assert(targetEnt != nullptr);
	Assert(linkEnt != nullptr);
	const char* target = targetEnt->GetEntityName();
	target = target?target : "<null name>";
	
	const char* camera;
	if(linkEnt->GetCamera() != nullptr) 
	{
		camera = linkEnt->GetCamera()->GetEntityName();
		camera = camera?camera : "<null name>";
	} else
		camera = "<no camera>";
	
	const char* link = linkEnt->GetEntityName();
	link = link?link : "<null name>";
	if(link[0] == '\0') link = "<empty name>";
		
	Msg("\t%s links to %s using %s\n",target, camera, link);
}


CON_COMMAND(dump_camera_links, "Dumps info_camera_link links")
{
	Msg("info_camera_link links:\n");
	FOR_EACH_MAP_FAST(_shared._linksOf, i)
		PrintLink(_shared._linksOf.Key(i),_shared._linksOf.Element(i));
}
#endif
