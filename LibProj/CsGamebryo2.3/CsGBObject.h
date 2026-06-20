
#pragma once

class CsGBObject : public NiMemObject
{
	CSGBMEMPOOL_H( CsGBObject );

public:
	struct sPATHINFO
	{
		sPATHINFO(){ Reset(); }

		void	Delete(){
			SAFE_DELETE_ARRAY( s_pData ); SAFE_DELETE_ARRAY( s_pPECount );
			SAFE_NIDELETE_ARRAY( s_pptTranse );
			s_uiPEGroupCount = 0;
			s_uiTotalPECount = 0;
		}
		void	Reset(){ s_uiPEGroupCount = 0; s_uiTotalPECount = 0; s_pptTranse = NULL; s_pPECount = NULL; s_pData = NULL; }

		UINT				s_uiPEGroupCount;
		UINT				s_uiTotalPECount;
		
		NiPoint2*			s_pptTranse;
		USHORT*				s_pPECount;
		long*				s_pData;
	};

	enum eSAVE_TYPE{
		ST_FIRST_OBJECT = 100,
		ST_EXIST_OBJECT,
	};
	struct sFILE_HEADER
	{
		sFILE_HEADER():s_dwSaveType(0),s_dwInfoSize(0) 
		{}

		DWORD		s_dwSaveType;
		DWORD		s_dwInfoSize;
	};
	struct sINFO
	{
		sINFO():s_dwUniqID(0),s_dwTexTypePlag(0),s_bCheckDoAnimation(false),s_fAniSpeed(1.0f)
				,s_fStopAniTime(1.0f),s_dwUserDefinePlag(0)
		{
			memset( s_cObjectPath, 0, sizeof(char)*OBJECT_PATH_LEN );
		}

		void	Reset(){
			s_dwUniqID = 0;
			memset( s_cObjectPath, 0, sizeof(char)*OBJECT_PATH_LEN );
			s_dwTexTypePlag = 0;
			s_dwUserDefinePlag = 0;
			s_PathInfo.Reset();
			s_bCheckDoAnimation = false;
			s_fAniSpeed = 1.0f;
			s_fStopAniTime = 0.0f;
		}
		BYTE	GetTextureTypeIndex( BYTE btGroupIndex ){ return (BYTE)( s_dwTexTypePlag & ( 0x0000000f<<(4*btGroupIndex) ) )>>(4*btGroupIndex); }
		void	SetTextureTypeIndex( BYTE btGroupIndex, BYTE btTexType ){
			s_dwTexTypePlag &= ~( 0x0000000f<<(4*btGroupIndex) );
			s_dwTexTypePlag |= ( btTexType << (4*btGroupIndex) );}

		DWORD						s_dwUniqID;
		char						s_cObjectPath[ OBJECT_PATH_LEN ];

		bool						s_bCheckDoAnimation;	// 이아래로 3바이트 빈다

		float						s_fStopAniTime;
		float						s_fAniSpeed;
		char						s_Dummy[ 88 ];
		NiTransform					s_trObject;
		nsCSGBFUNC::sTERRAIN_BOUND	s_Bound;

		//===== UserDefine
		DWORD						s_dwUserDefinePlag;
		DWORD						s_dwTexTypePlag;

		//===== PathEngine
		sPATHINFO					s_PathInfo;
	};
	// 이미 존재하는 오브젝트일경우
	struct sEXIST_INFO
	{
		sEXIST_INFO():s_dwUniqID(0) {};

		DWORD						s_dwUniqID;
	};

	struct sCHILD_INFO
	{
		sCHILD_INFO():s_dwChildUniqID(0),s_dwSlotID(0) {};
		DWORD		s_dwChildUniqID;
		DWORD		s_dwSlotID;
	};
	struct sTOOL_INFO
	{
		sTOOL_INFO(){ s_nChildCount = 0; s_dwHaveChildUniqID = 0; }
		int			s_nChildCount;
		DWORD		s_dwHaveChildUniqID;
	};

	enum eOBJ_TYPE{ OT_NORMAL, OT_PORTAL, OT_ROTATION, OT_EFFECT, };
public:
	CsGBObject();
	virtual ~CsGBObject();

public:
	virtual eOBJ_TYPE	GetType(){ return OT_NORMAL; }
	virtual void		Delete();
	virtual void		Init( NiNode* pNiNode, int nGetID, sINFO* pObjectInfo, sUSER_DEFINE* pUserDefine );

