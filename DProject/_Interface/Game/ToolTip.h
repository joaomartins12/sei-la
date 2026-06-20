

#pragma once

#include "../Adapt/AdaptPartySystem.h"

#define TOOLTIP_FONT_1_SIZE				CFont::FS_10
#define TOOLTIP_MAX_TEXT_LEN			256
#define TOOLTIP_COLOR1					NiColor( 0.8f, 0.8f, 0.8f )
#define TOOLTIP_MAX_SIZE				310
#define TOOLTIP_CUT_SIZE				280

#ifdef VERSION_USA
#ifdef ITEM_LIMIT_STRING_BUG
	#define	TOOLTIP_TAB_SIZE				38 //제한레벨 텍스트에 레벨 값 가려지는 현상 수정 (36 -> 38) (chu8820/12.07.27)
#endif
#else
	#define	TOOLTIP_TAB_SIZE				30
#endif


#define TOOLTIP_CANNOT_COLOR			NiColor( 240/255.0f, 0/255.0f, 51/255.0f )

class cTooltip : public NiMemObject
{
public:
	enum eTYPE{ NONE, ITEM, ITEMCOMPARE, SKILL, QUICK_EVOL, STAT, STAT_EXPLAIN, D_ATTRIBUTE, D_GROW_TYPE, EXP, TEXT, ELEMENT, WORLDMAP, TACTICS, PARTY, 
		BUFFINFO, GUILDUP, BATTLE_TAG, TARGET_ATTACK, TARGET_ELEMENT, TARGET_NATURAL, MAIL, PLAYTIME, 

#ifdef SIMPLE_TOOLTIP
		UIHELP,
#endif
		SEALMASTER,
		DECKLIST,
		DECKBUFF,
#ifdef GUILD_RENEWAL
		GUILD_LEVEL,	// 커뮤니티UI - 길드 레벨
		GUILD_CONTRIBUTION,		// 커뮤니티UI - 길드 공헌도(개인)
		GUILD_SKILL,	// 커뮤니티UI - 길드 스킬
		GUILD_LOTATE,	// 커뮤니티UI - 길드원 위치
		GUILD_BUFF,		// 시스템버프UI - 적용 중인 길드 스킬 
#endif
		SPIRIT_DIGIMON,		// 스피릿 디지몬 정보
		FRIENDSHIP_POINT,	// 친화력
		DATA_TRADE,			// 데이터 교환

		SKILL_SIMPLE,
		QUICK_EVOL_SIMPLE,
		ENCHANT_EXPLAIN,
	};
	enum eSLOT{ OPEN_SLOT, CLOSE_SLOT, };
	enum eWORLDMAP{ WORLDMAP_AREA, WORLDMAP_WORLD, WORLDMAP_MAIN, };
public:
	cTooltip();
	virtual ~cTooltip();

protected:
	cPopUp*			m_pPopupWindow;
	cSprite*		m_pLine;
	cStringList		m_StringList;
	cStringList		m_StringList_Name;
	cSprite*		m_pStatUp;
	cSprite*		m_pStatDown;
	cSprite*		m_pEmptySlot;
	cSprite*		m_pPost;

	bool			m_bRenderTooltip;	

	eTYPE			m_eType;
	__int64				m_nMainID;
	__int64				m_nSubID_1;
	__int64				m_nSubID_2;
	__int64				m_nSubID_3;
	void*			m_pData;

	int				m_nMaxSizeX;
	int				m_nCount;			// 현재 소지 개수
	CFont::eFACE_SIZE	m_eFontSize;

	CsPoint			m_ptRenderPos;
	CsPoint			m_ptRenderPoint;
	CsPoint			m_ptPatchSize;
	CsPoint			m_ptMaxSize;

	TCHAR			m_szText[ MAX_PATH ];

