
#pragma once 

#include "../00.Contents/MainFrame_Contents.h"

class cXaiGuageUI : public cBaseWindow, public CMainFrameContents::ObserverType
{
	struct sCrystalUI
	{
		sCrystalUI():pBackImg(NULL),pFullImg(NULL),bEmpty(true)
		{}

		void SetEnable( bool bEnable );
		bool IsEmpty() const;

		cSprite* pBackImg;
		cSprite* pFullImg;
		bool bEmpty;
	};
public:
	cXaiGuageUI();
	~cXaiGuageUI();

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			PlayOpenSound(){}
	virtual void			PlayCloseSound(){}

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();

	virtual void			OnRButtonUp( CsPoint pos );
	virtual	void			OnLButtonUp( CsPoint pos );

	virtual void			ResetDevice();

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					_UpdateGuage( u2 const& nCrystal, u4 const& nGuage );
	void					_UpdateMaxGuage( unsigned short const& nMaxCrystal, unsigned int const& nMaxGuage );
	void					_ClickCrystal(void* pSender, void* pdata);

private:
	cUI_ProgressBar*		m_pXaiGuage;
	cGridListBox*			m_pXaiDia;
	std::list<sCrystalUI>	m_listCrystalUI;
};