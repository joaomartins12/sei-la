
#pragma once 

class cMiniMap : public cBaseWindow, public CMapContents::ObserverType
{
public:
	static	void		GlobalInit();
	static	void		GlobalShotDown();

	// 엔피씨 관련
	struct sNPC_INFO : public NiMemObject
	{
		CSGBMEMPOOL_H( cMiniMap::sNPC_INFO );
		sNPC_INFO():s_pFTNpc(NULL),s_pNpc(NULL),s_vPos(NiPoint2::ZERO)
		{};	

		CsNpc*		s_pFTNpc;
		CNpc*		s_pNpc;
		NiPoint2	s_vPos;
	};

public:
	cMiniMap();
	~cMiniMap();

public:
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_MINIMAP; }
	virtual void				Destroy();
	virtual void				DeleteResource();
	virtual void				Create( int nValue = 0 );

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();

	virtual void				Render();
	virtual void				ResetDevice();

	void						LoadMiniMap();
	void						LoadDatsMiniMap( bool bSet );
	void						SetRegion( TCHAR const* szRegion );
	void						SetPosFind();
	TCHAR *						Get_MapName();

protected:
	void						_Update_ForMouse_Party();
	void						_Update_TamerPos();
	void						_DeleteNpcInfo();
	void						_CalMapValue();
	CsPoint						_CalZoneMapPos( float fZoom, NiPoint2 vPos, CsPoint ptImageSize );
	void						_RenderNpc();
	void						_RenderParty();

protected:
	virtual	bool				Construct(void);
	virtual	void				Notify(int const& iNotifiedEvt, ContentsStream const& kStream);



	// 메모리잡힌 컨트롤
protected:
	cSprite*					m_pMiniMap;
	cText*						m_pMapText;
	cText*						m_pRegionText;
	cButton*					m_pNpcFindBtn;
	cButton*					m_pUpScaleBtn;
	cButton*					m_pDownScaleBtn;	
	cButton*					m_pNpcCancel;
	cButton*					m_pMapCancel;

	// 포인터만 있는 컨트롤
protected:
	cSprite*						m_pArrow;
	cButton*						m_pToggleMapBtn;
	cButton*						m_pPosFindCancelBtn;

	CsQuestCompRequire::eTYPE		m_eType;
	sCHAR_IMAGE*					m_pImage;

protected:
	bool						m_bHeightHigh;
	DWORD						m_dwOldMapID;
	float						m_fBackupZoom;
	float						m_fZoom;

	TCHAR						m_szMap[ MAX_FILENAME ];
	TCHAR						m_szRegion[ FT_MAP_REGION_DISC_LEN ];

	CsAreaMap::eFIELD_TYPE		m_eFieldType;
	bool						m_bRenderRegionText;

protected:	
	std::list< sNPC_INFO* >		m_listNpcInfo;
	// 계산하는데 필요한 값
	CsPoint						m_ptOrgPos;
	NiPoint2					m_vTamerPos;
	float						m_fAspectiveHeight;
	float						m_Meter_To_Pixel;	
	float						m_fInvertImageSize;
	float						m_fHalfDeltaSize;
	float						m_fMaxSize;
	float						m_fTexselSize;
};