	CsCoolTimeSeq	m_DelayTime;

public:
	struct sUPDATE_TIME
	{
		DWORD				s_nTimeTS;
		cString::sTEXT*		s_pText;
	};

protected:
	CsVectorPB< sUPDATE_TIME >	m_vpEditElement;

public:
	virtual void	Delete();
	virtual void	Init();	
	virtual void	Render();
	virtual void	Render( CsPoint ptPos, bool right = false );	//2017-01-19-nova 오른쪽 아래 고정툴팁 예외처리
	virtual void	ResetDevice();

	void			ReleaseRenderToolTip( eTYPE type ){ if( m_eType == type ){ m_bRenderTooltip = false; } }
	void			SetTooltip( CsPoint pos, CsPoint patch, int nMaxSizeX, eTYPE type, __int64 nMainID, __int64 nSubID_1 = 0, __int64 nSubID_2 = 0, __int64 nSubID_3 = 0, void* pData = NULL );	
	//void			SetTooltip_Text( CsPoint pos, CsPoint patch, TCHAR const* szText ); //글자 가로와 상관없이 고정넓이로 적용되는 함수
	//글자에 가로넓이에 맞춰서 툴팁 만드는함수			
	void			SetTooltip_Text( CsPoint pos, CsPoint patch, TCHAR const* szText, CFont::eFACE_SIZE nFontSize = CFont::FS_10 );  
	void			SetCompareTooltip( CsPoint pos, CsPoint patch, int nMaxSizeX, eTYPE type, __int64 nMainID, __int64 nSubID_1 = 0, __int64 nSubID_2 = 0, __int64 nSubID_3 = 0, void* pData = NULL );
	CsPoint			GetRenderPoint() { return m_ptRenderPoint; }
	int				GetMaxSizeX() { return m_ptMaxSize.x; }
	int				GetMaxSizeY() { return m_ptMaxSize.y; }	//2017-01-19-nova 

	virtual void	Update( float const& fDeltaTime );

protected:
	//virtual	void	_Update();
	void			_Update_TimeInfo();
	bool			_IsEnableOverToolTip();

public:
	enum eTIMETYPE{ TT_FULL, TT_SIMPLE, TT_SYMBOL,
		TT_LASTTIME,	// 최근 접속 시간
	};
	static TCHAR*	Time2Str( u4 nDestTS, u4 nSrcTS, eTIMETYPE type );

	static void		GetNatureString( TCHAR* sz, int nLen, int nNature );	

protected:
	cString*		_AddItemIcon( int nIconID );
	//cString*		_AddSkillIcon( int nIconID );
	cString*		_AddIcon( ICONITEM::eTYPE type, int nIconID, CsPoint size = CsPoint( 32, 32 ), int nListSizeX = TOOLTIP_TAB_SIZE * 2, CsPoint delta = CsPoint( 12, -7 ), cString* pString = NULL );

	void			_AddLine( bool bIncludeEmptyString );
	void			_AddStr( cText::sTEXTINFO* ti, TCHAR szStr, int nFontSize, NiColor niColor );	

protected:
	void			_MakeTooltip_Item(bool compare = false);	//2017-01-19-nova 툴팁이 비교창인지 아닌지 여부 추가
	//void			_MakeTooltip_ItemCompare();

	void			_MakeTooltip_Skill();
	void			_MakeTooltip_SimpleSKill();
	void			_AddSkillState( CsSkill::sINFO* pFTInfo );
	void			_SimpleAddSkillState( CsSkill::sINFO* pFTInfo, int const& nSkillLv );


	void			_MakeTooltip_QuickEvol();
	void			_Simple_MakeTooltip_QuickEvol();
	void			_MakeTooltip_Stat();
	void			_MakeTooltip_Stat_Explain();
	void			_MakeTooltip_Enchant_Explain();
	void			_MakeTooltip_GrowType();
	void			_MakeTooltip_WorldMap();
	void			_MakeTooltip_GuildUpGrade();
	void			_MakeTooltip_BattleTag();

	void			_MakeTooltip_Target_Attack();
	void			_MakeTooltip_Target_Element();
	void			_MakeTooltip_Target_Natural();
	void			_MakeTooltip_Mail();

