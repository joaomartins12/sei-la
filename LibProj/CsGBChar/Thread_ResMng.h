
#pragma once

class CsC_Thread_ResMng : public NiMemObject
{
public:
	CsC_Thread_ResMng();
	~CsC_Thread_ResMng();

	//=====================================================================================
	//
	//	베이스
	//
	//=====================================================================================
protected:
	CsCriticalSection		m_CS;

public:
	void			Destroy();
	void			Init();
	void			RefCheck();


protected:
	void			_CheckUserDefine( NiAVObject* pNiObject );

	//=====================================================================================
	//
	//	DefaultLight
	//
	//=====================================================================================
protected:
	NiDirectionalLight*		m_pDefaultLight;

public:
	void					SetDefaultLight( NiDirectionalLight* pLight ){ _SYNCHRONIZE_THREAD_( &m_CS ); m_pDefaultLight = pLight; }

	//=====================================================================================
	//
	//	SettingLight, SettingFog
	//
	//=====================================================================================
protected:	
	NiFogProperty*			m_pSettingFog;
	std::list< NiLight* >	m_SettingLightList;

public:
	void					ReleaseImmediatelyResource();
	void					ReleaseConnetTerrain();
	void					ApplyConnetTerrain();

	//=====================================================================================
	//
	// 콜백
	//
	//=====================================================================================	

	//=====================================================================================
	//
	//	Nif Part
	//
	//=====================================================================================
protected:
	NiTStringPointerMap< sLOAD_PART* >*		m_pmapNiPart;
public:
	void			GetPart( sLOAD_PART* pDestObject, const char* cObjectPath );

protected:
	void			_RefCheck_Part( DWORD dwCurTime );

	//=====================================================================================
	//
	//	NiF Object
	//
	//=====================================================================================
protected:
	NiTStringPointerMap< sLOAD_NIF* >*		m_pmapNiObject;
public:
	void			GetNIFObject( sLOAD_NIF* pDestObject, const char* cObjectPath, sCREATEINFO* pProp );

protected:
	void			_RefCheck_NIF( DWORD dwCurTime );

	//=====================================================================================
	//
	//	NiF Attach
	//
	//=====================================================================================
public:
	void			GetNIFAttach( sLOAD_NIF* pDestObject, const char* cObjectPath, sCREATEINFO* pProp );

	//=====================================================================================
	//
	//	KFM Object
	//
	//=====================================================================================
protected:
	NiTStringPointerMap< sLOAD_KFM* >*		m_pmapKFMObject;
public:
	void			GetKFMObject( sLOAD_KFM* pDestObject, const char* cObjectPath, sCREATEINFO* pProp );

protected:
	void			_RefCheck_KFM( DWORD dwCurTime );
};

