
#pragma once

#define TIME_ALPHA			0.7f

namespace ICONMINIMAP
{
	enum	NAME
	{
		ITEM_PRODUCTION_NPC=7,
	};
}
namespace ICONITEM
{
	enum eTYPE
	{
		ARMOR_1					=	1,
		ARMOR_2					=	2,
		ELEMENT_NO_CHECK		=	3,
		ELEMENT_CHECK			=	4,		
		CHIPSET					=	5,
		CARD					=	6,
		ITEM_CARD				=	7,
		QUEST_ITEM				=	8,
		DT_MEMORY				=	9,
		ITEM_ICON				=	10,
// 		ITEM_ICON40				=	40,
// 		ITEM_ICON41				=	41,
// 		ITEM_ICON42				=	42,
// 		ITEM_ICON43				=	43,
// 		ITEM_ICON44				=	44,
// 		ITEM_ICON45				=	45,
// 		ITEM_ICON46				=	46,
 		ITEM_ICON47				=	47,
 		ITEM_ICON48				=	48,
		ITEM_ICON49				=	49,

		SKILL1					=	11,
		SKILL2					=	12,
		SKILL3					=	13,
		SKILL_MASK				=	14,
		SKILL4					=	15,
		SKILL5					=	16,
		SKILL6					=	17,

		DEvol					=	21,
		DAttribute				=	22,
		DGrowType				=	23,

		MiniMap					=	31,
		MonsterAI				=	32,

		Stat					=	33,
		Community				=	34,		
		CHAT_LV					=	35,
		Enchant_Lv				=	36,	

		New_Taget_DAttribute	=	37,	
		Talk					=	101,		

		NUMBER					=	400,

		ITEM_MASK				=	500,
	};
}

namespace ICON_Dynamic
{
	enum eTYPE
	{		
		Protect					=	200,
		Achieve					=	201,
		Achieve2				=	202,
#ifdef PC_BANG_SERVICE
		PcBangIcon				=	203,
#endif
		Achieve3,
		CashShop_Mask			=	1000,
		CashShop_02_101			=	2101,
		CashShop_02_102			=	2102,
		CashShop_02_103			=	2103,
		CashShop_02_104			=	2104,
		CashShop_02_105			=	2105,
		CashShop_02_106			=	2106,
		CashShop_02_107			=	2107,
		CashShop_02_108			=	2108,
		CashShop_02_109			=	2109,
		CashShop_02_201			=	2201,
		CashShop_02_301			=	2301,
		CashShop_03_101			=	3101,
		CashShop_03_102			=	3102,
		CashShop_03_103			=	3103,
		CashShop_03_104			=	3104,
		CashShop_03_105			=	3105,
		CashShop_03_201			=	3201,
		CashShop_03_301			=	3301,
		CashShop_03_401			=	3401,
		CashShop_03_501			=	3501,
		CashShop_04_101			=	4101,
		CashShop_04_102			=	4102,
		CashShop_04_103			=	4103,
		CashShop_04_201			=	4201,
		CashShop_04_301			=	4301,
		CashShop_04_401			=	4401,
		CashShop_04_501			=	4501,
		CashShop_04_601			=	4601,
		CashShop_05_101			=	5101,			
		CashShop_05_102			=	5102,		
		CashShop_05_103			=	5103,	
		CashShop_05_104			=	5104,	
	};
}

namespace ICONMASK
{
	enum eMASK_TYPE
	{
		IF_ICON_MOUSE_DRAG = 0,
		IF_ICON_MOUSE_ON,
		IF_ICON_MOUSE_CLICK,
		IF_ICON_SELECT,
		IF_ICON_RED,
		IF_CURSOR_ICON,
		IF_SOCKET,
		IF_ICON_REAL_RED,
		IF_ICON_NORMAL_RED,
		MAX_COUNT,
	};
}

namespace NUMBERSPRITE
{
	enum eCOLOR
	{
		RED		=	0,
		ORANGE,		
		BLUE,
		GREEN,
		WHITE,
		YELLOW,
		MAX_COLOR,
	};
}

struct sIFIcon
{
	enum eSTATE{ NONE, MOUSE_DRAG, MOUSE_ON, MOUSE_LCLICK, MOUSE_RCLICK };	

	sIFIcon(){ s_eState = NONE; }

	inline 	void		Init( CsRect rc );
	inline 	bool		MouseOn( CsPoint ptLocal );
	inline 	bool		MouseLBtnClick( CsPoint ptLocal );
	inline 	bool		MouseRBtnClick( CsPoint ptLocal );

	inline	void		RenderMask( CsPoint pos, CsPoint size );
	inline	CsPoint		GetPos();
	inline	CsPoint		GetSize();

	inline	void		ReleaseState( bool bIncluseDrag = false );
	void				SetPos( CsRect rc ) { s_Rect = rc; }
	CsRect		s_Rect;
	eSTATE		s_eState;
};

class cIconMng : public NiMemObject
{
public:
	static void			GlobalInit();
	static void			GlobalShotdown();
	static void			GlobalUpdate();			// 퀘스트 관련 아이콘 깜박임 효과

protected:
typedef	std::map< DWORD, cIcon* >::const_iterator	ICON_CITER;
	cSprite*							m_pCoolDown;
	cSprite*							m_pIconMask[ ICONMASK::MAX_COUNT ];
	cSprite*							m_pNumber[ NUMBERSPRITE::MAX_COLOR ];
	cSprite*							m_pIconSkillMask;	
	std::map< DWORD, cIcon* >			m_mapIconItem;	
	std::map< DWORD, cIcon* >			m_mapDynamicIcon;

public:
	void				Delete();
	void				Init();
	void				ResetDevice();

