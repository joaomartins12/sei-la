
#pragma once

class cMoneyIF;
class cString : public NiMemObject
{
public:
	struct sELEMENT : public NiMemObject
	{
		enum eTYPE{ TEXT, SPRITE, };

		virtual eTYPE	GetType() = 0;
		virtual void	Delete() = 0;
		virtual void	Render( CsPoint pos ) = 0;
		virtual void	SetAlpha( float fAlpha ) = 0;
		virtual void	ResetDevice(){}

		CsPoint			s_ptSize;
	};
	struct sTEXT : public sELEMENT
	{
		virtual eTYPE	GetType(){ return TEXT; }
		void			Delete(){ s_Text.Delete(); }
		void			Render( CsPoint pos ){ s_Text.Render( pos ); }
		void			SetAlpha( float fAlpha ){ s_Text.SetAlpha( fAlpha ); }
		virtual void	ResetDevice(){ s_Text.ResetDevice(); }

		cText		s_Text;
	};
	struct sSPRITE : public sELEMENT
	{
		virtual eTYPE	GetType(){ return SPRITE; }
		void			Delete(){ s_pSprite = NULL; }
		void			Render( CsPoint pos ){ s_pSprite->Render( pos + s_ptDeltaPos ); }
		void			SetAlpha( float fAlpha ){ s_pSprite->SetAlpha( fAlpha ); }
		void			ResetDevice(){ s_pSprite->ResetDevice(); }

		CsPoint		s_ptDeltaPos;
		cSprite*	s_pSprite;
	};	

public:
	enum { INVALIDE_STRING_VALUE = INT_MAX, };
	cString(){ _Reset(); }
	~cString(){	Delete(); }

	void	Delete();
	void	ResetDevice();
	void	AddText( cText::sTEXTINFO* pTextInfo );
	void	AddText_Head( cText::sTEXTINFO* pTextInfo );	
	void	AddSprite( cSprite* pSprite, CsPoint deltaPos = CsPoint::ZERO );

	int		Render( CsPoint pos );
	int		Render( CsPoint pos, UINT Align );
	void	SetAlpha( float fAlpha /*부하가적다*/ );

protected:
	void	_Reset(){ m_nMaxSize = CsPoint::ZERO; m_nValue1 = m_nValue2 = INVALIDE_STRING_VALUE; }

protected:
	std::list< sELEMENT* >	m_listText;
	CsPoint					m_nMaxSize;
	int						m_nValue1;
	int						m_nValue2;

public:
	void		CalMaxSize();
	CsPoint		GetMaxSize(){ return m_nMaxSize; }

	void		SetValue1( int nValue1 ){ m_nValue1 = nValue1; }
	int			GetValue1(){ return m_nValue1; }
	void		SetValue2( int nValue2 ){ m_nValue2 = nValue2; }
	int			GetValue2(){ return m_nValue2; }

	void		AddSizeY( int nSizeY ){ m_nMaxSize.y += nSizeY; }

	void		TailAddSizeX( int nSizeX );

	int			GetListSize(){ return (int)m_listText.size(); }

	// Find
public:
	sELEMENT*	GetElement( int nIndex );
	bool		IsText( TCHAR* szText );
};

class cStringList : public NiMemObject
{
public:
	enum{ INVALIDE_FIND = INT_MAX, };

public:
	cStringList(){}
	~cStringList(){	Delete(); }

public:
	void		Delete();
	void		ResetDevice();

	int			Update_ForMouse( int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY, CsSIZE CheckSize );
	void		Render( CsPoint pos, int nDeltaY, UINT Align = DT_LEFT );
	void		SetAlpha( float fAlpha /*부하가적다*/ );

public:
	int			GetSize(){ return (int)m_list.size(); }
	void		AddHead( cString* pString );
	void		AddTail( cString* pString );
	void		AddIndexAfter( cString* pString, int nIndex );

	cString*	GetString( int nIndex );
	cString*	GetTail();
	void		RemoveHead();
	void		RemoveTail();

	CsPoint		CalMaxSize( int nDeltaY );

	
	std::list< cString* >*	GetList(){ return &m_list; }


	//====================================================================================================
	//
	//		Find
	//
	//====================================================================================================
public:
	int			FindText( TCHAR* szText );


public:
	std::list< cString* >	m_list;
};

