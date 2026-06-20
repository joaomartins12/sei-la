#pragma once
#include "../00.Contents/FriendListContents.h"
class cFriendList : public cBaseWindow, public cFriendListContents::ObserverType
{
	struct sListBoxIndexInfo : public CUSTOMDATA
	{
		sListBoxIndexInfo(int data, int ui)
		{
			sdataIndex = data;
			sUIIndex = ui;
		}
		~sListBoxIndexInfo() {};
		int sdataIndex;		//contents의 데이터 리스트 내의 인덱스
		int sUIIndex;		//UI Listbox에서의 인덱스
	};
public:
	cFriendList();
	~cFriendList();

public:
	enum eTAB { TAB_FRIEND = 0, TAB_SHUTOUT, };
	eTAB			m_eRadioIndex;

	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual	bool			Construct(void);
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetMap(){ Close(false); }
	virtual void			PlayOpenSound(){}

	virtual bool			OnEscapeKey();
	virtual void			OnMoveWindow();
	
	

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	//친구
	void			MakeFriend();	// 초기화
	void			AddFriend( const cFriendListContents::FRIEND_INFO* pInfo, int nRealDataIdx );
	//차단
	void			MakeShutOut();	// 초기화
	void			AddShutOut( const cFriendListContents::SHUTOUT_INFO* pInfo, int nRealDataIdx );

	void			_SetPopup( int nDataIdx );
	int				DeletePerson( int nDataIdx );

	eTAB			_GetTab(){ return m_eRadioIndex; }
	void			_SetTab( eTAB eCurTab );

private:
	void			TabClick(void* pSender, void* pData);
	void			CloseClick(void* pSender, void* pData);
	void			OfflineDisplayClick(void* pSender, void* pData);
	void			TamerMenuPop(void* pSender, void* pData);
	
	

private:
	cButton*		m_pCancelButton;	//닫기	
	cCheckBox*		m_pOnOffCheck;		//오프라인 표시 체크

	cRadioButton*	m_pRadioBtn;		//친구/차단 탭 버튼
	cText*			m_pFriendCnt;		//친구 인원 텍스트 "%d / %d"
	cListBox*		m_pPersonInfoListBox;	//친구/차단 리스트
};
