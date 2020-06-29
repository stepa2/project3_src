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
	int count = pMaterial->ShaderParamCount();
	IMaterialVar** params = pMaterial->GetShaderParams();
	
	for (int i = 0; i < count; ++i)
	{
		IMaterialVar* param = params[i];
		
		if(param->GetType() != MATERIAL_VAR_TYPE_TEXTURE
			&& param->GetType() != MATERIAL_VAR_TYPE_STRING) continue;

		char const* value = param->GetStringValue();
			
		if(stricmp(value,"_rt_Camera") != 0) continue;

#ifdef DEBUG
		if(_textureToReplace != nullptr)
		{
			AssertMsg(false, "Replacing more than one texture is not supported");
			return false;
		}
#endif
		

		_textureToReplace = param;

#ifndef DEBUG
		break;
#endif
	}

	return _textureToReplace != nullptr;
}

IMaterial* CProxyCameraRTReplace::GetMaterial()
{
	if (_textureToReplace == nullptr) return nullptr;

	return _textureToReplace->GetOwningMaterial();
}

void CProxyCameraRTReplace::OnBind(C_BaseEntity* pBaseEntity)
{
	if(_textureToReplace == nullptr)
	{
		AssertMsg(false, "Nothing to replace");
		return;
	}
	
	if(pBaseEntity == nullptr)
	{
		AssertMsg(false, "CameraRTReplace proxy used on material of non-entity object");
		return;
	}

	
	_cameraLink = GetLinkOf(pBaseEntity);

	if(_cameraLink != nullptr && _cameraLink->GetCamera()->IsActive())
		_textureToReplace->SetTextureValue(_cameraLink->GetCamera()->GetRTTexture());
	else
		_textureToReplace->SetStringValue("debug/debugempty");
}


EXPOSE_INTERFACE(CProxyCameraRTReplace, IMaterialProxy, "CameraRTReplace" IMATERIAL_PROXY_INTERFACE_VERSION);