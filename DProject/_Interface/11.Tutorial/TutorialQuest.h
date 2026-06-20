#pragma once

#include "../00.Contents/TutorialContents.h"

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트의 완료 조건 별 클래스 모음.
//////////////////////////////////////////////////////////////////////////

// 튜토리얼 퀘스트 완료조건 인터페이스
class IF_TRequireCondition : public NiMemObject
{
public:
	IF_TRequireCondition();
	virtual ~IF_TRequireCondition();

	virtual	bool Initalize();
	virtual void ReloadMap() {};	// 맵 이동시 호출
	virtual void Update( float const& fElapsedTime ){};
	virtual void SetData( int const& nValue1, int const& nValue2 ){};

	void		SetCurrentNMaxCount( int const& nCurrentCount, int const& nMaxCount );
	bool		IsComplete() const;
	bool		IsSendComplete() const;
	void		SetSendComplete();
	int			GetCurrentCount() const;
	bool		IncreaseCurCount() const;

protected:
	bool			m_bInitalize;
	mutable bool	m_bComplete;

private:
	mutable int		m_nCurrCount;
	int				m_nMaxCount;
	bool			m_bSendSuccess;
};

// 퀘스트를 받으면 완료 되는 조건
class CTC_QuestAccpet : public IF_TRequireCondition
{
public:
	CTC_QuestAccpet();
	virtual ~CTC_QuestAccpet();
	virtual void Update( float const& fElapsedTime );
	virtual void SetData( int const& nValue1, int const& nValue2 );
};

// 특정 위치로 이동해야 완료 되는 조건
class CTC_MovePosition : public IF_TRequireCondition
{
public:
	CTC_MovePosition();
	virtual ~CTC_MovePosition();
	virtual	bool Initalize();
	virtual void ReloadMap() {m_bInitalize = false;}	// 맵 이동시 
	virtual void Update( float const& fElapsedTime );
	virtual void SetData( int const& nValue1, int const& nValue2 );
	
	bool	SetObjectVisible();
	void	SetObjectHide();

private:
	DWORD			m_dwObjectIdx;
	NiPoint2		m_mapPosition;
	float			m_fRadius;
};

// 카메라 좌우 조작
class CTC_CameraState : public IF_TRequireCondition
{
public:
	CTC_CameraState();
	virtual ~CTC_CameraState();
	virtual	bool Initalize();
	virtual void ReloadMap() {m_bInitalize = false;}	// 맵 이동시 
	virtual void Update( float const& fElapsedTime );
	virtual void SetData( int const& nValue1, int const& nValue2 );

private:
	int		m_nInputType;
	float	m_fCompleteValue;
	float	m_T1_m_fRoll;
	float	m_T1_m_fPitch;
	float	m_T1_m_fDist;
};

// Target 위치로 이동해야 완료 되는 조건
class CTC_TargetTrace : public IF_TRequireCondition
{
public:
	CTC_TargetTrace();
	virtual ~CTC_TargetTrace();
	virtual void Update( float const& fElapsedTime );
	virtual void SetData( int const& nValue1, int const& nValue2 );

private:
	int				m_nTraceObjectType; // 1 : NPC, 2 : Monster
	int				m_nObjectIDX;		// 추적할 오브젝트 인덱스 번호
};

// 전투 조건에 따른 완료 조건
class CTC_AttackState : public IF_TRequireCondition
{
public:
	CTC_AttackState();
	virtual ~CTC_AttackState();
	virtual void Update( float const& fElapsedTime );
	virtual void SetData( int const& nValue1, int const& nValue2 );

private:
	int			m_nAttackType;
	int			m_nObjectIDX;
};

