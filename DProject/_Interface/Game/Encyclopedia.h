#pragma once
#include "EncyclopediaContents.h"

#define ENCY_DISABLE_ALPHA			0.65f
#define ENCY_ENABLE_ALPHA			1.0f
#define ENCY_DISABLE_COLOR			NiColor( 40.0f/255.0f, 40.0f/255.0f, 40.0f/255.0f )
#define ENCY_ENABLE_COLOR			NiColor( 255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f )

#define IF_ENCY_MAX_ENCHANT_COUNT	5
#define IF_ENCY_VIEW_ENCHANT_COUNT	2

class cEncyclopedia : public cBaseWindow, public EncyclopediaContents::ObserverType
{
public:
	struct sDIGIMON_ID : public CUSTOMDATA
	{
		sDIGIMON_ID(int digimonId, int count = 0)
		{
			iDigimonId = digimonId;
			iCount = count;
		}
		~sDIGIMON_ID() {};

		int iDigimonId;
		int	iCount;
	};

	struct sICON_INFO
	{
		sICON_INFO()
		{
			s_count				= 0;
			s_nGroupIdx			= 0;
			s_dwBaseDigimonID	= 0;
			s_dwDestDigimonID	= 0;
		}

		cString::sSPRITE*	s_Sprite;
		int					s_count;		
		USHORT				s_nGroupIdx;
		DWORD				s_dwBaseDigimonID;
		DWORD				s_dwDestDigimonID;
	};

public:
	cEncyclopedia();
	virtual ~cEncyclopedia();

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);	

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_ENCYCLOPEDIA; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			OnMoveWindow();

	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey();

	virtual void			UserDie(){ Close( false ); }

	/************************************************************************/
	/*						도감 UI - 디지몬 목록							*/
	/************************************************************************/
protected:
	cButton*			m_pCancelButton;	// 닫기 버튼
	cButton*			m_pStatisticsBtn;	// 통계창 오픈 버튼
	cRadioButton*		m_pRadioBtn;		// 탭( 가~나, 다~라, ...)
	cScrollBar*			m_pScrollBar;
	cListBox*			m_pEncyListBox;
	
	int					m_nCurTab;			// 현재 탭, 탭 변경이 아닌 Refresh일 경우 확인용	
	int					m_nGroupCount;
	int					iScrollPos;
	bool				bIsTooltip;

	std::list<sICON_INFO*>								m_IconList;
	std::map< int, EncyclopediaContents::sEVOL_INFO* > m_pCurTabList_map;				// 현재 탭에 속한 디지몬 계열체 맵

	void				_SetTabList( int nCurRadioIdx );								// 현재 탭 디지몬 리스트 셋
	void				_RefreshList();
	
	void				SetTabListData();
	//void				SetTabListDataRenderCount();									//2017-05-24-nova Scroll 마지막페이지 리스트 크기에 따른 렌더카운트 설정
	void				SetTabGroupListData();
	void				SetTabGroupListDataRenderCount();								//2017-05-24-nova Scroll 마지막페이지 리스트 크기에 따른 렌더카운트 설정

	void				PressCancelButton( void* pSender, void* pData );				//2017-05-25-nova
	void				PressStatisticsButton( void* pSender, void* pData );			//2017-05-25-nova
	void				PressGroupListBtn( void* pSender, void* pData );				//2017-05-25-nova

	/************************************************************************/
	/*							도감 UI - 툴팁								*/
	/************************************************************************/
protected:
	cSprite*			m_pTooltipBG;								// 계열체 툴팁 배경
	cText*				m_szName;									// 툴팁 :: 디지몬 이름
	cText*				m_szLevel;									// 툴팁 :: 레벨
	cText*				m_szScale;									// 툴팁 :: 스케일
	cSprite*			m_pEnchant[IF_ENCY_MAX_ENCHANT_COUNT] ;		// 툴팁 강화 수치 종류
	cText*				m_szEnchantVal[IF_ENCY_VIEW_ENCHANT_COUNT];	// 툴팁 :: 공격력
	cStringList*		m_szExplain;								// 툴팁 :: 계열체 설명
	CsCoolTimeSeq		m_DelayTime;								// 툴팁 :: 툴팁 표시 대기 시간( 마우스 오버 후 툴팁 띄우기까지의 시간)

	int					m_nTooltipDigimonID;						// 툴팁 중인 디지몬, Set 한번하도록 이걸로 체크
	bool				m_bIsMouseOn;								// 마우스 오버 중인지 체크, true일때만 랜더
	std::map <int, int> m_pEnchantVal;								// 강화수치

	void				_SetTooltip( int nDigimonID );				// 도감 툴팁
	void				_RenderTooltip( CsPoint ptMousePos );
	void				_Updata_ForMouse_Group( int count );		// 덱 마우스 업데이트
	void				_SetGroupTooltip( int nGroupIdx );			// 덱 툴팁	

	/************************************************************************/
	/*						도감 UI - 보상아이템 획득						*/
	/************************************************************************/
protected:
	int					m_nSelectDigimonID;			// 보상아이템 받을 계열체 디지몬 번호, 버튼 누를때 저장
	int					m_nSelectDeckIdx;			// 효과 받을 덱 인덱스, 버튼 누를때 저장

public:
	int					_GetSelectDigimonID(){ return m_nSelectDigimonID; }	// 보상아이템 받을 계열체 디지몬 번호
	int					_GetSelectDeckIdx(){ return m_nSelectDeckIdx; }		// 효과 받을 덱 인덱스	
};