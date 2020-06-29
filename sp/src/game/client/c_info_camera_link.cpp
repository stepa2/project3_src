#include "cbase.h"
#include "c_info_camera_link.h"

#include "tier0/memdbgon.h"

#ifdef MAPBASE

IMPLEMENT_CLIENTCLASS_DT(C_InfoCameraLink, DT_InfoCameraLink, CInfoCameraLink)
	RecvPropEHandle(RECVINFO(m_hCamera)),
	RecvPropEHandle(RECVINFO(m_hTargetEntity))
END_RECV_TABLE()

static CUtlMap<C_BaseEntity*, C_InfoCameraLink*> _linksOf;

bool C_BaseEntity_Ptr_LessFunc(C_BaseEntity* const& left, C_BaseEntity* const& right)
{
	return left < right;
}

static class InitHack
{
public:
	InitHack()
	{
		_linksOf.SetLessFunc(C_BaseEntity_Ptr_LessFunc);
	}
} _initHack;





C_InfoCameraLink::~C_InfoCameraLink()
{
	_linksOf.Remove(GetTarget());
}

void C_InfoCameraLink::Spawn()
{
	BaseClass::Spawn();



}

void C_InfoCameraLink::PostDataUpdate(DataUpdateType_t updateType)
{
	BaseClass::PostDataUpdate(updateType);

	ushort index = _linksOf.Find(GetTarget());

	if (index == _linksOf.InvalidIndex())
		_linksOf.Insert(GetTarget(), this);
	
	char const* camName = GetCamera()->GetEntityName();
	camName = camName?camName:"<unnamed camera>";
	char const* targetName = GetTarget()->GetEntityName();
	targetName = targetName?targetName:"<unnamed target>";
	
	Msg("info_camera_link: %s %s\n", camName, targetName);
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
	
	ushort index = _linksOf.Find(entity);
	if(index == _linksOf.InvalidIndex()) return nullptr;

	return _linksOf.Element(index);
}

CON_COMMAND(dump_camera_links, "Dumps info_camera_link links")
{
	Msg("info_camera_link links:\n");
	for ( int i = _linksOf.FirstInorder(); i != _linksOf.InvalidIndex(); i = _linksOf.NextInorder( i ) )
	{
		C_BaseEntity* targetEnt = _linksOf.Key(i);
		C_InfoCameraLink* linkEnt = _linksOf.Element(i);
		
		const char* target = targetEnt->GetEntityName();
		target = target?target : "<unnamed>";
		const char* camera;
		if(linkEnt->GetCamera() != nullptr) 
		{
			camera = linkEnt->GetCamera()->GetEntityName();
			camera = camera?camera : "<unnamed>";
		} else
			camera = "<no camera>";
		const char* link = linkEnt->GetEntityName();
		link = link?link : "<unnamed>";
		
		
		Msg("\t%s links to %s using %s\n",target, camera, link);
	}
}
#endif
