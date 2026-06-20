
#pragma once 

class cDivision : public cBaseWindow, public CPartyContents::ObserverType
{
public:
	cDivision();
	~cDivision();

	enum eCROP { CROP_BASE = 0, CROP_FREE, CROP_ORDER, CROP_DICE, CROP_END };
	enum eITEM_CLASS { CLASS_LV1 = CROP_END + 1, CLASS_LV2, CLASS_LV3, CLASS_LV4 };

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DIVISION; }
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			PreResetMap(){ Close( false ); }
	virtual void			ResetDevice();	

	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();		

	virtual void			Render();

protected:
	void					_SetBooty();
	void					_SetRareRate();

protected:		
	cStringList				m_strDivision;
	cPopUp*					m_pBGWindow;	
	cSprite*				m_pSelMask;
	CsPoint					m_ptSelMask;
	bool					m_bRenderSelMask;

};