	void				InitDynamicIcon( ICON_Dynamic::eTYPE eType, CsPoint patch, CsPoint size , char* cPath );
	void				DeleteDynamicIcon( ICON_Dynamic::eTYPE eType );
	NiSourceTexturePtr	GetTexture( ICON_Dynamic::eTYPE eType );
	cSprite*			GetSprite(ICON_Dynamic::eTYPE eType);
	cIcon*				GetIcon(ICON_Dynamic::eTYPE eType);

	static void			_IconIDToTexIndex( DWORD& dwTex, int& nIconIdx, cItemInfo* pItemInfo, int nIconID );
	//===================================================================================================
	//
	//		아이템
	//
	//===================================================================================================
public:
	cIcon*				GetIcon(int nItemID);
	void				SetItemAlpha( int nItemID, float fAlpha );
	void				RenderItem( int nItemID, CsPoint pos, CsPoint size, float fAlpha );

	void				SetIconAlpha( ICONITEM::eTYPE type, float fAlpha ){ m_mapIconItem[ type ]->GetSprite()->SetAlpha( fAlpha ); }

	void				RenderItem( cItemInfo* pItem, CsPoint pos, bool bRenderCount );
	void				RenderItem( cItemInfo* pItem, CsPoint pos, CsPoint size, bool bRenderCount );
	void				RenderItem( int nItemID, CsPoint pos );
	void				RenderItem( int nItemID, CsPoint pos, CsPoint size );
	void				RenderItem( int nItemID, CsPoint pos, int nRenderCount );

	void				RenderSkill( cSkill::sINFO* pSkill, CsPoint pos );
	void				RenderSkill( unsigned short const& nIconIdx, CsPoint pos, int nLevel = 0 );
	void				RenderSkill( CsSkill* pSkill, CsPoint pos, int nLevel = 0 );
	void				RenderSkillMask( USHORT nSkill, CsPoint pos, int nTacticsIndex = -1 );
	void				RendeCursorSkillMask( USHORT nSkill, CsPoint pos, int nTacticsIndex = -1 );

	void				RenderDEvol( nsCsDigimonTable::eEVOLUTION_TYPE et, CsPoint pos );
	void				RenderDEvol( nsCsDigimonTable::eEVOLUTION_TYPE et, CsPoint pos, CsPoint size );

	enum eDAttribute{ DAttribute, Family, Nature };
	void				RenderDAttribute( eDAttribute da, int nIndex, CsPoint pos );
	void				RenderDAttribute( eDAttribute da, int nIndex, CsPoint pos, CsPoint size );

	void				RenderIcon( ICONITEM::eTYPE type, CsPoint pos, int nIndex );
	void				RenderIcon( ICONITEM::eTYPE type, CsPoint pos, CsPoint size, int nIndex, int nStackCnt = 0 );	//	아이콘 렌더 수정 (중첩 표현) chu8820

	void				RenderTalk( CsPoint pos, CsPoint size, int nIndex );

	void				RenderCashShopIcon( ICON_Dynamic::eTYPE type, CsPoint pos, int nIndex );				//2017-03-07-nova 캐쉬샵 아이콘 그리기
	void				RenderCashShopIcon( ICON_Dynamic::eTYPE type, CsPoint pos, CsPoint size, int nIndex );	//2017-03-07-nova 캐쉬샵 아이콘 그리기

	void				RenderDynamicIcon( ICON_Dynamic::eTYPE type, CsPoint pos, int nIndex );	
	void				RenderDynamicIcon( ICON_Dynamic::eTYPE type, CsPoint pos, CsPoint size, int nIndex );		

public:
	void				RenderIFIconMask( sIFIcon::eSTATE state, CsPoint pos, CsPoint size );
	void				RenderMask( ICONMASK::eMASK_TYPE mt, CsPoint pos, CsPoint size );

	void				RenderCursorIconItem( cItemInfo* pItem, CsPoint pos );
	void				RenderCursorIconItem( int nIconIndex, CsPoint pos );

	void				RenderCursorIconSkill( cSkill::sINFO* pSkill, CsPoint pos );	
	void				RenderCursorIconSkill( int nIconIndex, CsPoint pos, int nTacticsIndex );
	void				RenderCursorIconMaskSkill( int nIconIndex, CsPoint pos );

	void				RenderCursorIconCharImage( sCHAR_IMAGE* pCharImage, CsPoint pos );	

	void				RenderMaskGlitter( ICONMASK::eMASK_TYPE mt, CsPoint pos, CsPoint size, float fAlpha );
protected:
	//void				_IconIDToTexIndex( DWORD& dwTex, int& nIconIdx, int nIconID );

	//===================================================================================================
	//
	//		넘버
	//
	//===================================================================================================
public:
	void				RenderNumber( int eAllien, NUMBERSPRITE::eCOLOR color, int nNum, CsPoint pos, CsPoint size, bool bRenderPersent );
	void				RenderNumber( int eAllien, NUMBERSPRITE::eCOLOR color, int nNum, CsPoint pos, CsPoint size, float fRatio, bool bRenderPersent );


	//===================================================================================================
	//
	//		쿨 타임
	//
	//===================================================================================================
public:
	void				SetTimeAlpha( float fAlpha ){ m_pCoolDown->SetAlpha( fAlpha*TIME_ALPHA ); }
	void				DefaultTimeAlpha();

	void				RenderTime( CsPoint pos, float fHalfLength, double fRemainTime, double fEndTime );
	void				RenderTime( CsPoint pos, float fHalfLength, CsCoolTimeSeq* pSeq );
	void				RenderTime( CsPoint pos, float fHalfLength, CsItem::sINFO* pFTItemInfo );

	void				RenderTimeInverse( CsPoint pos, float fHalfLength, float fRemainTime, float fEndTime );
};

extern cIconMng*		g_pIconMng;


#include "IconMng.inl"