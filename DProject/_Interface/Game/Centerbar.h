
#pragma once 

#define CENTERBAR_X		3
#define CENTERBAR_Y		2

class cCenterbar : public cBaseWindow
{
public:
	cCenterbar();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CENTERBAR; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime){}
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnMouseOutWindow();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			ResetDevice();
	virtual void			ResetMap(){}

	//virtual bool			OnMacroKey();	

	static void				CheckAllSlot();	

protected:
	int					_InvenToCenterBar( int nInvenIndex, cResist::sCHAR_CENTERBAR::sINFO* pData );
	int					_CenterBarToCenterBar( int nInvenIndex, cResist::sCHAR_CENTERBAR::sINFO* pData );

protected:
	cSprite*				m_pBg;
	int						m_nValue;

protected:
	struct sCENTER
	{
		sCENTER(){}
		void Delete(){ s_IFIcon.ReleaseState( true ); s_bRender = false; }
				
		sIFIcon								s_IFIcon;
		bool								s_bRender;
	};
	sCENTER			m_CenterInfo[ CENTERBAR_X * CENTERBAR_Y ];	
};