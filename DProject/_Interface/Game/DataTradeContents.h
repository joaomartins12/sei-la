#pragma once

typedef	CsModelData::sINFO		stModelInfo;	// 용병의 모델 정보
typedef	cData_PostLoad::sDATA	stLoadData;		// 불러온 용병 정보

class DataTradeContents
	: public IUIContentSubject< DataTradeContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eSTATE {//	DATA_TRADE_SYSTEM
		eTRADE_NONE=-2,			//	WT_DATA_TRADE_DIGIMON_SELECT
		eTRADE_SELECT,			//	WT_DATA_TRADE_DIGIMON_SELECT
		eTRADE_SCALE=0,			//	WT_DATA_TRADE_DIGIMON_SCALE	// 0번부터 사용. 바꾸면 안됨. 데이터 교환 클래스가 전부 물려 있음..
		eTRADE_UPGRADE,			//	WT_DATA_TRADE_DIGIMON_UPGRADE
		eTRADE_EVOLUTION,		//	WT_DATA_TRADE_DIGIMON_EVOLUTION
		MAX_TRADE,
		
	};

public:
	DataTradeContents(void);
	virtual ~DataTradeContents(void);

	enum eEventType		// DataTradeContents
	{
		// DataTradeCenter (데이터센터)
		eCenter_Reset,				// 리셋.
		eCenter_CreateEffect,		// 이펙트 효과 시작. 데이터 교환 시작 직전에 실행
		eCenter_EndEffect,			// 이펙트 효과 종료 되었는지 검사.
		eCenter_CreateDigimonLeft,	// 좌측 디지몬 생성
		eCenter_CreateDigimonRight,	// 우측 디지몬 생성
		eCenter_DeletDigimonLeft,	// 좌측 디지몬 삭제
		eCenter_DeletDigimonRight,	// 우측 디지몬 삭제


		// DataTradeSelect (교환 선택창)
		eSelect_Reset,		//

		// DataTradeDigimonScale( 크기 정보 )
		eScale_Reset,		// 재설정. 데이터 교환 성공 후
		eScale_Trade,		// 데이터 교환 실행

		// DataTradeDigimonUpgrade( 강화 정보 )
		eUpgrade_Reset,
		eUpgrade_Trade,

		// DataTradeDigimonEvolution( 진화 정보 )
		eEvolution_Reset,
		eEvolution_Trade,
	};

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);

private:
	eSTATE			m_eState;						// 현재 콘텐츠의 트레이드 상태.
	bool			m_bEffectOn;					// 현재 이펙트의 실행 유무. 데이터 교환 중에만 true
	bool			m_bPickLeft, m_bPickRight;		// 좌우 슬롯이 피킹 되었는지 알려준다.

private:
	void			OpenSelect(void*	pData);			// 교환 선택창 열기, 돌아가기	pData가 있으면 플로우도 열기.	
	void			StartTradeData(void*	pData);		// 데이터 교환 시작.

public:
	void			CloseSelect(bool	bFlow=false);	// 교환 선택창 닫기.			pData가 있으면 플로우도 닫기.
	void			OpenTrade(int state);				// 교환창 열기.
	void			CloseTrade();						// 교환창 닫기.
	void			EndTradeData();						// 데이터 교환 끝.
	void			SendTradeData(void*		pData);		// 데이터 교환 (크기, 강화, 진화) 정보 보내기

private:
	void			RecvTradeScale(void*	pData);		// 데이터 교환 크기 정보 받기
	void			RecvTradeUpgrade(void*	pData);		// 데이터 교환 강화 정보 받기
	void			RecvTradeEvolution(void*	pData);	// 데이터 교환 진화 정보 받기
	void			RecvTradeFailure(void* pData);		// 데이터 교환 실패

	void			OnTradeDataEffect();				// 데이터 교환 이펙트 켬.
	void			OffTradeDataEffect();				// 데이터 교환 이펙트 끔.

	// type : 0,1,2 : 크기,강화,진화 타입으로 구분한다.
	bool			IsRequireItem(eSTATE eType);			// 현재 타입의 필요아이템이 충분히 있는지 검사 한다.	
	int				GetUseRank(eSTATE eType, int code);	// 현재 아이템 타입 + 코드 값을 기준으로 소모 우선 순위를 얻는다.	
	void			DecreaseItemRequire(eSTATE eType);	// 현재 타입의 아이템의 갯수를 요구치 만큼 감소 시킨다.

public:
	void			CreateTradeCenterModelLeft(int ID);	// 좌측 디지몬 3D 모델 생성
	void			CreateTradeCenterModelRight(int ID);// 우측 디지몬 3D 모델 생성

	void			DeleteTradeCenterModelLeft();		// 좌측 디지몬 3D 모델 삭제
	void			DeleteTradeCenterModelRight();		// 우측 디지몬 3D 모델 삭제

	int				GetItemCodeCount(eSTATE	eType);					// 현재 타입의 아이템_코드가 몇개 있는지 얻는다.
	int				GetItemCode(eSTATE	eType, int typecount=0);		// 현재 타입의 아이템_코드를 얻는다.
	int				GetRequireItemCount(eSTATE	eType, int typecount=0);	// 현재 타입의 필요아이템 갯수가 몇개 있는 얻는다.	// 0이면 잘못된 경우.
	int				GetInvenItemCount(eSTATE	eType);					// 현재 인벤토리에 있는 필요 아이템 갯수를 알려준다.
	bool			IsTradeDataEffect();				// 데이터 교환 이펙트 실행 중인지 여부.
	void			MessageToNotify(int	Message, TCHAR*	text1=NULL);		// 알림창 메세지. 타입별 메시지 표시	MESSAGE_LACK..
	bool			DataTradeCheck(stLoadData*	pL_Data, stLoadData*	pR_Data, wstring	wStr);	// 데이터 교환 실행이 가능한지 판단

	//
	void			SetPickLeftSlot(bool	bPick);
	void			SetPickRightSlot(bool	bPick);

	bool			IsPickLeftSlot() const;
	bool			IsPickRightSlot() const;
	
	// 현재 교환 상태 체크
	eSTATE			GetIntToState(int state);
	eSTATE			GetState() const;
private:
	void			SetState(eSTATE	state);
};
typedef	DataTradeContents	DTCON;