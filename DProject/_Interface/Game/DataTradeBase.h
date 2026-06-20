#pragma once

class DataTradeBase : public cBaseWindow, public DataTradeContents::ObserverType
{
public:
	DataTradeBase(void);
	virtual ~DataTradeBase(void);

public:
	struct stDataTradeInfo
	{
	private:
		int				s_nModelID;		// 디지몬 데이터의 ID 번호
		int				s_SlotIndex;	// 참조할 용병의 슬롯 번호( 0 ~ 2 )

		sIFIcon			s_IFIcon;		// 마우스 클릭, 드래그, 오버 등의 상태 보관.
		string			s_ImgPath;		// 이미지 위치
		bool			s_bUseWorkPath;	// 데이터 폴더 위치 지정 여부
		stLoadData*		s_LoadData;		// 참조할 용병의 정보.

		cSprite*		s_pSprite;		// 디지몬 이미지
		cText*			s_pNickName;	// 디지몬 닉네임
		cText*			s_pTypeName;	// 디지몬 계열체명
		cText*			s_pVariableName;// 디지몬별 유동적 바뀔 정보명

	public:
		stDataTradeInfo();
		virtual ~stDataTradeInfo();

	public:
		void			ReleaseIconState( bool bIncluseDrag = false );
		void			ReferenceLoadData(stLoadData* pData);	// 로드정보 참조
		void			Delete();

	public:
		void			SetModelID(int ID);
		void			SetSlotIndex(int idx);
		void			SetIconState(sIFIcon::eSTATE state);
		void			SetIconInit( CsRect rc );
		void			SetImagePath(const char* path);
		void			SetUseWorkPath(bool bUse);

		void			SetSprite(cSprite* pSpr);		// 이미지 할당.참조
		void			SetNickName(TCHAR*	pName);		// 닉네임 설정
		void			SetTypeName(TCHAR*	pName);		// 계열체명 설정
		void			SetVariableName(TCHAR*	pName);	// 정보명 설정

		void			SetSpritePos(CsPoint pos);		// 이미지 위치 설정
		void			SetNickNamePos(CsPoint pos);		// 닉네임 위치 설정
		void			SetTypeNamePos(CsPoint pos);		// 계열체명 위치 설정
		void			SetVariableNamePos(CsPoint pos);	// 정보명 위치 설정

		void			CreateNickName(cText*	pName);		// 닉네임 생성 및 참조
		void			CreateTypeName(cText*	pName);		// 계열체명 생성 및 참조
		void			CreateVariableName(cText*	pName);	// 정보명 생성 및 참조

	public:
		stLoadData*		GetLoadData()	const;
		int				GetSlotIndex()	const;
		const char*		GetSprPath()	const;
		cSprite*		GetSprite()		const;
		sIFIcon*		GetIcon();
		CsPoint			GetIconPos();
		CsPoint			GetVariableNamePos();
		CsSIZE			GetIconSize();

		cText*			GetNickName()		const;
		cText*			GetTypeName()		const;
		cText*			GetVariableName()	const;

		TCHAR*			GetNickNameText()		const;
		TCHAR*			GetTypeNameText()		const;
		TCHAR*			GetVariableNameText()	const;

		int				GetModelID()		const;
		bool			GetUseWokrPath()	const;
		stModelInfo*	GetModelInfo()		const;
	};	
	typedef					stDataTradeInfo				_DATA_INFO;	// 데이터 교환 정보

	////////////							COMMON				//////////////////
protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

protected:
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();	
	virtual bool			Close( bool bSound /* = true  */ );
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	

public:
	virtual eWINDOW_TYPE	GetWindowType() = 0;

	////////////						UNCOMMON				//////////////////
protected:
	enum eDIGIMONSLOT	// 보유 디지몬 목록
	{
		PARTNER_MON= -1,		// 파트너몬 사용 안함.
		SLOT_TOP_MON1,			// 위측 슬롯1 용병
		SLOT_TOP_MON2,			// 위측 슬롯2 용병
		SLOT_TOP_MON3,			// 위측 슬롯3 용병
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
		SLOT_TOP_MON4,			// 위측 슬롯4 용병
#endif
		SLOT_LEFT,				// 좌측 교환 슬롯
		SLOT_RIGHT,				// 우측 교환 슬롯
		SLOT_MOVE,				// 이동 중 슬롯 정보
		MAX_MON,
	};
private:
	void					_UpdateIcon_ForMouse();
	eDIGIMONSLOT			GetIntToEnum(int	i);
	void					DragToImageStart(eDIGIMONSLOT	eSlot, _DATA_INFO*	pInfo);	// 이미지 드래그 시작
	void					DragToImageEnd(eDIGIMONSLOT	eSlot, _DATA_INFO*	pInfo);		// 이미지 드래그 종료
	void					RbuttonClick(eDIGIMONSLOT	eSlot, _DATA_INFO*	pInfo);		// TOP_UPDATE에서만 사용 CURSOR::RBUTTON_DOWN이 눌리면 데이터를 생성한다.
	
