#include "stdafx.h"
#include "TutorialIndicator.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
cTraceCondition::cTraceCondition(void): m_pChildTrace(NULL),m_pTraceKind(NULL)
{

}
cTraceCondition::~cTraceCondition(void)
{
	SAFE_NIDELETE( m_pChildTrace );
	SAFE_NIDELETE( m_pTraceKind );
}


void cTraceCondition::SetTooltipImgFile(const char* pFIle)
{
	m_TraceData.m_TooltipImgFile = pFIle;
}

void cTraceCondition::SetUITraceData( sTraceData const& pData )
{
	m_TraceData.m_dwTargetValue = pData.m_dwTargetValue;
	m_TraceData.m_dwTargetValueSub = pData.m_dwTargetValueSub;
	m_TraceData.m_nType = pData.m_nType;
	m_TraceData.m_wsTargetName = pData.m_wsTargetName;
	m_TraceData.m_wsTargetWindowName = pData.m_wsTargetWindowName;
}

void cTraceCondition::AddEffectData( sObjectEffectData const& sData )
{
	m_TraceData.m_EffectData.push_back( sData );
}

void cTraceCondition::SetPortalData(DWORD dwPortalID, bool bPortalEnable)
{
	m_TraceData.m_dwPortalID = dwPortalID;
	m_TraceData.m_bPortalEnable = bPortalEnable;
}

void cTraceCondition::SetTalkData(int const& nTalkTarget, std::wstring const& wsTalkMsg)
{
	m_TraceData.m_nTalkTarget = nTalkTarget;
	m_TraceData.m_wsTalkMsg = wsTalkMsg;
}

void cTraceCondition::SetVideoData(std::string const& strFile)
{
	m_TraceData.m_VideoFile = strFile;
}

void cTraceCondition::SetNextIndicatorData(DWORD dwNextQuestID, int const& nNextStep, int const& nNextTraceIdx)
{
	m_TraceData.m_dwNextQuestID = dwNextQuestID;
	m_TraceData.m_nNextStep = nNextStep;
	m_TraceData.m_nNextTraceIdx = nNextTraceIdx;
}

void cTraceCondition::AddMonsterData(sMonsterData const& cData)
{
	m_TraceData.m_MonsterData.push_back( cData );
}

