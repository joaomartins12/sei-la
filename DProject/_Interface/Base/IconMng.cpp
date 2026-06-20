

#include "stdafx.h"
#include "IconMng.h"
//#include <atlconv.h>
#define ICON_IMAGE_SIZE				CsPoint( 512, 512 )
#define ICON_IMAGE_SIZE_ICON8		CsPoint( 1024, 1024 )
#define ICON_IMAGE_SIZE_MASK		CsPoint( 128, 128 )
#define ICON_IMAGE_SIZE_STAT		CsPoint( 160, 128 )
#define IF_ICONSIZE CsPoint(32, 32)

cIconMng*		g_pIconMng = NULL;

void cIconMng::GlobalShotdown()
{
	cIcon::GlobalShotdown();

	if( g_pIconMng )
		g_pIconMng->Delete();
	SAFE_NIDELETE( g_pIconMng );
}

void cIconMng::GlobalInit()
{
	assert_cs( g_pIconMng == NULL );
	g_pIconMng = NiNew cIconMng;
	g_pIconMng->Init();

	cIcon::GlobalInit();
}

void cIconMng::GlobalUpdate()
{
	SAFE_POINTER_RET(g_pIconMng);

	cIcon::GlobalUpdate();
}
void cIconMng::Delete()
{
	// 쿨다운 제거
	SAFE_NIDELETE( m_pCoolDown );

	// 마스크 제거
	for(int i=0; i<ICONMASK::MAX_COUNT; ++i )
		SAFE_NIDELETE( m_pIconMask[ i ] );

	// 아이콘들 제거
	std::map< DWORD, cIcon* >::iterator it = m_mapIconItem.begin();
	std::map< DWORD, cIcon* >::iterator itEnd = m_mapIconItem.end();
	for( ; it!=itEnd; ++it )
	{
		SAFE_NIDELETE( it->second );
	}
	m_mapIconItem.clear();

	it = m_mapDynamicIcon.begin();
	itEnd = m_mapDynamicIcon.end();
	for( ; it!=itEnd; ++it )
	{
		SAFE_NIDELETE( it->second );				
	}
	m_mapDynamicIcon.clear();

	// 숫자 제거
	for(int i=0; i<NUMBERSPRITE::MAX_COLOR; ++i )
		SAFE_NIDELETE( m_pNumber[ i ] );

	SAFE_NIDELETE( m_pIconSkillMask );		
}


