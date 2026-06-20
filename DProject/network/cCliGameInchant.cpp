#include "stdafx.h"
#include "common_vs2019/pGame.h"
#include "cCliGame.h"

// nType 1: AT 2: DE 3: CR 4: AS 5: EV 6: HT, 아이템 슬롯, 리커버리 디스크 슬롯 미사용시 0으로 전송해 주시면 됩니다.
void cCliGame::SendDigimonInchant(uint nType, uint ItemSlot, int RecoverSlot)
{
	// nType 강화 타입에 0 값은 사용하지 않습니다.

	if( RecoverSlot == cData_Inven::INVALIDE_INVEN_INDEX )
		RecoverSlot = -1;	

	newp( pGame::DigimonInchant );
		push( nType );
		push( ItemSlot );
		push( RecoverSlot );
	endp( pGame::DigimonInchant );
	send();
}

void cCliGame::RecvDigimonInchantResult()
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCHANT_TACTICS ) == false )
		return;

	u4 nResult = 0;				// 결과
	pop( nResult );	// 강화 결과

	int nSelect; 	
	nSelect = g_pGameIF->GetEnchantTactics()->GetSelectState();
	g_pGameIF->GetEnchantTactics()->RecvEnchantServer();	

	CDigimon::sENCHENT_STAT* pEState;
	pEState = ((CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat();

	if( nResult == 0 ) // 강화에 실패하였으나 변동은 없음 (보존)
	{
		cPrintMsg::PrintMsg( 30405 );
		g_pDatsCenter->EnchantFail( nSelect );
		return;
	}
	if( nResult == 99 ) // 강화 실패 했으나 백업 디스크 사용으로 손실 방지한 경우 변경된 파라미터 없음
	{
		cPrintMsg::PrintMsg( 30401 );
		g_pDatsCenter->EnchantFail( nSelect );
		return;
	}	

	// 강화 전 스텟 백업
	int nChangeAP = pEState->GetEnchantValue( ET_AT );
	int nChangeDP = pEState->GetEnchantValue( ET_BL );
	int nChangeCA = pEState->GetEnchantValue( ET_CR );
	int nChangeAS = pEState->GetEnchantValue( ET_AS );
	int nChangeEV = pEState->GetEnchantValue( ET_EV );
	int nChangeHT = pEState->GetEnchantValue( ET_HT );
	int nChangeHP = pEState->GetEnchantValue( ET_HP );

	u2 nInchantLevel = 0;
	pop( nInchantLevel );

	pEState->SetEnchantLevel( nInchantLevel );

	u2 nEnchantValue[ nExtendState::MaxExtendStat ] = {0, };
	u2 nEnchantLevel[ nExtendState::MaxExtendStat ] = {0, };

	pop( nEnchantValue[ nExtendState::ET_AT ] );
	pop( nEnchantValue[ nExtendState::ET_BL ] );
	pop( nEnchantValue[ nExtendState::ET_CR ] );
	pop( nEnchantValue[ nExtendState::ET_AS ] );
	pop( nEnchantValue[ nExtendState::ET_EV ] );
	pop( nEnchantValue[ nExtendState::ET_HT ] );	
	pop( nEnchantValue[ nExtendState::ET_HP ] );	

	pop( nEnchantLevel[ nExtendState::ET_AT ] );
	pop( nEnchantLevel[ nExtendState::ET_BL ] );
	pop( nEnchantLevel[ nExtendState::ET_CR ] );
	pop( nEnchantLevel[ nExtendState::ET_AS ] );
	pop( nEnchantLevel[ nExtendState::ET_EV ] );
	pop( nEnchantLevel[ nExtendState::ET_HT ] );	
	pop( nEnchantLevel[ nExtendState::ET_HP ] );

	for( int i = nExtendState::ET_AT; i < nExtendState::MaxExtendStat; ++i )
	{
		pEState->SetEnchantInfo( i, nEnchantLevel[ i ], nEnchantValue[ i ] );
	}

	nChangeAP -= nEnchantValue[ nExtendState::ET_AT ];
	nChangeDP -= nEnchantValue[ nExtendState::ET_BL ];
	nChangeCA -= nEnchantValue[ nExtendState::ET_CR ];
	nChangeAS -= nEnchantValue[ nExtendState::ET_AS ];
	nChangeEV -= nEnchantValue[ nExtendState::ET_EV ];
	nChangeHT -= nEnchantValue[ nExtendState::ET_HT ];
	nChangeHP -= nEnchantValue[ nExtendState::ET_HP ];

	CDigimonUser::sUSER_STAT* pUserStat = (CDigimonUser::sUSER_STAT*)g_pCharMng->GetDigimonUser( 0 )->GetBaseStat();
	FMDigimon::sDINFO info;
	info.s_nDigimonID		= g_pCharMng->GetDigimonUser( 0 )->GetFTID();
	info.s_nLevel			= pUserStat->GetLevel();
	info.s_fGameOrgScale	= g_pCharMng->GetDigimonUser( 0 )->GetOrgGameScale();

	TCHAR szChange[ 128 ];	
	if( nChangeAP != 0 )
	{	
		int nBase = FMDigimon::GetBaseAtt( &info ) + FMDigimon::GetEvolAtt( &info );
		float fValue = ( nBase * (nEnchantValue[ nExtendState::ET_AT ] / 100.0f) ) * 10.0f;
		_stprintf_s( szChange, 128, L"AT %d", (int)(fValue / 10) );
	}
	else if( nChangeDP != 0 )
	{
		_stprintf_s( szChange, 128, L"BL %d%%", nEnchantValue[ nExtendState::ET_BL ] );
	}
	else if( nChangeCA != 0 )
	{
		float fValue = ( FMDigimon::GetBaseCr( &info ) * nEnchantValue[ nExtendState::ET_CR ] / 10000.0f ) * 10.0f;
		_stprintf_s( szChange, 128, L"CT %.2f%%", fValue / 10.0f  );
	}	
	else if( nChangeEV != 0 )
	{
		float fValue = ( FMDigimon::GetBaseEv( &info ) * nEnchantValue[ nExtendState::ET_EV ] / 10000.0f ) * 10.0f;
		_stprintf_s( szChange, 128, L"EV %.2f%%", fValue / 10.0f  );		
	}
	else if( nChangeHP != 0 )
	{
		// 여기에 변화된 수치 표시
		int nBase = FMDigimon::GetBaseMaxHP( &info ) + FMDigimon::GetEvolMaxHP( &info );
		float fValue = ( nBase * (nEnchantValue[ nExtendState::ET_HP ] / 100.0f) ) * 10.0f;
		_stprintf_s( szChange, 128, L"HP %d", (int)(fValue / 10) );
	}
	/*else if( nChangeAS != 0 )
	{
		_stprintf_s( szChange, 128, L"AS %d%% -> %d%%", pDBase->GetAttSpeed_Enchant(), pDBase->GetAttSpeed_Enchant() + nChangeAS );
	}
	else if( nChangeHT != 0 )
	{
		_stprintf_s( szChange, 128, L"HR %d%% -> %d%%", pDBase->GetHitRate_Enchant(), pDBase->GetHitRate_Enchant() + nChangeHT );
	}*/
	else
	{
		//강화에 성공이나 실패했는데 변동수치가 0인경우 위에서 별도로 처리한 result값에 들지 않고
		assert_csm( false, _T( "강화 변동수치가 존재하지 않음" ) );
		return;
	}
	

	// 1~ 6까지는 강화 실패로 해당 게이지가 손실된 것 입니다.
	switch( nResult )
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		{	// 디지몬 강화 실패
			cPrintMsg::PrintMsg( 30402, szChange );
			g_pDatsCenter->EnchantFail( nSelect );
		}
		break;	
	case 100:	// 강화 성공			
		{
			cPrintMsg::PrintMsg( 30400, szChange );
			g_pDatsCenter->EnchantSuccess( nSelect );
			GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_ENCHANT_COMPLETE, NULL );
		}
		break;
	default:
		{
			assert_csm1( false, _T( "강화 결과 잘못된값이 날라옴 : %d" ), nResult ); 
			return;
		}	
		break;
	}

	g_pGameIF->GetEnchantTactics()->SetNextEnchantText();

	GS2C_RECV_ENCYCLOPEDIA_INCHANTRESULT recv;
	recv.nDigimonID = g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID();
	recv.nAPLevel = nEnchantLevel[ nExtendState::ET_AT ];
	recv.nCALevel = nEnchantLevel[ nExtendState::ET_CR ];
	recv.nEVLevel = nEnchantLevel[ nExtendState::ET_EV ];
	recv.nDPLevel = nEnchantLevel[ nExtendState::ET_BL ];
	recv.nHPLevel = nEnchantLevel[ nExtendState::ET_HP ];


	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::ENCYCLOPEDIA_INCHANTRESULT, &recv );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL );
}

