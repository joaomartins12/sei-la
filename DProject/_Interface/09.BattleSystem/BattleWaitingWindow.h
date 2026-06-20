#pragma once

#include "../00.Contents/BattleSystemContent.h"

class CBattleWaitWindow : public cBaseWindow, public CBattleSystemContents::ObserverType
{
	struct sTeamUI
	{
		enum eReadyState
		{
			eReject = -1,
			eNone = 0,
			eAccept = 1,
		};
		sTeamUI();
		~sTeamUI();

		void			SetReadyType( sTeamUI::eReadyState eType );

		cSprite*		m_pDigimonImg;
		cSprite*		m_pImgCover;
		cSprite*		m_pReadyStateImg;
		int				m_nReadyType; // -1, 0, 1
	};

	enum eWaitType
	{
		eWait,
		eAsk,
		eAccpt_My,
		eReject_My,
		eAccpt_Other,
		eReject_Other,
	};
public:
	CBattleWaitWindow();
	virtual ~CBattleWaitWindow();

	virtual	bool	Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();	

	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();

	virtual void			Render();		
	virtual void			PreResetMap();
	virtual bool			OnEscapeKey();

protected:
	void					Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					_CloseWindow(void*pSender, void* pData);
	void					_BattleAccept( void* pSender, void* pData );
	void					_BattleCancel(void* pSender, void* pData);
	void					_CloseWindow( float const& fDelayTime );

	void					_DataUpdate();
	void					_MakeEmptySlot(int nTeamCode, cGridListBox * pControl, int count);


	void					_MakeAskMsg(eWaitType const& nBattleType);

	bool					_IsAllReady();
	void					_StopTimer();
	sTeamUI *				_GetTeamUI( DWORD const& playerUIDX );
	void					_SetRejectPlayer(DWORD const& playerUIDX);
	void					_SetAcceptPlayer(DWORD const& playerUIDX);
	void					_SetPlayerUI( int const& nTeamCode, cGridListBox* pControl, BWait::sWait::sPlayerInfo const& playerInfo, int const& nCount);

	void					_BattleReadRequest();
	void					_CloseWaitWindow( float const& fDelay );

private:	
	cButton*				m_pBtnClose;
	cButton*				m_pBtnJoinBattle;
	cButton*				m_pBtnCancelBattle;
	cText*					m_pTextBattleMsg;
	cText*					m_pTextBattleMsg_Ask;
	cUI_TimerProgressBar*	m_pProgressBar;
	float					m_fCloseDelayTime;
	cGridListBox*			m_pRedTeamList;
	cGridListBox*			m_pBlueTeamList;
	cImage*					m_pVersusType;

	std::map<DWORD/*Player UIDX*/,sTeamUI>		m_PlayerUIContiner;
};