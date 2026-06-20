#pragma once

#define IF_INCUBATOR_MAX_DIGIMON_COUNT	3
#define IF_INCUBATOR_EXPLAIN_LEN					512
#define IF_INCUBATOR_EXPLAIN_DELTA_Y			5

class cDigimon_Incubator : public cBaseWindow
{
public:
	cDigimon_Incubator();
public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DIGIMON_INCUBATOR; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			OnMoveWindow();
// 	virtual void			OnMouseOutWindow();

	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey();
// 	virtual void			OnLButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			UserDie(){ Close( false ); }

	/************************************************************************/
	/*                    인큐베이터 내 디지몬 별 정보                        */
	/************************************************************************/
protected:
	sIFIcon				m_pDigimon_IFIcon[ IF_INCUBATOR_MAX_DIGIMON_COUNT ];	//디지몬 아이콘
	sIFIcon				m_pItem_IFIcon[ IF_INCUBATOR_MAX_DIGIMON_COUNT ];		//아이템 아이콘

	struct sDATA_IMG {
		bool					s_bData;		//데이터 있는지 없는지 체크
		sCHAR_IMAGE*	s_pDigimonImage;	// 인큐내에 표시되는 디지몬 
		sCHAR_IMAGE*	s_pImage_Serv;		// 실제 서버에 등록되어있는 디지몬
		void					Init(){ s_bData = false; s_pDigimonImage = NULL; s_pImage_Serv = NULL; } 
		};
public:
	sDATA_IMG		m_pDigimonImg[ IF_INCUBATOR_MAX_DIGIMON_COUNT ];	//디지몬 초상화 정보

	/************************************************************************/
	/*								버튼 관련                                         */
	/************************************************************************/
protected:
	cButton*			m_pBoostBtn[ IF_INCUBATOR_MAX_DIGIMON_COUNT ];	// 촉진 버튼
	cButton*			m_pCancelButton;	// 닫기버튼

public:
	cImage*			m_pLockImg[ IF_INCUBATOR_MAX_DIGIMON_COUNT ];		// 자물쇠 이미지

	void				_SetBtnText();	// 버튼 텍스트 셋팅

	/************************************************************************/
	/*								아이템 관련                                       */
	/************************************************************************/
protected:
public:
	struct sDATA_ITEM{
		int				s_nInvenSlot;	// 인벤번호	( -1 : 서버등록아이템, -2 : 빈아이템 )
		cItemInfo	s_pItemInfo;	// 등록된 아이템 정보
	};
	sDATA_ITEM		m_pItemData[ IF_INCUBATOR_MAX_DIGIMON_COUNT ];			// 등록된 아이템 정보

	void				_SetResist( int nInvenIndex );			// 인큐베이터에 아이템 등록

	/************************************************************************/
	/*							마우스 아이콘 관련                                 */
	/************************************************************************/
protected:
	int					m_nSelectIdx;		// 인큐베이터에서 선택한 인덱스
	int					m_nIconIdx;		// 아이콘 커서 클릭 인덱스(디지몬보관소 인덱스)

	bool				_Update_ForMouse_Icon();		//아이콘 업데이트
public:
	int					_GetSelectedIdx() { return m_nSelectIdx; }		// 인큐베이터에서 선택한 인덱스 Get
	int					_GetCursorIdx() { return m_nIconIdx; }		// 아이콘 커서 클릭 인덱스(디지몬보관소 인덱스)
	void				_SetSelectedIdx( int nIdx ) { m_nSelectIdx = nIdx; }	//인큐베이터 인덱스
	void				_SetIconIdx( int nIdx ) { m_nIconIdx = nIdx; }			//디보 인덱스

	int					_GetClickIdx();
	/************************************************************************/
	/*								기타 UI  관련                                     */
	/************************************************************************/
protected:
	cSprite*			m_pLine[ IF_INCUBATOR_MAX_DIGIMON_COUNT ];	// 디지몬, 아이템 가운데 연결선
	cStringList*		m_pExplain;		//	설명 스트링
	cScrollBar*		m_pScrollBar;		//	스크롤바

	/************************************************************************/
	/*							 	 데이터 셋팅                                      */
	/************************************************************************/
public:
	cItemInfo		m_pItem_Serv[ IF_INCUBATOR_MAX_DIGIMON_COUNT ];			// 등록된 아이템 정보

	void				_RecvFacilitation( u1 nSlot, u4 nRemainTime );					//서버데이터 수신
	void				_RecvExtraction( u4 nSlot );					// 촉진 중지
	void				_RecvDigimonChange( int nIncuSlot, int nHouseSlot );		//	촉진 중인 디지몬 교체 - 인큐베이터 슬롯(0~4), 디보 슬롯(1000~)
};