bool cTraceCondition::LoadScript( TiXmlElement* pEl )
{
	SAFE_POINTER_RETVAL( pEl, false );

	const char* pKind = pEl->Attribute("Type");
	if( NULL == pKind )
		return false;
	
	if( 0 == ::stricmp( pKind, "UI" ) )						m_pTraceKind = NiNew cUITrace;
	else if( 0 == ::stricmp( pKind, "ObjectTrace" ) )		m_pTraceKind = NiNew cObjectTrace;
	else if( 0 == ::stricmp( pKind, "GamePage" ) )			m_pTraceKind = NiNew cGamePageTrace;
	else if( 0 == ::stricmp( pKind, "Map" ) )				m_pTraceKind = NiNew cMapTrace;
	else if( 0 == ::stricmp( pKind, "ObjectNearCheck" ) )	m_pTraceKind = NiNew cObjectNeerCheck;
	else if( 0 == ::stricmp( pKind, "WindowState" ) )		m_pTraceKind = NiNew cWindowStateTrace;
	//else if( 0 == ::stricmp( pKind, "VideoState" ) )		m_pTraceKind = NiNew cVideoStateTrace;
	else if( 0 == ::stricmp( pKind, "Monster" ) )			m_pTraceKind = NiNew cMonsterTrace;
	else												return false;

	if( !m_pTraceKind->LoadScript( pEl ) )
		return false;	

	TiXmlNode* pChildNode = pEl->FirstChild();
	while( pChildNode )
	{
		TiXmlElement* pChildEl = pChildNode->ToElement();
		if( pChildEl )
		{
			if( 0 == ::stricmp(pChildEl->Value(), "Tooltip" ) )
			{
				const char* pFile = pChildEl->Attribute( "img" );
				if( pFile )
					SetTooltipImgFile( pFile );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Effect" ) )
			{
				const char* pTargetType = pChildEl->Attribute( "Type" );				
				SAFE_POINTER_BEK(pTargetType);

				const char* pViewType = pChildEl->Attribute( "View" );
				SAFE_POINTER_BEK(pViewType);

				double duIdx = 0;
				pChildEl->Attribute("TargetID",&duIdx);
				if( 0 == duIdx )
					break;

				sObjectEffectData addEffect;
				addEffect.dwTargetIDX = duIdx;
				if( DmCS::StringFn::iCmp( pTargetType, "Npc" ) )
					addEffect.cTargetType = sObjectEffectData::eNpc;
				else if( DmCS::StringFn::iCmp( pTargetType, "Monster" ) )
					addEffect.cTargetType = sObjectEffectData::eMonster;

				if( DmCS::StringFn::iCmp( pViewType, "Show" ) )
					addEffect.bShow = true;
				else if( DmCS::StringFn::iCmp( pTargetType, "Hide" ) )
					addEffect.cTargetType = false;

				AddEffectData( addEffect );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "TraceUI" ) )
			{				
				const char* pName = pChildEl->Attribute( "Win" );
				SAFE_POINTER_BEK(pName);

				sTraceData pNewData;
				pNewData.m_wsTargetWindowName = pName;

				const char* pType = pChildEl->Attribute( "Type" );
				if( pType )
				{
					if( 0 == ::stricmp(pType, "Control") )
						pNewData.m_nType = sTraceData::eControl;
					else if( 0 == ::stricmp(pType, "Quest") )
						pNewData.m_nType = sTraceData::eQuest;
					else if( 0 == ::stricmp(pType, "Item") )
						pNewData.m_nType = sTraceData::eItem;
					else if( 0 == ::stricmp(pType, "Window") )
						pNewData.m_nType = sTraceData::eWindow;
					else if( DmCS::StringFn::iCmp( pType, "QuickSlot" ) )
						pNewData.m_nType = sTraceData::eQuickSlot;
				}

				const char* pControlName = pChildEl->Attribute( "Control" );
				if( pControlName )
					pNewData.m_wsTargetName = pControlName;

				double pTargetValue= 0;
				pChildEl->Attribute( "TargetValue", &pTargetValue );
				pNewData.m_dwTargetValue = pTargetValue;

				double pTargetValueSub= 0;
				pChildEl->Attribute( "TargetValueSub", &pTargetValueSub );
				pNewData.m_dwTargetValueSub = pTargetValueSub;

				SetUITraceData( pNewData );				
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Condition" ) )
			{
				cTraceCondition* pCondition = NiNew cTraceCondition;
				if( pCondition->LoadScript( pChildEl ) )
					m_pChildTrace = pCondition;
				else
					SAFE_NIDELETE( pCondition );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Portal" ) )
			{
				int nPortalID = 0;
				pChildEl->Attribute( "PortalID", &nPortalID );
				int nPortalEnable = 0;
				pChildEl->Attribute( "PortalEnable", &nPortalEnable );
				SetPortalData( nPortalID, nPortalEnable );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Monster" ) )
			{
				int nID = 0;
				pChildEl->Attribute( "ID", &nID );
				int nPosX = 0;
				pChildEl->Attribute( "PosX", &nPosX );
				int nPosY = 0;
				pChildEl->Attribute( "PosY", &nPosY );

				sMonsterData sData;
				sData.dwMonsterID = nID;
				sData.ptMonsterPos = CsPoint( nPosX, nPosY );
				AddMonsterData( sData );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Talk" ) )
			{
				int nType = 0;
				pChildEl->Attribute( "TargetType", &nType );
				const char* pTargetTalk = pChildEl->Attribute( "TargetTalk" );
				if( pTargetTalk )
				{
					std::wstring wsTalk;
					DmCS::StringFn::FromUTF8( wsTalk, pTargetTalk );
					SetTalkData( nType, wsTalk );
				}
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Video" ) )
			{
				const char* pFilePath = pChildEl->Attribute( "FilePath" );
				if( pFilePath )
				{
					std::string strFile = pFilePath;
					SetVideoData( strFile );
				}
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "NextIndicator" ) )
			{
				int nNextQuestID = 0;
				pChildEl->Attribute( "NextQuestID", &nNextQuestID );
				int nNextStep = 0;
				pChildEl->Attribute( "NextStep", &nNextStep );
				int nNextTraceIdx = 0;
				pChildEl->Attribute( "NextTraceIdx", &nNextTraceIdx );
				SetNextIndicatorData( nNextQuestID, nNextStep, nNextTraceIdx );
			}
		}
		pChildNode = pChildNode->NextSibling();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

cTrace::cTrace(void) : m_pChildTrace(NULL),m_nConditionIdx(0), m_nComplete(false)
{

}

cTrace::~cTrace(void)
{
	SAFE_NIDELETE( m_pChildTrace );
}


void cTrace::SetTooltipImgFile(const char* pFIle)
{
	m_TraceData.m_TooltipImgFile = pFIle;
}

void cTrace::AddEffectData( sObjectEffectData const& sData )
{
	m_TraceData.m_EffectData.push_back( sData );
}

void cTrace::SetUITraceData( sTraceData const& pData )
{
	m_TraceData.m_dwTargetValue = pData.m_dwTargetValue;
	m_TraceData.m_dwTargetValueSub = pData.m_dwTargetValueSub;
	m_TraceData.m_nType = pData.m_nType;
	m_TraceData.m_wsTargetName = pData.m_wsTargetName;
	m_TraceData.m_wsTargetWindowName = pData.m_wsTargetWindowName;
}

void cTrace::SetPortalData(DWORD dwPortalID, bool bPortalEnable)
{
	m_TraceData.m_dwPortalID = dwPortalID;
	m_TraceData.m_bPortalEnable = bPortalEnable;
}

void cTrace::SetTalkData(int const& nTalkTarget, std::wstring const& wsTalkMsg)
{
	m_TraceData.m_nTalkTarget = nTalkTarget;
	m_TraceData.m_wsTalkMsg = wsTalkMsg;
}

void cTrace::SetVideoData(std::string const& strFile)
{
	m_TraceData.m_VideoFile = strFile;
}

void cTrace::SetNextIndicatorData(DWORD dwNextQuestID, int const& nNextStep, int const& nNextTraceIdx)
{
	m_TraceData.m_dwNextQuestID = dwNextQuestID;
	m_TraceData.m_nNextStep = nNextStep;
	m_TraceData.m_nNextTraceIdx = nNextTraceIdx;
}

void cTrace::AddMonsterData(sMonsterData const& cData)
{
	m_TraceData.m_MonsterData.push_back( cData );
}

bool cTrace::LoadScript( TiXmlElement* pEl )
{
	SAFE_POINTER_RETVAL( pEl, false );

	pEl->Attribute("idx",&m_nConditionIdx);
	
	TiXmlNode* pChildNode = pEl->FirstChild();
	while( pChildNode )
	{
		TiXmlElement* pChildEl = pChildNode->ToElement();
		if( pChildEl )
		{
			if( 0 == ::stricmp(pChildEl->Value(), "Tooltip" ) )
			{
				const char* pFile = pChildEl->Attribute( "img" );
				if( pFile )
					SetTooltipImgFile( pFile );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Effect" ) )
			{
				const char* pTargetType = pChildEl->Attribute( "Type" );				
				SAFE_POINTER_BEK(pTargetType);

				const char* pViewType = pChildEl->Attribute( "View" );
				SAFE_POINTER_BEK(pViewType);

				double duIdx = 0;
				pChildEl->Attribute("TargetID",&duIdx);
				if( 0 == duIdx )
					break;

				sObjectEffectData addEffect;
				addEffect.dwTargetIDX = duIdx;
				if( DmCS::StringFn::iCmp( pTargetType, "Npc" ) )
					addEffect.cTargetType = sObjectEffectData::eNpc;
				else if( DmCS::StringFn::iCmp( pTargetType, "Monster" ) )
					addEffect.cTargetType = sObjectEffectData::eMonster;

				if( DmCS::StringFn::iCmp( pViewType, "Show" ) )
					addEffect.bShow = true;
				else if( DmCS::StringFn::iCmp( pTargetType, "Hide" ) )
					addEffect.cTargetType = false;

				AddEffectData( addEffect );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "TraceUI" ) )
			{
				const char* pName = pChildEl->Attribute( "Win" );
				SAFE_POINTER_BEK(pName);

				sTraceData pNewData;
				pNewData.m_wsTargetWindowName = pName;

				const char* pType = pChildEl->Attribute( "Type" );
				if( pType )
				{
					if( DmCS::StringFn::iCmp( pType, "Control" ) )
						pNewData.m_nType = sTraceData::eControl;
					else if( DmCS::StringFn::iCmp( pType, "Quest" ) )
						pNewData.m_nType = sTraceData::eQuest;
					else if( DmCS::StringFn::iCmp( pType, "Item" ) )
						pNewData.m_nType = sTraceData::eItem;
					else if( DmCS::StringFn::iCmp( pType, "Window" ) )
						pNewData.m_nType = sTraceData::eWindow;
					else if( DmCS::StringFn::iCmp( pType, "QuickSlot" ) )
						pNewData.m_nType = sTraceData::eQuickSlot;
				}

				const char* pControlName = pChildEl->Attribute( "Control" );
				if( pControlName )
					pNewData.m_wsTargetName = pControlName;

				double pTargetValue= 0;
				pChildEl->Attribute( "TargetValue", &pTargetValue );
				pNewData.m_dwTargetValue = pTargetValue;

				double pTargetValueSub= 0;
				pChildEl->Attribute( "TargetValueSub", &pTargetValueSub );
				pNewData.m_dwTargetValueSub = pTargetValueSub;

				SetUITraceData( pNewData );	
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Condition" ) )
			{
				cTraceCondition* pCondition = NiNew cTraceCondition;
				if( pCondition->LoadScript( pChildEl ) )
					m_pChildTrace = pCondition;
				else
					SAFE_NIDELETE( pCondition );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Portal" ) )
			{
				int nPortalID = 0;
				pChildEl->Attribute( "PortalID", &nPortalID );
				int nPortalEnable = 0;
				pChildEl->Attribute( "PortalEnable", &nPortalEnable );
				SetPortalData( nPortalID, nPortalEnable );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Monster" ) )
			{
				int nID = 0;
				pChildEl->Attribute( "ID", &nID );
				int nPosX = 0;
				pChildEl->Attribute( "PosX", &nPosX );
				int nPosY = 0;
				pChildEl->Attribute( "PosY", &nPosY );

				sMonsterData sData;
				sData.dwMonsterID = nID;
				sData.ptMonsterPos = CsPoint( nPosX, nPosY );
				AddMonsterData( sData );
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Talk" ) )
			{
				int nType = 0;
				pChildEl->Attribute( "TargetType", &nType );
				const char* pTargetTalk = pChildEl->Attribute( "TargetTalk" );
				if( pTargetTalk )
				{
					std::wstring wsTalk;
					DmCS::StringFn::FromUTF8( wsTalk, pTargetTalk );
					SetTalkData( nType, wsTalk );
				}
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "Video" ) )
			{
				const char* pFilePath = pChildEl->Attribute( "FilePath" );
				if( pFilePath )
				{
					std::string strFile = pFilePath;
					SetVideoData( strFile );
				}
			}
			else if( 0 == ::stricmp(pChildEl->Value(), "NextIndicator" ) )
			{
				int nNextQuestID = 0;
				pChildEl->Attribute( "NextQuestID", &nNextQuestID );
				int nNextStep = 0;
				pChildEl->Attribute( "NextStep", &nNextStep );
				int nNextTraceIdx = 0;
				pChildEl->Attribute( "NextTraceIdx", &nNextTraceIdx );
				SetNextIndicatorData( nNextQuestID, nNextStep, nNextTraceIdx );
			}
		}
		pChildNode = pChildNode->NextSibling();
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

cIndicatorInfos::cIndicatorInfos(void):m_pCurrentTrace(NULL),m_dwQuestID(0),m_nCurrentStep(0),m_nCurrentConditionIdx(-1)
{

}

cIndicatorInfos::~cIndicatorInfos(void)
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	_StopIndicator();
	
	MAP_INDICATOR_INFO_IT it = m_Indicators.begin();	
	for( ; it != m_Indicators.end(); ++it )
	{
		MAP_QUEST_INDICATOR_IT subIT = it->second.begin();
		for( ; subIT != it->second.end(); ++subIT )
		{
			MAP_CONDITIONS_IT secIT = subIT->second.begin();
			for( ; secIT != subIT->second.end(); ++secIT )
				SAFE_NIDELETE( secIT->second );

			subIT->second.clear();
		}
		it->second.clear();
	}
	m_Indicators.clear();
}

void cIndicatorInfos::LoadTutorialScript()
{
	TiXmlDocument TiDoc;
	if( CsFPS::CsFPSystem::IsExistOnlyPack( 0, "Data\\Interface\\Tutorial\\TutorialIndicator.xml" ) )
	{
		char* buffer = NULL;
		size_t size = CsFPS::CsFPSystem::Allocate_GetFileData( 0, &buffer, "Data\\Interface\\Tutorial\\TutorialIndicator.xml" );
		if( 0 == size )
			return;

		TiDoc.Parse( buffer );
		if( TiDoc.Error() )
			return;
	}
	else if( !TiDoc.LoadFile( "Data\\Interface\\Tutorial\\TutorialIndicator.xml" ) )
		return;

	TiXmlNode* pXmlNode = TiDoc.FirstChild();
	SAFE_POINTER_RET( pXmlNode );

	TiXmlNode * pNode = pXmlNode->NextSibling();
	SAFE_POINTER_RET( pNode );

	if( 0 != ::stricmp( "QUESTINDICATOR", pNode->Value() ) )
		return;

	pNode = pNode->FirstChild();

	while( pNode )
	{
		_LoadScript( pNode->ToElement() );
		pNode = pNode->NextSibling();
	}
}

bool cIndicatorInfos::_LoadScript( TiXmlElement* pEl )
{
	SAFE_POINTER_RETVAL( pEl, false );

	if( 0 != ::stricmp( pEl->Value(), "Quest" ) )
		return false;

	double du;
	pEl->Attribute("ID", &du);

	auto _Rib = m_Indicators.insert( std::make_pair( du, MAP_QUEST_INDICATOR() ) );
	if( !_Rib.second )
		return false;

	MAP_INDICATOR_INFO_IT it = _Rib.first;

	TiXmlNode* pChildNode = pEl->FirstChild();
	while( pChildNode )
	{
		TiXmlElement* pChildEl = pChildNode->ToElement();
		if( pChildEl )
		{
			int nStepType = eStep::eNone;
			if( 0 == ::stricmp(pChildEl->Value(), "Befor" ) )				nStepType = eStep::eBefor;
			else if( 0 == ::stricmp(pChildEl->Value(), "Accept" ) )			nStepType = eStep::eAccpet;
			else if( 0 == ::stricmp(pChildEl->Value(), "Complete" ) )		nStepType = eStep::eComplete;

			if( nStepType != eStep::eNone )
			{
				auto conRib = it->second.insert( std::make_pair(nStepType, MAP_CONDITIONS()) );
				if( conRib.second )
				{
					MAP_QUEST_INDICATOR_IT subIT = conRib.first;

					TiXmlNode* pNode = pChildEl->FirstChild();
					while( pNode )
					{
						if( 0 == ::stricmp(pNode->Value(), "Trace" ) )
						{
							TiXmlElement* pChild = pNode->ToElement();
							if( pChild )
							{
								int nConditionIdx = 0;
								pChild->Attribute( "idx", &nConditionIdx );

								cTrace * pAddInfo = NiNew cTrace;
								if( pAddInfo->LoadScript( pChild ) )
									subIT->second.insert( std::make_pair( nConditionIdx, pAddInfo ) );
								else
									SAFE_NIDELETE( pAddInfo );
							}
						}
						pNode = pNode->NextSibling();
					}
				}
			}
		}		

		pChildNode = pChildNode->NextSibling();
	}

	return true;
}

void cIndicatorInfos::GetTutorialQuestList(std::list< DWORD >& lQuestList)
{
	MAP_INDICATOR_INFO_IT it = m_Indicators.begin();
	for( ; it != m_Indicators.end(); ++it )
		lQuestList.push_back( it->first );
}

sTraceData* cIndicatorInfos::_UpdateIndicator( DWORD const& dwQuestID, cTrace* pTrace )
{
	SAFE_POINTER_RETVAL( pTrace, NULL );
	
	sTraceData* pData = _CheckIndicator( dwQuestID, pTrace->m_pChildTrace );
	if( pData )
		return pData;

	return &pTrace->m_TraceData;
}

sTraceData* cIndicatorInfos::_CheckIndicator( DWORD const& dwQuestID, cTraceCondition* pData )
{
	SAFE_POINTER_RETVAL( pData, NULL );
	if( pData->m_pChildTrace )
	{
		sTraceData* pTrace = _CheckIndicator( dwQuestID, pData->m_pChildTrace );
		if( pTrace )
			return pTrace;
	}

	SAFE_POINTER_RETVAL( pData->m_pTraceKind, NULL );

	if( !pData->m_pTraceKind->CheckIndicator(dwQuestID) )
		return NULL;

	return &pData->m_TraceData;
}

void cIndicatorInfos::Update( float const& fElapsedTime )
{
	MAP_INDICATOR_INFO_IT it = m_Indicators.find( m_dwQuestID );
	if( it == m_Indicators.end() )
		return;

	MAP_QUEST_INDICATOR_IT subIT = it->second.find( m_nCurrentStep );
	if( subIT == it->second.end() )
		return;

	MAP_CONDITIONS_IT secIT = subIT->second.find( m_nCurrentConditionIdx );
	if( secIT == subIT->second.end() )
		return;

	sTraceData* pCurrentTrace = _UpdateIndicator( it->first, secIT->second );
	
	_UpdateTraceUIData( pCurrentTrace );
}

void cIndicatorInfos::_UpdateTraceUIData( sTraceData* pTraceData )
{
	if( pTraceData == m_pCurrentTrace )
		return;

	m_pCurrentTrace = pTraceData;

	if( GetSystem() )
		GetSystem()->SetUITraceData( m_pCurrentTrace );
}

void cIndicatorInfos::_StopIndicator()
{
	m_dwQuestID = 0;
	m_nCurrentStep = eNone;
	if( m_pCurrentTrace )
	{
		LIST_EFFECT_DATA_CIT it = m_pCurrentTrace->m_EffectData.begin();
		for( ; it != m_pCurrentTrace->m_EffectData.end(); ++it )
			GetSystem()->SetObjectHighlight( (*it).cTargetType, (*it).dwTargetIDX, false );
		m_pCurrentTrace = NULL;
	}
	m_nCurrentConditionIdx = -1;

	if( GetSystem() )
	{
		GetSystem()->CloseTooltipImage();
		GetSystem()->CloseUITrace();
	}
}

void cIndicatorInfos::_SetTutorialIndicator( DWORD const& dwQuestID, int const& nType )
{
	_StopIndicator();

	MAP_INDICATOR_INFO_IT it = m_Indicators.find( dwQuestID );
	if( it == m_Indicators.end() )
		return;

	MAP_QUEST_INDICATOR_IT subIt = it->second.find( nType );
	if( subIt == it->second.end() )
		return;

	m_dwQuestID = dwQuestID;
	m_nCurrentStep = nType;
	m_nCurrentConditionIdx = -1;
	m_pCurrentTrace = NULL;

	MAP_CONDITIONS_IT secIT = subIt->second.begin();
	for( ; secIT != subIt->second.end(); ++secIT )
	{
		SAFE_POINTER_CON( secIT->second );
		if( !secIT->second->m_nComplete )
		{
			m_nCurrentConditionIdx = secIT->second->m_nConditionIdx;
			break;
		}
	}
}

void cIndicatorInfos::_RemoveIndicator( DWORD const& dwQuestID )
{
	if( m_dwQuestID != dwQuestID )
		return;

	_StopIndicator();
}

void cIndicatorInfos::_UpdateQuestIndicatorInfo(DWORD const& dwQuestID, int const& nStep, int const& nConditionIdx)
{
	if( 0 != dwQuestID )
		m_dwQuestID = dwQuestID;

	if( eNone != nStep )
		m_nCurrentStep = nStep;

	if( -1 != nConditionIdx )
		m_nCurrentConditionIdx = nConditionIdx;
}

void cIndicatorInfos::_UpdateTutorialQuestCondition(DWORD const& dwQuestID, int const& nConditionIdx)
{
	if( m_dwQuestID != dwQuestID )
		return;
	
	if( m_nCurrentConditionIdx != nConditionIdx )
		return;
	
	_SetNextConditionIndicator( nConditionIdx + 1);
}

void cIndicatorInfos::_SetNextConditionIndicator( int const& nConditionIdx )
{
	MAP_INDICATOR_INFO_IT it = m_Indicators.find( m_dwQuestID );
	if( it == m_Indicators.end() )
		return;

	MAP_QUEST_INDICATOR_IT subIT = it->second.find( m_nCurrentStep );
	if( subIT == it->second.end() )
		return;

	MAP_CONDITIONS_IT secIT = subIT->second.find( nConditionIdx );
	if( secIT == subIT->second.end() )
	{
		_StopIndicator();
		return;
	}

	SAFE_POINTER_RET( secIT->second );
	if( secIT->second->m_nComplete )
		_SetNextConditionIndicator( nConditionIdx + 1 );
	else
		m_nCurrentConditionIdx = nConditionIdx;
}

void cIndicatorInfos::_SetProcessTutorialIndicator( DWORD const& dwQuestID )
{
	MAP_INDICATOR_INFO_IT it = m_Indicators.find( dwQuestID );
	if( it == m_Indicators.end() )
		return;

	MAP_QUEST_INDICATOR_IT subIT = it->second.find( eAccpet );
	if( subIT == it->second.end() )
		return;

	SAFE_POINTER_RET( g_pDataMng );
	cData_Quest* pQuestInfo = g_pDataMng->GetQuest();
	SAFE_POINTER_RET( pQuestInfo );

	cData_Quest::sPROCESS* pQuestProcess = pQuestInfo->GetProcess( dwQuestID ) ;
	SAFE_POINTER_RET( pQuestProcess );

	CsQuestCompRequire::LIST* pRequireGroup = pQuestProcess->s_CurCompRequireGroup.GetList();	
	CsQuestCompRequire::LIST_IT requireIt = pRequireGroup->begin();
	for( int n = 0; requireIt != pRequireGroup->end(); ++requireIt, ++n )
	{
		MAP_CONDITIONS_IT conIT = subIT->second.find( n );
		if( conIT != subIT->second.end() )
			conIT->second->m_nComplete = (*requireIt)->IsCompleat();
	}

	_SetTutorialIndicator( dwQuestID, eAccpet );
}

bool cIndicatorInfos::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eTutorialQuestAccpetAble, this );	
	GetSystem()->Register( SystemType::eTutorialQuestAccpet, this );	
	GetSystem()->Register( SystemType::eProccesQuestAllComplete, this );
	GetSystem()->Register( SystemType::eTutorialQuestCompleted, this );
	GetSystem()->Register( SystemType::eProccesQuestConditionUpdate, this );
	GetSystem()->Register( SystemType::eSetProcessTutorialQuest, this );
	GetSystem()->Register( SystemType::eTutorialQuestCancel, this );

	GetSystem()->Register( SystemType::eUpdateQuestIndicatorInfo, this );
	
	return true;
}

void cIndicatorInfos::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eSetProcessTutorialQuest:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_SetProcessTutorialIndicator( dwQuestID );
		}
		break;
	case SystemType::eTutorialQuestCancel:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_RemoveIndicator( dwQuestID );
		}
		break;
	case SystemType::eTutorialQuestAccpetAble:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_SetTutorialIndicator( dwQuestID, eBefor );
		}
		break;
	case SystemType::eTutorialQuestAccpet:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_SetTutorialIndicator( dwQuestID, eAccpet );
		}
		break;
	case SystemType::eProccesQuestConditionUpdate:
		{
			DWORD dwQuestID = 0;
			int	nConditionIdx = 0;
			kStream >> dwQuestID >> nConditionIdx;
			_UpdateTutorialQuestCondition( dwQuestID, nConditionIdx );
		}
		break;
	case SystemType::eProccesQuestAllComplete:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_SetTutorialIndicator( dwQuestID, eComplete );
		}
		break;
	case SystemType::eTutorialQuestCompleted:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_RemoveIndicator( dwQuestID );
		}
		break;
	case SystemType::eUpdateQuestIndicatorInfo:
		{
			DWORD dwQuestID = 0;
			int nStep = 0;
			int nConditionIdx = 0;
			kStream >> dwQuestID >> nStep >> nConditionIdx;
			_UpdateQuestIndicatorInfo( dwQuestID, nStep, nConditionIdx );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