void cIconMng::Init()
{
	//===================================================================================================
	//
	//		기본 셋팅
	//
	//===================================================================================================
	m_pCoolDown = NiNew cSprite;
	m_pCoolDown->CreateTriangle( NiColorA( 0.1f, 0.1f, 0.1f, TIME_ALPHA ) );

	//===================================================================================================
	//
	//		마스크 생성
	//
	//===================================================================================================
	for(int i=0; i<ICONMASK::MAX_COUNT; ++i)
		m_pIconMask[ i ] = NULL;

	m_pIconMask[ ICONMASK::IF_ICON_MOUSE_DRAG ] = NiNew cSprite;
	m_pIconMask[ ICONMASK::IF_ICON_MOUSE_DRAG ]->Init( NULL, CsPoint::ZERO, IF_ICONSIZE, "Icon\\Mask_Drag.tga", false );

	m_pIconMask[ ICONMASK::IF_ICON_MOUSE_ON ] = NiNew cSprite;
	m_pIconMask[ ICONMASK::IF_ICON_MOUSE_ON ]->Init( NULL, CsPoint::ZERO, IF_ICONSIZE, "Icon\\Mask_Over.tga", false );

	m_pIconMask[ ICONMASK::IF_ICON_MOUSE_CLICK ] = NiNew cSprite;
	m_pIconMask[ ICONMASK::IF_ICON_MOUSE_CLICK ]->Init( NULL, CsPoint::ZERO, IF_ICONSIZE, "Icon\\Mask_Click.tga", false );

	m_pIconMask[ ICONMASK::IF_ICON_SELECT ] = NiNew cSprite;
	m_pIconMask[ ICONMASK::IF_ICON_SELECT ]->Init( NULL, CsPoint::ZERO, IF_ICONSIZE, "Icon\\Mask_Select.tga", false );
	//m_pIconMask[ ICONMASK::IF_ICON_SELECT ]->GetAlphaProp()->SetDestBlendMode( NiAlphaProperty::ALPHA_ONE );

	m_pIconMask[ ICONMASK::IF_ICON_RED ] = NiNew cSprite;
	m_pIconMask[ ICONMASK::IF_ICON_RED ]->Init( NULL, CsPoint::ZERO, IF_ICONSIZE, NiColorA( 39/255.0f, 39/255.0f, 39/255.0f, 0.8f ), false );

	m_pIconMask[ ICONMASK::IF_CURSOR_ICON ] = NiNew cSprite;
	m_pIconMask[ ICONMASK::IF_CURSOR_ICON ]->Init( NULL, CsPoint::ZERO, CsPoint( 27, 27 ), "Icon\\Item_Cursor.tga", false );

	m_pIconMask[ ICONMASK::IF_SOCKET ] = NiNew cSprite;
	m_pIconMask[ ICONMASK::IF_SOCKET ]->Init( NULL, CsPoint::ZERO, IF_ICONSIZE, "Icon\\Mask_Socket.tga", false );

	m_pIconMask[ ICONMASK::IF_ICON_REAL_RED ] = NiNew cSprite;
	m_pIconMask[ ICONMASK::IF_ICON_REAL_RED ]->Init( NULL, CsPoint::ZERO, IF_ICONSIZE, NiColorA( 1.0f, 0.f, 0.f, 0.6f ), false );

	m_pIconMask[ ICONMASK::IF_ICON_NORMAL_RED ] = NiNew cSprite;
	m_pIconMask[ ICONMASK::IF_ICON_NORMAL_RED ]->Init( NULL, CsPoint::ZERO, IF_ICONSIZE, NiColorA( 1.0f, 0.f, 0.f, 0.4f ), false );

	//===================================================================================================
	//
	//		숫자 생성
	//
	//===================================================================================================
	for(int i=0; i<NUMBERSPRITE::MAX_COLOR; ++i )
		m_pNumber[ i ] = NULL;

	m_pNumber[ NUMBERSPRITE::RED ] = NiNew cSprite;
	m_pNumber[ NUMBERSPRITE::RED ]->Init( NULL, CsPoint::ZERO, CsPoint( 384, 128 ), "Number0.tga", false );

	m_pNumber[ NUMBERSPRITE::ORANGE ] = NiNew cSprite;
	m_pNumber[ NUMBERSPRITE::ORANGE ]->Init( NULL, CsPoint::ZERO, CsPoint( 384, 128 ), "Number1.tga", false );

	m_pNumber[ NUMBERSPRITE::BLUE ] = NiNew cSprite;
	m_pNumber[ NUMBERSPRITE::BLUE ]->Init( NULL, CsPoint::ZERO, CsPoint( 384, 128 ), "Number2.tga", false );

	m_pNumber[ NUMBERSPRITE::GREEN ] = NiNew cSprite;
	m_pNumber[ NUMBERSPRITE::GREEN ]->Init( NULL, CsPoint::ZERO, CsPoint( 384, 128 ), "Number3.tga", false );

	m_pNumber[ NUMBERSPRITE::WHITE ] = NiNew cSprite;
	m_pNumber[ NUMBERSPRITE::WHITE ]->Init( NULL, CsPoint::ZERO, CsPoint( 384, 128 ), "Number4.tga", false );

	m_pNumber[ NUMBERSPRITE::YELLOW ] = NiNew cSprite;
	m_pNumber[ NUMBERSPRITE::YELLOW ]->Init( NULL, CsPoint::ZERO, CsPoint( 384, 128 ), "Number5.tga", false );

	m_pIconSkillMask = NiNew cSprite;
	m_pIconSkillMask->Init( NULL, CsPoint::ZERO, IF_ICONSIZE, "Icon\\Digimon_Face.tga", false );

	//===================================================================================================
	//
	//		아이콘 셋팅
	//
	//===================================================================================================
	m_mapIconItem[ ICONITEM::ARMOR_1 ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::ARMOR_1 ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\Icon01.dds");

	m_mapIconItem[ ICONITEM::ARMOR_2 ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::ARMOR_2 ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\Icon02.dds");

	m_mapIconItem[ ICONITEM::ELEMENT_NO_CHECK ]  = NiNew cIcon;
	m_mapIconItem[ ICONITEM::ELEMENT_NO_CHECK ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\Icon03.dds");

	m_mapIconItem[ ICONITEM::ELEMENT_CHECK ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::ELEMENT_CHECK ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\Icon04.dds");

	m_mapIconItem[ ICONITEM::CHIPSET ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::CHIPSET ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\Icon05.dds");

	m_mapIconItem[ ICONITEM::CARD ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::CARD ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\Icon06.dds");

	m_mapIconItem[ ICONITEM::ITEM_CARD ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::ITEM_CARD ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\Icon07.dds");

	m_mapIconItem[ ICONITEM::QUEST_ITEM ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::QUEST_ITEM ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE_ICON8, "Icon\\Icon08.dds");

	m_mapIconItem[ ICONITEM::DT_MEMORY ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::DT_MEMORY ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\Icon09.dds");

	m_mapIconItem[ ICONITEM::ITEM_ICON ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::ITEM_ICON ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE_ICON8, "Icon\\Icon10.dds");

	m_mapIconItem[ICONITEM::ITEM_ICON47] = NiNew cIcon;
	m_mapIconItem[ICONITEM::ITEM_ICON47]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE_ICON8, "Icon\\Icon47.dds");

	m_mapIconItem[ICONITEM::ITEM_ICON48] = NiNew cIcon;
	m_mapIconItem[ICONITEM::ITEM_ICON48]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE_ICON8, "Icon\\Icon48.dds");

	m_mapIconItem[ ICONITEM::ITEM_ICON49 ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::ITEM_ICON49 ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE_ICON8, "Icon\\Icon49.dds");

	m_mapIconItem[ ICONITEM::SKILL1 ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::SKILL1 ]->Init(IF_ICONSIZE,ICON_IMAGE_SIZE, "Icon\\sIcon01.dds");

	m_mapIconItem[ ICONITEM::SKILL2 ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::SKILL2 ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\sIcon02.dds");

	m_mapIconItem[ ICONITEM::SKILL3 ]= NiNew cIcon;
	m_mapIconItem[ ICONITEM::SKILL3 ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\sIcon03.dds");

	m_mapIconItem[ ICONITEM::SKILL4 ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::SKILL4 ]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\sIcon04.dds");

	m_mapIconItem[ICONITEM::SKILL5] = NiNew cIcon;
	m_mapIconItem[ICONITEM::SKILL5]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\sIcon05.dds");

	m_mapIconItem[ICONITEM::SKILL6] = NiNew cIcon;
	m_mapIconItem[ICONITEM::SKILL6]->Init(IF_ICONSIZE, ICON_IMAGE_SIZE, "Icon\\sIcon06.dds");

	m_mapIconItem[ ICONITEM::DEvol ]= NiNew cIcon;
	m_mapIconItem[ ICONITEM::DEvol ]->Init( CsPoint( 60, 19 ), CsPoint( 60, 114 ), "Icon\\DEvolIcon.tga" );

#ifdef VERSION_USA
	CsPoint ptTextPos( 40, 2 );
#else
	CsPoint ptTextPos( 30, 2 );
#endif

	CsBaseMng::MAP_EVOLUTION_BASE_INFOS const& evoInfo = nsCsFileTable::g_pBaseMng->GetBaseEvoInfos();
	CsBaseMng::MAP_EVOLUTION_BASE_INFOS_CIT it = evoInfo.begin();
	for( int n = 0; it != evoInfo.end(); ++it, ++n )
		m_mapIconItem[ ICONITEM::DEvol ]->SetIconText( n, it->second.m_EvolutionTypeName.c_str(), ptTextPos );

	m_mapIconItem[ ICONITEM::DAttribute ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::DAttribute ]->Init( CsPoint( 19, 19 ), CsPoint( 114, 114 ), "Icon\\DAicon.tga" );

	m_mapIconItem[ ICONITEM::DGrowType ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::DGrowType ]->Init( CsPoint( 30, 19 ), CsPoint( 30, 76 ), "Icon\\GrowType.tga" );

	m_mapIconItem[ ICONITEM::MiniMap ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::MiniMap ]->Init( CsPoint( 22, 16 ), CsPoint( 110, 80 ), "Icon\\Minimap_Icon.tga" );

	m_mapIconItem[ ICONITEM::MonsterAI ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::MonsterAI ]->Init( CsPoint( 19, 19 ), CsPoint( 38, 19 ), "Icon\\MonsterAI.tga" );

	m_mapIconItem[ ICONITEM::Stat ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::Stat ]->Init( IF_ICONSIZE, ICON_IMAGE_SIZE_STAT, "Icon\\Stat.tga" );

	m_mapIconItem[ ICONITEM::Community ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::Community ]->Init( CsPoint( 18, 18 ), CsPoint( 144, 144 ), "Icon\\Community_Icon.tga" );

	m_mapIconItem[ ICONITEM::CHAT_LV ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::CHAT_LV ]->Init( CsPoint( 14, 14 ), CsPoint( 140, 14 ), "Icon\\Chat_Lv.tga" );

	m_mapIconItem[ ICONITEM::Enchant_Lv ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::Enchant_Lv ]->Init( CsPoint( 23, 16 ), CsPoint( 230, 32 ), "Icon\\Enchant_Lv.tga" );

	m_mapIconItem[ ICONITEM::New_Taget_DAttribute ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::New_Taget_DAttribute ]->Init( CsPoint( 40, 40 ), CsPoint( 200, 40 ), "Icon\\digi_basetype.dds" );

	m_mapIconItem[ ICONITEM::Talk ] = NiNew cIcon;
	m_mapIconItem[ ICONITEM::Talk ]->Init( CsPoint( 16, 16 ), CsPoint( 128, 128 ), "Icon\\Talk_Icon.tga" );

	m_mapIconItem[ ICONITEM::ITEM_MASK ] = NiNew cIcon;	
	m_mapIconItem[ ICONITEM::ITEM_MASK ]->Init( IF_ICONSIZE, ICON_IMAGE_SIZE_MASK, "Icon\\ItemMask.tga" );	

	/////////////////////////////////////////////////////////////////////
	//2017-02-28-nova 캐시샵 아이콘 이미지
	m_mapIconItem[ICON_Dynamic::CashShop_Mask] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_Mask]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop_Ribon.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_101] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_101]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_101.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_102] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_102]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_102.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_103] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_103]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_103.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_104] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_104]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_104.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_105] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_105]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_105.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_106] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_106]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_106.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_107] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_107]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_107.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_108] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_108]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_108.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_109] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_109]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_109.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_201] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_201]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_201.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_02_301] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_02_301]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop2_301.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_03_101] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_03_101]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop3_101.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_03_102] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_03_102]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop3_102.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_03_103] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_03_103]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop3_103.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_03_104] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_03_104]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop3_104.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_03_105] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_03_105]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop3_105.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_03_201] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_03_201]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop3_201.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_03_301] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_03_301]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop3_301.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_03_401] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_03_401]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop3_401.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_03_501] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_03_501]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop3_501.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_04_101] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_04_101]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop4_101.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_04_102] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_04_102]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop4_102.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_04_103] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_04_103]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop4_103.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_04_201] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_04_201]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop4_201.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_04_301] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_04_301]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop4_301.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_04_401] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_04_401]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop4_401.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_04_501] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_04_501]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop4_501.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_04_601] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_04_601]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop4_601.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_05_101] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_05_101]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop5_101.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_05_102] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_05_102]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop5_102.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_05_103] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_05_103]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop5_103.tga");

	m_mapIconItem[ICON_Dynamic::CashShop_05_104] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::CashShop_05_104]->Init(CsPoint( 80, 80 ), CsPoint( 480, 480 ), "Icon\\CashShop5_104.tga");

	m_mapIconItem[ICON_Dynamic::Achieve] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::Achieve]->Init(CsPoint( 32, 32 ), CsPoint( 512, 512 ), "Icon\\Achieve_Icon.tga");

	m_mapIconItem[ICON_Dynamic::Achieve2] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::Achieve2]->Init(CsPoint( 32, 32 ), CsPoint( 512, 512 ), "Icon\\Achieve_Icon_02.tga");