void cCliGame::SendDecreaseDigimonInchant(const UINT& nType, const int& nItemSlot)
{
	// nType 1: AT 2: DE 3: CR 4: AS 5: EV 6: HT
	// nType 강화 타입에 0 값은 사용하지 않습니다.	

	newp( pGame::DecreaseDigimonInchant );
		push( nType );
		push( nItemSlot );
	endp( pGame::DecreaseDigimonInchant );
	send();
}

void cCliGame::RecvDecreaseDigimonInchantResult()
{
	CDigimon::sENCHENT_STAT* pEState;
	pEState = ((CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat();

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCHANT_TACTICS ) )
	{		
		g_pGameIF->GetEnchantTactics()->RecvWeakServer();
	}

	u2 nInchantLevel = 0;	// 1 ~ 24
	pop( nInchantLevel );
	pEState->SetEnchantLevel( nInchantLevel );

	u2 nEnchantValue[ nExtendState::MaxExtendStat ] = {0, };
	u2 nEnchantLevel[ nExtendState::MaxExtendStat ] = {0, };

	pop( nEnchantValue[ nExtendState::ET_AT ] );
	pop( nEnchantValue[ nExtendState::ET_BL ] );
	pop( nEnchantValue[ nExtendState::ET_CR ] );
	pop( nEnchantValue[ nExtendState::ET_AS ] );
	pop( nEnchantValue[ nExtendState::ET_EV ] );
	pop( nEnchantValue[ nExtendState::ET_HT ] );	
	pop( nEnchantValue[ nExtendState::ET_HP ] );	

	pop( nEnchantLevel[ nExtendState::ET_AT ] );
	pop( nEnchantLevel[ nExtendState::ET_BL ] );
	pop( nEnchantLevel[ nExtendState::ET_CR ] );
	pop( nEnchantLevel[ nExtendState::ET_AS ] );
	pop( nEnchantLevel[ nExtendState::ET_EV ] );
	pop( nEnchantLevel[ nExtendState::ET_HT ] );
	pop( nEnchantLevel[ nExtendState::ET_HP ] );

	for( int i = nExtendState::ET_AT; i < nExtendState::MaxExtendStat; ++i )
	{
		pEState->SetEnchantInfo( i, nEnchantLevel[ i ], nEnchantValue[ i ] );
	}

	g_pDatsCenter->WeakSuccess();

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL );
}