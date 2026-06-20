
#pragma once 
#include "../Game/SealMasterContents.h"

#define GROUPBUTTONGAP 32
//
class cSealMaster : public cBaseWindow, public cSealMasterContents::ObserverType
{
	struct sSealCardControl
	{
		sSealCardControl():sBack(NULL),sDigimonImg(NULL),sFavoriteOn(NULL),sFavoriteOff(NULL),sCount(NULL),spItem(NULL)
		{}

		void		SetSealDigimon( std::string const& imgFile );
		void		SetFavoriteOnOff( bool bValue );
		void		SetSealCount( int const& nCount );
		void		SetSealGrade( int const& nGrade, std::string const& imgFile );

		cSprite*	sBack;
		cSprite*	sDigimonImg;
		cButton*	sFavoriteOn;
		cButton*	sFavoriteOff;
		cString::sTEXT* sCount;
		cGridListBoxItem* spItem;
	};

public:
	struct sMapGroupData : public CUSTOMDATA
	{
		sMapGroupData(const int buttonindex, const int groupType, const int mapID ):sButtonIndex(buttonindex), sGroupType(groupType), sMapID(mapID)
		{};
		virtual ~sMapGroupData() {};
		const int GetItemType() const;

		int			sButtonIndex;
		int			sGroupType;
		int			sMapID;
	};

	struct sSealGridData : public CUSTOMDATA
	{
		sSealGridData ( const DWORD sealID, const DWORD sealModelID, const int sealCount, const int grade, int tableCod, int effectType, bool favorite )
		:sSealID(sealID), sSealModelID(sealModelID), sSealCount(sealCount), sGrade(grade), sTableCode(tableCod), sEffectType(effectType), sFavorite(favorite)
		{};
		virtual ~sSealGridData () {};

		DWORD			sSealID;
		DWORD			sSealModelID;
		int				sSealCount;
		int				sGrade;
		int				sTableCode;
		int				sEffectType;
		bool			sFavorite;
	};
public:
	enum RESOLUTIONKEY
	{
		KEY_SEARCH_MAX = 14,
		KEY_DEAFULT = 13,
		KEY_MAX = 14,
	};
	enum LEADERSTATE
	{
		LEADER_NOTSELECT	= 0,	// 선택된 리더가 없습니다.			팝업 시 리더 설정
		LEADER_SELECT = 1,			// 같은 카드를 선택하여 리더 해제,	팝업 시 리더 해제
		LEADER_CHANGE	= 2,		// 리더가 선택된 상태에서 마스터 카드를 선택, 팝업 시 리더 변경
	};

	struct sResolutionKey
	{
		int		nWidth;		// 가로 해상도
		int		nHeight;	// 세로 해상도
		sResolutionKey(){}
		sResolutionKey(int w, int h) : nWidth(w) , nHeight(h){}
	};
	struct sTextPoint
	{
		CFont::eFACE_SIZE e1;
		CFont::eFACE_SIZE e2;
		int			w2;
	};

public:
	cSealMaster();
	~cSealMaster();

	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual	bool			Construct(void);
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetMap();
	virtual void			PlayOpenSound();

	virtual bool			OnEscapeKey();
	virtual void			OnMoveWindow();

private:
	void					MakeMapTree();
	void					MakeSubMapTree( cTreeBoxItem& Root, DWORD nGroupType );
	void					MapTreeClick(void* pSender, void* pData);
	void					MapTreeButtonClick(void* pSender, void* pData);

	void					MakeAllWatch();
	void					MakeFavorit();
	void					MakeActive();

	void					MakePlusState();
	void					MakeStateFilterComboBox();

	void					MakeSealGrid();

	void					ViewCardEffect(void* pSender, void* pData);
	// 그리드 버튼 클릭 처리
	void					ViewButton(void* pSender, void* pData);
	// 콤보박스 아이템 선택(능력치로 필터링)
	void					SelectStateFilter(void* pSender, void* pData);
	// 그리드 우클릭 처리(리더선택, 재밀봉resealing)
	void					RButton(void* pSender, void* pData);
	// 닫기 버튼
	void					CloseButtonEvent(void* pSender, void* pData);

	void					Update_ToolTip();

	void					SetFilter();		//필터 조건에 따라 grid의 visible 세팅
	int						EffectTypeTransToComboIndex(int effectType);		//추가 능력치 타입과 콤보 인덱스를 매칭
	void					SetPlusValueText();	//능력치 추가치 세팅
	
	void					SetActiveSealCountText(int nActiveSealCount, int nViewCount);
	void					SortforSealMaster();
	void					SortforSealMasterCount();

	void					_CreateSealCard(DWORD const& dwModelID);
	void					_CardRender();
	void					_ChangeTextureNiObject( NiAVObject* pObj, std::string const & sFileName );
	//////////////////////////////////////////////////////////////////////////
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	
private:
	cTreeBox*				m_pMapgroupTreeBox;		//맵 표시
	cGridListBox*			m_pGridListBox;
	cComboBox*				m_pCombo;				//추가 능력치 필터 콤보 박스
	cButton*				m_pButtonClose;
	cText*					m_pPlusValueText[STATEKINDS];		//추가 능력치 추가량 TEXT
	cStringList*			m_pActiveSealCountText;				//보여지고 있는 씰중 활성화된 씰의 갯수

	cButton*				m_pSelectedTree;

	bool					m_bViewCard;

	// 필터링 옵션
	int						m_nCurStateFilter;		//능력치에 따른 필터
	std::pair<int, int>		m_MapFilterPair;	//맵그룹과 번호에 따른 필터
	bool					m_bFavoriteFilter;		//즐겨찾기 필터 On
	bool					m_bMapAllFilter;		//모든 맵 보기 필터
	bool					m_bCountFilter;			//활성화 카드만 보기 필터(상시 작동 필터가 아님)

	int						m_nViewGridCount;			//필터에 걸려져 보여지고 있는 그리드의 숫자
	int						m_nViewnActiveGridCount;	//보여지고 있는 그리드중 1개 이상 개봉된 seal 숫자

	bool					m_bisTextCreated;

	cRenderTex*				m_pRenderTex;		// 이펙트 렌더텍스
	CsNodeObj				m_cardEffect;
	float					m_fAniTime;

	std::map<DWORD, sSealCardControl>		m_mapSealCardCon;

};