#ifdef PC_BANG_SERVICE
	m_mapIconItem[ICON_Dynamic::PcBangIcon] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::PcBangIcon]->Init(CsPoint( 34, 32 ), CsPoint( 34, 32 ), "Event\\PC_Bang.tga");	
#endif

	m_mapIconItem[ICON_Dynamic::Achieve3] = NiNew cIcon;
	m_mapIconItem[ICON_Dynamic::Achieve3]->Init(CsPoint( 32, 32 ), CsPoint( 512, 512 ), "Icon\\Achieve_Icon_03.tga");
}

void cIconMng::DeleteDynamicIcon( ICON_Dynamic::eTYPE eType )
{
	std::map< DWORD, cIcon* >::iterator it = m_mapDynamicIcon.begin();
	std::map< DWORD, cIcon* >::iterator itEnd = m_mapDynamicIcon.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->first == eType )
		{
			SAFE_NIDELETE( it->second );
			m_mapDynamicIcon.erase( it );
			return;
		}
	}			
}	

void cIconMng::InitDynamicIcon( ICON_Dynamic::eTYPE eType, CsPoint patch, CsPoint size , char* cPath )
{
	cIcon* pIcon = NiNew cIcon;
	pIcon->Init( patch, size, cPath );
	m_mapDynamicIcon[ eType ] = pIcon;		
}

NiSourceTexturePtr cIconMng::GetTexture( ICON_Dynamic::eTYPE eType )
{
	std::map< DWORD, cIcon* >::iterator it = m_mapIconItem.find(eType);
	if( it == m_mapIconItem.end() )
		return NULL;
	
	SAFE_POINTER_RETVAL( it->second, NULL );
	cSprite* pSp = it->second->GetSprite();
	SAFE_POINTER_RETVAL( pSp, NULL );
	NiTexturingProperty* pTexturePro = pSp->GetTexProp();
	SAFE_POINTER_RETVAL( pTexturePro, NULL );
	NiTexture* pText = pTexturePro->GetBaseTexture();
	return NiDynamicCast(NiSourceTexture, pText);
}
cSprite*	cIconMng::GetSprite(ICON_Dynamic::eTYPE eType)
{
	std::map< DWORD, cIcon* >::iterator it = m_mapIconItem.find(eType);
	if( it == m_mapIconItem.end() )
		return NULL;

	SAFE_POINTER_RETVAL( it->second, NULL );
	cSprite* pSp = it->second->GetSprite();
	return pSp;
}
cIcon*		cIconMng::GetIcon(ICON_Dynamic::eTYPE eType)
{
	std::map< DWORD, cIcon* >::iterator it = m_mapIconItem.find(eType);
	if( it == m_mapIconItem.end() )
		return NULL;

	return it->second;

}


void cIconMng::ResetDevice()
{
	cIcon::GlobalResetDevice();

	std::map< DWORD, cIcon* >::iterator it = m_mapIconItem.begin();
	std::map< DWORD, cIcon* >::iterator itEnd = m_mapIconItem.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->ResetDevice();
	}

	std::map< DWORD, cIcon* >::iterator it2 = m_mapDynamicIcon.begin();
	std::map< DWORD, cIcon* >::iterator it2End = m_mapDynamicIcon.end();
	for( ; it2!=it2End; ++it2 )
	{
		it2->second->ResetDevice();
	}

	// 마스크
	for(int i=0; i<ICONMASK::MAX_COUNT; ++i )
		m_pIconMask[ i ]->ResetDevice();
}


//===================================================================================================
//
//		아이템
//
//===================================================================================================

#define ICON_ITEM_MASK		1000

void cIconMng::_IconIDToTexIndex( DWORD& dwTex, int& nIconIdx, cItemInfo* pItemInfo, int nIconID )
{
	dwTex = nIconID / ICON_ITEM_MASK;
	nIconIdx = nIconID % ICON_ITEM_MASK;

	if( dwTex == ICONITEM::ELEMENT_NO_CHECK )
	{
		if( ( pItemInfo )&&( pItemInfo->m_nRate != 0 ) )
			dwTex += 1;
	}
	assert_cs( nIconIdx < 1024 );			//	512 맵에 32 사이즈 기준
}

void cIconMng::RenderItem( cItemInfo* pItem, CsPoint pos, bool bRenderCount )
{
	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
	SAFE_POINTER_RET( pFT );

	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, pItem, pFT->GetInfo()->s_nIcon );
	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );

	cIcon* pIcon = m_mapIconItem[ dwTex ];
	pIcon->Render( nIconIdx, pos );

	m_mapIconItem[ ICONITEM::ITEM_MASK ]->Render( pFT->GetInfo()->s_nClass - 1, pos );

	if( bRenderCount == true )
	{
		pIcon->RenderCount( pItem->m_nCount, pos );
	}
}

void cIconMng::RenderItem( cItemInfo* pItem, CsPoint pos, CsPoint size, bool bRenderCount )
{
	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
	SAFE_POINTER_RET( pFT );

	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, pItem, pFT->GetInfo()->s_nIcon );
	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );

	cIcon* pIcon = m_mapIconItem[ dwTex ];
	CsPoint ptOldSize = pIcon->GetSprite()->GetSize();
	pIcon->Render( nIconIdx, pos, size );
	pIcon->GetSprite()->SetSize( ptOldSize );

	m_mapIconItem[ ICONITEM::ITEM_MASK ]->Render( pFT->GetInfo()->s_nClass - 1, pos, size );
	m_mapIconItem[ ICONITEM::ITEM_MASK ]->GetSprite()->SetSize( ptOldSize );

	CsPoint ptDeltaPos = size - ptOldSize;
	if( bRenderCount == true )
	{
		pIcon->RenderCount( pItem->m_nCount, pos + ptDeltaPos );
	}
}

