#include "stdafx.h"
#include "TutorialQuest.h"

//////////////////////////////////////////////////////////////////////////

IF_TRequireCondition::IF_TRequireCondition():m_bComplete(false),m_nCurrCount(0), m_nMaxCount(1), m_bInitalize(false),m_bSendSuccess(false){}
IF_TRequireCondition::~IF_TRequireCondition(){}
bool IF_TRequireCondition::Initalize()				{	m_bInitalize = true;	return true;	}
void IF_TRequireCondition::SetCurrentNMaxCount( int const& nCurrentCount, int const& nMaxCount )
{	
	m_nCurrCount = nCurrentCount;
	m_nMaxCount = nMaxCount;

	if( m_nCurrCount >= m_nMaxCount )
	{
		m_bComplete = true;
		m_bSendSuccess = true;
	}
}

bool IF_TRequireCondition::IsComplete() const		{	return m_bComplete;	}
bool IF_TRequireCondition::IsSendComplete() const	{	return m_bSendSuccess;	}
void IF_TRequireCondition::SetSendComplete()		{	m_bSendSuccess = true;	}
int IF_TRequireCondition::GetCurrentCount() const	{	return m_nCurrCount; }

bool IF_TRequireCondition::IncreaseCurCount() const
{
	if( m_bComplete )	
		return true;

	m_nCurrCount++;

	if( m_nCurrCount >= m_nMaxCount )
		m_bComplete = true;

	return m_bComplete;
}

//////////////////////////////////////////////////////////////////////////
CTC_QuestAccpet::CTC_QuestAccpet()
{}

CTC_QuestAccpet::~CTC_QuestAccpet()
{
}

void CTC_QuestAccpet::Update( float const& fElapsedTime )
{
	if( m_bComplete )
		return;

	if( !m_bInitalize )
	{	
		if( !Initalize() )
			return;
	}

	IncreaseCurCount();
}

void CTC_QuestAccpet::SetData( int const& nValue1, int const& nValue2 )
{

}
//////////////////////////////////////////////////////////////////////////

CTC_MovePosition::CTC_MovePosition():m_fRadius(0.0f)
{

}

CTC_MovePosition::~CTC_MovePosition()
{
	SetObjectHide();
}

bool CTC_MovePosition::Initalize()
{	
	if( !SetObjectVisible() )
		return false;

	return IF_TRequireCondition::Initalize();
}

bool CTC_MovePosition::SetObjectVisible()
{
	SAFE_POINTER_RETVAL( nsCsGBTerrain::g_pCurRoot, false );
	CsGBObject* pEffect = nsCsGBTerrain::g_pCurRoot->FindCsGBObject(m_dwObjectIdx);
	SAFE_POINTER_RETVAL( pEffect, false );

	NiNode * pNode = pEffect->GetNiNode();
	SAFE_POINTER_RETVAL( pNode, false );

	m_mapPosition.x = pNode->GetTranslate().x;
	m_mapPosition.y = pNode->GetTranslate().y;
	NiBound bound = pNode->GetWorldBound();
	m_fRadius = bound.GetRadius() / 4;

	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_MapObject* pMapObj = g_pDataMng->GetMapObject();
	SAFE_POINTER_RETVAL( pMapObj, false );
	pMapObj->SetMapObjectAni( m_dwObjectIdx, 3/*Target_point_Anim::OPENIDLE*/ );
	return true;
}

void CTC_MovePosition::SetObjectHide()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_MapObject* pMapObj = g_pDataMng->GetMapObject();
	SAFE_POINTER_RET( pMapObj );
	pMapObj->SetMapObjectAni( m_dwObjectIdx, 1/*Target_point_Anim::CLOSEIDLE*/ );
}

void CTC_MovePosition::SetData( int const& nValue1, int const& nValue2 )
{
	m_dwObjectIdx = nValue1;
	//m_mapPosition.x = nValue1;
	//m_mapPosition.y = nValue2;
}

void CTC_MovePosition::Update( float const& fElapsedTime )
{
	if( m_bComplete )		return;
	if( !m_bInitalize )
	{	
		if( !Initalize() )
			return;
	}

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	NiPoint2 vTamer = pUser->GetPos2D();
	if( ( m_mapPosition - vTamer ).Length() < m_fRadius  )
	{
		SetObjectHide();
		IncreaseCurCount();
	}
}
//////////////////////////////////////////////////////////////////////////

