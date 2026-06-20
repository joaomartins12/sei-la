#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "MacroProtectContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

int const CMacroProtectContents::IsContentsIdentity(void)
{
	return E_CT_MACROPROTECT;
}

CMacroProtectContents::CMacroProtectContents(void):
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
m_nMaxTimeSec(0),m_nRemainTimeSec(0),m_nRemainChanceCount(0),m_nQuestionType(nsMacroProtector::NONE)
#else
m_nMacroAnswer(0),m_nRandomAnswer(0),m_nTryCount(0)
#endif
{
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	ZeroMemory( &m_pQuestionData, CAPTCHA_BMP_SIZE);
#endif
	
	// 매크로 프로텍트를 타겟의 이름으로 검사
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::USING_MACROPROTECT, this, &CMacroProtectContents::_UsingMacroProtect );
	// 매크로 프로텍트를 타겟의 UIDX로 검사 
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::USING_MACROPROTECT_TARGET_UIDX, this, &CMacroProtectContents::_UsingMacroProtect_UIDX );
	// 서버에 특정 유저에게 매크로 프로텍트 사용 요청 보냄
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SEND_MACROPROTECT_QUESTION, this, &CMacroProtectContents::_Send_MacroProtect_Using );
	// 내가 매크로 프로텍트 요청한 플레이어에 대한 결과값.
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MACROPROTECT_QUESTION_RESULT, this, &CMacroProtectContents::_Recv_MacroProtect_Question_Result );	
	// 매크로 플레이어인지에 대한 결과값
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MACRO_PLAYER_RESULT, this, &CMacroProtectContents::_Recv_MacroPlayer_Result );	
	// 내가 매크로 프로텍트 요청 옴.
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::NTF_MACROPROTECT_QUESTION_REQUEST, this, &CMacroProtectContents::_NTF_MacroProtect_Question_Request );	
}

CMacroProtectContents::~CMacroProtectContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const CMacroProtectContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CMacroProtectContents::Initialize(void)
{
	return true;
}

void CMacroProtectContents::UnInitialize(void)
{

}

bool CMacroProtectContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void CMacroProtectContents::Update(float const& fElapsedTime)
{

}

void CMacroProtectContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CMacroProtectContents::MakeMainActorData(void)
{
	_ResetData();
}

void CMacroProtectContents::ClearMainActorData(void)
{
	_ResetData();
}

void CMacroProtectContents::MakeWorldData(void)
{

}

void CMacroProtectContents::ClearWorldData(void)
{

}

// 매크로 프로텍트를 사용 할 수 없는 지역 검사
bool CMacroProtectContents::_IsUsingCurrentMap()
{
	SAFE_POINTER_RETVAL( nsCsGBTerrain::g_pCurRoot, false );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, false );

	CsGBTerrainRoot::sINFO* pMapInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	SAFE_POINTER_RETVAL( pMapInfo, false );

	return nsCsFileTable::g_pBaseMng->IsEnableCheckMacro( pMapInfo->s_dwMapID );
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 인벤에서 아이템을 사용했을 때
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_UsingMacroProtect(void* pData)
{
	SAFE_POINTER_RET( g_pCharResMng );

	// 타겟 대상이 있는지 체크
	CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
	SAFE_POINTER_RET( pTarget );

	_Show_AskWindow( pTarget );
}

//////////////////////////////////////////////////////////////////////////
// 타겟 창에서 사용했을 때	
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_UsingMacroProtect_UIDX(void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( g_pMngCollector );

	unsigned int nTargetUIDX = *static_cast<unsigned int*>(pData);
	CsC_AvObject* pTamer = g_pMngCollector->GetObject( nTargetUIDX );
	_Show_AskWindow( pTamer );
}

//////////////////////////////////////////////////////////////////////////
// 매크로 프로텍트 아이템을 사용시 한번 더 물어보는 창 띄움.
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_Show_AskWindow( CsC_AvObject* pTarget )
{
	if( !_IsUsingMacroProtect(pTarget) )
		return;

	cPrintMsg::PrintMsg( 30326, pTarget->GetName() );
	cMessageBox* pMsgBox = cMessageBox::GetMessageBox( 30326 );
	if( pMsgBox )
		pMsgBox->SetUintValue1( pTarget->GetUniqID() );

	// 이동 중지
	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();
}