cIcon*	cIconMng::GetIcon(int nItemID)
{
	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( nItemID );
	SAFE_POINTER_RETVAL( pFT, NULL );

	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, NULL, pFT->GetInfo()->s_nIcon );

	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );	

	return m_mapIconItem[ dwTex ];
}

void cIconMng::SetItemAlpha( int nItemID, float fAlpha )
{
	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( nItemID );
	SAFE_POINTER_RET( pFT );

	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, NULL, pFT->GetInfo()->s_nIcon );

	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );
	m_mapIconItem[ dwTex ]->GetSprite()->SetAlpha( fAlpha );
}

void cIconMng::RenderItem( int nItemID, CsPoint pos )
{
	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( nItemID );
	SAFE_POINTER_RET( pFT );

	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, NULL, pFT->GetInfo()->s_nIcon );

	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );
	m_mapIconItem[ dwTex ]->Render( nIconIdx, pos );

	m_mapIconItem[ ICONITEM::ITEM_MASK ]->Render( pFT->GetInfo()->s_nClass - 1, pos );
}

void cIconMng::RenderItem( int nItemID, CsPoint pos, CsPoint size, float fAlpha )
{
	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( nItemID );
	SAFE_POINTER_RET( pFT );

	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, NULL, pFT->GetInfo()->s_nIcon );

	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );
	CsPoint ptOldSize = m_mapIconItem[ dwTex ]->GetSprite()->GetSize();
	float fOldAlpha = m_mapIconItem[ dwTex ]->GetSprite()->GetColorA().a;
	m_mapIconItem[ dwTex ]->Render( nIconIdx, pos, size, fAlpha );
	m_mapIconItem[ dwTex ]->GetSprite()->SetSize( ptOldSize );
	m_mapIconItem[ dwTex ]->GetSprite()->SetAlpha( fOldAlpha );

	m_mapIconItem[ ICONITEM::ITEM_MASK ]->Render( pFT->GetInfo()->s_nClass - 1, pos, size, fAlpha );
	m_mapIconItem[ ICONITEM::ITEM_MASK ]->GetSprite()->SetSize( ptOldSize );
	m_mapIconItem[ ICONITEM::ITEM_MASK ]->GetSprite()->SetAlpha( fOldAlpha );
	
}

void cIconMng::RenderItem( int nItemID, CsPoint pos, CsPoint size )
{
	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( nItemID );
	SAFE_POINTER_RET( pFT );

	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, NULL, pFT->GetInfo()->s_nIcon );

	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );
	CsPoint ptOldSize = m_mapIconItem[ dwTex ]->GetSprite()->GetSize();
	m_mapIconItem[ dwTex ]->Render( nIconIdx, pos, size );
	m_mapIconItem[ dwTex ]->GetSprite()->SetSize( ptOldSize );

	m_mapIconItem[ ICONITEM::ITEM_MASK ]->Render( pFT->GetInfo()->s_nClass - 1, pos, size );
	m_mapIconItem[ ICONITEM::ITEM_MASK ]->GetSprite()->SetSize( ptOldSize );
}

void cIconMng::RenderItem( int nItemID, CsPoint pos, int nRenderCount )
{
	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( nItemID );
	SAFE_POINTER_RET( pFT );

	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, NULL, pFT->GetInfo()->s_nIcon );

	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );
	m_mapIconItem[ dwTex ]->Render( nIconIdx, pos );

	m_mapIconItem[ ICONITEM::ITEM_MASK ]->Render( pFT->GetInfo()->s_nClass - 1, pos );

	cIcon* pIcon = m_mapIconItem[ dwTex ];	
	pIcon->RenderCount( nRenderCount, pos );	
}

void cIconMng::RenderSkill( cSkill::sINFO* pSkill, CsPoint pos )
{
	SAFE_POINTER_RET( pSkill );
	CsSkill* pFTSkill = pSkill->s_pFTSkill;
	SAFE_POINTER_RET( pFTSkill );
	CsSkill::sINFO* pSkillInfo = pFTSkill->GetInfo();
	SAFE_POINTER_RET( pSkillInfo );

	cIcon* pIcon;	
	if( pSkillInfo->s_nIcon >= 4000 )
		pIcon = m_mapIconItem[ ICONITEM::SKILL4 ];
	else if( pSkillInfo->s_nIcon >= 3000 )
		pIcon = m_mapIconItem[ ICONITEM::SKILL3 ];
	else if( pSkillInfo->s_nIcon >= 2000 )
		pIcon = m_mapIconItem[ ICONITEM::SKILL2 ];
	else
		pIcon = m_mapIconItem[ ICONITEM::SKILL1 ];

	switch( pSkillInfo->s_nIcon )
	{
	case CsSkill::IT_CHANGE1:
	case CsSkill::IT_CHANGE2:
	case CsSkill::IT_CHANGE3:
		RenderSkillMask( pSkillInfo->s_nIcon, pos, pSkill->s_nTacticsIndex );		
		break;
	default:
		pIcon->Render( pSkillInfo->s_nIcon%1000, pos, IF_ICONSIZE );
		break;
	}	
}

void cIconMng::RenderSkill( CsSkill* pSkill, CsPoint pos, int nLevel/* = 0*/ )
{
	CsSkill::sINFO* pSkillInfo = pSkill->GetInfo();

	cIcon* pIcon;	
	if( pSkillInfo->s_nIcon >= 4000 )
		pIcon = m_mapIconItem[ ICONITEM::SKILL4 ];
	else if( pSkillInfo->s_nIcon >= 3000 )
		pIcon = m_mapIconItem[ ICONITEM::SKILL3 ];
	else if( pSkillInfo->s_nIcon >= 2000 )
		pIcon = m_mapIconItem[ ICONITEM::SKILL2 ];
	else
		pIcon = m_mapIconItem[ ICONITEM::SKILL1 ];

	switch( pSkillInfo->s_nIcon )
	{
	case CsSkill::IT_CHANGE1:	 //디지몬 교체 스킬	
	case CsSkill::IT_CHANGE2:	 //디지몬 교체 스킬	
	case CsSkill::IT_CHANGE3:	 //디지몬 교체 스킬	
		RenderSkillMask( pSkillInfo->s_nIcon, pos );		
		break;
	default:
		pIcon->Render( pSkillInfo->s_nIcon%1000, pos, IF_ICONSIZE );
		break;
	}	

	if( nLevel > 0 )
	{
		pIcon->RenderCount( nLevel, pos );
	}
}

