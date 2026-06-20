#pragma once

#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfinitewarNoticeBoardDailyUI : public cScriptUI, public CInfiniteWarRankRewardContents::ObserverType
{
	struct sRewardUIContiner
	{
		sRewardUIContiner();
		~sRewardUIContiner();

		cString::sTEXT*		s_pNeedPointText;
		int					s_nNeedPoint;
		std::list<cString::sSPRITE*>	s_pCheckImgs;

		void				SetRewardItemGet(DWORD const& dwTotalPoint);
	};

public:
	CInfinitewarNoticeBoardDailyUI();
	virtual ~CInfinitewarNoticeBoardDailyUI();

public:
	bool			Create(cWindow* pRoot);
	BOOL			UpdateMouse();					
	BOOL			UpdateKeyboard(const MSG& p_kMsg);

	virtual	bool	Construct(void);
	virtual void	RenderScript();

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void			Destroy();
	void			_InsertPoint(void* pSender, void* pData);
	void			_LeftBtnEvent(void* pSender, void* pData);
	void			_PreviousBtnEvent(void* pSender, void* pData);
	void			_RightBtnEvent(void* pSender, void* pData);
	void			_NextBtnEvent(void* pSender, void* pData);

	void			_SetEditBoxCount( int nValue );
	void			_EditBoxCountChange( int nValue );
	void			_UpdateRewardItemList();
	void			_UpdateTodayTotalInjectedPoint( DWORD const& dwPoint );
	void			_ResetRemainTime( __int64 const& nTime );

	void			_SetEditBoxFocus(void* pSender, void* pData);
	void			_ReleaseEditBoxFocus(void* pSender, void* pData);
	void			_SetNeedItem(DWORD const& dwItemCode);
	void			_SendRequestDataReset(void* pSender, void* pData);

private:
	cListBox*				m_pRewardItemsList;	// 보상 아이템 목록 리스트
	cText*					m_pTodayInsertedPoint;	// 오늘 투입한 총 포인트

	cText*					m_pNeedItemName;		// 포인트 투입시 필요한 아이템의 이름
	cButton*				m_pInsertPointBtn;

	cButton*				m_pLeftBtn;
	cButton*				m_pPreviousBtn;
	cButton*				m_pRightBtn;
	cButton*				m_pNextBtn;
	cEditBox*				m_pEditCount;
	cUI_TimerProgressBar*	m_pTimerBar;

	DWORD					m_dwRenderItemCode;
	DWORD					m_dwMaxNeedItemCount;
	
	std::list<sRewardUIContiner>	m_ControlsContiner;
};