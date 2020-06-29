#include "cbase.h"
#include "proxyentity.h"
#include "c_info_camera_link.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/itexture.h"
#include "materialsystem/imaterialsystem.h"

#include "tier0/memdbgon.h"

class CProxyCameraRTReplace: public CEntityMaterialProxy
{
public:
	bool Init(IMaterial* pMaterial, KeyValues* pKeyValues) override;
protected:
	void OnBind(C_BaseEntity* pBaseEntity) override;
public:
	IMaterial* GetMaterial() override;
protected:
	C_InfoCameraLink* _cameraLink = nullptr;
	IMaterialVar* _textureToReplace = nullptr;
};

bool CProxyCameraRTReplace::Init(IMaterial* pMaterial, KeyValues* pKeyValues)
{
	const char* paramToReplace = pKeyValues->GetString("paramToReplace",nullptr);

	if (paramToReplace == nullptr)
	{
		Warning("CameraRTReplace in %s: paramToReplace is not specified", pMaterial->GetName());
		return false;
	}

	bool isOk = true;
	
	_textureToReplace = pMaterial->FindVar(paramToReplace, &isOk);

	if(!isOk)
	{
		Warning("CameraRTReplace in %s: parameter \"%s\" specified in paramToReplace not found", pMaterial->GetName(), paramToReplace);
		return false;
	}

	return true;
}

IMaterial* CProxyCameraRTReplace::GetMaterial()
{
	if (_textureToReplace == nullptr) return nullptr;

	return _textureToReplace->GetOwningMaterial();
}

bool IsCameraActive(C_InfoCameraLink* link)
{
	if(link == nullptr) return false;
	if(link->GetCamera() == nullptr) return false;
	return link->GetCamera()->IsActive();
}
 
void CProxyCameraRTReplace::OnBind(C_BaseEntity* pBaseEntity)
{
	if(pBaseEntity == nullptr)
	{
		AssertMsg(false, "CameraRTReplace proxy used on material of non-entity object");
		return;
	}

	
	_cameraLink = GetLinkOf(pBaseEntity);

	if(IsCameraActive(_cameraLink))
		_textureToReplace->SetTextureValue(_cameraLink->GetCamera()->GetRTTexture());
	else
		_textureToReplace->SetStringValue("debug/debugempty");
}


EXPOSE_INTERFACE(CProxyCameraRTReplace, IMaterialProxy, "CameraRTReplace" IMATERIAL_PROXY_INTERFACE_VERSION);