#pragma once
 

struct sObjectEffectData
{
	enum eTargetType
	{
		eNpc,
		eMonster,
	};

	sObjectEffectData():cTargetType(eNpc),dwTargetIDX(0),bShow(false)
	{}
	~sObjectEffectData()
	{}

	char	cTargetType;
	DWORD	dwTargetIDX;
	bool	bShow;
};

typedef std::list<sObjectEffectData>					LIST_EFFECT_DATA;
typedef std::list<sObjectEffectData>::iterator			LIST_EFFECT_DATA_IT;
typedef std::list<sObjectEffectData>::const_iterator	LIST_EFFECT_DATA_CIT;

struct sMonsterData
{
	sMonsterData() : dwMonsterID(0) {}
	~sMonsterData() {}

	DWORD		dwMonsterID;
	CsPoint		ptMonsterPos;
};
typedef std::list<sMonsterData>				LIST_MONSTER_DATA;
typedef	LIST_MONSTER_DATA::iterator			LIST_MONSTER_DATA_IT;
typedef LIST_MONSTER_DATA::const_iterator	LIST_MONSTER_DATA_CIT;

//////////////////////////////////////////////////////////////////////////
class cIndicatorBase : public NiMemObject
{
public:
	cIndicatorBase(void) {};
	virtual ~cIndicatorBase(void) {};

	virtual bool LoadScript( TiXmlElement* pEl ) = 0;
};

struct sTraceData
{
public:
	enum eType
	{
		eNone = 0,
		eControl,
		eItem,
		eQuest,
		eWindow,
		eQuickSlot,
	};

	enum eTalkType
	{
		eDigimon,
		eTamer,
	};

	sTraceData():m_dwTargetValue(0),m_nType(0),m_dwTargetValueSub(0)
				,m_dwNextQuestID(0),m_nNextStep(-1),m_nNextTraceIdx(-1)
				,m_nTalkTarget(0),m_dwPortalID(0),m_bPortalEnable(false)
	{
		m_wsTargetWindowName="empty";
		m_wsTargetName="empty";
	};

	virtual ~sTraceData() {};

	std::string			m_TooltipImgFile;
	std::string			m_wsTargetWindowName;

	int					m_nType;
	std::string			m_wsTargetName;
	DWORD				m_dwTargetValue;	// 아이템 코드, 퀘스트 번호, 윈도우 인덱스 번호
	DWORD				m_dwTargetValueSub;	// 아이템 코드, 퀘스트 번호, 윈도우 인덱스 번호

	LIST_EFFECT_DATA	m_EffectData;

	// Indicator next info
	DWORD				m_dwNextQuestID;
	int					m_nNextStep;
	int					m_nNextTraceIdx;

	// Talk Balloon : Target - 0 : Digimon / 1 : Tamer
	int					m_nTalkTarget;
	std::wstring		m_wsTalkMsg;

	// Portal Enable
	DWORD				m_dwPortalID;
	bool				m_bPortalEnable;

	std::string			m_VideoFile;
	LIST_MONSTER_DATA	m_MonsterData;
};

//////////////////////////////////////////////////////////////////////////
class cTraceBase : public NiMemObject
{
public:
	enum eTargetType
	{
		eNone = 0,
		eNpc,
		eMonster,
		eDigivice,
		eItem,
		eWindow,
		eVideo,
	};

	cTraceBase(void):m_bStop(false), m_bCompleteStop(false) {};
	virtual ~cTraceBase(void) {};

	virtual bool LoadScript( TiXmlElement* pEl ) = 0;
	virtual bool CheckIndicator(DWORD const& dwQuestID) = 0;

	bool		IsStop();
	void		SetStop();

	bool		m_bCompleteStop;
	bool		m_bStop;	// 현재 상태에 도달하면 UI가 뒤로 가지 않고 항상 UI만 보이게 함.
};

class cWindowStateTrace : public cTraceBase
{
public:
	cWindowStateTrace() : m_nState(eWS_None), m_nCmpState(eWS_None), m_nWindowType(-1) {}
	~cWindowStateTrace() {}

	virtual bool LoadScript( TiXmlElement* pEl );
	virtual bool CheckIndicator(DWORD const& dwQuestID);

	enum eWindowState
	{
		eWS_None,
		eWS_On,
		eWS_Off,
	};

	int			m_nState;
	int			m_nCmpState;
	int			m_nWindowType;
};

class cVideoStateTrace : public cTraceBase
{
public:
	cVideoStateTrace() : m_eState(eVS_None) {}
	~cVideoStateTrace() {}

	virtual bool LoadScript( TiXmlElement* pEl );
	virtual bool CheckIndicator(DWORD const& dwQuestID);

	enum eVideoState
	{
		eVS_None,
		eVS_Off,
	};

	eVideoState			m_eState;
	std::string			m_strFilePath;
};

class cUITrace : public cTraceBase
{
public:
	cUITrace(void):m_dwTargetIDX(0),m_dwTargetWindowIDX(0),m_TargetType(eNone) {};
	virtual ~cUITrace(void) {};

	virtual bool LoadScript( TiXmlElement* pEl );
	virtual bool CheckIndicator(DWORD const& dwQuestID);

	std::string			m_sWindowName;
	std::string			m_sControlName;

	DWORD				m_dwTargetIDX;
	DWORD				m_dwTargetWindowIDX;
	eTargetType			m_TargetType;
};

class cObjectTrace : public cTraceBase
{
public:
	cObjectTrace(void):m_dwTargetIDX(0),m_TargetType(eNone){};
	virtual ~cObjectTrace(void) {};

	virtual bool LoadScript( TiXmlElement* pEl );
	virtual bool CheckIndicator(DWORD const& dwQuestID);

	DWORD				m_dwTargetIDX;
	eTargetType			m_TargetType;
};

class cGamePageTrace : public cTraceBase
{
public:
	cGamePageTrace(void) : m_nPageIdx(-1) {}
	virtual ~cGamePageTrace(void) {};

	virtual bool LoadScript( TiXmlElement* pEl );
	virtual bool CheckIndicator(DWORD const& dwQuestID);

	int					m_nPageIdx;
};

class cMapTrace : public cTraceBase
{
public:
	cMapTrace(void):m_dwTargetMapIDX(0){};
	virtual ~cMapTrace(void) {};

	virtual bool LoadScript( TiXmlElement* pEl );
	virtual bool CheckIndicator(DWORD const& dwQuestID);

	DWORD				m_dwTargetMapIDX;
};

class cObjectNeerCheck : public cTraceBase
{
public:
	cObjectNeerCheck(void) :m_dwTargetIDX(0), m_TargetType(eNone), m_BaseType(eTamer) {};
	virtual ~cObjectNeerCheck(void) {};

	virtual bool LoadScript( TiXmlElement* pEl );
	virtual bool CheckIndicator(DWORD const& dwQuestID);

	enum eBaseType
	{
		eTamer,
		eDigimon,
	};

	DWORD				m_dwTargetIDX;
	eTargetType			m_TargetType;
	eBaseType			m_BaseType;
};

class cMonsterTrace : public cTraceBase
{
public:
	cMonsterTrace() : m_dwMonsterID(0), m_nMonsterHp(0) {}
	virtual ~cMonsterTrace() {}

	virtual bool LoadScript( TiXmlElement* pEl );
	virtual bool CheckIndicator(DWORD const& dwQuestID);

	DWORD				m_dwMonsterID;
	int					m_nMonsterHp;
};