void cIconMng::RenderSkill( unsigned short const& nIconIdx, CsPoint pos, int nLevel/* = 0*/ )
{
	cIcon* pIcon = NULL;

	if( nIconIdx >= 4000 )			pIcon = m_mapIconItem[ ICONITEM::SKILL4 ];
	else if( nIconIdx >= 3000 )		pIcon = m_mapIconItem[ ICONITEM::SKILL3 ];
	else if( nIconIdx >= 2000 )		pIcon = m_mapIconItem[ ICONITEM::SKILL2 ];
	else							pIcon = m_mapIconItem[ ICONITEM::SKILL1 ];

	if( NULL == pIcon )
		return;

	switch( nIconIdx )
	{
	case CsSkill::IT_CHANGE1:	 //디지몬 교체 스킬	
	case CsSkill::IT_CHANGE2:	 //디지몬 교체 스킬	
	case CsSkill::IT_CHANGE3:	 //디지몬 교체 스킬	
		RenderSkillMask( nIconIdx, pos );		
		break;
	default:
		pIcon->Render( nIconIdx%1000, pos, IF_ICONSIZE );
		break;
	}	

	if( nLevel > 0 )
		pIcon->RenderCount( nLevel, pos );
}

void cIconMng::RenderSkillMask( USHORT nSkill, CsPoint pos, int nTacticsIndex /*= -1*/ )
{
	cIcon* pIcon = m_mapIconItem[ ICONITEM::SKILL1 ];
	SAFE_POINTER_RET( pIcon );

// 	switch( nSkill ) 
// 	{
// 	case CsSkill::IT_CHANGE1: //디지몬 1번 교체 스킬
// 		if( g_pGameIF->GetTacticsWindow( 0 )->GetTargetImage() )
// 			g_pGameIF->GetTacticsWindow( 0 )->GetTargetImage()->Render( pos );
// 		else
// 			pIcon->Render( 15, pos );			
// 		break;
// 	case CsSkill::IT_CHANGE2: //디지몬 2번 교체 스킬
// 		if( g_pGameIF->GetTacticsWindow( 1 )->GetTargetImage() )
// 			g_pGameIF->GetTacticsWindow( 1 )->GetTargetImage()->Render( pos );
// 		else
// 			pIcon->Render( 15, pos );			
// 		break;
// 	case CsSkill::IT_CHANGE3: //디지몬 3번 교체 스킬
// 		if( g_pGameIF->GetTacticsWindow( 2 )->GetTargetImage() )
// 			g_pGameIF->GetTacticsWindow( 2 )->GetTargetImage()->Render( pos );
// 		else
// 			pIcon->Render( 15, pos );			
// 		break;
// 	}

	switch( nSkill )
	{
	case CsSkill::IT_CHANGE1:
	case CsSkill::IT_CHANGE2:
	case CsSkill::IT_CHANGE3:
		{
			// cData_Tactics 에 용병 슬롯 크기가 nLimit::Memory /*= 6*/ 박혀있음
			if( 0 > nTacticsIndex || nLimit::DigimonBaseSlot - 1 <= nTacticsIndex )
				break;

			SAFE_POINTER_BEK( g_pDataMng );
			cData_Tactics* pTactics = g_pDataMng->GetTactics();
			SAFE_POINTER_BEK( pTactics );
			if( pTactics->IsTactics( nTacticsIndex ) )
			{
				cData_PostLoad::sDATA* pTacticsData = pTactics->GetTactics( nTacticsIndex );
				SAFE_POINTER_BEK( pTacticsData );
				SAFE_POINTER_BEK( nsCsFileTable::g_pDigimonMng );
				DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimonModelIdx( pTacticsData->s_dwBaseDigimonID );
				SAFE_POINTER_BEK( g_pModelDataMng );
				std::string strPath = g_pModelDataMng->GetLargeModelIconFile( dwModelID );
				SAFE_POINTER_BEK( m_pIconSkillMask );
				m_pIconSkillMask->ChangeTexture( strPath.c_str(), false );
				m_pIconSkillMask->Render( pos );
			}
			else
				pIcon->Render( 15, pos );
		}
		break;
	default:
		break;
	}

//	m_pIconSkillMask->Render( pos );
}

void cIconMng::RendeCursorSkillMask( USHORT nSkill, CsPoint pos, int nTacticsIndex /*= -1*/ )
{
	cIcon* pIcon = m_mapIconItem[ ICONITEM::SKILL1 ];
// 	switch( nSkill )
// 	{
// 	case CsSkill::IT_CHANGE1: //디지몬 1번 교체 스킬	
// 		if( g_pGameIF->GetTacticsWindow( 0 )->GetTargetImage() )
// 		{
// 			g_pGameIF->GetTacticsWindow( 0 )->GetTargetImage()->Render( pos, CsPoint( 27, 27 ) );
// 			g_pGameIF->GetTacticsWindow( 0 )->GetTargetImage()->SetSize( IF_ICONSIZE );
// 		}
// 		else
// 		{
// 			pIcon->Render( 15, pos );			
// 		}
// 		break;
// 	case CsSkill::IT_CHANGE2: //디지몬 2번 교체 스킬
// 		if( g_pGameIF->GetTacticsWindow( 1 )->GetTargetImage() )
// 		{
// 			g_pGameIF->GetTacticsWindow( 1 )->GetTargetImage()->Render( pos, CsPoint( 27, 27 ) );
// 			g_pGameIF->GetTacticsWindow( 1 )->GetTargetImage()->SetSize( IF_ICONSIZE );
// 		}
// 		else
// 		{
// 			pIcon->Render( 15, pos, CsPoint( 27, 27 ) );			
// 		}
// 		break;
// 	case CsSkill::IT_CHANGE3: //디지몬 3번 교체 스킬
// 		if( g_pGameIF->GetTacticsWindow( 2 )->GetTargetImage() )
// 		{
// 			g_pGameIF->GetTacticsWindow( 2 )->GetTargetImage()->Render( pos, CsPoint( 27, 27 ) );
// 			g_pGameIF->GetTacticsWindow( 2 )->GetTargetImage()->SetSize( IF_ICONSIZE );
// 		}
// 		else
// 		{
// 			pIcon->Render( 15, pos, CsPoint( 27, 27 ) );			
// 		}
// 		break;
// 	}	

	switch( nSkill )
	{
	case CsSkill::IT_CHANGE1:
	case CsSkill::IT_CHANGE2:
	case CsSkill::IT_CHANGE3:
		{
			// cData_Tactics 에 용병 슬롯 크기가 nLimit::Memory /*= 6*/ 박혀있음
			if( 0 > nTacticsIndex || nLimit::DigimonBaseSlot - 1 <= nTacticsIndex )
				break;

			SAFE_POINTER_BEK( g_pDataMng );
			cData_Tactics* pTactics = g_pDataMng->GetTactics();
			SAFE_POINTER_BEK( pTactics );
			if( pTactics->IsTactics( nTacticsIndex ) )
			{
				cData_PostLoad::sDATA* pTacticsData = pTactics->GetTactics( nTacticsIndex );
				SAFE_POINTER_BEK( pTacticsData );
				SAFE_POINTER_BEK( nsCsFileTable::g_pDigimonMng );
				DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimonModelIdx( pTacticsData->s_dwBaseDigimonID );
				SAFE_POINTER_BEK( g_pModelDataMng );
				std::string strPath = g_pModelDataMng->GetLargeModelIconFile( dwModelID );
				SAFE_POINTER_BEK( m_pIconSkillMask );
				m_pIconSkillMask->ChangeTexture( strPath.c_str(), false );
				m_pIconSkillMask->Render( pos, CsPoint( 27, 27 ) );
				m_pIconSkillMask->SetSize( IF_ICONSIZE );	// 사이즈 원상복구
			}
			else
				pIcon->Render( 15, pos, CsPoint( 27, 27 ) );
		}
		break;
	default:
		break;
	}

// 	m_pIconSkillMask->Render( pos, CsPoint( 27, 27 ) );
// 	
// 	// 사이즈 원상 복귀
// 	m_pIconSkillMask->SetSize( IF_ICONSIZE );
}