	int					TOP_UPDATE(CURSOR::eTYPE eCursor);
	int					LEFT_RIGHT_UPDATE(CURSOR::eTYPE eCursor);

protected:
	CsPoint					m_TopPos;			// 위측에 위치한 보유한 교체 디지몬 UI의 좌표
	CsPoint					m_LeftPos;			// 좌측에 위치한 교환 디지몬 UI의 좌표
	CsPoint					m_RightPos;			// 우측에 위치한 교환 디지몬 UI의 좌표
	CsPoint					m_NickNamePos, m_TypeNamePos, m_VarNamePos, m_VarPos;		// 좌, 우 텍스트 좌표	// 현재 팝업창에서의 위치.
	cText::sTEXTINFO		m_tempText;			// 텍스트 추가를 텍스트 정보
	bool					m_bIconClick;		// 아이콘 클릭 중인지.
	CsSIZE					m_BlockSize;		// 아이콘 블록의 사이즈.
	CsSIZE					m_IconSize;			// 아이콘의 사이즈.

private:
	_DATA_INFO				m_DigimonInfo[ MAX_MON ];	// 디지몬 정보창. 파트너 몬은 제외
	cPopUp*					m_pPopup[3];				// 팝업창
	cSprite*				m_pPopupBG[3];				// 팝업창 배경.
	cButton*				m_pDataTradeButton;			// 데이터 교환 버튼
	cButton*				m_pRetuonToSelectButton;	// 이전 데이터 교환 선택창으로 돌아가기.
	wstring					m_ValiableName;				// 가변 정보명
	bool					m_OutWindow;				// 윈도우창에서 클릭이 될지를 결정
private:
	CsPoint					m_TradeItemImagePos;		// 데이터 교환 아이템 이미지의 위치
	cText*					m_TradeItemNumbering;		// 데이터 교환 아이템의 수량 표시
	void					GetNumberingText();			// 수량 표시 정보 만들기.
	int						m_TradeItemCount;			// 데이터 교환 아이템의 현재 갯수.

private:
	void		CreateData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);			// 공통적인 정보 생성
	void		SetTradeSlotUpdate(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);	// 좌우 데이터 교환 슬롯 갱신
	void		ClearPrevTopSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);	// 이전 위측 참조 슬롯 해제

protected:
	
	void		CreateDataTradeEffect(void* pSender, void* pData);							// 교환 실행전 이펙트 실행 이후에	SendToTrade 실행한다.
virtual	void	CreateTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);	// 좌우 데이터 교환 슬롯 정보 생성.
virtual void	DeleteTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);
virtual void	ResetPosition();											// 해상도 재설정 시에 위치 재설정
virtual	void	ResetData();												// 데이터 교환 후 교환 된 데이터로 리셋해준다.
	
	void		SendToTrade();			// 교환 실행. 서버로 정보 전송	
	int			GetImagePath(int slotNum, string	&RefStr);
	void		AddSpriteImage(int slotNum, char* ImgPath=NULL, bool UseWokrPath=false);

	void		SetVariableName(TCHAR* VarName);	// 하위 클래스에서 가변적으로 바뀔 정보명
	TCHAR* 		GetVariableName();					// 하위 클래스에서 가변적으로 바뀔 정보명. 갱신

	void		CreateNickName(_DATA_INFO &info, TCHAR* pName, CsPoint pos);		// 닉네임 생성 및 참조
	void		CreateTypeName(_DATA_INFO &info, TCHAR* pName, CsPoint pos);		// 계열체명 생성 및 참조
	void		CreateVariableName(_DATA_INFO &info, TCHAR* pName, CsPoint pos);	// 정보명 생성 및 참조
	
	_DATA_INFO*	GetDataInfo(eDIGIMONSLOT etype);

	void		SetSpriteInfo(int type, cSprite* pSpr);
	void		SetOutWindow(bool w);
	CsPoint		GetCenterPos() const;

	void		InitSize();
	cPopUp*		GetPopup(int idx);	// 팝업창 가져오기.

	void		ReturnToSelect(void* pSender, void* pData);		// 이전 화면으로 돌아가기. 교환 선택창.

protected:
	bool		m_bFlicker;			// 교환 성공 후 점멸 기능
	int			m_FlickerCnt;		// 점멸 횟수
	float		m_FlickerTime;		// 점멸 시간
	float		m_FlickerChangeTime;// 점멸 온오프 되는 시간

	bool		InFlickerOn() const;
	bool		IsFlicker() const;
virtual void	OnFlicker();
	void		StartFlicker(int Cnt=10, float Ctime=0.2f);
	void		EndFlicker();

};