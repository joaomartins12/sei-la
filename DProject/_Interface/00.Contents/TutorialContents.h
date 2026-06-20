#pragma once

#include "../../ContentsSystem/ContentsSystem.h"
#include "TutorialUITraceData.h"

class cTutorialQuest;
class cIndicatorInfos;

//-------------------------------------------------------------------------------------------------------------------------------------
class CTutorialContents	: public IUIContentSubject< CTutorialContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eEventType
	{
		eChangeShowImage,
		eUpdateUITrace,
		eSetProcessTutorialQuest,
		eTutorialQuestAccpetAble,
		eTutorialQuestAccpet,
		eTutorialQuestCancel,
		eProccesQuestConditionUpdate,
		eProccesQuestAllComplete,
		eTutorialQuestCompleted,
		eTutorialQuestDigiviceCheck,

		eUpdateQuestIndicatorInfo,
	};
public:
	CTutorialContents(void);
	virtual ~CTutorialContents(void);

	virtual int const		GetContentsIdentity(void) const;
	virtual bool 			Initialize(void);
	virtual void 			UnInitialize(void);
	virtual bool 			IntraConnection(ContentsSystem& System);
	virtual void 			Update(float const& fElapsedTime);
	virtual void 			NotifyEvent(int const& iNotifiedEvt);
	virtual void 			NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void 			MakeMainActorData(void);
	virtual void 			ClearMainActorData(void);
	virtual void 			MakeWorldData(void);
	virtual void 			ClearWorldData(void);
	virtual void			OnOverride(int const& nNextFlowID);
	//virtual	void			OnResume(int const& nBeforFlowID);

	bool					IsTutorialPlaying() const;

	void					ShowTooltipImage( std::string const& fileName );
	
	void					SetUITraceData(sTraceData const* pData);
	void					CloseTooltipImage();
	void					CloseUITrace();
	void					SetObjectHighlight( int const& nTargetType, DWORD const& dwTargetIDX, bool bShow );

	void					AddProcessTutorialQuest( DWORD const& dwQuestID );
	void					HaveAcceptableQuest(DWORD const& dwQuestID);
	void					ProcessConditionAllComplete( DWORD const& dwQuestID );
	bool					TutorialQuestConditionUpdate( DWORD const& dwQuestID, int const& nConditionIdx, int const& nValue );

	void					TutorialQuest_Skip_Ask();

private:
	void					_AddTutorialQuestEvent();
	void					_RemoveTutorialQuestEvet();

	bool					_CheckTutorialQuestAllComplete() const;//	튜토리얼 퀘스트를 모두 완료 했는지 체크

	void					_TutorialQuest_Accept( void* data );
	void					_TutorialQuest_Cancel(void* pData);
	void					_TutorialQuest_Complete( void* data );
	void					_TutorialQuest_ProcessUpdate(void* pData);
	void					_TutorialQuest_AllComplete(void* pData);
	void					_TutorialQuest_CheckNewDigivice(void* pData);
	void					_TutorialQuest_AskCancel(void* pData);
	void					_TutorialQuest_Skip_Accept(void* pData);
	void					_IsTutorialPlaying(void* pData);

	void					_ClearTutorialQuestData();

private: 
	bool									m_bTutorialQuestCompleted;	// 튜토리얼 퀘스트를 모두 완료 했는지 여부

	//////////////////////////////////////////////////////////////////////////
	TOwnershipGuard< cIndicatorInfos >		m_pQuestIndicatorInfo;
	TOwnershipGuard< cTutorialQuest >		m_pTutorialQuest;
	bool									m_nUpdateLoadingStop;	// 로딩중에 업데이트 중지 하도록 함

	//////////////////////////////////////////////////////////////////////////

private:
	// 튜토리얼용 플레이어 데이터
	struct sTutorialPlayerData
	{
		sTutorialPlayerData() : s_nDigimonFTID(0), s_nBaseDigimonID(0), s_nTamerCostumeID(0) {}
		~sTutorialPlayerData() {}

		bool			LoadPlayerData();
		void			Reset();

		std::wstring	s_wsDigimonName;
		uint			s_nDigimonFTID;
		uint			s_nBaseDigimonID;
		uint			s_nTamerCostumeID;
	};

	sTutorialPlayerData		m_sPlayerData;

private:
	void					_TutorialEvent_Initialize( void* pData );		// 튜토리얼용 데이터 초기화
	void					_TutorialEvent_ClearData();
	void					_TutorialEvent_PlayVideo( std::wstring const& wsPath );
	void					_TutorialEvent_Skip( void* pData );

	void					_CheckAndOpenDoor(void* pData);

	void					_LoadTutorialQuest();

	bool					_CheckCurTutorialQuestComplete();
	bool					_CheckPreTutorialQuestComplete();

public:
	void					SetTutorialMonster( DWORD const& dwID, CsPoint const& ptPos );

	void					SkipTutorialEvent();

	void					TutorialQuestCompleteAll();

	bool					IsTutorialPlaying_ClientMode() const;
};
