
#pragma once 

class cComboBox : public cButton
{
public:
	cComboBox();
	virtual ~cComboBox();
	
enum COMBO_EVENT
	{
		COMBO_ADDITEM  = 0,
		COMBO_INSERTITEM,
		COMBO_REMOVEITEM,
		COMBO_SELECTITEM,
	};
public:
	enum eINDEX{ CI_INVALIDE = -1, CI_SAME_CLICK = -2, CI_CLICK = -3, CI_INRECT = -4, };	
	enum eComboType{ CT_NORMAL = 0, CT_NORMAL2,
	};

	//=======================================================================================================================
	//
	//		Base
	//
	//=======================================================================================================================
public:
	void			Delete();
	void			Init( cWindow* pParent, eComboType eType, CsPoint pos, CsPoint size, CFont::eFACE_SIZE eFontSize, int eFontAlign, bool bClear, bool bAutoSelect, bool bApplyWindowSize, bool bStaticCenter = false );	

	virtual void	Update(float const& fDeltaTime);
	virtual int		Update_ForMouse();
	virtual void	Render();
	virtual void	Render( CsPoint pos );

	virtual eTYPE	GetType();

	virtual void	SetAlpha( float fAlpha );

	virtual void	ResetDevice();

	void			SetComboListWindow( char const* pPopUpFile );
	void			ChangeComboBoxImg( char const* pPopUpFile, CsPoint tokenSize );


public:
	void			AddItem( std::wstring* szItem, int nData = 0, NiColor color = FONT_WHITE );
	void			AddItem( TCHAR const* szItem, int nData = 0, NiColor color = FONT_WHITE );
	void			AddItem( cString* pString, int nData = 0, NiColor color = FONT_WHITE );

	void			RemoveAllItem(void);
	bool			SetCurSel( int nSel, bool bEvent = true );
	bool			SetCurSel_FromData( int nData );

	int				GetCurSel_Data();
	int				GetCurSel_Idx();

	int				GetCount();
	int				GetData( int nSel );	

	bool			IsRenderPopup();
	bool			IsSelect(); 
	void			SetVisibleRenderPopup(bool bValue);

	cString*		GetSelectedItem();

	void			SetDeltaPos( CsPoint ptDeltaPos );

protected:
	cPopUp*			m_pPopupWindow;
	cSprite*		m_pSelMask;
	CsPoint			m_ptPopupPos;
	CsPoint			m_ptSelMaskPos;
	bool			m_bRenderPopup;
	bool			m_bRenderSelMask;

	cStringList		m_StringList;


	CFont::eFACE_SIZE	m_eFontSize;
	int					m_eFontAlign;

	CsPoint			m_ptDeltaPos;			// 현재 선택된 아이템의 글자 출력 위치 조정

	int				m_nCurSelIndex;
	cString*		m_pCurSel;
	bool			m_bAutoSelect;
	bool			m_bStaticCenter;
};