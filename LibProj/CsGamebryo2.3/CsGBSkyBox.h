
#pragma once

class CsGBSkyBox : public NiMemObject
{
public:	
	CsGBSkyBox();
	~CsGBSkyBox();

public:
	struct sINFO
	{
		sINFO():s_fHeight(0.0f)
		{
			memset( s_cSkyBox, 0, MAX_PATH );
		}

		void Init(){
			memset( s_cSkyBox, 0, MAX_PATH );
			//s_cSkyBox[ 0 ] = NULL;
			s_fHeight = 0.0f;
		}
		char			s_cSkyBox[ MAX_PATH ];
		float			s_fHeight;
	};

	enum eGEOM_TYPE
	{
		GT_POS_STATIC		=	0x00000001,
		GT_POS_CHAR_FOLLOW	=	0x00000002,
		GTMASK_POS			=	0x0000000f,

		GT_ROTATION			=	0x00000010,
		GT_NET_ROTATION		=	0x00000020,

		GT_ANIMATION		=	0x00000100,		// 가장 마지막 업데이트 체크
		GTMASK_UPDATE		=	0x00000ff0,

		GT_USER_SORTING		=	0x00001000,

		GTD_ROTATION		=	GT_ROTATION | GT_NET_ROTATION,
	};

	struct sGEOM_INFO : public NiMemObject
	{
		CSGBMEMPOOL_H( CsGBSkyBox::sGEOM_INFO );

		void Reset(){ s_dwType = 0; s_pNiGeom = NULL; s_fAniTime = 0; s_fRotation = 0.0f; s_fAccumeTime = 0.0f; s_fRotLoopTime = 0.0f; }
		DWORD			s_dwType;
		float			s_fValue1;
		CGeomTR*		s_pNiGeom;

		float			s_fAccumeTime;
		float			s_fRotLoopTime;

		float			s_fAniTime;
		float			s_fAniPlayRate;
		float			s_fRotation;

		NiPoint3		s_vOrgPos;
	};
protected:
	CsNodeObj						m_SkyBox;
	std::list< sGEOM_INFO* >		m_listGeomInfo;
	sINFO							m_Info;
	NiNodePtr						m_pNiNode;
	
public:
	void			Delete();
	void			Create();

	void			Update( float fElapsedTime );
	void			Render();
	void			Render_Shader( char* cSkinShader, char* cObjectShader );

	void			SetViewerPos( NiPoint3 pos );

	sINFO*			GetInfo(){ return &m_Info; }

	void			SetHeight( float fHeight );

	// User Define
protected:
	void			_ParcingUserDefine( sGEOM_INFO* pGeomInfo, std::map< int, sGEOM_INFO* >* pmapSort );

	void			_Update_Rotation( float fElapsedTime, sGEOM_INFO* pGeomInfo );
	void			_Update_Animation( float fElapsedTime, sGEOM_INFO* pGeomInfo );

	// Extra Data
public:
	void			SaveExtraData( CsNiNodePtr pBaseNiNode );
	void			LoadExtraData( CsNiNodePtr pNiNode );
};

