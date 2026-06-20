#pragma once


#define MAX_QUESTION_IMG_COUNT			64	// 이지미 최대 갯수
#define MAX_QUESTION_IMG_ONELINE_COUNT	8	// 이지미 최대 갯수
#define MAX_TRY_COUNT					2	// 최대 도전 횟수

class CMacroProtectContents : public IUIContentSubject< CMacroProtectContents >
{
public:
	enum eEvent
	{
		eResetAnswer,
	};

	CMacroProtectContents(void);
	virtual ~CMacroProtectContents(void);

	static int const IsContentsIdentity(void);
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

public:

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	void			Send_MacroAnswer( std::wstring const& wsAnswer );
	char const*		GetQuestionImgData() const;
#else
	void			Send_MacroAnswer( int const& nAnswerValue );
	// TotalSize 범위에서 범위 안에 있는 nIncludeNum 값을 포함한 nGetCount개 만큼의 랜덤 값을 얻어 오는 함수
	void			RandomShuffle( int TotalSize, int nIncludeNum, int nGetCount, std::vector<int>& getVector );
#endif

	// 매크로 프로텍터로 질문은 받은 상태인지 체크 
	bool			IsMacroProtectUsing() const;
	std::list<int> const& GetQuestionNumber() const;

private:
	bool		_IsUsingCurrentMap();							// 매크로 프로텍트를 사용 할 수 없는 지역 검사
	void		_UsingMacroProtect(void* pData);				// 아이템을 사용했을 때
	void		_UsingMacroProtect_UIDX(void* pData);			// 타겟 창에서 사용했을 때	
	bool		_IsUsingMacroProtect( CsC_AvObject* pTarget );	// 매크로 프로텍트를 사용할 수 있는지 체크함
	void		_Show_AskWindow( CsC_AvObject* pTarget );		// 매크로 프로텍트 아이템을 사용시 한번 더 물어보는 창 띄움.


	void		_Send_MacroProtect_Using(void* pData);			// 서버에 매크로 아이템을 사용하겠다고 보냄
	void		_Recv_MacroProtect_Question(void* pData);
	void		_NTF_MacroProtect_Question_Request(void* pData);	// 다른 유저가 내게 매크로 의심 했을 경우.
	void		_Recv_MacroProtect_Question_Result(void* pData);	// 내가 매크로 의심 유저의 요청에 대한 결과 값
	void		_Recv_MacroPlayer_Result(void* pData);

	void		_ShowMacroProtectWindow();

	void		_ResetData();
	void		_CloseWindow();

private:
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	int			m_nMaxTimeSec;
	int			m_nRemainTimeSec;
	int			m_nRemainChanceCount;
	char		m_pQuestionData[CAPTCHA_BMP_SIZE];
	int			m_nQuestionType;
	std::list<int>	m_nQuestionNumbers;
#else
	int			m_nMacroAnswer;
	int			m_nRandomAnswer;	
	int			m_nTryCount;	
#endif

};