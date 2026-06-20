#pragma once
#include "../00.Contents/TutorialContents.h"

//////////////////////////////////////////////////////////////////////////
class cTraceCondition : public cIndicatorBase
{

public:
	cTraceCondition();
	virtual ~cTraceCondition();

	virtual bool		LoadScript( TiXmlElement* pEl );

	void				SetTooltipImgFile(const char* pFIle);
	void				SetUITraceData( sTraceData const& pData );
	void				AddEffectData( sObjectEffectData const& sData );

	void				SetPortalData( DWORD dwPortalID, bool bPortalEnable );
	void				SetTalkData( int const& nTalkTarget, std::wstring const& wsTalkMsg );
	void				SetVideoData( std::string const& strFile );
	void				SetNextIndicatorData( DWORD dwNextQuestID, int const& nNextStep, int const& nNextTraceIdx );
	void				AddMonsterData( sMonsterData const& cData );

	cTraceBase*			m_pTraceKind;
	sTraceData			m_TraceData;
	cTraceCondition*	m_pChildTrace;
};

class cTrace : public cIndicatorBase
{
public:
	cTrace(void);
	virtual ~cTrace(void);

	virtual bool		LoadScript( TiXmlElement* pEl );

	void				SetTooltipImgFile(const char* pFIle);
	void				SetUITraceData( sTraceData const& pData );
	void				AddEffectData( sObjectEffectData const& sData );

	void				SetPortalData( DWORD dwPortalID, bool bPortalEnable );
	void				SetTalkData( int const& nTalkTarget, std::wstring const& wsTalkMsg );
	void				SetVideoData( std::string const& strFile );
	void				SetNextIndicatorData( DWORD dwNextQuestID, int const& nNextStep, int const& nNextTraceIdx );
	void				AddMonsterData( sMonsterData const& cData );

	int					m_nConditionIdx;
	bool				m_nComplete;// 현재 조건이 완료가 되었는지 체크
	sTraceData			m_TraceData;
	cTraceCondition*	m_pChildTrace;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef std::map<int/*ConditionIdx*/, cTrace*>					MAP_CONDITIONS;
typedef std::map<int/*ConditionIdx*/, cTrace*>::iterator		MAP_CONDITIONS_IT;
typedef std::map<int/*ConditionIdx*/, cTrace*>::const_iterator	MAP_CONDITIONS_CIT;

typedef std::map<int/*eStep*/, MAP_CONDITIONS>					MAP_QUEST_INDICATOR;
typedef std::map<int/*eStep*/, MAP_CONDITIONS>::iterator		MAP_QUEST_INDICATOR_IT;
typedef std::map<int/*eStep*/, MAP_CONDITIONS>::const_iterator	MAP_QUEST_INDICATOR_CIT;

typedef std::map<DWORD,	MAP_QUEST_INDICATOR>					MAP_INDICATOR_INFO;
typedef std::map<DWORD,	MAP_QUEST_INDICATOR>::iterator			MAP_INDICATOR_INFO_IT;
typedef std::map<DWORD,	MAP_QUEST_INDICATOR>::const_iterator	MAP_INDICATOR_INFO_CIT;

class cIndicatorInfos : public CTutorialContents::ObserverType
{
	enum eStep
	{
		eNone = -1,
		eBefor = 0,
		eAccpet,
		eComplete,
	};

public:
	cIndicatorInfos(void);
	~cIndicatorInfos(void);
	virtual	bool			Construct(void);

	void					Update( float const& fElapsedTime );
	void					LoadTutorialScript();

	void					GetTutorialQuestList( std::list< DWORD >& lQuestList );

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					_UpdateTraceUIData( sTraceData* pTraceData );
	bool					_LoadScript( TiXmlElement* pNode );
	sTraceData*				_CheckIndicator( DWORD const& dwQuestID, cTraceCondition* pTrace );
	sTraceData*				_UpdateIndicator( DWORD const& dwQuestID, cTrace* pTrace );

	void					_UpdateTutorialQuestCondition(DWORD const& dwQuestID, int const& nConditionIdx);
	void					_SetTutorialIndicator( DWORD const& dwQuestID, int const& nType );
	void					_RemoveIndicator( DWORD const& dwQuestID );
	void					_StopIndicator();
	void					_SetProcessTutorialIndicator( DWORD const& dwQuestID );
	void					_SetNextConditionIndicator( int const& nConditionIdx );

	void					_UpdateQuestIndicatorInfo( DWORD const& dwQuestID, int const& nStep, int const& nConditionIdx );

private:
	DWORD					m_dwQuestID;
	int						m_nCurrentStep;
	int						m_nCurrentConditionIdx;

	MAP_INDICATOR_INFO		m_Indicators;	
	sTraceData*				m_pCurrentTrace;
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////