void cIconMng::RenderDEvol( nsCsDigimonTable::eEVOLUTION_TYPE et, CsPoint pos )
{
	cIcon* pIcon = m_mapIconItem[ ICONITEM::DEvol ];
	// 디지타마를 0 기반 인덱스로
	int nIndex = et-nsCsDigimonTable::ET_DIGITAMA;
	pIcon->Render( nIndex, pos );
}

void cIconMng::RenderDEvol( nsCsDigimonTable::eEVOLUTION_TYPE et, CsPoint pos, CsPoint size )
{
	cIcon* pIcon = m_mapIconItem[ ICONITEM::DEvol ];
	// 디지타마를 0 기반 인덱스로
	int nIndex = et-nsCsDigimonTable::ET_DIGITAMA;
	pIcon->Render( nIndex, pos, size );
}

void cIconMng::RenderDAttribute( eDAttribute da, int nIndex, CsPoint pos )
{
	cIcon* pIcon = m_mapIconItem[ ICONITEM::DAttribute ];

	int nCol = nIndex%6;
	int nRow = 0;
	switch( da )
	{
	case DAttribute:		nRow = 0;						break;
	case Family:			nRow = nIndex/6 + 1;			break;
	case Nature:			nRow = nIndex/6 + 3;			break;
	default:				assert_cs( false );
	}
	pIcon->Render( nCol, nRow, pos );
}

void cIconMng::RenderDAttribute( eDAttribute da, int nIndex, CsPoint pos, CsPoint size )
{
	cIcon* pIcon = m_mapIconItem[ ICONITEM::DAttribute ];

	int nCol = nIndex%6;
	int nRow = 0;
	switch( da )
	{
	case DAttribute:		nRow = 0;						break;
	case Family:			nRow = nIndex/6 + 1;			break;
	case Nature:			nRow = nIndex/6 + 3;			break;
	default:				assert_cs( false );
	}
	pIcon->Render( nCol, nRow, pos, size );
}

void cIconMng::RenderIcon( ICONITEM::eTYPE type, CsPoint pos, int nIndex )
{
	ICON_CITER citer = m_mapIconItem.find(type);

	if(citer == m_mapIconItem.end())
		return;

	cIcon* pIcon = citer->second;

	if( type == ICONITEM::MiniMap && pIcon->FlickerMode(nIndex) )
		return;

	int nCol = nIndex%pIcon->GetIndexCount().x;
	int nRow = nIndex/pIcon->GetIndexCount().x;

	pIcon->Render( nCol, nRow, pos );
}

void cIconMng::RenderIcon( ICONITEM::eTYPE type, CsPoint pos, CsPoint size, int nIndex, int nStackCnt )	//	아이콘 렌더 수정 (중첩 표현) chu8820
{
	ICON_CITER citer = m_mapIconItem.find(type);

	if(citer == m_mapIconItem.end())
		return;

	cIcon* pIcon = citer->second;

	if( type == ICONITEM::MiniMap && pIcon->FlickerMode(nIndex) )
		return;

	int nCol = nIndex%pIcon->GetIndexCount().x;
	int nRow = nIndex/pIcon->GetIndexCount().x;

	CsPoint ptOldSize = pIcon->GetSprite()->GetSize();
	pIcon->Render( nCol, nRow, pos, size );
	pIcon->GetSprite()->SetSize( ptOldSize );

	if( nStackCnt > 0 )	//	중첩 수
	{
		pos -= CsPoint( 10, 8 );
		pIcon->RenderCount( nStackCnt, pos );
	}
}

void cIconMng::RenderTalk( CsPoint pos, CsPoint size, int nIndex )
{
	cIcon* pIcon = m_mapIconItem[ ICONITEM::Talk ];

	int nCol = nIndex%pIcon->GetIndexCount().x;
	int nRow = nIndex/pIcon->GetIndexCount().x;
	pIcon->Render( nCol, nRow, pos, size );
}

void cIconMng::RenderCashShopIcon( ICON_Dynamic::eTYPE type, CsPoint pos, int nIndex )
{
	cIcon* pIcon = m_mapIconItem[ type ];

	if( type == ICON_Dynamic::Achieve2 )	// Achieve2는 아이콘 번호 300부터 시작
		nIndex -= 300;	// 300을 빼주어 기존과 같은 로직에서 아이콘의 행과 열이 구해지도록 함.

	int nCol = nIndex%pIcon->GetIndexCount().x;
	int nRow = nIndex/pIcon->GetIndexCount().x;
	pIcon->Render( nCol, nRow, pos );
}

void cIconMng::RenderCashShopIcon( ICON_Dynamic::eTYPE type, CsPoint pos, CsPoint size, int nIndex )
{
	cIcon* pIcon = m_mapIconItem[ type ];
	CsPoint ptSize = pIcon->GetSprite()->GetSize();

	if( type == ICON_Dynamic::Achieve2 )	// Achieve2는 아이콘 번호 300부터 시작
		nIndex -= 300;	// 300을 빼주어 기존과 같은 로직에서 아이콘의 행과 열이 구해지도록 함.

	int nCol = nIndex%pIcon->GetIndexCount().x;
	int nRow = nIndex/pIcon->GetIndexCount().x;
	pIcon->Render( nCol, nRow, pos, size );
	pIcon->GetSprite()->SetSize( ptSize );
}

void cIconMng::RenderDynamicIcon( ICON_Dynamic::eTYPE type, CsPoint pos, int nIndex )
{
	cIcon* pIcon = m_mapDynamicIcon[ type ];

	if( type == ICON_Dynamic::Achieve2 )	// Achieve2는 아이콘 번호 300부터 시작
		nIndex -= 300;	// 300을 빼주어 기존과 같은 로직에서 아이콘의 행과 열이 구해지도록 함.

	int nCol = nIndex%pIcon->GetIndexCount().x;
	int nRow = nIndex/pIcon->GetIndexCount().x;
	pIcon->Render( nCol, nRow, pos );
}

void cIconMng::RenderDynamicIcon( ICON_Dynamic::eTYPE type, CsPoint pos, CsPoint size, int nIndex )
{
	cIcon* pIcon = m_mapDynamicIcon[ type ];
	CsPoint ptSize = pIcon->GetSprite()->GetSize();

	if( type == ICON_Dynamic::Achieve2 )	// Achieve2는 아이콘 번호 300부터 시작
		nIndex -= 300;	// 300을 빼주어 기존과 같은 로직에서 아이콘의 행과 열이 구해지도록 함.

	int nCol = nIndex%pIcon->GetIndexCount().x;
	int nRow = nIndex/pIcon->GetIndexCount().x;
	pIcon->Render( nCol, nRow, pos, size );
	pIcon->GetSprite()->SetSize( ptSize );
}



