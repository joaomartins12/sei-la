#pragma once

class cCardReSeal : public cBaseWindow
{

public:
	cCardReSeal(void);
	virtual ~cCardReSeal(void);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CARDRESEAL; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();


	virtual void			Render();
	virtual void			OnMoveWindow();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			UserDie(){ Close( false ); }

	virtual bool			Close( bool bSound = true );


	
	// Refactoring
protected:
	enum{SLOT_SEAL, SLOT_RESEAL, SLOT_MAX = 2,};

	struct sSLOT
	{
		cWindow*			s_pWindowRoot;		// Card_ReSeal의 포인터
		sIFIcon				s_IFIcon;			// 아이템 슬롯의 정보
		cItemInfo			s_ItemInfo;			// 씰과 언씰 아이템
		cText*				s_pRenderText;		// 아이템 바로 뼁?출력할 텍스트
		cText::sTEXTINFO	s_Font;				// 폰트 정보
		CsPoint				s_TextPos;			// UI 상에서 위치하는 문자열의 좌표
		CsPoint				s_IconPos;			// 아이콘 좌표
		int					s_InvenIndex;		// 인벤토리 상의 인덱스

		cCardReSeal::sSLOT();
		void DeleteResource();
		void InitUI(IN CsPoint RegistPos, IN CsPoint RegistItemPos, IN cWindow* pRoot);
		void InitFont(IN CFont::eFACE_SIZE eFontSize, IN cText::sTEXTINFO::eBOLD_LEVEL Level);
		void SetText(TCHAR const* pText);
		void Render();
	};

protected:
	sSLOT				m_arrSLOT[SLOT_MAX];	// 좌슬롯, 우슬롯에 대한 구조체
	cButton*			m_pCancelBtn;			// 취소 버튼
	cButton*			m_pEnchantBtn;			// 확인 버튼
	

protected:
	void				_SetTitle(int nSealItemID);					// 제목 설정
	void				_SetSealText(int nAmount);					// 씰 보유량 "%d"
	void				_SetSealIcon(int nSealItemID, int nAmount);	// 씰 아이콘 등록
	void				_SetReSealItem();							// 밀봉 아이템 자동 등록
	bool				_UpdateIcon_ForMouse();						// 툴팁 생성
	void				_ClearItemLock();					// 씰 봉인 해제 UI를 오픈하면서 걸었던 씰의 락을 해제합니다.

protected:
	void			GetINFO( IN int nInvenIndex, OUT CsItem& FTItem, OUT CsItem::sINFO& FTInfo, OUT cItemInfo& ItemINFO );// 인벤토리에 있는 아이템 정보를 얻어옵니다.
	
protected:
	int					m_nNeedReSeal;		// 밀봉 필요 개수
	int					m_nHaveReSeal;		// 보유한 밀봉 아이템 개수
	bool				m_bIsRegistReSeal;		// 밀봉 아이템이 등록되었는지 여부, 무조건 수량 부족 처리

public:
	void				SetReSealText(int nSealItemID, int nAmount);	// 밀봉할 씰의 이름을 등록, Seperate.cpp에서 호출
	void				SendReSeal();		// CallBy MessageBox
	void				RecvReSealProcess(int nSealID, int nSealAmount, int nReSealUse);
};
