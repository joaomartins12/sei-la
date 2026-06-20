
#pragma once 

class cSmallMap : public cBaseWindow, public CMapContents::ObserverType
{
public:
	cSmallMap();

public:
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_SMALLMAP; }
	virtual void				Destroy();	
	virtual void				DeleteResource();	
	virtual void				Create( int nValue = 0 );

	virtual bool				Close( bool bSound =true );

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();

	virtual void				Render();
	virtual void				ResetDevice();		

	void						LoadSmallMap();
	void						LoadSmallMap2( DWORD nMap );

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	void			_MoveUi( void* pSender, void* pData );			// 2016-03-15 미니맵 이동을 위한 함수

protected:	
	void						_CalValue( DWORD dwMapID );
	CsPoint						_CalMapPos( NiPoint2 vPos, CsPoint ptSize );
	void						_Update_TamerPos();
	bool						_ResetSize( int nNum );

	void						_DeleteNpcInfo();	
	void						_RenderNpc();
	void						_RenderRegion();
protected:
	cSprite*					m_pMap;	
	cSprite*					m_pTamerPoint;
	cSprite*					m_pPosPoint;
	cButton*					m_pCancelButton;
	cButton*					m_pTranslate;
	cButton*					m_pMoveWindow;	
	cButton*					m_pPlus;	
	cButton*					m_pMinus;	

	bool						m_bDungeon;			// 던전인 경우는 지역명 표시 안함.
	cSprite*					m_pRegionIcon;		// 지역명 표시 아이콘
	cText*						m_RegionName;		// 지역명

	float						m_fSize; 
	float						m_fMove;
	int							m_nSign;

protected:
	// 계산하는데 필요한 값
	bool						m_bHeightHigh;
	CsPoint						m_ptArrowPos;
	float						m_Meter_To_Pixel;
	float						m_fHalfDeltaSize;
	float						m_fMaxSize;
	float						m_fAspectiveHeight;

protected:	
	std::list< cMiniMap::sNPC_INFO* >		m_listNpcInfo;

#ifdef ZONEMAP_CLICK_MOVE	// SmallMap 에서는 없었던 기능으로 GM텔레포트와 클릭 길찾기를 둘다 포함
	void						_GMTeleport();
	void						_ClickMapMove();
	CsPoint						_GetMoveArrivePos();	// 이동하려는 맵의 XY좌표를 반환
#endif
	
};