void cIconMng::RenderIFIconMask( sIFIcon::eSTATE state, CsPoint pos, CsPoint size )
{
	switch( state )
	{
	case sIFIcon::MOUSE_DRAG:
		m_pIconMask[ ICONMASK::IF_ICON_MOUSE_DRAG ]->Render( pos, size );	break;
	case sIFIcon::MOUSE_ON:
		m_pIconMask[ ICONMASK::IF_ICON_MOUSE_ON ]->Render( pos, size );		break;
	case sIFIcon::MOUSE_LCLICK:
	case sIFIcon::MOUSE_RCLICK:
		m_pIconMask[ ICONMASK::IF_ICON_MOUSE_CLICK ]->Render( pos, size );	break;
	default:
		assert_cs( false );
	}
}

void cIconMng::RenderMask( ICONMASK::eMASK_TYPE mt, CsPoint pos, CsPoint size )
{
	m_pIconMask[ mt ]->Render( pos, size );
}

void cIconMng::RenderCursorIconItem( cItemInfo* pItem, CsPoint pos )
{
	assert_cs( pItem->IsEnable() == true );	

	// 아이템 테이블에서 정보 가져 온다.
	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
	assert_cs( pFT != NULL );

	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, pItem, pFT->GetInfo()->s_nIcon );
	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );

	// 아이콘 랜더링 - 사이즈 작게 해서
	cIcon* pIcon = m_mapIconItem[ dwTex ];
	pIcon->Render( nIconIdx, pos+CsPoint( 1, 1 ), CsPoint( 25, 25 ) );	

	// 마스크 랜더링
	m_pIconMask[ ICONMASK::IF_CURSOR_ICON ]->Render( pos );

	// 사이즈 원상 복귀
	pIcon->GetSprite()->SetSize( IF_ICONSIZE );
}


void cIconMng::RenderCursorIconItem( int nIconIndex, CsPoint pos )
{
	DWORD dwTex;
	int nIconIdx;
	_IconIDToTexIndex( dwTex, nIconIdx, NULL, nIconIndex );
	assert_cs( m_mapIconItem.find( dwTex ) != m_mapIconItem.end() );

	// 아이콘 랜더링 - 사이즈 작게 해서
	cIcon* pIcon = m_mapIconItem[ dwTex ];
	pIcon->Render( nIconIdx, pos+CsPoint( 1, 1 ), CsPoint( 25, 25 ) );	

	// 마스크 랜더링
	m_pIconMask[ ICONMASK::IF_CURSOR_ICON ]->Render( pos );

	// 사이즈 원상 복귀
	pIcon->GetSprite()->SetSize( IF_ICONSIZE );
}


void cIconMng::RenderCursorIconSkill( cSkill::sINFO* pSkill, CsPoint pos )
{
	// 아이콘 랜더링 - 사이즈 작게 해서
	SAFE_POINTER_RET( pSkill );
	CsSkill*pFTSkill = pSkill->s_pFTSkill;
	SAFE_POINTER_RET( pFTSkill );
	CsSkill::sINFO* pSkillInfo = pFTSkill->GetInfo();
	SAFE_POINTER_RET( pSkillInfo );
	RenderCursorIconSkill( pSkillInfo->s_nIcon, pos, pSkill->s_nTacticsIndex );
}

void cIconMng::RenderCursorIconSkill( int nIconIndex, CsPoint pos, int nTacticsIndex )
{
	cIcon* pIcon = NULL;

	if( nIconIndex >= 4000 )
		pIcon = m_mapIconItem[ ICONITEM::SKILL4 ];
	else if( nIconIndex >= 3000 )
		pIcon = m_mapIconItem[ ICONITEM::SKILL3 ];
	else if( nIconIndex >= 2000 )
		pIcon = m_mapIconItem[ ICONITEM::SKILL2 ];
	else
		pIcon = m_mapIconItem[ ICONITEM::SKILL1 ];

	switch( nIconIndex )
	{
	case CsSkill::IT_CHANGE1:	 //디지몬 교체 스킬	
	case CsSkill::IT_CHANGE2:	 //디지몬 교체 스킬	
	case CsSkill::IT_CHANGE3:	 //디지몬 교체 스킬	
		RendeCursorSkillMask( nIconIndex, pos, nTacticsIndex );
		break;
	default:
		pIcon->Render( nIconIndex%1000, pos+CsPoint( 1, 1 ), CsPoint( 25, 25 ) );
		break;;
	}	

	// 마스크 랜더링
	m_pIconMask[ ICONMASK::IF_CURSOR_ICON ]->Render( pos );

	// 사이즈 원상 복귀
	pIcon->GetSprite()->SetSize( IF_ICONSIZE );
}

void cIconMng::RenderCursorIconCharImage( sCHAR_IMAGE* pCharImage, CsPoint pos )
{
	pCharImage->Render_S( pos + CsPoint( 1, 1 ), CsPoint( 25, 25 ) );
	// 마스크 랜더링
	m_pIconMask[ ICONMASK::IF_CURSOR_ICON ]->Render( pos );
}

//===================================================================================================
//
//		넘버
//
//===================================================================================================
void cIconMng::RenderNumber( int eAllien, NUMBERSPRITE::eCOLOR color, int nNum, CsPoint pos, CsPoint size, bool bRenderPersent )
{
	assert_cs( color < NUMBERSPRITE::MAX_COLOR );

	int nPow[ 5 ] = { 1, 10, 100, 1000, 10000 };
	cSprite* pNum = m_pNumber[ color ];

	// 자릿수	
	int nDeltaX = (int)( size.x / 1.7f );
	int nNumLevel = 0;
	if( nNum < 10 )				nNumLevel = 1;
	else if( nNum < 100 )		nNumLevel = 2;
	else if( nNum < 1000 )		nNumLevel = 3;
	else if( nNum < 10000 )		nNumLevel = 4;
	else if( nNum < 100000 )	nNumLevel = 5;

	switch( eAllien )
	{
	case DT_LEFT:											break;
	case DT_CENTER:		pos.x -= nNumLevel*nDeltaX/2;		break;		
	case DT_RIGHT:		pos.x -= nNumLevel*nDeltaX;			break;
	default:			assert_cs( false );
	}	

	while( nNumLevel != 0 )
	{
		int nOneNumber = nNum / nPow[ nNumLevel - 1 ];
		if( nOneNumber < 6  )
		{
			pNum->Render( pos, size, NiPoint2( nOneNumber/6.0f, 0 ), NiPoint2( ( nOneNumber + 1 )/6.0f, 0.5f ) );
		}
		else
		{
			pNum->Render( pos, size, NiPoint2( (nOneNumber-6)/6.0f, 0.5f ), NiPoint2( ( nOneNumber-5 )/6.0f, 1 ) );
		}
		nNum -= ( nOneNumber * nPow[ nNumLevel - 1 ] );
		--nNumLevel;
		pos.x += nDeltaX;
	}

	if( bRenderPersent == true )
	{
		pos.x += (int)( nDeltaX * 0.4f );
		pos.y += (int)( size.y*( 1.0f - 0.7f ) );
		pNum->Render( pos, size*0.7f, NiPoint2( 4/6.0f, 0.5f ), NiPoint2( 5/6.0f, 1 ) );
	}
}

