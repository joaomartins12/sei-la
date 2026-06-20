#include "stdafx.h"
#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"
#include "TutorialUITraceData.h"

bool cTraceBase::IsStop()
{
	return m_bStop;
}

void cTraceBase::SetStop()
{
	if( m_bStop )
		return;

	if( m_bCompleteStop )
		m_bStop = true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool cUITrace::LoadScript( TiXmlElement* pEl )
{
	const char* pTargetWindow = pEl->Attribute("Win");
	if( pTargetWindow )
		m_sWindowName = pTargetWindow;

	const char* pTargetControl = pEl->Attribute("Control");
	if( pTargetControl )
		m_sControlName = pTargetControl;

	const char* pType = pEl->Attribute("TargetType");
	if( pType )
	{
		if( 0 == ::stricmp( pType, "NPC" ) )			m_TargetType = eNpc;
		else if( 0 == ::stricmp( pType, "Monster" ) )	m_TargetType = eMonster;
		else if( 0 == ::stricmp( pType, "Digivice" ) )	m_TargetType = eDigivice;
		else if( 0 == ::stricmp( pType, "Item" ) )		m_TargetType = eItem;
		else											m_TargetType = eNone;
	}
	else
		m_TargetType = eNone;

	double targetID = 0;
	pEl->Attribute("TargetID", &targetID);
	m_dwTargetIDX = targetID;
	return true;
}

bool cUITrace::CheckIndicator(DWORD const& dwQuestID)
{
	SAFE_POINTER_RETVAL( g_pGameIF, false );

	if( m_sWindowName.empty() )
		return true;

	if( IsStop() )
		return true;

	cBaseWindow* pBse = g_pGameIF->GetWindow( m_sWindowName.c_str() );
	SAFE_POINTER_RETVAL( pBse, false );

	switch( pBse->GetWindowType() )
	{
	case cBaseWindow::WT_TALK:
		{
			cTalk* pTalkWIndow = dynamic_cast<cTalk*>( pBse );
			SAFE_POINTER_RETVAL( pTalkWIndow, false );
			CsC_AvObject* pTarget = pTalkWIndow->GetTarget();
			SAFE_POINTER_RETVAL( pTarget, false );

			switch( m_TargetType )
			{
			case eDigivice:
				{
					SAFE_POINTER_RETVAL( g_pNpcMng, false );
					CNpc* pDigiviceNpc = g_pNpcMng->GetDigivice();
					if( pDigiviceNpc != pTarget )
						return false;
				}
				break;
			default:
				if( pTarget->GetFTID() != m_dwTargetIDX )
					return false;
				break;
			}		
		}
		break;
	case cBaseWindow::WT_QUEST_REV:
		{
			cQuestRev* pTQeustRecv = dynamic_cast<cQuestRev*>( pBse );
			SAFE_POINTER_RETVAL( pTQeustRecv, false );
			CsC_AvObject* pTarget = pTQeustRecv->GetTarget();
			SAFE_POINTER_RETVAL( pTarget, false );

			switch( m_TargetType )
			{
			case eDigivice:
				{
					SAFE_POINTER_RETVAL( g_pNpcMng, false );
					CNpc* pDigiviceNpc = g_pNpcMng->GetDigivice();
					if( pDigiviceNpc != pTarget )
						return false;
				}
				break;
			default:
				if( pTarget->GetFTID() != m_dwTargetIDX )
					return false;
				break;
			}	

			if( static_cast<cQuestRev*>( pBse )->GetQuestUIDX() != dwQuestID )
				return false;
		}
		break;
	case cBaseWindow::WT_DIGITAMA_SCAN:
		{
			cDigitama_Scan* pWin = dynamic_cast<cDigitama_Scan*>( pBse );
			SAFE_POINTER_RETVAL( pWin, false );
			if( !pWin->IsShowWindow() )
				return false;

			if( !m_sControlName.empty() )
			{
				if( 0 == ::stricmp( m_sControlName.c_str(), "RegistIcon" ) )
				{
					cItemInfo* pItem = pWin->GetResistItem();
					SAFE_POINTER_RETVAL( pItem, false );
					if( !pItem->IsEnable() )
						return false;
				}
			}
		}
		break;
	case cBaseWindow::WT_ENCHANT_TACTICS:
		{
			cEnchantTactics* pWin = dynamic_cast<cEnchantTactics*>( pBse );
			SAFE_POINTER_RETVAL( pWin, false );
			if( !pWin->IsShowWindow() )
				return false;

			if( 0 == ::stricmp( m_sControlName.c_str(), "RegistDigiClone" ) )
			{
				cItemInfo* pItem = pWin->GetRegistDigiClone();
				SAFE_POINTER_RETVAL( pItem, false );
				if( !pItem->IsEnable() )
					return false;
			}
			else if( 0 == ::stricmp( m_sControlName.c_str(), "RegistSubItem" ) )
			{
				cItemInfo* pItem = pWin->GetRegistSubItem();
				SAFE_POINTER_RETVAL( pItem, false );
				if( !pItem->IsEnable() )
					return false;
			}
			else
			{
				cWindow* pControl = pWin->GetChildControl(m_sControlName);
				SAFE_POINTER_RETVAL( pControl, false );
			}
		}
		break;
	case cBaseWindow::WT_MAKE_TACTICS:
		{
			cMakeTactics* pWin = dynamic_cast<cMakeTactics*>( pBse );
			SAFE_POINTER_RETVAL( pWin, false );
			if( !pWin->IsShowWindow() )
				return false;

			if( !m_sControlName.empty() )
			{
				if( 0 == ::stricmp( m_sControlName.c_str(), "RegistIcon" ) )
				{
					cItemInfo* pItem = pWin->GetResistItem();
					SAFE_POINTER_RETVAL( pItem, false );
					if( !pItem->IsEnable() )
						return false;
				}
				else if( 0 == ::stricmp( m_sControlName.c_str(), "RequiteBtn" ) )
				{
					cWindow* pControl = pWin->GetChildControl("RequiteBtn");
					SAFE_POINTER_RETVAL( pControl, false );
					if( cWindow::T_Button == pControl->GetType() )
					{
						if( !static_cast<cButton*>(pControl)->IsEnable() )
							return false;
					}
					else
						return false;
				}
				else
				{
					cWindow* pControl = pWin->GetChildControl(m_sControlName);
					SAFE_POINTER_RETVAL( pControl, false );
				}
			}
		}
		break;
	case cBaseWindow::WT_DIGITAMA_SELL:
		{
			cDigitama_Sell* pWin = dynamic_cast<cDigitama_Sell*>( pBse );
			SAFE_POINTER_RETVAL( pWin, false );
			if( !pWin->IsShowWindow() )
				return false;

			if( !m_sControlName.empty() )
			{
				if( 0 == ::stricmp( m_sControlName.c_str(), "RegistIcon" ) )
				{
					cItemInfo* pItem = pWin->GetResistItem();
					SAFE_POINTER_RETVAL( pItem, false );
					if( !pItem->IsEnable() )
						return false;
				}
			}
		}
		break;
	case cBaseWindow::WT_ZONEMAP:
		{
			cZoneMap* pZoneMap = dynamic_cast<cZoneMap*>( pBse );
			SAFE_POINTER_RETVAL( pZoneMap, false );
			if( !pZoneMap->IsShowWindow() )
				return false;

			if( !m_sControlName.empty() )
			{
				cZoneMap::eRadio_Type pState = pZoneMap->GetCheckRadioIndex();

				if( 0 == ::stricmp( m_sControlName.c_str(), "Nomal" ) )
				{
					if( pState != cZoneMap::eType_Nor )
						return false;
				}
				else if( 0 == ::stricmp( m_sControlName.c_str(), "Npc" ) )
				{
					if( pState != cZoneMap::eType_Npc )
						return false;
				}
				else if( 0 == ::stricmp( m_sControlName.c_str(), "Monster" ) )
				{
					if( pState != cZoneMap::eType_Mon )
						return false;
				}
				else if( 0 == ::stricmp( m_sControlName.c_str(), "MyPosition" ) )
				{
					if( pState != cZoneMap::eType_Pos )
						return false;
				}
				else if( 0 == ::stricmp( m_sControlName.c_str(), "MyEmployment" ) )
				{
					if( pState != cZoneMap::eType_Employment )
						return false;
				}
				else
					return false;
			}
		}
		break;
	}

	SetStop();

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool cObjectTrace::LoadScript( TiXmlElement* pEl )
{
	double targetID = 0;
	pEl->Attribute("TargetID", &targetID);
	m_dwTargetIDX = targetID;

	const char* completeStop = pEl->Attribute("Stop");
	if( completeStop )
	{
		if( 0 == ::stricmp( completeStop, "true" ) )
			m_bCompleteStop = true;
	}


	const char* pType = pEl->Attribute("TargetType");
	if( pType )
	{
		if( 0 == ::stricmp( pType, "NPC" ) )			m_TargetType = eNpc;
		else if( 0 == ::stricmp( pType, "Monster" ) )	m_TargetType = eMonster;
		else if( 0 == ::stricmp( pType, "Digivice" ) )	m_TargetType = eDigivice;
		else if( 0 == ::stricmp( pType, "Item" ) )		m_TargetType = eItem;
		else											m_TargetType = eNone;
	}
	else
		m_TargetType = eNone;
	return true;
}

bool cObjectTrace::CheckIndicator(DWORD const& dwQeustID)
{
	SAFE_POINTER_RETVAL( g_pHelpArrow, false );

	if( IsStop() )
		return true;

	cHelpArrow::sPosFind* pTarget = g_pHelpArrow->GetInfo();
	SAFE_POINTER_RETVAL(pTarget, false);

	if( pTarget->s_nFileID != m_dwTargetIDX )
		return false;

	SetStop();
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


bool cGamePageTrace::LoadScript( TiXmlElement* pEl )
{	
	const char* pPageName = pEl->Attribute("PageName");
	SAFE_POINTER_RETVAL( pPageName, false );
	if( 0 == ::stricmp( pPageName, "DigimonEnchantPage" ) )
		m_nPageIdx = Flow::CFlow::FLW_DATSCENTER;
	else if( 0 == ::stricmp( pPageName, "DigimonDataTradePage" ) )
		m_nPageIdx = Flow::CFlow::FLW_DATATRADECENTER;
	else
		return false;
	return true;
}

bool cGamePageTrace::CheckIndicator(DWORD const& dwQeustID)
{
	if( IsStop() )
		return true;

	if( !FLOWMGR_ST.IsCurrentFlow( m_nPageIdx ) )
		return false;

	SetStop();
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


bool cMapTrace::LoadScript( TiXmlElement* pEl )
{	
	double targetID = 0;
	pEl->Attribute("TargetMapIDX", &targetID);
	m_dwTargetMapIDX = targetID;
	return true;
}

bool cMapTrace::CheckIndicator(DWORD const& dwQeustID)
{
	if( IsStop() )
		return true;

	SAFE_POINTER_RETVAL( nsCsGBTerrain::g_pCurRoot, false );
	CsGBTerrainRoot::sINFO* pMapInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	SAFE_POINTER_RETVAL( pMapInfo, false );

	if( pMapInfo->s_dwMapID != m_dwTargetMapIDX )
		return false;
	
	SetStop();
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool cObjectNeerCheck::LoadScript( TiXmlElement* pEl )
{
	double targetID = 0;
	pEl->Attribute("TargetID", &targetID);
	m_dwTargetIDX = targetID;

	const char* completeStop = pEl->Attribute("Stop");
	if( completeStop )
	{
		if( 0 == ::stricmp( completeStop, "true" ) )
			m_bCompleteStop = true;
	}

	const char* pBaseType = pEl->Attribute("BaseType");
	if (pBaseType)
	{
		if (0 == ::stricmp(pBaseType, "Tamer"))			m_BaseType = eTamer;
		else if (0 == ::stricmp(pBaseType, "Digimon"))	m_BaseType = eDigimon;
		else												m_BaseType = eTamer;
	}
	else
		m_BaseType = eTamer;

	const char* pType = pEl->Attribute("TargetType");
	if( pType )
	{
		if( 0 == ::stricmp( pType, "NPC" ) )			m_TargetType = eNpc;
		else if( 0 == ::stricmp( pType, "Monster" ) )	m_TargetType = eMonster;
		else											m_TargetType = eNone;
	}
	else
		m_TargetType = eNone;
	return true;
}

bool cObjectNeerCheck::CheckIndicator(DWORD const& dwQeustID)
{
	if( IsStop() )
		return true;

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTamerUser, false );

	switch( m_TargetType )
	{
	case eNpc:
		{
			SAFE_POINTER_RETVAL( g_pNpcMng, false );
			CNpc* pTargetNpc = g_pNpcMng->GetNpc( m_dwTargetIDX );
			SAFE_POINTER_RETVAL( pTargetNpc, false );

			NiPoint2 vTargetPos = pTargetNpc->GetPos2D();
			NiPoint2 vTamer = pTamerUser->GetPos2D();
			if( ( vTargetPos - vTamer ).Length() > 300.0f  )
				return false;
		}
		break;
	case eMonster:
		{
			CMonster* pTargetMob = g_pCharMng->FindNearMonster( m_dwTargetIDX );
			SAFE_POINTER_RETVAL( pTargetMob, false );
			NiPoint2 vTargetPos = pTargetMob->GetPos2D();
			NiPoint2 vBasePos;

			if (m_BaseType == eTamer)
				vBasePos = pTamerUser->GetPos2D();
			else if (m_BaseType == eDigimon)
			{
				CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
				if (pDigimonUser)
					vBasePos = pDigimonUser->GetPos2D();
			}

			if ((vTargetPos - vBasePos).Length() > 300.0f)
				return false;
		}
		break;
	}

	SetStop();
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool cMonsterTrace::LoadScript(TiXmlElement* pEl)
{
	double targetID = 0;
	pEl->Attribute( "TargetID", &targetID );
	m_dwMonsterID = targetID;

	int targetHp = 0;
	pEl->Attribute( "TargetHp", &targetHp );
	m_nMonsterHp = targetHp;

	return true;
}

bool cMonsterTrace::CheckIndicator(DWORD const& dwQuestID)
{
	if( IsStop() )
		return true;

	SAFE_POINTER_RETVAL( g_pCharMng, false );

	// Check monster die
	if( m_nMonsterHp == 0 )
	{
		std::map< DWORD, CMonster* >* mapMonster = g_pCharMng->GetMonsterVector();
		std::map< DWORD, CMonster* >::iterator it = mapMonster->begin();
		for( ; it != mapMonster->end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
			if( m_dwMonsterID == it->second->GetFTID() )
				return false;
		}
		SetStop();
		return true;
	}

	// Check targeting monster hp
	SAFE_POINTER_RETVAL( g_pCharResMng, false );
	CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
	SAFE_POINTER_RETVAL( pTarget, false );
	if( pTarget->GetLeafRTTI() != RTTI_TUTORIAL_MONSTER &&
		pTarget->GetLeafRTTI() != RTTI_MONSTER )
		return false;

	CsC_AvObject::sBASE_STAT* pStat = pTarget->GetBaseStat();
	SAFE_POINTER_RETVAL( pStat, false );

	float fHpRate = static_cast<float>(m_nMonsterHp) / 100.0f;	// 100ºÐÀ²
	if( pStat->GetHPRate() > fHpRate )
		return false;

	SetStop();
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool cWindowStateTrace::LoadScript(TiXmlElement* pEl)
{
	pEl->Attribute( "WinType", &m_nWindowType );
	pEl->Attribute( "WinState", &m_nCmpState );
	return true;
}

bool cWindowStateTrace::CheckIndicator(DWORD const& dwQuestID)
{
	if( IsStop() )
		return true;

	switch( m_nState )
	{
	case eWS_None:
		{
			cBaseWindow* pWin = g_pGameIF->GetDynamicIF( static_cast<cBaseWindow::eWINDOW_TYPE>(m_nWindowType) );
			if( pWin )
				m_nState = eWS_On;
		}
		break;
	case eWS_On:
		{
			if( !g_pGameIF->IsActiveWindow( static_cast<cBaseWindow::eWINDOW_TYPE>(m_nWindowType) ) )
				m_nState = eWS_Off;
		}
		break;
	case eWS_Off:
		break;
	}

	if( m_nState == m_nCmpState )
	{
		m_nState = eWS_None;
		SetStop();
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool cVideoStateTrace::LoadScript(TiXmlElement* pEl)
{
	const char* pFilePath = pEl->Attribute( "FilePath" );
	if( pFilePath )
		m_strFilePath = pFilePath;

	return true;
}

bool cVideoStateTrace::CheckIndicator(DWORD const& dwQuestID)
{
	if( IsStop() )
		return true;

	if( m_strFilePath.empty() )
		return true;

	switch( m_eState )
	{
	case eVS_None:
		{
			std::wstring wsFilePath;
			wsFilePath.assign( m_strFilePath.begin(), m_strFilePath.end() );

			SAFE_POINTER_RETVAL( g_pRenderer, false );

			g_pRenderer->SetBackgroundColor( NiColorA(16.0f / 255.0f, 16.0f / 255.0f, 16.0f / 255.0f, 1) );
			g_pRenderer->BeginFrame();
			g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
			g_pRenderer->EndUsingRenderTargetGroup();
			g_pRenderer->EndFrame();
			g_pRenderer->DisplayFrame();

			if( nsDIRECTSHOW::InitDShowWindow(const_cast<TCHAR*>( wsFilePath.c_str()), CsPoint::ZERO, CsPoint::ZERO, true ) == true )
				nsDIRECTSHOW::WaitForKey_ForWindow_AniKey();
			g_pRenderer->SetBackgroundColor( NiColor::BLACK );

			m_eState = eVS_Off;
			SetStop();
		}
		return true;
	case eVS_Off:
		SetStop();
		return true;
	}

	return false;
}
