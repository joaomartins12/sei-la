
#pragma once 


class cEvent_Pinokimon : public cBaseWindow
{
#define MAX_CARD_COUNT		5

public:
	cEvent_Pinokimon();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_EVENT_PINOKIMON; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnMoveWindow();

	virtual void			UserDie(){ Close( false ); }
	/************************************************************************/
	/*                              이미지                                  */
	/************************************************************************/
private:
	cSprite*			m_pBG;							// 배경

	/************************************************************************/
	/*                              버튼                                    */
	/************************************************************************/
private:
	cButton*			m_pCancelBtn;					// 닫기버튼
	cButton*			m_pInsertBtn[5];				// 선택버튼

	/************************************************************************/
	/*                              텍스트                                  */
	/************************************************************************/
private:
	cText*			m_szMyCnt[ MAX_CARD_COUNT ];	// 카드별 투표 갯수( 자신 )
	cText*			m_szAllCnt[ MAX_CARD_COUNT ];	// 카드별 투표 갯수( 전체 )
	cText*			m_szRemainTime;					// 다음 갱신 시간 알림

public:
	TCHAR			m_szResetTime[ 32 ];		//다음 리셋 시간 (캐릭터형으로 받아서 바로 뿌림)
	//정보 갱신
	void			RefreshList();

	/************************************************************************/
	/*						  			  ETC			                    */
	/************************************************************************/
public:
	u4				m_nCheckTime;				// 시간 갱신 확인용
	//타겟 NPC
	CsC_AvObject*	m_pTarget;

	void			SetTarget( CsC_AvObject* pTarget );
};

struct sEvent_PinokimonInfo		//투표 정보
{
	sEvent_PinokimonInfo(){ Init(); }
	void Init(){ s_nNextResetTime = 0; for( int i = 0 ; i < MAX_CARD_COUNT ; i++ ) { s_nMyCnt[ i ] = 0; s_nAllCnt[ i ] = 0; } }

	//리셋 타임
	u4		s_nNextResetTime;				// 서버에게 받는 다음 리셋 시간 - 초
	//각 카드에 대한( 카드 5장 고정 )
	u4		s_nMyCnt[ MAX_CARD_COUNT ];		//본인 투표수
	u4		s_nAllCnt[ MAX_CARD_COUNT ];	//전체 투표수
};