// 디지몬 스캔/회수 완료 조건
class CTC_DigimonScanAndRetrieve : public IF_TRequireCondition
{
public:
	CTC_DigimonScanAndRetrieve();
	virtual ~CTC_DigimonScanAndRetrieve();
	virtual bool Initalize();
	virtual void Update( float const& fElapsedTime );
	virtual void SetData( int const& nValue1, int const& nValue2 );

private:
	void		_DigitamaScanResult(void* pData);
	void		_DigitamaSellResult(void* pData);

	int			m_nType;		// 1 : 스캔, 2 : 회수
	int			m_nItemCode;	// 스캔 및 회수에 사용될 아이템 인덱스 번호(값이 0인경우 아무 아이템이나 스캔/회수 했을 시 완료)
};

// 디지몬 부화 완료 조건
class CTC_DigimonHatch : public IF_TRequireCondition
{
public:
	CTC_DigimonHatch();
	virtual ~CTC_DigimonHatch();
	virtual	bool Initalize();
	virtual void Update( float const& fElapsedTime );
	virtual void SetData( int const& nValue1, int const& nValue2 );

private:
			void _DigitamaHatchResult(void* pData);

};

// 디지몬 강화 완료 조건
class CTC_DigimonEnchant : public IF_TRequireCondition
{
public:
	CTC_DigimonEnchant();
	virtual ~CTC_DigimonEnchant();
	virtual	bool Initalize();
	virtual void Update( float const& fElapsedTime );
	virtual void SetData( int const& nValue1, int const& nValue2 );

private:
			void _DigimonEnchantResult(void* pData);
};

// 디지몬 교체 완료 조건
class CTC_DigimonChange : public IF_TRequireCondition
{
public:
	CTC_DigimonChange();
	virtual ~CTC_DigimonChange();
	virtual	bool Initalize();
	virtual void Update( float const& fElapsedTime );
	virtual void SetData( int const& nValue1, int const& nValue2 );

private:
			void _DigimonEnchantResult(void* pData);
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------------------------------------------
struct sCompCondition
{
	sCompCondition();
	~sCompCondition();

	void	DeleteData();
	bool	AddQuestCompeteRequire(int const& nIdx, CsQuestCompRequire* pComp);
	size_t	HaveConditionCount() const;
	bool	IsAllComplete() const;
	void	ReLoadMap();

	DWORD								m_dwQuestIDX;			// 현재 진행 중인 퀘스트 번호
	std::map<int,IF_TRequireCondition*>	m_mapCondition;	
};

typedef std::list< sCompCondition* >						LIST_CONDITIONS;
typedef std::list< sCompCondition* >::iterator				LIST_CONDITIONS_IT;
typedef std::list< sCompCondition* >::const_iterator		LIST_CONDITIONS_CIT;


class cTutorialQuest : public CTutorialContents::ObserverType
{
public:
	cTutorialQuest();
	virtual ~cTutorialQuest();

	virtual	bool			Construct(void);
	void					Update(float const& fElapsedTime);

	void					LoadTutorialQuest();
	void					LoadTutorialQuest(std::list< DWORD >& lQuestList);
	void					RecheckTutorial();
	void					ReloadMap();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	bool					_AddProcessTutorialQuest( DWORD const& dwQuestID, cData_Quest::sPROCESS * sProcess );
	void					_AcceptTutorialQuest( DWORD const& dwQuestID );
	void					_CancelTutorialQuest( DWORD const& dwQuestID );
	void					_RemoveAcceptableList( DWORD const& dwQuestID );

	void					_CompleteTutorialQuest( DWORD const& dwQuestID );
	void					_AddAcceptableQuest(DWORD const& dwQuestID);
	bool					_DeleteAcceptQuest( DWORD const& dwQuestID );
	void					_DeleteAllAcceptQuest();
	void					_CheckAcceptableQuest(DWORD const& dwQuestID);

private:
	std::list<DWORD>		m_listAcceptableQuest;	// 받을 수 있는 튜토리얼 퀘스트 번호들
	LIST_CONDITIONS			m_listAcceptQuest;		// 진행 중인 튜토리얼 퀘스트
};