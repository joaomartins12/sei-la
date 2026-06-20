
#pragma once 

class cJumpBuster : public cBaseWindow
{
public:
	cJumpBuster();
	virtual ~cJumpBuster();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_JUMPBUSTER; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual void			PreResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnMoveWindow();

protected:
	bool					_CheckMapList( CsWorldMap* pZone );

	void					_MakeZoneList();
	void					_MakeMapList( CsWorldMap* pZone );

public:
	void					SetJumpBusterItemInvenPos( int nInvenPos );
	void					SetVipMemberShip(bool bValue);

	// 메모리잡힌 컨트롤
protected:
	cStringList		m_ZoneList;
	cStringList		m_MapList;	

	// 포인터만 있는 컨트롤
protected:
	cScrollBar*		m_pZoneScrollBar;
	cScrollBar*		m_pMapScrollBar;

	cButton*		m_pCancelButton;
	cButton*		m_pAcceptButton;

	// 셀렉트 관련
protected:
	cSprite*		m_pMouseOnMask;
	cSprite*		m_pSelectMask;

	int				m_nMouseOnMaskIndex_Zone;
	int				m_nSelectMaskIndex_Zone;
	bool			m_bRenderMouseOnMask_Zone;
	bool			m_bRenderSelectMask_Zone;

	int				m_nMouseOnMaskIndex_Map;
	int				m_nSelectMaskIndex_Map;
	bool			m_bRenderMouseOnMask_Map;
	bool			m_bRenderSelectMask_Map;

protected:
	int				m_nJumpBusterItemID;
	int				m_nItemInvenPos;
	bool			m_bVipMemberShip;
};