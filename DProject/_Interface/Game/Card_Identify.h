#pragma once

class cCardIdentify : public cBaseWindow
{

	// 확인 버튼에 대한 메시지
	enum
	{
		MSG_NOTREADY,			// 윈도우가 닫혀있을 때,
		MSG_SHORTAGEUNSEALITEM,	// 보유한 밀봉 해제 아이템이 부족합니다.(캐쉬+지급의 총합이 부족)
		MSG_HAVEOVERSEAL,		// 등록하려는 씰이 3000장을 초과했습니다.
		MSG_ENABLERESISTSEAL,	// 씰을 정상적으로 등록 가능할 때
		MSG_MAX,
	};

public:
	cCardIdentify();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CARDIDENTIFY; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	// 호출 시 nValue는 인벤토리의 인덱스입니다.

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();


	virtual void			Render();
	virtual void			OnMoveWindow();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			UserDie(){ Close( false ); }

	virtual bool			Close( bool bSound = true );

public:
	int				IsEnableUnseal();
	void			SetResist( int nInvenIndex );
	
protected:
	bool			_UpdateIcon_ForMouse();				// 등록된 아이템의 툴팁 생성
	void			SetTitleText( int nInvenIndex );	// 사용하려는 씰 아이템의 인덱스를 넣어줍니다.
	void			SetUnSealText( int nInvenIndex );	// 현재 보유 중인 봉인 해제 아이템 개수의 문자열을 입력합니다.
	void			SetUnSealItem();					// 보유한 봉인 해제 아이템을 찾아서 등록합니다.
	void			_ClearItemLock();					// 씰 봉인 해제 UI를 오픈하면서 걸었던 씰의 락을 해제합니다.
	// 함수 Getter
protected:
	void			GetINFO( IN int nInvenIndex, OUT CsItem& FTItem, OUT CsItem::sINFO& FTInfo, OUT cItemInfo& ItemINFO );// 인벤토리에 있는 아이템 정보를 얻어옵니다.

protected:

	enum{SLOT_SEAL, SLOT_UNSEAL, SLOT_MAX = 2,};
	
	struct sSLOT
	{
		cWindow*			s_pWindowRoot;		// Card_Identify
		sIFIcon				s_IFIcon;			// 아이템 슬롯의 정보
		cItemInfo			s_ItemInfo;			// 씰과 언씰 아이템
		cText*				s_pRenderText;		// 아이템 바로 뼁?출력할 텍스트
		cText::sTEXTINFO	s_Font;				// 폰트 정보
		CsPoint				s_TextPos;			// UI 상에서 위치하는 문자열의 좌표
		CsPoint				s_IconPos;			// 아이콘 좌표
		int					s_InvenIndex;		// 인벤토리 상의 인덱스

		cCardIdentify::sSLOT();
		void DeleteResource();
		void InitUI(IN CsPoint RegistPos, IN CsPoint RegistItemPos, IN cWindow* pRoot);
		void InitFont(IN CFont::eFACE_SIZE eFontSize, IN cText::sTEXTINFO::eBOLD_LEVEL Level);
		void SetText(IN TCHAR const* pText);
		void Render();
	};

	sSLOT				m_arrSLOT[SLOT_MAX];

	cButton*			m_pCancelBtn;
	cButton*			m_pEnchantBtn;	

	int					m_nCardIndex;			// 인벤토리에서 카드를 찾을 때

	int					m_nNeedUnSealAmount;	// 필요한 봉인 해제 아이템 수량
	int					m_nHaveUnSealAmount;	// 보유한 봉인 해제 아이템 총 수량


	bool				m_bIsRegistUnSeal;		// 봉인 해제 아이템이 등록되었는지 여부, 무조건 수량 부족 처리
public:
	void				SendUnSeal();
	void				RecvUnSealProcess(int nSealInvenIdx, int nSealUse, int nUnSealUse);
};