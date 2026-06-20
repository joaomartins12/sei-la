
#pragma once

//struct sNiObjectInfo
//{
//	void	Delete(){}
//	void	Init(){}
//};

class CsGBObjectResMng : public NiMemObject
{
public:
	CsGBObjectResMng();
	~CsGBObjectResMng();

public:
	struct sNINODE_INFO
	{
		sNINODE_INFO(){ s_pNiNode = NULL; s_nGetID = 0; }
		~sNINODE_INFO(){ s_pNiNode = 0; }
		NiNodePtr		s_pNiNode;
		int				s_nGetID;
	};

protected:
	MAP_GBTERRAIN_OBJECT									m_mapObject;
	//NiTStringPointerMap< sNiObjectInfo* >*				m_pmapNiObjectInfo;
	NiTStringPointerMap< sNINODE_INFO* >*					m_pmapNiObject;
	NiTStringPointerMap< sUSER_DEFINE* >*					m_pmapUserDefine;
	NiTStringPointerMap< NiTexturingPropertyPtr >*			m_pmapTexture;



public:
	MAP_GBTERRAIN_OBJECT*	GetMapObject(){ return &m_mapObject; }

public:
	void			Delete();
	void			Init();

	// ==== Object
protected:
	void			_DeleteObject();
	void			_InitObject();
public:
	CsGBObject*		GetObject( CsGBObject::sINFO* pObjectInfo );
	CsGBObject*		GetObject( DWORD dwObjectUniqID );
	bool			IsObject( DWORD dwObjectUniqID ){ return ( m_mapObject.find( dwObjectUniqID ) != m_mapObject.end() ); }
	void			DeleteObject( DWORD dwUniqID );	
	void			SetObjPath( CsGBObject::sINFO* pObjectInfo );
	// ==== NiObject
protected:
	void			_DeleteNiObject();
	void			_InitNiObject();

	bool			_DMObjectAppendUV( NiNode* pNiNode );
	bool			_CheckDMObject( const char* cObjectPath, NiNodePtr pNiNode );
	void			_CloneTexturingProp( NiNode* pDestNiNode, NiNode* pSrcNiNode );
public:
	void			GetNiObject( sNINODE_INFO* pDestNode, const char* cObjectPath, sUSER_DEFINE* pUserDefine );
	NiNodePtr		GetNiObject( const char* cObjectPath, sUSER_DEFINE* pUserDefine );

	// ==== GetID
protected:
	std::queue< int >	m_queueGetID;
protected:
	int				_PopGetID();
	void			_PushGetID( int nID );

	// ==== NiObjectInfo
//protected:
//	void			_DeleteNiObjectInfo();
//	void			_InitNiObjectInfo();
//public:
//	sNiObjectInfo*	GetNiObjectInfo( const char* cObjectPath, NiNode* pNiNode );

	// ==== UserDefine
protected:
	void			_DeleteUserDefine();
	void			_InitUserDefine();
public:
	sUSER_DEFINE*	GetUserDefine( CsGBObject::sINFO* pObjectInfo );
protected:
	sUSER_DEFINE*	_ParcingUserDefined( CsGBObject::sINFO* pObjectInfo, NiNode* pNiNode );

	// ==== Map Texture
protected:
	void			_DeleteTexture();
	void			_InitTexture();
public:
	NiTexturingProperty*			GetTexture( const char* cTexturePath, NiTexturingProperty::ApplyMode eApply );
	NiTexturingProperty*			GetTexture( NiPixelData* pPData );
	NiTexturingProperty*			GetTexture();

	NiTexturingProperty::ShaderMap* GetShaderMap( const char* cTexturePath );
	NiTexturingProperty::ShaderMap* GetShaderMap( int nWidth, int nHeight, NiPixelFormat pf, DWORD dwInitColor );
	NiTexturingProperty::ShaderMap* GetShaderMap( int nWidth, int nHeight, NiPixelFormat pf, BYTE btInitColor );
	NiTexturingProperty::ShaderMap* GetShaderMap( int nWidth, int nHeight, NiPixelFormat pf, NiTexturingProperty::ShaderMap* pSrcShader,
												  DWORD dwInitColor );
	NiTexturingProperty::ShaderMap* GetShaderMap( NiPixelData* pPData );

	void							GetColor( NiPixelData* pPixelData, NiPoint2 fUV, BYTE& r, BYTE& g, BYTE& b );
	void							GetColorAverage( const char* cTexturePath, BYTE& r, BYTE& g, BYTE& b );
};