	// ======= RefCount
protected:
	DWORD				m_dwRefCount;
public:
	void				IncreaseRefCount(){ ++m_dwRefCount; }
	void				DecreaseRefCount();
	bool				IsRefZero(){ return ( m_dwRefCount == 0 ); }
	bool				IsRefCount( int nSrcCount ){ return ( m_dwRefCount == nSrcCount ); }

	// ======= Update
protected:
	float				m_fAniTime;
public:
	virtual void		Update( float fElapsedTime, bool bCulling = true );
	void				SetAniTime( float fAniTime ){ m_fAniTime = fAniTime; }
	float				GetAniTime(){ return m_fAniTime; }
protected:
	bool				_UpdateCulling( float fElapsedTime, bool bUpdate );

	// ======= Render
protected:
	bool				m_bCallReady;
	bool				m_bEnableRender;
public:
	inline void			ResetCallReady(){ m_bCallReady = true; }

	// ==== Light
public:
	virtual	void		ApplyLight( CsGBLight* pLight );
	virtual	void		ReleaseLight( CsGBLight* pLight );

	// ======= Info
protected:
	sINFO				m_Info;
	int					m_nGetID;
	bool				m_bEnableUse;
public:
	sINFO*				GetInfo(){ return &m_Info; }
	int					GetGetterID(){ return m_nGetID; }
	void				SetScale( float fScale );
	void				SetEnableUse( bool bEnable ){ m_bEnableUse = bEnable; }
	bool				IsEnableUse(){ return m_bEnableUse; }

	virtual CsGBObject*	Clone();

	// ======= Collision
public:
	enum eCOLLISION_TYPE{ CT_IN_BOUND, CT_OUT_BOUND };
protected:
public:
	eCOLLISION_TYPE	InBound( NiPoint3 ptCenter, float fRadius );
	eCOLLISION_TYPE	InBound( NiPoint2 ptCenter, float fRadius );

	// ======= UserDefine
public:
	enum eACT_TYPE{ ACT_DELETE, };
	enum ePATH_TYPE{ PATH_DELETE, };

protected:
	sUSER_DEFINE*		m_pUserDefine;
public:
	bool				UD_IsNoShadow(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_NOSHADOW ) != 0 ); }
	bool				UD_IsAnimation(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_ANIMATION ) != 0 ); }
	bool				UD_IsAnimationRandom(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_ANIMATION_RANDOM ) != 0 ); }
	bool				UD_IsAlwaysAnimation(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_ALWAYS_ANIMATION ) != 0 ); }
	bool				UD_IsTableObj(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_TABLE_OBJ ) != 0 ); }
	bool				UD_IsUseTexType(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_USE_TEXTYPE ) != 0 ); }
	bool				UD_IsCameraAlpha(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_CAMERA_ALPHA ) != 0 ); }
	bool				UD_IsCameraAlphaPick(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_CAMERA_ALPHA_PICK ) != 0 ); }
	bool				UD_IsCameraCollision(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_CAMERA_COLLISION ) != 0 ); }
	bool				UD_IsCharShadow(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_CHAR_SHADOW ) != 0 ); }
	bool				UD_IsTerrainBasePick(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_TERRAIN_BASE_PICK ) != 0 ); }
	bool				UD_IsNoFog(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_NO_FOG ) != 0 ); }
	bool				UD_IsNoLight(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_NO_LIGHT ) != 0 ); }
	bool				UD_IsPortal(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_PORTAL ) != 0 ); }
	bool				UD_IsRotation(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_ROTATION ) != 0 ); }
	bool				UD_IsRotationLimit(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_ROTATION_LIMIT ) != 0 ); }
	bool				UD_IsMerge(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_MERGE ) != 0 ); }
	bool				UD_IsDarkMap(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_DARKMAP ) != 0 ); }
	bool				UD_IsTerrainHidePick(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_TERRAIN_HIDE_PICK ) != 0 ); }

	bool				UD_IsSound(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_SOUND ) != 0 ); }

	bool				UD_IsSkin(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_SKIN ) != 0 ); }

	bool				UD_IsF1(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_F1 ) != 0 ); }

	//bool				UD_IsShadowPick(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_SHADOW_PICK ) != 0 ); }

	bool				UD_IsNoClipping(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_NO_CLIPPING ) != 0 ); }
	bool				UD_IsNoPick(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_NO_PICK ) != 0 ); }
	bool				UD_IsQuestOn(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_QUEST_ON ) != 0 ); }
	bool				UD_IsQuestOff(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_QUEST_OFF ) != 0 ); }

	bool				UD_IsEffectRender(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_EFFECTRENDER ) != 0 ); }
	bool				UD_IsActorManager(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_ACTORMANAGER ) != 0 ); }
	bool				UD_IsAct(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_ACT ) != 0 ); }
	bool				UD_IsPath(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_PATH ) != 0 ); }

	//bool				UDF_IsDynamic_Pos(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_DYNAMIC_POS ) != 0 ); }
	bool				UDF_IsFirstRender(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_FIRSTRENDER ) != 0 ); }
	bool				UDF_IsCameraAlpha(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_CAMERAALPHA ) != 0 ); }
	bool				UDF_IsCameraPick(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_CAMERA_PICK ) != 0 ); }
	//bool				UDF_IsTBType(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_TB_TYPE ) != 0 ); }
	bool				UDF_IsRotation(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_ROTATION ) != 0 ); }
	bool				UDF_IsHaveChild(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_HAVE_CHILD ) != 0 ); }
	bool				UDF_IsAnimation(){ return ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_ANIMATION ) != 0 ); }
	
	

