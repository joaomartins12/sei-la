
#pragma once 


class cMoneyIF : public cWindow
{
public:
	cMoneyIF();
	virtual ~cMoneyIF(){ Delete(); }

public:
	enum eDISP_TYPE{ DISP_FULL, DISP_SIMPLE_1, DISP_DIGICORE, DISP_EVENT, DISP_PROPERTY, };
	enum eUNIT{ BIT, MEGA, TERA, DIGICORE, EVENT, UNIT_MAX_COUNT, };

	// Àü¿ª
public:
	static void			GlobalInit();
	static void			GlobalShotDown();
protected:
	static cSprite*		g_pMbox;
	static cSprite*		g_pBG;
	static cSprite*		g_pUnit[ UNIT_MAX_COUNT ];


public:
	virtual eTYPE	GetType(){ return T_Money; }
	virtual void	Delete();
	virtual void	ResetDevice();
	virtual	void		OnMouseLeave() {};
	virtual void		Update(float const& fDeltaTime) {}

	void			Init( eDISP_TYPE dt, sMONEY money, bool bShowBackground, NiColor color = FONT_WHITE, int eAllign = DT_RIGHT, CFont::eFACE_SIZE eFontSize = CFont::FS_9 );
	void			Init_ForEventStore(DWORD dwItemCode, eDISP_TYPE dt, sMONEY money, bool bShowBackground, NiColor color = FONT_WHITE, int eAllign = DT_RIGHT, CFont::eFACE_SIZE eFontSize = CFont::FS_9 );

	void			SetMoney( sMONEY money );
	void			SetEventItemID( DWORD itemID );

	cString*		GetString() const;

	void			Init( eDISP_TYPE dt, sMONEY money, cItemInfo* ItemInfo, DWORD dwNpcID, int nKind, int nIcon, bool bShowBackground, NiColor color = FONT_WHITE, int eAllign = DT_RIGHT, CFont::eFACE_SIZE eFontSize = CFont::FS_9 );

	void			Init_ForPropertyStore( DWORD dwNpcID );

	void			SetPropertyStorePrice( sMONEY money, int nKind, int nIcon );
	void			SetMoney( sMONEY money, cItemInfo ItemInfo );	
	cSprite*		Getg_pMbox() { return g_pMbox; }
	bool			GetShowBG() { return m_bShowBG; }
	void			SetShowBG(bool bShow) { m_bShowBG = bShow; }
	void			Render( CsPoint pos );
	virtual void	Render();



	void			SetColor_BeforeSetMoney( NiColor color );
	void			SetColor( NiColor color );
	NiColor			GetColor() const;

protected:
	void			_SetFullMoney();
	void			_SetSimpleMoney();
	void			_SetDigicore();
	void			_SetEvent(DWORD dwItemCode = 0);

	void			_SetPropertyA(int nIconID );
	void			_SetPropertyB(int nIconID );
	void			_SetPropertyC(int nIconID );
	void			_SetPropertyD(int nIconID );
protected:
	cString*		m_pString;
	bool			m_bShowBG;
	float			m_fAlpha;
	int				m_eAllign;
	CFont::eFACE_SIZE	m_eFontSize;

	NiColor			m_Color;
	sMONEY			m_Money;

	eDISP_TYPE		m_eDisplayType;

	CsPoint			m_ptSize;

	CsPoint			m_tempPt;
	DWORD			m_dwIconItemCode;
	DWORD					m_dwNpcID;

public:
	sMONEY			GetMoney(){ return m_Money; }
	CsPoint			GetSize(){ return m_ptSize; }
};