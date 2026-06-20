
#pragma once

#include "CardEventContents.h"

#define EVENT_MAX_CARD		5
#define CARD_WIDTH			150
#define CARD_HEIGHT			210
#define BUTTON_WIDTH		80
#define BUTTON_HEIGHT		30

class cCardEvent : public cBaseWindow, public CardEventContents::ObserverType
{
public:
	cCardEvent();
	~cCardEvent();

	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CARD_EVENT ; }
	virtual void			Destroy();

	virtual bool			Close( bool bSound = true );
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual eMU_TYPE		Update_ForMouse_Disable();
	virtual void			Render();

	virtual void			OnMouseOutWindow();
	virtual void			OnLButtonDown( CsPoint pos );
	virtual void			OnLButtonUp( CsPoint pos );

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	int						Card_Init();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					getResultItemIven(void* pSender, void* pData);

protected:
	CsC_AvObject*		m_pTarget;

	cSprite*			m_pCardBack;					// 카드 뒷면
	
	cButton*			m_pCompButton;					// 완료 확인 버튼
	cButton*			m_pTakeButton;					// 아이템 받기 버튼
	bool				m_bTakeStatus;					// 아이템 받았는지 여부 상태 확인

	int					m_nTiming;						// 처음 피에몬의 손동작에 맞춰 카드가 그려질 타이밍
	int					m_nSelectCount;					// Open된 카드가 몇장있는지 카운트
	int					m_nResultIndex;					// 마지막으로 선택된 카드 인덱스 값
	int					m_nResultID;					// 마지막으로 그려질 카드 인덱스 값
	float				m_fCurTime;
	float				m_fOldTime;
	bool				m_bFirstRender;					// 손동작이 완료되기 전까지 그려질 카드에 대한 bool 변수

	u4					m_nItemID;						// 유저에게 줄 아이템 번호
	int					m_nItemCount;					// 유저에게 줄 아이템 개수

	struct sEventCard
	{
		sEventCard():s_rcCard(CsRect::ZERO), s_bSelect(false),s_nCardIndex(0)
		{};

		CsRect			s_rcCard;		// 카드의 rect 범위
		NiPoint2		s_ptPos;		// 카드 위치
		bool			s_bSelect;		// 선택된 카드
		int				s_nCardIndex;	// 카드 앞면에 대한 인덱스
	};

	struct sCardFront
	{
		sCardFront():s_pCardFront(NULL), s_Path(NULL), s_nItemID(0)
		{};

		cSprite*		s_pCardFront;
		u4				s_nItemID;
		char*			s_Path;
	};

	sEventCard			m_EventCard[ EVENT_MAX_CARD ];	// 각 카드의 rect값 및 postion을 저장할 배열
	sCardFront			m_CardFront[ EVENT_MAX_CARD ];	// 카드 앞면을 저장할 배열

	short				m_Switch[ EVENT_MAX_CARD ];		// 랜덤으로 카드를 그려주기 때문에 중복되지 않도록 하는 배열
	int					m_SwitchCount;					// 랜덤 카드 카운트

	bool				m_bPause;						// 테이머 일시 정지를 위한 bool 변수

	cSpriteAni*			m_pAniCardSelect;

public:

	bool				m_bSelectAni;	// 마지막 카드 선택 시 애니매이션 1회용
};