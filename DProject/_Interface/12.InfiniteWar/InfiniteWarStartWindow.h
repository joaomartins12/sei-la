#pragma once 

#include "../00.Contents/InfiniteWar_Contents.h"

class CInfiniteWarStartWindow : public cBaseWindow, public CInfiniteWarContents::ObserverType
{
	struct sStepUIControls
	{
		sStepUIControls();
		~sStepUIControls();
		cSprite*			m_pBackGlow;
		cSprite*			m_pMonsterImage;
		cString::sTEXT*		m_pMonsterName;		
	};
public:
	CInfiniteWarStartWindow();
	virtual ~CInfiniteWarStartWindow();

	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	void					ResetDevice();
	bool					OnEscapeKey();

private:
	void					_CloseWindow( void* pSender, void* pData );
	void					_CreateBattleList();
	void					_makeSetListItem(int const& nStage, CInfiniteWarContents::sInfiniteWarInfo const& stepInfo, cListBox * pList);
	void					_UpdateStageInfo( int const& nStage );

	void					_UpdateTotalPoint( DWORD const& dwTotalPoint );
	void					_NextChallengeBtnEvent(void* pSender, void* pData);
	void					_ChallengeGiveupBtnEvent(void* pSender, void* pData);
	void					_SetCurrentChallengeStage(int const& nChallengeStage);

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	cButton*				m_pCloseBtn;
	cButton*				m_pNextChallengeBtn;
	cButton*				m_pGiveupBtn;
	cListBox*				m_pStepList;
	cText*					m_pTotalPoint;
	std::map<int, sStepUIControls>	m_mapStepControls;
};