void cIconMng::RenderNumber(int eAllien, NUMBERSPRITE::eCOLOR color, int nNum, CsPoint pos, CsPoint size, float fRatio, bool bRenderPersent)
{
	assert_cs( color < NUMBERSPRITE::MAX_COLOR );

	int nPow[ 5 ] = { 1, 10, 100, 1000, 10000 };
	cSprite* pNum = m_pNumber[ color ];

	// 자릿수	
	int nDeltaX = (int)( size.x / fRatio );
	int nNumLevel = 0;
	if( nNum < 10 )				nNumLevel = 1;
	else if( nNum < 100 )		nNumLevel = 2;
	else if( nNum < 1000 )		nNumLevel = 3;
	else if( nNum < 10000 )		nNumLevel = 4;
	else if( nNum < 100000 )	nNumLevel = 5;

	switch( eAllien )
	{
	case DT_LEFT:											break;
	case DT_CENTER:		pos.x -= nNumLevel*nDeltaX/2;		break;		
	case DT_RIGHT:		pos.x -= nNumLevel*nDeltaX;			break;
	default:			assert_cs( false );
	}	

	while( nNumLevel != 0 )
	{
		int nOneNumber = nNum / nPow[ nNumLevel - 1 ];
		if( nOneNumber < 6  )
		{
			pNum->Render( pos, size, NiPoint2( nOneNumber/6.0f, 0 ), NiPoint2( ( nOneNumber + 1 )/6.0f, 0.5f ) );
		}
		else
		{
			pNum->Render( pos, size, NiPoint2( (nOneNumber-6)/6.0f, 0.5f ), NiPoint2( ( nOneNumber-5 )/6.0f, 1 ) );
		}
		nNum -= ( nOneNumber * nPow[ nNumLevel - 1 ] );
		--nNumLevel;
		pos.x += nDeltaX;
	}

	if( bRenderPersent == true )
	{
		pos.x += (int)( nDeltaX * 0.4f );
		pos.y += (int)( size.y*( 1.0f - 0.7f ) );
		pNum->Render( pos, size*0.7f, NiPoint2( 4/6.0f, 0.5f ), NiPoint2( 5/6.0f, 1 ) );
	}
}

//===================================================================================================
//
//		쿨 타임
//
//===================================================================================================

void cIconMng::RenderTime( CsPoint pos, float fHalfLength, CsItem::sINFO* pFTItemInfo )
{
	RenderTime( pos, fHalfLength, pFTItemInfo->GetCoolTimeSeq() );
}

void cIconMng::RenderTime( CsPoint pos, float fHalfLength, CsCoolTimeSeq* pSeq )
{
	if( pSeq == NULL )
		return;

	if( pSeq->IsCoolTime() == false )
		return;

	if( pSeq->GetRemainTime() > pSeq->GetEndTime() )
		pSeq->Start();

	RenderTime( pos, fHalfLength, pSeq->GetRemainTime(), pSeq->GetEndTime() );
}

void cIconMng::RenderTime( CsPoint pos, float fHalfLength, double fRemainTime, double fEndTime )
{
	assert_cs( fRemainTime >= 0.0f );
	assert_cs( fEndTime != 0.0f );
	float fRate = (float)( fRemainTime / fEndTime );

	int count = 0;	
	float f125 = fHalfLength / 0.125f;
	float fLine;

	NiPoint2 a( fHalfLength, fHalfLength );
	NiPoint2 b, c;
	float fLength = 2*fHalfLength;

	while(1)
	{		
		if( fRate > 0.125f)		fLine = fHalfLength;
		else					fLine = fRate * f125;

		switch( count )
		{
		case 0:
			b = NiPoint2( fHalfLength			, 0 );
			c = NiPoint2( fHalfLength - fLine	, 0 );						
			break;
		case 1:
			b = NiPoint2( 0						, 0 );
			c = NiPoint2( 0						, fLine );
			break;
		case 2:
			b = NiPoint2( 0						, fHalfLength );
			c = NiPoint2( 0						, fHalfLength + fLine );
			break;
		case 3:
			b = NiPoint2( 0						, fLength );
			c = NiPoint2( fLine					, fLength );
			break;
		case 4:
			b = NiPoint2( fHalfLength			, fLength );
			c = NiPoint2( fHalfLength + fLine	, fLength );
			break;
		case 5:
			b = NiPoint2( fLength				, fLength );
			c = NiPoint2( fLength				, fLength - fLine );
			break;
		case 6:
			b = NiPoint2( fLength				, fHalfLength );
			c = NiPoint2( fLength				, fHalfLength - fLine );
			break;
		case 7:			
			b = NiPoint2( fLength				, 0 );
			c = NiPoint2( fLength - fLine		, 0 );
			break;
		}
		m_pCoolDown->RenderTriangle( pos.x, pos.y, a, b, c );

		if( ( fRate -= 0.125f ) <= 0.0f )
			break;

		++count;
	}
}

void cIconMng::RenderTimeInverse( CsPoint pos, float fHalfLength, float fRemainTime, float fEndTime )
{
	if(fRemainTime < 0.f)
	{
		return;
	}

	if( fEndTime <= 0.f )
	{
		return;
	}

	assert_cs( fRemainTime >= 0.0f );
	assert_cs( fEndTime != 0.0f );
	float fRate = (fEndTime - fRemainTime) / fEndTime;

	int count = 0;
	float f125 = fHalfLength / 0.125f;
	float fLine;

	NiPoint2 a( fHalfLength, fHalfLength );
	NiPoint2 b, c;
	float fLength = 2*fHalfLength;

	while(1)
	{		
		if( fRate > 0.125f)		fLine = fHalfLength;
		else					fLine = fRate * f125;

		switch( count )
		{
		case 0:
			b = NiPoint2( fHalfLength + fLine	, 0 );
			c = NiPoint2( fHalfLength			, 0 );			
			break;
		case 1:
			b = NiPoint2( fLength				, fLine );
			c = NiPoint2( fLength				, 0 );
			break;
		case 2:
			b = NiPoint2( fLength				, fHalfLength + fLine );
			c = NiPoint2( fLength				, fHalfLength );
			break;
		case 3:
			b = NiPoint2( fLength - fLine		, fLength );
			c = NiPoint2( fLength				, fLength );
			break;
		case 4:
			b = NiPoint2( fHalfLength - fLine	, fLength );
			c = NiPoint2( fHalfLength			, fLength );			
			break;
		case 5:
			b = NiPoint2( 0						, fLength - fLine );
			c = NiPoint2( 0						, fLength );			
			break;
		case 6:
			b = NiPoint2( 0						, fHalfLength - fLine );
			c = NiPoint2( 0						, fHalfLength );			
			break;
		case 7:
			b = NiPoint2( fLine					, 0 );
			c = NiPoint2( 0						, 0 );			
			break;
		}
		m_pCoolDown->RenderTriangle( pos.x, pos.y, a, b, c );

		if( ( fRate -= 0.125f ) <= 0.0f )
			break;

		++count;
	}
}

void cIconMng::DefaultTimeAlpha()
{
	m_pCoolDown->SetAlpha( TIME_ALPHA );
}

void cIconMng::RenderMaskGlitter( ICONMASK::eMASK_TYPE mt, CsPoint pos, CsPoint size, float fAlpha )
{
	m_pIconMask[ mt ]->SetAlpha( fAlpha );
	m_pIconMask[ mt ]->Render( pos, size );
}