public:
	sUSER_DEFINE*		GetUserDefine(){ return m_pUserDefine; }

	void				SetTextureType( BYTE btGroup, BYTE btTextureType );

	static void			GlobalSetTextureType( NiNode* pNiNode, sINFO* pInfo, sUSER_DEFINE* pUserDefine, BYTE btGroup, BYTE btTextureType );
	static void			GlobalApplyUserDefine( NiNode* pNiNode, sINFO* pInfo, sUSER_DEFINE* pUserDefine );
	static void			GlobalApplyUserDefine( CsGBObject* pCsGBObject );

protected:
	void				_ApplyUserDefine();

	// ======= NiNode
protected:
	//NiNodePtr			m_pNiNode;
	CsNodeObj			m_Node;
public:
	void				SetTransform( NiTransform tr );
	NiNode*				GetNiNode(){ return m_Node.m_pNiNode; }
	CsNodeObj*			GetCsNode(){ return &m_Node; }

	// ======= Shader
public:
	void				SetShader( NiSingleShaderMaterial* pShader );

	// ======= Child
protected:
	std::list< sCHILD_INFO* >	m_listChild;
	sTOOL_INFO*					m_pToolInfo;
	DWORD						m_dwParentUniqID;	
public:
	std::list< sCHILD_INFO* >*	GetChildList(){ return &m_listChild; }

	void						DeleteChild_FromUniqID( DWORD dwUniqID );

	void						InsertChild( sCHILD_INFO ci );	
	sCHILD_INFO*				GetChild( int nChildIndex );
	sCHILD_INFO*				GetChild_FormSlotID( int nSlotID );
	sTOOL_INFO*					GetToolInfo(){ return m_pToolInfo; }

	void						SetParentID( DWORD dwParentUniqID ){ m_dwParentUniqID = dwParentUniqID; }
	DWORD						GetParentID(){ return m_dwParentUniqID; }

	// ======= Pick
protected:
	bool				m_bCallPicked;
public:
	inline void			ResetPicking(){ m_bCallPicked = false; }
		   void			Picking( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir );
	inline bool			IsCallPicked();

	// ======= Transform
public:
	void				Reposition( float fDeltaHeight );

	// ======= Update ByTool
public:
	inline bool			IsCallReady();

	// ======= Prop
protected:
	CsGBObjPropFactor*	m_pPropFactor;
protected:
	inline CsGBObjPropFactor*	_GetPropFactor();
public:
	inline CsGBObjProp*		GetProp( CsGBObjProp::ePROP_TYPE eType );
	void					PropAvCap_Pick( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir, bool bAppend=true );	
	void					PropAvTbp_Pick( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir, bool bAppend=true );

	// ======= Option
protected:
	CsGBObjOptFactor*	m_pOptionFactor;
protected:
	inline CsGBObjOptFactor*	_GetOptFactor();
public:
	CsGBObjOptFactor*	GetOptionFactor(){ return m_pOptionFactor; }

	// ==== Extra Data
public:
	virtual void		PreSaveExtraData();
	virtual UINT		GetSaveSize();
	virtual bool		SaveExtraData( char* pData, UINT& uiOffset );	
	virtual void		LoadExtraData( char* pData, UINT& uiOffset, DWORD dwCheckPlag );
	static void			LoadObjectInfo( sINFO* pInfo, char* pData, UINT& uiOffset );

	virtual UINT		GetSaveToolSize();
	virtual bool		SaveToolExtraData( char* pData, UINT& uiOffset );
	virtual void		LoadToolExtraData( char* pData, UINT& uiOffset );

	// ==== Shader
public:
	void				CheckShader();

	// ==== Texture
public:
	void				ResetTextureFilterMode();
};

#include "CsGBObject.inl"