// 매크로 프로텍트를 사용할 수 있는지 체크함
bool CMacroProtectContents::_IsUsingMacroProtect( CsC_AvObject* pTarget )
{
	// 타겟 대상이 있는지 체크
	if( pTarget == NULL )
	{
		cPrintMsg::PrintMsg( 16001 );
		return false;
	}

	// 타겟 대상이 테이머 인지 체크
	if( pTarget->GetLeafRTTI() != RTTI_TAMER )
	{
		cPrintMsg::PrintMsg( 11101 );
		return false;
	}

	// 매크로 프로텍트를 사용 할 수 없는 지역 검사
	if( !_IsUsingCurrentMap() )
	{
		cPrintMsg::PrintMsg( 30336 );
		return false;
	}


	// 인벤토리에 매크로 프로텍트 아이템이 있는지 검사
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	int nInvenIndex = pInven->GetFirstSlot_Item_TypeLS( ITEM_TYPELS_MACRO_CHECK );	
	if( nInvenIndex == cData_Inven::INVALIDE_INVEN_INDEX )
	{
		cPrintMsg::PrintMsg( 11040, ITEM_STR_MACRO_CHECK );
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 서버에 매크로 아이템을 사용하겠다고 보냄
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_Send_MacroProtect_Using(void* pData)
{
	SAFE_POINTER_RET( pData );
	unsigned int targetUIDX = *static_cast<unsigned int*>(pData);
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( targetUIDX );
	if( _IsUsingMacroProtect(pTarget) )
	{
		std::wstring name = pTarget->GetName();
		net::game->SendMacroQuestion( name );
	}
}

//////////////////////////////////////////////////////////////////////////
// 다른 유저가 내게 매크로 의심 했을 경우.
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_NTF_MacroProtect_Question_Request(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_NTF_MACRO_QUESTION_REQUEST * pRecv = static_cast<GS2C_NTF_MACRO_QUESTION_REQUEST*>(pData);

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	m_nMaxTimeSec = pRecv->m_nTimeMax;
	m_nRemainTimeSec = pRecv->m_nTimeLeft;
	m_nRemainChanceCount = pRecv->m_nChanceLeft;

	ZeroMemory( &m_pQuestionData, CAPTCHA_BMP_SIZE);
	cCaptchaConvert::ConvertToBmp( pRecv->m_pData, m_pQuestionData );

	if( (MACROPROTECT_MAX_CHANCE_COUNT - 1) == m_nRemainChanceCount )
		cPrintMsg::PrintMsg( 30341 );
	else
		cPrintMsg::PrintMsg( 30343, &m_nRemainChanceCount);


	#ifdef SDM_MACRO_PROTECT_QUESTION_20190524
		m_nQuestionType = pRecv->m_nQuestionType;
		m_nQuestionNumbers.clear();
		for( int n = 0; n < CAPTCHA_QUESTION_LENGTH;++n )
		{
			if( 0 == pRecv->m_nQuestion[n] )
				break;
			m_nQuestionNumbers.push_back( pRecv->m_nQuestion[n] );
		}
	#endif
#else
	m_nMacroAnswer = pRecv->m_nHint - pRecv->m_nRandNum;	
	m_nTryCount = 0;
#endif

	NotifyContents( CONTENTS_EVENT::EEvt_MacroProtect_Start );	
	_ShowMacroProtectWindow();
}

void CMacroProtectContents::_ShowMacroProtectWindow()
{
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_PROTECT ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_PROTECT );

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	ContentsStream kTemp;
	kTemp << m_nMaxTimeSec << m_nRemainTimeSec << m_nRemainChanceCount << m_nQuestionType;
	Notify( eResetAnswer, kTemp );
#else
	srand( (unsigned)time(NULL));
	m_nRandomAnswer	= rand() % 64;
	ContentsStream kTemp;
	kTemp << m_nRandomAnswer;
	Notify( eResetAnswer, kTemp );
#endif
}

//////////////////////////////////////////////////////////////////////////
// 내가 매크로 의심 유저의 요청에 대한 결과 값
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_Recv_MacroProtect_Question_Result(void* pData)
{
	SAFE_POINTER_RET( pData );
	C2GS_RECV_MACRO_QUESTION_RESULT * recv = static_cast<C2GS_RECV_MACRO_QUESTION_RESULT*>(pData);
	switch( recv->m_nResult )
	{
	case 0 :	
		{
			SAFE_POINTER_RET( g_pDataMng );
			cData_Inven* pInVen = g_pDataMng->GetInven();
			SAFE_POINTER_RET( pInVen );
			pInVen->DecreaseItem_TypeLS( ITEM_TYPELS_MACRO_CHECK, 1, true, false );		// 아이템 제거
			cPrintMsg::PrintMsg( 30339 );
		}break;	// 매크로 유저 차단.
	case 1 :	cPrintMsg::PrintMsg( 30327 );		break;// 대상 유저는 현재 맵에 접속중이지 않습니다.
	case 2 :	cPrintMsg::PrintMsg( 30328 );		break;// 대상 유저는 macro free상태입니다.
	case 3 :	cPrintMsg::PrintMsg( 30329 );		break;// 이미 누군가에 의해 체크중입니다.
	case 4 :	cPrintMsg::PrintMsg( 30331 );		break;// cool down time 이 아직 남아 있습니다.
	case 6 :	cPrintMsg::PrintMsg( 30337 );		break;// 대상 유저는 자리 비움 상태입니다.
	case 5 :	cPrintMsg::PrintMsg( 11040, ITEM_STR_MACRO_CHECK );		break;// 매크로 체크 아이템 부족
	case 7 :	cPrintMsg::PrintMsg( 30347 );		break;// 피로도 조건을 만족하지 않아 매크로프로텍터를 사용할 수 없습니다.
	}
}

void CMacroProtectContents::_Recv_MacroPlayer_Result(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_MACRO_PLAYER_RESULT* pRecv = static_cast<GS2C_RECV_MACRO_PLAYER_RESULT*>(pData);

	if(pRecv->m_nResult)
		cPrintMsg::PrintMsg( 30333 );		// 대상이 틀렸다
	else
		cPrintMsg::PrintMsg( 30332 );		// 대상이 맞췄다
}

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
void CMacroProtectContents::Send_MacroAnswer( std::wstring const& wsAnswer )
{
	if( net::game )
		net::game->SendMacroAnswer( wsAnswer );
	_ResetData();
	_CloseWindow();
}

char const* CMacroProtectContents::GetQuestionImgData() const
{
	return m_pQuestionData;
}

#else
void CMacroProtectContents::Send_MacroAnswer( int const& nAnswerValue )
{
	bool bSuccess = (m_nRandomAnswer == nAnswerValue)? true : false;
	m_nTryCount++;

	if( !bSuccess && m_nTryCount < MAX_TRY_COUNT )
	{
		_ShowMacroProtectWindow();
		return;
	}

	int nRandNo = nBase::rand(9, 999);
	int nAnswer = nRandNo;
	if( bSuccess )
	{
		nAnswer += m_nMacroAnswer;
		cPrintMsg::PrintMsg( 30335 );
	}
	else
	{
		nAnswer += 19;
		cPrintMsg::PrintMsg( 30325 );
	}

	if( net::game )
		net::game->SendMacroAnswer( nRandNo, nAnswer );

	_ResetData();
	_CloseWindow();
}

//////////////////////////////////////////////////////////////////////////
// TotalSize 범위에서 범위 안에 있는 nIncludeNum 값을 포함한 nGetCount개 만큼의 랜덤 값을 얻어 오는 함수
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::RandomShuffle( int TotalSize, int nIncludeNum, int nGetCount, std::vector<int>& getVector )
{
	std::vector<int> temp;
	temp.resize(TotalSize);
	for( int n = 0; n < TotalSize; ++n )
		temp[n] = n;

	std::random_shuffle( temp.begin(), temp.end() );

	int nFindPos = -1;
	for( int n = 0; n < temp.size(); ++n )
	{
		if( temp[n] == nIncludeNum )
		{
			nFindPos = n;
			break;
		}
	}

	int nStartPos = 0;
	if( nFindPos - nGetCount < 0 )// 앞에서 nGetCount 앞에 있는경우
		nStartPos = 0;
	else if( TotalSize - nFindPos < nGetCount ) // 앞에서 nGetCount 앞에 있는경우
		nStartPos = TotalSize - nGetCount;
	else
		nStartPos = nFindPos - rand() % nGetCount;

	for( int n = nStartPos; n < nStartPos + nGetCount; ++n )
		getVector.push_back( temp[n] );

	std::random_shuffle( getVector.begin(), getVector.end() );
}
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_ResetData()
{
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	ZeroMemory( &m_pQuestionData, CAPTCHA_BMP_SIZE);
	m_nMaxTimeSec = 0;
	m_nRemainTimeSec = 0;
	m_nRemainChanceCount = 0;
	m_nQuestionType = nsMacroProtector::NONE;
	m_nQuestionNumbers.clear();
#else
	m_nMacroAnswer = 0;
	m_nRandomAnswer = 0;
	m_nTryCount = 0;
#endif
}

void CMacroProtectContents::_CloseWindow()
{
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PROTECT );
}
// 매크로 프로텍터로 질문은 받은 상태인지 체크 
bool CMacroProtectContents::IsMacroProtectUsing() const
{
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	if( m_nMaxTimeSec != 0 )
		return true;

	if( m_nRemainTimeSec != 0 )
		return true;

	if( m_nRemainChanceCount != 0 )
		return true;

	return false;
#else
	if( m_nMacroAnswer != 0 )
		return true;
	if( m_nRandomAnswer != 0 )
		return true;
	return false;
#endif
}

std::list<int> const& CMacroProtectContents::GetQuestionNumber() const
{
	return m_nQuestionNumbers;
}