CTC_CameraState::CTC_CameraState():m_fCompleteValue(0.0f),m_nInputType(0)
,m_T1_m_fRoll(0.0f),m_T1_m_fPitch(0.0f),m_T1_m_fDist(0.0f)
{
}

CTC_CameraState::~CTC_CameraState()
{

}

bool CTC_CameraState::Initalize()
{
	SAFE_POINTER_RETVAL( CAMERA_STPTR, false );

	m_T1_m_fRoll = CAMERA_ST.GetCurRoll();
	m_T1_m_fPitch = CAMERA_ST.GetCurPitch();
	m_T1_m_fDist = 0;
	return IF_TRequireCondition::Initalize();
}

void CTC_CameraState::SetData( int const& nValue1, int const& nValue2 )
{
	m_nInputType = nValue1;
	m_fCompleteValue = nValue2;
	if( m_nInputType == 1 )// 카메라 좌우
		m_fCompleteValue *= 0.1f;
}

void CTC_CameraState::Update( float const& fElapsedTime )
{
	if( m_bComplete )
		return;

	if( !m_bInitalize )
	{	
		if( !Initalize() )
			return;
	}

	switch( m_nInputType )
	{
	case 1:	// 카메라 좌우
		{
			float fRoll = CAMERA_ST.GetCurRoll();
			float fPitch = CAMERA_ST.GetCurPitch();
			if( ( abs( fRoll - m_T1_m_fRoll ) > m_fCompleteValue )||( abs( fPitch - m_T1_m_fPitch ) > m_fCompleteValue ) )
				IncreaseCurCount();
		}
		break;
	case 2:	// 카메라 줌
		{
			int nWheel = CURSOR_ST.GetWheel();
			m_T1_m_fDist += abs( nWheel );
			if( m_T1_m_fDist > m_fCompleteValue )
				IncreaseCurCount();
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

CTC_TargetTrace::CTC_TargetTrace()
{

}

CTC_TargetTrace::~CTC_TargetTrace()
{
	if( g_pHelpArrow )
		g_pHelpArrow->ReleaseArrowPoint();
}

void CTC_TargetTrace::SetData( int const& nValue1, int const& nValue2 )
{
	m_nTraceObjectType = nValue1;
	m_nObjectIDX = nValue2;
}

void CTC_TargetTrace::Update( float const& fElapsedTime )
{
	if( m_bComplete )
		return;

	if( !m_bInitalize )
	{	
		if( !Initalize() )
			return;
	}

	switch( m_nTraceObjectType )
	{
	case 1:	// NPC 추적
		{
			SAFE_POINTER_RET( g_pCharMng );
			CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
			SAFE_POINTER_RET( pTamerUser );

			CNpc* pTargetNpc = g_pNpcMng->GetNpc( m_nObjectIDX );
			SAFE_POINTER_RET( pTargetNpc );

			NiPoint2 vTargetPos = pTargetNpc->GetPos2D();
			NiPoint2 vTamer = pTamerUser->GetPos2D();
			if( ( vTargetPos - vTamer ).Length() < 300.0f  )
				IncreaseCurCount();
		}
		break;
	case 2:	// 몬스터 추적
		{
			SAFE_POINTER_RET( g_pCharMng );
			CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
			SAFE_POINTER_RET( pTamerUser );
			CMonster* pTargetMob = g_pCharMng->FindNearMonster( m_nObjectIDX );
			SAFE_POINTER_RET( pTargetMob );
			NiPoint2 vTargetPos = pTargetMob->GetPos2D();
			NiPoint2 vTamer = pTamerUser->GetPos2D();
			if( ( vTargetPos - vTamer ).Length() < 300.0f  )
				IncreaseCurCount();
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

CTC_AttackState::CTC_AttackState()
{

}

CTC_AttackState::~CTC_AttackState()
{

}

void CTC_AttackState::SetData( int const& nValue1, int const& nValue2 ) 
{
	m_nAttackType = nValue1;
	m_nObjectIDX = nValue2;
}

void CTC_AttackState::Update( float const& fElapsedTime )
{
	if( m_bComplete )
		return;

	if( !m_bInitalize )
	{	
		if( !Initalize() )
			return;
	}


	switch( m_nAttackType )
	{
	case 0:		// 일반+스킬
		{
			CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
			SAFE_POINTER_RET(pTarget);
			if( pTarget->GetFTID() != m_nObjectIDX  )
				return;

			CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
			SAFE_POINTER_RET(pDUser);

			switch( pDUser->GetDigimonUserState() )
			{
			case CDigimonUser::DU_ATTACK:
			case CDigimonUser::DU_SKILL:		IncreaseCurCount();		break;
			}
		}
		break;	
	case 1:	// 일반 공격
		{
			CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
			SAFE_POINTER_RET(pTarget);
			if( pTarget->GetFTID() != m_nObjectIDX  )
				return;

			CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
			SAFE_POINTER_RET(pDUser);

			if( CDigimonUser::DU_ATTACK == pDUser->GetDigimonUserState() )
				IncreaseCurCount();
		}
		break;
	case 2:	// 스킬 공격
		{
			CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
			SAFE_POINTER_RET(pTarget);
			if( pTarget->GetFTID() != m_nObjectIDX  )
				return;

			CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
			SAFE_POINTER_RET(pDUser);

			if( CDigimonUser::DU_SKILL == pDUser->GetDigimonUserState() )
				IncreaseCurCount();
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

CTC_DigimonScanAndRetrieve::CTC_DigimonScanAndRetrieve()
{

}

CTC_DigimonScanAndRetrieve::~CTC_DigimonScanAndRetrieve()
{
	if( GAME_EVENT_STPTR )
		GAME_EVENT_STPTR->DeleteEventAll(this);
}

bool CTC_DigimonScanAndRetrieve::Initalize()
{
	switch( m_nType )
	{
	case 1:	// 디지몬 스캔
		{
			SAFE_POINTER_RETVAL( GAME_EVENT_STPTR, false );
			GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_SCAN_ADD_DATA, this, &CTC_DigimonScanAndRetrieve::_DigitamaScanResult );
		}
		return IF_TRequireCondition::Initalize();
	case 2:	// 디지몬 회수
		{
			SAFE_POINTER_RETVAL( GAME_EVENT_STPTR, false );
			GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_SELL_SUCCESS, this, &CTC_DigimonScanAndRetrieve::_DigitamaSellResult );

		}
		return IF_TRequireCondition::Initalize();
	}
	return false;
}

void CTC_DigimonScanAndRetrieve::SetData( int const& nValue1, int const& nValue2 )
{
	m_nType = nValue1;
	m_nItemCode = nValue2;
}

void CTC_DigimonScanAndRetrieve::_DigitamaScanResult(void* pData)
{
	IncreaseCurCount();
}

void CTC_DigimonScanAndRetrieve::_DigitamaSellResult(void* pData)
{
	IncreaseCurCount();
}

void CTC_DigimonScanAndRetrieve::Update( float const& fElapsedTime )
{
	if( m_bComplete )
		return;

	if( !m_bInitalize )
	{	
		if( !Initalize() )
			return;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CTC_DigimonHatch::CTC_DigimonHatch()
{

}

CTC_DigimonHatch::~CTC_DigimonHatch()
{
	if( GAME_EVENT_STPTR )
		GAME_EVENT_STPTR->DeleteEventAll(this);
}

bool CTC_DigimonHatch::Initalize()
{
	SAFE_POINTER_RETVAL( GAME_EVENT_STPTR, false );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_HATCHOUT_COMPLETE, this, &CTC_DigimonHatch::_DigitamaHatchResult );
	return IF_TRequireCondition::Initalize();
}

void CTC_DigimonHatch::_DigitamaHatchResult(void* pData)
{
	IncreaseCurCount();
}

void CTC_DigimonHatch::SetData( int const& nValue1, int const& nValue2 ) 
{

}

void CTC_DigimonHatch::Update( float const& fElapsedTime )
{
	if( m_bComplete )
		return;

	if( !m_bInitalize )
	{	
		if( !Initalize() )
			return;
	}

}

//////////////////////////////////////////////////////////////////////////

CTC_DigimonEnchant::CTC_DigimonEnchant()
{

}

CTC_DigimonEnchant::~CTC_DigimonEnchant()
{
	if( GAME_EVENT_STPTR )
		GAME_EVENT_STPTR->DeleteEventAll(this);
}

bool CTC_DigimonEnchant::Initalize()
{
	SAFE_POINTER_RETVAL( GAME_EVENT_STPTR, false );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_ENCHANT_COMPLETE, this, &CTC_DigimonEnchant::_DigimonEnchantResult );
	return IF_TRequireCondition::Initalize();
}

void CTC_DigimonEnchant::_DigimonEnchantResult(void* pData)
{
	IncreaseCurCount();
}

void CTC_DigimonEnchant::SetData( int const& nValue1, int const& nValue2 )
{

}

void CTC_DigimonEnchant::Update( float const& fElapsedTime )
{
	if( m_bComplete )
		return;

	if( !m_bInitalize )
	{	
		if( !Initalize() )
			return;
	}

}

//////////////////////////////////////////////////////////////////////////

CTC_DigimonChange::CTC_DigimonChange()
{

}

CTC_DigimonChange::~CTC_DigimonChange()
{

}

bool CTC_DigimonChange::Initalize()
{
	return IF_TRequireCondition::Initalize();
}

void CTC_DigimonChange::SetData( int const& nValue1, int const& nValue2 )
{

}

void CTC_DigimonChange::Update( float const& fElapsedTime )
{
	if( m_bComplete )
		return;

	if( !m_bInitalize )
	{	
		if( !Initalize() )
			return;
	}

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



sCompCondition::sCompCondition()
{

}

sCompCondition::~sCompCondition()
{
	DeleteData();
}

void sCompCondition::DeleteData()
{
	std::map<int,IF_TRequireCondition*>::iterator it = m_mapCondition.begin();
	for( ; it != m_mapCondition.end(); ++it )
		NISAFE_DELETE( it->second );

	m_mapCondition.clear();
}

size_t sCompCondition::HaveConditionCount() const
{
	return m_mapCondition.size();
}

bool sCompCondition::IsAllComplete() const
{
	std::map<int,IF_TRequireCondition*>::const_iterator it = m_mapCondition.begin();
	for( ; it != m_mapCondition.end(); ++it )
	{
		if( !it->second->IsComplete() )
			return false;
	}
	return true;
}

void sCompCondition::ReLoadMap()
{
	std::map<int,IF_TRequireCondition*>::iterator it = m_mapCondition.begin();
	for( ; it != m_mapCondition.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );	
		if( it->second->IsComplete() )
			continue;
		it->second->ReloadMap();
	}
}

bool sCompCondition::AddQuestCompeteRequire(int const& nIdx, CsQuestCompRequire* pComp)
{
	SAFE_POINTER_RETVAL( pComp, false );

	IF_TRequireCondition* pAddCondition = NULL;
	if( CsQuestCompRequire::CLIENT != pComp->GetType() )
	{
		pAddCondition  = NiNew IF_TRequireCondition;
	}
	else
	{
		switch( pComp->GetTypeID() )
		{
		case clientCompType::eAccept:			
			{
				pAddCondition = NiNew CTC_QuestAccpet;
			}
			break;
		case clientCompType::eMovePosition:		
			{
				pAddCondition = NiNew CTC_MovePosition;
				pAddCondition->SetData( pComp->GetTargetID(), 0 );
			}break;
		case clientCompType::eCamerControl:		
			{
				pAddCondition = NiNew CTC_CameraState;
				pAddCondition->SetData( pComp->GetSubValue(), pComp->GetSubValue2() );
			}break;
		case clientCompType::eTargetTrace:		
			{
				pAddCondition = NiNew CTC_TargetTrace;
				pAddCondition->SetData( pComp->GetSubValue(), pComp->GetTargetID() );
			}break;
		case clientCompType::eAttack:			
			{
				pAddCondition = NiNew CTC_AttackState;
				pAddCondition->SetData( pComp->GetSubValue(), pComp->GetTargetID() );
			}break;
		case clientCompType::eScanAndRetrieve:				
			{
				pAddCondition = NiNew CTC_DigimonScanAndRetrieve;		
				pAddCondition->SetData( pComp->GetSubValue(), pComp->GetTargetID() );
			}break;
		case clientCompType::eDigimonHatch:		
			{
				pAddCondition = NiNew CTC_DigimonHatch;		
				pAddCondition->SetData( pComp->GetSubValue(), pComp->GetTargetID() );
			}break;
		case clientCompType::eDigimonEnchant:	
			{
				pAddCondition = NiNew CTC_DigimonEnchant;		
				pAddCondition->SetData( pComp->GetSubValue(), pComp->GetTargetID() );
			}break;
		case clientCompType::eDigimonChange:	
			{
				pAddCondition = NiNew CTC_DigimonChange;		
				pAddCondition->SetData( pComp->GetSubValue(), pComp->GetTargetID() );
			}break;
		default:
			return false;
		}
	}

	SAFE_POINTER_RETVAL( pAddCondition, false );
	pAddCondition->SetCurrentNMaxCount( pComp->GetCurCount(), pComp->GetTypeCount() );
	m_mapCondition.insert( std::make_pair( nIdx, pAddCondition ) );
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

cTutorialQuest::cTutorialQuest()
{

}

cTutorialQuest::~cTutorialQuest()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	_DeleteAllAcceptQuest();
	m_listAcceptableQuest.clear();
}

void cTutorialQuest::Update(float const& fElapsedTime)
{
	LIST_CONDITIONS_IT it = m_listAcceptQuest.begin();
	for( ; it != m_listAcceptQuest.end(); ++it )
	{
		std::map<int,IF_TRequireCondition*>::iterator subIT = (*it)->m_mapCondition.begin();
		for( ; subIT != (*it)->m_mapCondition.end(); ++subIT )
		{
			IF_TRequireCondition* pCondition = subIT->second;
			SAFE_POINTER_CON( pCondition );

			if( pCondition->IsSendComplete() )
				continue;

			pCondition->Update( fElapsedTime );

			if( !pCondition->IsComplete() )
				continue;

			int nCurrentCount = pCondition->GetCurrentCount();
			if( GetSystem()->TutorialQuestConditionUpdate( (*it)->m_dwQuestIDX, subIT->first, nCurrentCount ) )
				pCondition->SetSendComplete();
		}
	}
}

void cTutorialQuest::LoadTutorialQuest()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Quest* pQuestInfo = g_pDataMng->GetQuest();
	SAFE_POINTER_RET( pQuestInfo );
	SAFE_POINTER_RET( nsCsFileTable::g_pQuestMng );	

	std::list<DWORD> tutorialQuest;
	nsCsFileTable::g_pQuestMng->GetQuestListFormType( CsQuest::QT_TUTORIAL, tutorialQuest );

	std::list<DWORD>::iterator it = tutorialQuest.begin();
	for( ; it != tutorialQuest.end(); ++it)
	{
		if( pQuestInfo->IsCompleate( *it ) )
			continue;

		if( pQuestInfo->IsProcess( *it ) )// 현재 진행 중인 튜토리얼 퀘스트
		{
			cData_Quest::sPROCESS* pQuestProcess = pQuestInfo->GetProcess( *it ) ;
			if( pQuestProcess )
				_AddProcessTutorialQuest( *it, pQuestProcess );
		}
		else// 수행 가능한 퀘스트
			_AddAcceptableQuest( *it );			
	} 
}

void cTutorialQuest::LoadTutorialQuest(std::list<DWORD>& lQuestList)
{
	SAFE_POINTER_RET(g_pDataMng);
	cData_Quest* pQuestInfo = g_pDataMng->GetQuest();
	SAFE_POINTER_RET(pQuestInfo);

	std::list<DWORD>::iterator it = lQuestList.begin();
	for (; it != lQuestList.end(); ++it)
	{
		if (pQuestInfo->IsCompleate(*it))
			continue;

		if (pQuestInfo->IsProcess(*it))// 현재 진행 중인 튜토리얼 퀘스트
		{
			cData_Quest::sPROCESS* pQuestProcess = pQuestInfo->GetProcess(*it);
			if (pQuestProcess)
				_AddProcessTutorialQuest(*it, pQuestProcess);
		}
		else// 수행 가능한 퀘스트
			_AddAcceptableQuest(*it);
	}
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트를 수락했을 때 처리
//////////////////////////////////////////////////////////////////////////
bool cTutorialQuest::_AddProcessTutorialQuest( DWORD const& dwQuestID, cData_Quest::sPROCESS * sProcess )
{
// 	if( sProcess->s_CurCompRequireGroup.IsCompleate() )
// 	{	// 모두 완료일 
// 		sCompCondition * newQuest = new sCompCondition;
// 		newQuest->m_dwQuestIDX = dwQuestID;
// 		m_listAcceptQuest.push_back( newQuest );
// 	}
// 	else
// 	{
		sCompCondition * newQuest = new sCompCondition;
		newQuest->m_dwQuestIDX = dwQuestID;
		CsQuestCompRequire::LIST* pRequireGroup = sProcess->s_CurCompRequireGroup.GetList();	
		CsQuestCompRequire::LIST_IT it = pRequireGroup->begin();
		for( int n = 0; it != pRequireGroup->end(); ++it, ++n )
		{
// 			if( CsQuestCompRequire::CLIENT != (*it)->GetType() )
// 				continue;
// 
// 			if( (*it)->IsCompleat() )
// 				continue;

			newQuest->AddQuestCompeteRequire( n, (*it) );
		}
		m_listAcceptQuest.push_back( newQuest );
//	}
	return true;
}

// 튜토리얼 퀘스트를 아직 수행 하지 않는 퀘스트 목록
// 퀘스트가 수행 가능하지 체크 해서 
void cTutorialQuest::_AddAcceptableQuest(DWORD const& dwQuestID)
{
	m_listAcceptableQuest.push_back( dwQuestID );
}

void cTutorialQuest::_DeleteAllAcceptQuest()
{
	LIST_CONDITIONS_IT it = m_listAcceptQuest.begin();
	for( ; it != m_listAcceptQuest.end(); ++it )
		SAFE_DELETE( *it );
	m_listAcceptQuest.clear();
}

bool cTutorialQuest::_DeleteAcceptQuest( DWORD const& dwQuestID )
{
	LIST_CONDITIONS_IT it = m_listAcceptQuest.begin();
	for( ; it != m_listAcceptQuest.end(); ++it )
	{
		if( (*it)->m_dwQuestIDX != dwQuestID )
			continue;

		SAFE_DELETE( *it );
		m_listAcceptQuest.erase(it);		
		return true;		
	}
	return false;
}

void cTutorialQuest::_AcceptTutorialQuest( DWORD const& dwQuestID )
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Quest* pQuestData = g_pDataMng->GetQuest();
	SAFE_POINTER_RET( pQuestData );

	cData_Quest::sPROCESS* pQuestProcess = pQuestData->GetProcess( dwQuestID ) ;
	SAFE_POINTER_RET( pQuestProcess );

	sCompCondition * newQuest = new sCompCondition;
	newQuest->m_dwQuestIDX = dwQuestID;

	CsQuestCompRequire::LIST* pRequireGroup = pQuestProcess->s_CurCompRequireGroup.GetList();	
	CsQuestCompRequire::LIST_IT it = pRequireGroup->begin();
	for( int n = 0; it != pRequireGroup->end(); ++it, ++n )
		newQuest->AddQuestCompeteRequire( n, (*it) );

	if( newQuest->HaveConditionCount() >= 0 )
		m_listAcceptQuest.push_back( newQuest );

	// 퀘스트를 수락할 경우 수행 가능한 목록에서 지워줌
	_RemoveAcceptableList( dwQuestID );
}

void cTutorialQuest::_CancelTutorialQuest( DWORD const& dwQuestID )
{
	if( !_DeleteAcceptQuest(dwQuestID) )
		return;

	_AddAcceptableQuest( dwQuestID );
	RecheckTutorial();
}

void cTutorialQuest::_CompleteTutorialQuest( DWORD const& dwQuestID )
{
	if( !_DeleteAcceptQuest(dwQuestID) )
		return;

	RecheckTutorial();
}

void cTutorialQuest::ReloadMap()
{
	LIST_CONDITIONS_IT it = m_listAcceptQuest.begin();
	for( ; it != m_listAcceptQuest.end(); ++it )
		(*it)->ReLoadMap();
}

void cTutorialQuest::RecheckTutorial()
{
	if( !m_listAcceptQuest.empty() )// 진행 중인 튜토리얼 퀘스트가 있는 경우
	{
		LIST_CONDITIONS_CIT it = m_listAcceptQuest.begin();
		if( (*it)->IsAllComplete() )
			GetSystem()->ProcessConditionAllComplete( (*it)->m_dwQuestIDX );
		else
			GetSystem()->AddProcessTutorialQuest( (*it)->m_dwQuestIDX );
		return;
	}

#ifdef LJW_TUTORIAL_RENEWAL_20191128
	// 진행 중인 퀘스트가 없고
	// 받을 수 있는 퀘스트도 없으면 튜토리얼 완료
	if (m_listAcceptableQuest.empty())
	{
		GetSystem()->TutorialQuestCompleteAll();
		return;
	}
#endif

	SAFE_POINTER_RET( nsCsFileTable::g_pQuestMng );
	std::list<DWORD>::iterator it = m_listAcceptableQuest.begin();
	for( ; it != m_listAcceptableQuest.end(); ++it )
	{
		CsQuest* pQuestInfo = nsCsFileTable::g_pQuestMng->GetQuest((*it));
		SAFE_POINTER_CON( pQuestInfo );

		// 튜토리얼 선행 조건 체크
		if( 1462 != pQuestInfo->GetUniqID() && !cData_Quest::CheckPreRequire( pQuestInfo ) )
			continue;

#ifndef LJW_TUTORIAL_RENEWAL_20191128
		if( CsQuest::ST_NPC == pQuestInfo->GetStartTarget_Type() )
		{
			DWORD dwRecvNpcID = pQuestInfo->GetStartTarget_ID();
			if( 0 != dwRecvNpcID && !g_pNpcMng->IsNpc( dwRecvNpcID ) )
				continue;
		}
#endif

		GetSystem()->HaveAcceptableQuest( pQuestInfo->GetUniqID() );
		break;
	}
}

void cTutorialQuest::_CheckAcceptableQuest(DWORD const& dwQuestID)
{
	if(!m_listAcceptQuest.empty())
		return;

	SAFE_POINTER_RET( nsCsFileTable::g_pQuestMng );
	std::list<DWORD>::iterator it = m_listAcceptableQuest.begin();
	for( ; it != m_listAcceptableQuest.end(); ++it )
	{
		if( (*it) != dwQuestID )
			continue;

		CsQuest* pQuestInfo = nsCsFileTable::g_pQuestMng->GetQuest((*it));
		SAFE_POINTER_CON( pQuestInfo );

		// 튜토리얼 선행 조건 체크
		if( !cData_Quest::CheckPreRequire( pQuestInfo ) )
			continue;

		GetSystem()->HaveAcceptableQuest( pQuestInfo->GetUniqID() );
		break;
	}
}

// 퀘스트를 수락할 경우 수행 가능한 목록에서 지워줌
void cTutorialQuest::_RemoveAcceptableList( DWORD const& dwQuestID )
{
	std::list<DWORD>::iterator it = m_listAcceptableQuest.begin();
	for( ; it != m_listAcceptableQuest.end(); ++it )
	{
		if( (*it) != dwQuestID )
			continue;
		m_listAcceptableQuest.erase(it);
		break;
	}
}

bool cTutorialQuest::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eTutorialQuestAccpet, this );
	GetSystem()->Register( SystemType::eTutorialQuestCancel, this );
	GetSystem()->Register( SystemType::eTutorialQuestCompleted, this );
	GetSystem()->Register( SystemType::eTutorialQuestDigiviceCheck, this );

	return true;
}

void cTutorialQuest::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	kStream.SetRp(0);
	switch( iNotifiedEvt )
	{
	case SystemType::eTutorialQuestAccpet:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_AcceptTutorialQuest( dwQuestID );
		}
		break;
	case SystemType::eTutorialQuestCancel:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_CancelTutorialQuest( dwQuestID );
		}
		break;
	case SystemType::eTutorialQuestCompleted:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_CompleteTutorialQuest( dwQuestID );
		}
		break;
	case SystemType::eTutorialQuestDigiviceCheck:
		{
			DWORD dwQuestID = 0;
			kStream >> dwQuestID;
			_CheckAcceptableQuest( dwQuestID );
		}
		break;
	}
}