
#pragma once 

class cWindow : public NiMemObject
{
	//=======================================================================================================================
	//
	//		»ý¼ºÀÚ ¼Ò¸êÀÚ
	//
	//=======================================================================================================================
public:
	cWindow(){		
		m_pParentWindow = NULL;
	}
	virtual ~cWindow(){}

	//=======================================================================================================================
	//
	//		static
	//
	//=======================================================================================================================
public:
	static void		GlobalInit();
	static void		GlobalShotDown();

	//=======================================================================================================================
	//
	//		Base
	//
	//=======================================================================================================================
protected:
	CsPoint				m_ptClient;	
	CsPoint				m_ptDefault;
	CsPoint				m_ptSize;

	float				m_fScaleUI;

	cWindow*			m_pParentWindow;
	bool				m_bApplyWindowSize;

public:
	virtual void		Delete(){}
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, float fScaleUI, bool bApplyWindowSize );	
	virtual void		Render(){}
	

	//=======================================================================================================================
	//
	//		state
	//
	//=======================================================================================================================
public:
	virtual void		SetPos( CsPoint ptPos );
	virtual void		SetSize( CsPoint ptSize );

	void				SetMove( CsPoint ptDelta );

	CsPoint				GetDefaultPos(){ return m_ptDefault; }

	CsPoint				GetClient(){ return m_ptClient; }
	CsPoint				GetSize(){ return m_ptSize; }

	//======================================================================================
	//
	//		ResetDevice
	//
	//======================================================================================
public:
	virtual void		ResetDevice( CsPoint pos );
	virtual void		ResetDevice();
};

