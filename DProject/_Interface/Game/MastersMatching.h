
#pragma once 
#define MAX_RANK		10

class cMastersMatching : public cBaseWindow
{
public:
	cMastersMatching();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_MASTERS_MATCHING; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnMoveWindow();

	virtual void			UserDie(){ Close( false ); }
/************************************************************************/
/*                              이미지                                             */
/************************************************************************/
private:
	cSprite*			m_pBG;							//윈도우 배경
	cSprite*			m_pTitle;						//타이틀
	cSprite*			m_pAlphaTeam;				//A팀 디지몬 이미지
	cSprite*			m_pOmegaTeam;				//B팀 디지몬 이미지
	
	cSprite*			m_pATeamScore;				//A팀 스코어보드
	cSprite*			m_pATeamRank;				//A팀 랭킹
	cSprite*			m_pBTeamScore;				//A팀 스코어보드
	cSprite*			m_pBTeamRank;				//B팀 랭킹

	cSprite*			m_pMyScore;					//본인 스코어보드
	
	cSprite*			m_pInsertBG;					//기부 배경

/************************************************************************/
/*                              버튼                                                */
/************************************************************************/
private:
	cButton*			m_pCancelBtn;				//닫기버튼
	cButton*			m_pInsertBtn;					//기부버튼
	cButton*			m_pIncreaseBtn;				//증가버튼
	cButton*			m_pIncreaseEndBtn;			//증가버튼(최대)
	cButton*			m_pDecreaseBtn;				//감소버튼
	cButton*			m_pDecreaseEndBtn;		//감소버튼(최대)

/************************************************************************/
/*                   기부 아이템 갯수 입력 관련                            */
/************************************************************************/
public:
	HWND			m_hEditCnt;							//개수 입력 에디트박스
	HBRUSH			m_hEditBrush;							//브러시
	HFONT			m_hEditFont;							//폰트
	int					m_nMaxCount;					//최대 개수
	cItemInfo		m_ResistItem;						//등록 아이템 정보

	void				_CreateEditBox( CsPoint ptPos, CsPoint ptSize );	//에디트박스 생성
	LRESULT			ApplyEditColor( HWND hEdit, HDC dc );				//색 설정
	void				_SetCount( int nCount, bool bSetEditControl );		//에디트박스 텍스트 셋
	int					_GetCount();													//에디트박스 텍스트 겟
	bool				OnChangeEdit( DWORD nCtrlID );						//에디트박스 텍스트 변경
	void				ChangEditText( void* pSender, void* pData );

	//  [4/6/2016 hyun] 추가
private:
	cEditBox*			m_pCountEditBox;
	// @@ 여기까지


/************************************************************************/
/*                              텍스트                                             */
/************************************************************************/
private:
	cText*			m_szMyCnt;				// 본인 기부 갯수
	cText*			m_szMyRank;					// 본인 랭크
	cText*			m_szMyTeam;					// 본인 팀 1 = A팀, 2 = B팀

	cText*			m_szTeam_Cnt_A;			// 1팀 기부 갯수
	cStringList*		m_pRankList_A;		// 10위까지의 랭커 정보

	cText*			m_szTeam_Cnt_B;			// 2팀 기부 갯수
	cStringList*		m_pRankList_B;		// 10위까지의 랭커 정보

	cText*			m_szItemName;			// 등록된 기부아이템 이름
	cText*			m_szInsertBtn;			// 기부버튼 "기부"
public:
	TCHAR			m_szResetTime[ 32 ];		//다음 리셋 시간 (캐릭터형으로 받아서 바로 뿌림)
	//10위까지 랭커 정보
	void				_SetRank( cStringList* pTeam_A, cStringList* pTeam_B ){ m_pRankList_A = pTeam_A; m_pRankList_B = pTeam_B; }
	//팀 기부 갯수 
	void				_SetTeamCnt( int nTeamIdx /* 1 = A팀, 2 = B팀*/, u4 nCnt );
	//본인 갯수 
	void				_SetMyCnt( u4 nBallCnt, u2 nRank );
	//정보 갱신
	void				RefreshList();
/************************************************************************/
/*                        아이템 등록 관련                                     */
/************************************************************************/
public:
	bool			_FindItem( int nFTItemCnt, bool bIsMax );	// NPC테이블에 있는 기부아이템이 인벤에 존재하는지 확인, 등록할 갯수
	bool			IsResist( int nInvenIndex );
	void			SetResist( int nInvenIndex );
	void			SetResist( int nInvenIndex, int nCount );
	int				m_nItemSlot;	//등록한 아이템 인벤 슬롯
	int				m_nItemCnt;	//등록 갯수

	void			_CheckAchieve( u4 nCount ); //기부 할 때 이펙트 터트릴 지 확인

/************************************************************************/
/*						  			  ETC			                                  */
/************************************************************************/
public:
	//타겟 NPC
	CsC_AvObject*			m_pTarget;
	//기부 아이템(NPC테이블)
	sNPC_TYPE_MASTER_MATCHING*		m_pFTInsertItem;

	void			SetTarget( CsC_AvObject* pTarget );
	void			SetInsertItem( sNPC_TYPE_MASTER_MATCHING* pFTInsertItem ) { m_pFTInsertItem = pFTInsertItem; }
};

//구조체, ENUM
enum eTEAM{
	//팀 구분 enum
	NONE ,			//미참여
	A_TEAM = 1,	//A팀
	B_TEAM = 2,	//B팀
};
struct sMastersMatch_MyInfo
{
	sMastersMatch_MyInfo()
	{
		s_eMyTeam = NONE;
		s_nMyCnt = 0;
		s_nMyRank = 0;
		s_nNextResetTime = 0;
		ZeroMemory( s_szNextResetTime, 32 );
	}	

	//본인 정보
	eTEAM			s_eMyTeam;					//팀 구분 ( 0 = 미참여, 1 = A팀, 2 = B팀	)
	u4					s_nMyCnt;						// 본인 기부 갯수
	u2					s_nMyRank;					// 본인 랭킹
	u4					s_nNextResetTime;			// 서버에게 받는 다음 리셋 시간 - 틱
	char				s_szNextResetTime[ 32 ];	// 서버에게 받는 다음 리셋 시간 - 실제 시간
};
struct sMastersMatch_RankInfo
{
	sMastersMatch_RankInfo()
	{
		s_pRankList_A = NULL;
		s_pRankList_B = NULL;
		for( int n =0; n < B_TEAM; ++n )
			s_nCnt[n] = 0;
	}
	//팀별 정보
	u4					s_nCnt[B_TEAM];		// 팀 기부 갯수 0 = A팀, 1 = B팀
	cStringList*		s_pRankList_A;						// 1팀 10위까지의 랭커 정보
	cStringList*		s_pRankList_B;						// 2팀 10위까지의 랭커 정보
};
