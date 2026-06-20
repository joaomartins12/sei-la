
#pragma once 


class cRadioButton : public cCheckBox
{
public:
	cRadioButton(){}
	virtual ~cRadioButton(){ Delete(); }

	enum RADIOBUTTON_EVENT
	{
		RadioButton_LBUP_EVENT = 0,
	};
	
public:
	enum eRADIO_CLICK{ INVALIDE_RADIO_CLICK = -1, ANI_RADIO_PRESS = -2, };

	//=======================================================================================================================
	//
	//		Base
	//
	//=======================================================================================================================
protected:
	int								m_nCheckIndex;		
	CsVectorPB< cRadioButton* >		m_vpRadioNode;
public:
	void			Delete();
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, CsPoint tex, char* cImagePath, bool bApplyWindowSize );
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, TCHAR const* cs, bool bApplyWindowSize );
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, cText::sTEXTINFO* ti, bool bApplyWindowSize );

	cRadioButton*	AddNode( CsPoint pos, CsPoint size, CsPoint tex, char* cImagePath );
	cRadioButton*	AddNode( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, TCHAR const* cs );
	cRadioButton*	AddNode( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, cText::sTEXTINFO* ti );

	int				GetSize() { return (int)m_vpRadioNode.Size(); }
	virtual int		Update_ForMouse();
	virtual void	Render();

	virtual eTYPE	GetType(){ return T_RadioButton; }

	void			SetCheckIndex( int nIndex, bool bEvent = false );
	int				GetCheckIndex(){ return m_nCheckIndex; }			
	virtual void	SetPos( CsPoint ptPos, CsPoint ptDelta );	

	virtual void	SetAlpha( float fAlpha );
	virtual void	ResetDevice();

	cRadioButton*	GetNode( int nIndex ){ assert_cs( m_vpRadioNode.IsExistElement( nIndex ) == true ); return m_vpRadioNode[ nIndex ]; }
	const cRadioButton*	GetSelectedNode();
};