	void			_MakeTooltip_SealMaster();
	void			SetSealMasterTootipTile( CsItem::sINFO const* pFTInfo, CsMaster_Card::sINFO const& sInfo, cStringList& stringList, int iCardGrade );
					// 실마스터 효과 문자열 추가
	void			SetSealMasterEffectString( cStringList& stringList, CsMaster_Card::sINFO const& sInfo, USHORT nEffType, int nEffValue );
	void			SetSealMasterNoneLevel( cStringList& stringList, CsMaster_Card::sINFO const& sInfo );
	void			SetSealMasterNextCountString( cStringList& stringList, CsMaster_Card::sINFO const& sInfo, int nNextGrade, int nSealCount );
	void			SetSealMasterNextEffectString(cStringList& stringList, CsMaster_Card::sINFO const& sInfo, USHORT nEffType, int nEffValue);

private:
	void			_MakeTooltip_Stat_Tamer();
	void			_MakeTooltip_Stat_Digimon();
	
	cString*		_MakeTooltip_Stat_Title( int const& nStatType, bool& bFloat, bool& bFloat2 );
	cString*		_MakeAddStat(bool bNew, ICONITEM::eTYPE const& type, int nIconIdx, CFont::eFACE_SIZE eFontSize, NiColor s_Color, int const& nVale, bool const& bFloat, bool const& bFloat2 );
	void			_MakeAddValueText( cString* pStr,bool bNew,CFont::eFACE_SIZE eFontSize, NiColor s_Color, __int64 const& nVale, bool const& bFloat, bool const& bFloat2 );

protected:	
	void			_MakeTooltip_DAttribute();
	void			__DAttribute_Family( TCHAR const* szTitle, TCHAR const* szBody );
	void			__DAttribute_Nature( TCHAR const* szTitle, int nUpIndex, int nDownIndex );

	void			__Element_ID_to_String( cText::sTEXTINFO* ti, int nElementID );

protected:
	float			GetExpAccessoryAttributeDamage(int type);	//악세서리( 반지, 목걸이, 귀걸이)들의 속성피해값 반환.
	void			_MakeTooltip_Exp();
	void			_MakeTooltip_Element();
	void			_MakeTooltip_Tactics();
	void			_MakeTooltip_Party();
	void			_MakeTooltip_PlayTime();
#ifdef SIMPLE_TOOLTIP
	void			_MakeTooltip_UIHelp();
#endif
protected:
	void			_MakeTooltip_BuffInfo();	
	void			__BuffExecpt( DWORD dwBuffID );

	void			_MakeTooltip_DeckList();	// 덱 UI 툴팁
	void			_MakeTooltip_DeckBuff();	// 적용 중인 덱 버프아이콘 툴팁
#ifdef GUILD_RENEWAL
	void			_MakeTooltip_GuildLevel();			// 길드 레벨 툴팁
	void			_MakeTooltip_GuildContribution();	// 길드 공헌도(개인) 툴팁
	void			_MakeTooltip_GuildSkill();			// 길드 스킬 툴팁
	void			_MakeTooltip_GuildLotate();			// 길드원 위치 툴팁
	void			_MakeTooltip_GuildSkillBuff();		// 적용 중인 길드 스킬 버프형태로 보여주는 것 툴팁
	void			_AddGuildSkillState( CsSkill::sINFO* pFTInfo, cString* pString = NULL, NiColor pColor = FONT_GREEN, CsPoint ptDeltaPos = CsPoint::ZERO );	// 길드 스킬 적용 수치
	cImage*			m_pGuildBuffImg;					// 길드 버프 아이콘
#endif

	void			_MakeTooltip_SpiritDigimon();		// 스피릿 디지몬 정보

	void			_MakeTooltip_FriendShipPoint();		// 친화력 툴팁

	void			_MakeTooltip_DataTrade();			// 데이터 교환 툴팁

protected:
	TOwnershipGuard<AdaptPartySystem>	m_pAdaptParty;
};

