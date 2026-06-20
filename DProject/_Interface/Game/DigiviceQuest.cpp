
#include "stdafx.h"
#include "DigiviceQuest.h"

#define IF_DQ_START_POS		CsPoint( g_nScreenWidth/2-96, g_nScreenHeight/2-286 )

#define IF_DQ_END_POS		CsPoint( g_nScreenWidth - 286, g_nScreenHeight - 122 )

#define IF_DQ_CREATE_TIME	2.0f
#define IF_DQ_MOVING_TIME	0.4f

cDigiviceQuest::cDigiviceQuest():m_pQuestBtn(NULL),m_pAniSprite(NULL)
{
}

void cDigiviceQuest::Destroy()
{
	cBaseWindow::Delete();
}

void cDigiviceQuest::DeleteResource()
{
	DeleteScript();
	//SAFE_NIDELETE( m_pQuestBtn );
	//SAFE_NIDELETE( m_pAniSprite );
}

void cDigiviceQuest::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( NULL, CsPoint( 784, 690 ), CsPoint( 41, 41 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );

	m_pQuestBtn = NiNew cButton;
	m_pQuestBtn->Init( m_pRoot, CsPoint::ZERO, CsPoint( 41, 41 ), "DigiviceQuest\\Quest.tga", false );
	m_pQuestBtn->SetTexToken( CsPoint( 41, 0 ) );
	m_pQuestBtn->SetButtonEffect( cSpriteAni::LOOP, CsPoint::ZERO, CsPoint( 41, 41 ), "DigiviceQuest\\Quest.tga", NULL, 12, false, CsPoint(41,0) , cSpriteAni::SPRITE_POS );	
	m_pQuestBtn->SetEffectAniTime( 0.1f);
	m_pQuestBtn->SetEffectOn( cButton::eSTATE::NORMAL , true );
	m_pQuestBtn->SetVisible(false);
	m_pQuestBtn->SetControlName( "DigiviceBtn" );
	AddChildControl( m_pQuestBtn );

	m_pAniSprite = NiNew cSpriteAni;
	m_pAniSprite->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( 178, 128 ), "DigiviceQuest\\Ani\\Di_Quest_", "tga", 4, false );
	m_pAniSprite->SetAniTime( 0.1f );
	AddChildControl( m_pAniSprite );

	RestartSpriteAni();	
	if( _IhaveCompleteQuest() )
		OpenDigiviceTalk();
}

void cDigiviceQuest::RestartSpriteAni()
{
	cWindow::PlaySound( "System\\Digivice_Quest.wav" );

	m_ptSpritePos = IF_DQ_START_POS;
	m_eSpriteState = SS_CREATE;
	m_fSpriteTime = IF_DQ_CREATE_TIME;
	if( m_pAniSprite )
	{
		m_pAniSprite->SetPos( m_ptSpritePos );
		m_pAniSprite->SetVisible( true );
	}
	if( m_pQuestBtn )
		m_pQuestBtn->SetVisible(false);
}

void cDigiviceQuest::Update(float const& fDeltaTime)
{
	switch( m_eSpriteState )
	{
	case SS_CREATE:
		{
			if( (m_fSpriteTime-=fDeltaTime) < 0.0f )
			{
				m_eSpriteState = SS_MOVING;
				m_fSpriteTime = 0.0f;
			}
			if( m_pAniSprite )
				m_pAniSprite->Update( fDeltaTime );
		}
		break;
	case SS_MOVING:
		{
			m_ptSpritePos += ( IF_DQ_END_POS - IF_DQ_START_POS )*(fDeltaTime/IF_DQ_MOVING_TIME);
			if( (m_fSpriteTime+=fDeltaTime) > IF_DQ_MOVING_TIME )
			{
				m_eSpriteState = SS_STOP;
				m_ptSpritePos = IF_DQ_END_POS;
				if( m_pAniSprite )
					m_pAniSprite->SetVisible(false);
				if( m_pQuestBtn )
					m_pQuestBtn->SetVisible(true);
			}
			else
			{
				if( m_pAniSprite )
				{
					m_pAniSprite->SetPos( m_ptSpritePos );
					m_pAniSprite->Update( fDeltaTime );
				}
			}
		}
		break;
	case SS_STOP:
		break;
	default:
		assert_cs( false );
	}

	if( m_pQuestBtn )
		m_pQuestBtn->Update(fDeltaTime);
}

cBaseWindow::eMU_TYPE cDigiviceQuest::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
	{
		m_pQuestBtn->Update_ForMouse();	//2016-02-15-nova
		return muReturn;
	}

	switch( m_eSpriteState )
	{
	case SS_CREATE:
	case SS_MOVING:
		return MUT_OUT_WINDOW;
	case SS_STOP:
		break;
	default:
		assert_cs( false );
	}

	if( m_pQuestBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		OpenDigiviceTalk();
	
	return muReturn;
}

bool cDigiviceQuest::_IhaveCompleteQuest()
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_QuestOwner* pQuestOwner = g_pDataMng->GetQuestOwner();
	SAFE_POINTER_RETVAL( pQuestOwner, false );
	cData_QuestOwner::sDigiviceOwner* pDigiviceOwner = pQuestOwner->GetProcDigiviceOwner();
	SAFE_POINTER_RETVAL( pDigiviceOwner, false );

	// 보상받을 퀘스트
	std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >* pQuestMap = pDigiviceOwner->GetMap();
	SAFE_POINTER_RETVAL( pQuestMap, false );	
	std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator it = pQuestMap->begin();
	for( ; it!=pQuestMap->end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		SAFE_POINTER_CON( it->second->s_pProcess );
		if( it->second->s_pProcess->s_bCompleate )
			return true;
	}

	return false;
}

bool cDigiviceQuest::OpenDigiviceTalk()
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TALK ) )
		return false;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_REV ) )
		return false;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_COMP ) )
		return false;

	cTalk* pTalk = dynamic_cast<cTalk*>(g_pGameIF->GetDynamicIF( cBaseWindow::WT_TALK ));
	SAFE_POINTER_RETVAL( pTalk, false );

	// 토크 중복 생성 되지 않게
	if( pTalk->GetTarget() != g_pNpcMng->GetDigivice() )
		pTalk->SetTarget( g_pNpcMng->GetDigivice() );
	return true;
}

void cDigiviceQuest::Render()
{
	RenderScript();
}

void cDigiviceQuest::ResetDevice()
{	
	ResetDeviceScript();
}
