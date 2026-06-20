
#include "stdafx.h"

#include "cCliGame.h"

#include "common_vs2019/pBuddy.h"
#include "common_vs2019/pParty.h"
#include "common_vs2019/pLength.h"


void cCliGame::RecvItemCropParty(void)
{
	// 아이템 줏어먹는 모션처리는 RecvItemCropSuccess()에서 해준다
	// 여기서는 파티원이 줏어먹은 아이템 메세지처리만 하자	

	GS2C_NTF_PARTY_GET_ITEM ntfPartyItemGet;
	pop(ntfPartyItemGet.m_nRealCropTamerIDX);

	u4 itemType;
	n2 itemCount;
	u1 itemRate;

	pop(itemType);
	pop(itemCount);
	pop(itemRate);

	ntfPartyItemGet.m_GetItem.m_nType=itemType;
	ntfPartyItemGet.m_GetItem.m_nCount=itemCount;
	ntfPartyItemGet.m_GetItem.m_nRate=itemRate;
	ntfPartyItemGet.m_GetItem.m_nLevel = 0;
	ntfPartyItemGet.m_GetItem.m_nLimited = 0;	
	
	pop(ntfPartyItemGet.m_szRealCropTamerName);	// 아이템을 획득한 사람
	
	n1 nDiceCount;			// 주사위 갯수
	pop(nDiceCount);		// 사용된 주사위 갯수

	if( nDiceCount > 0 )
	{
		for( int n = 0; n < nDiceCount; ++n ) // 채팅 메시지에 결과 출력
		{
			TDiceResult result;
			pop(result.m_nDiceNumber);
			pop(result.m_szDiceTamerName);
			ntfPartyItemGet.m_DiceResult.push_back( result );
		}
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::NTF_PARTY_MEMBER_LOOT_ITEM, &ntfPartyItemGet );


// 	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemType )->GetInfo();	
// 
// 
// 
// 
// 	int nMaxNum = 0;
// 	int nMyNum = 0;
// 
// 
// 	// 게임중일때
// 	if( g_pCharMng->GetTamerUser() != NULL )
// 	{
// 		if( nDiceCount > 0 ) // 아이템 획득방식이 랜덤획득일시에..
// 		{
// 			for(int i=0; i<nDiceCount; i++)
// 			{
// 				n1 nDiceNumber;
// 				pop(nDiceNumber);
// 
// 				std::wstring szDiceTamerName;
// 				pop(szDiceTamerName);
// 
// 				assert_cs( nDiceNumber != 0 );
// 				if( nDiceNumber > nMaxNum )
// 					nMaxNum = nDiceNumber;
// 
// 				if( szDiceTamerName.compare(g_pCharMng->GetTamerUser()->GetName() ) == 0 )
// 				{
// 					nMyNum = nDiceNumber;
// 				}			
// 
// 				TCHAR szMessage[MAX_PATH];
// 				_stprintf_s( szMessage, MAX_PATH, _LAN( "%s님이 %s아이템에 대해 %d의 랜덤수가 나왔습니다." ), szDiceTamerName.c_str(), pFTItem->s_szName, nDiceNumber );				
// 				ContentsStream kStream;
// 				wstring str = szMessage;
// 				bool bParcing = false;	bool bCheckSameString = false;
// 				kStream << str << bParcing << bCheckSameString;
// 				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_SYSTEM_PROCESS, &kStream );
// 			}
// 		}
// 
// 		if( szRealCropTamerName.compare(g_pCharMng->GetTamerUser()->GetName()) == 0 ) // 파티아이템을 내가 먹은경우
// 		{
// 			// [4/26/2016 hyun] 메세지는 cCliGame::RecvItemCropSuccess() 함수를 통해서 출력한다
// // 			assert_csm( item.m_nType != 0, _ERROR_MSG( "RecvItemCropParty 아이템 타입오류" ) );
// // 			assert_csm( nsCsFileTable::g_pItemMng->IsItem( item.m_nType ), _ERROR_MSG( "RecvItemCropParty 아이템 타입오류" ) );
// // 			//int nRemain = g_pDataMng->GetInven()->ItemCrop( &item );
// // 			CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( item.m_nType )->GetInfo();
// // 			// [4/14/2016 hyun] 수정. 수정. 아이템 획득시 컬러값으로 출력을 위해
// // 			if(pFTItem)
// // 			{
// // 				int GetItemCount = static_cast<int>(nItemCount);
// // 				if(GetItemCount == 1)
// // 					cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_CROP, pFTItem->s_szName, &g_pDataMng->GetItemColor(pFTItem->s_nClass) );
// // 				else if(GetItemCount > 1)
// // 					cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_MANYCROP, pFTItem->s_szName, &GetItemCount, &g_pDataMng->GetItemColor(pFTItem->s_nClass));
// // 			}
// // 
// // 			// @@ 여기까지
// // 			if( pFTItem->IsNoticeItem() )
// // 			{
// // 				cDigimonTalk::Print( 50008 );
// // 			}
// 			// @@ 여기까지
// 		}	
// 		else // 파티아이템을 다른사람이 먹은경우
// 		{
// 			cPrintMsg::PrintMsg( 11024, const_cast<TCHAR*>(szRealCropTamerName.c_str()), pFTItem->s_szName );
// 		}
// 
// 		bool bLoot = false;	
// 		switch( g_pDataMng->GetParty()->GetPartyCropDispRate() )
// 		{
// 		case nCrop::RareRate::Lv1:
// 			bLoot = true;
// 			break;
// 		case nCrop::RareRate::Lv2:
// 			bLoot = ( pFTItem->s_nClass >= 3 ) ? true : false;
// 			break;
// 		case nCrop::RareRate::Lv3:
// 			bLoot = ( pFTItem->s_nClass >= 4 ) ? true : false;
// 			break;
// 		case nCrop::RareRate::Lv4:
// 			bLoot = ( pFTItem->s_nClass >= 5 ) ? true : false;
// 			break;
// 		}
// 
// 		if( bLoot == true )
// 		{			
// 			cData_Party::sITEMLOOT* pInfo = NiNew cData_Party::sITEMLOOT;
// 			pInfo->s_dwItemID = nItemType;
// 			pInfo->s_nPartyIdx = g_pDataMng->GetParty()->GetPartyWinIndex( szRealCropTamerName.c_str() ) + 1;
// 			pInfo->s_nDiceNum = nMaxNum;
// 			pInfo->s_nMyDiceNum = nMyNum;
// 			_tcscpy_s( pInfo->s_szName, Language::pLength::name + 1, szRealCropTamerName.c_str() );
// 			g_pDataMng->GetParty()->AddPartyLoot( pInfo );
// 		}
// 	}
// 	else
// 	{
// 		cPrintMsg::PrintMsg( 11024, const_cast<TCHAR*>(szRealCropTamerName.c_str()), pFTItem->s_szName );
// 	}
}


void cCliGame::SendPartyInvite(std::wstring const & szTamerName)
{	
	newp(pParty::Invite);
		push( szTamerName );
	endp(pParty::Invite);
	send();
}


void cCliGame::SendPartyLeave(void)
{
	newp(pParty::Leave);
	endp(pParty::Leave);
	send();
}


void cCliGame::RecvPartyLeave(void)
{
	GS2C_RECV_PARTY_MEMBER_LEAVE recv;
	pop(recv.m_cLeaveMemberSlotNum);
	// 자기 자신거는 안들어옴
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PARTY_LEAVE, &recv );

// 	
// 	// 파티에서 나감 
// 	g_pDataMng->GetParty()->DelParty( nSlotNo, cData_Party::LT_LEAVE );
}


// 파티원 강퇴
void cCliGame::RecvPartyKick(void)
{
	GS2C_RECV_PARTY_MEMBER_BANISH recv;
	pop(recv.m_cBanishMemberSlotNum);
	// 자기 자신거 들어옴
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PARTY_MEMBER_BANISH, &recv );

// 	// 파티에서 탈퇴시킴, 해당 슬롯을 비움
// 	g_pDataMng->GetParty()->DelParty( nSlotNo, cData_Party::LT_KICK );
	//CHAT_TEXT2( _T( "pParty::Kick slot:%d" ), nSlotNo );
}


void cCliGame::SendPartyKick(std::wstring const& szTamerName)
{
	//std::wstring tamerName = szTamerName;
	newp(pParty::Kick);
		push( szTamerName );
	endp(pParty::Kick);
	send();
}


void cCliGame::SendPartyChat(TCHAR const* szMsg)
{
// 	// 파티 아닐때는 패스
// 	if( g_pDataMng->GetParty()->IsJoinParty() == false )
// 	{
// 		cPrintMsg::PrintMsg( 30078 );
// 		return;
// 	}

	std::wstring msg(szMsg);

	newp(pParty::Chat);
		push( msg );
	endp(pParty::Chat);
	send();
}


void cCliGame::RecvPartyChat(void)
{
	std::wstring szTamerName;
	std::wstring szMsg;

	pop(szTamerName);
	pop(szMsg);

	CsC_AvObject* pTamer = g_pCharMng->GetTamerUser();
	if( pTamer && ( g_pResist->m_Global.s_bChatBalloone == true ) )
	{
		if( szTamerName.compare( pTamer->GetName() ) == 0 )
		{
			g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_1, g_pCharMng->GetTamerUser()->GetUniqID(), szMsg.c_str() );
		}
		else
		{
// 			cData_Party::sINFO* pPartyInfo = g_pDataMng->GetParty()->GetParty_FromName( szTamerName.c_str() );
// 			if( pPartyInfo )
// 			{
// 				CsC_AvObject* pObject = g_pMngCollector->GetObject( pPartyInfo->s_TamerStat.s_nUniqID );
// 				if( ( pObject )&&( pObject->GetProp_Alpha()->IsHideDistOut() == false ) )
// 				{
// 					g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_1, pPartyInfo->s_TamerStat.s_nUniqID, szMsg.c_str() );
// 				}
// 			}
		}
	}

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::PARTY;
	CProtocol.m_wStr = szTamerName;
	CProtocol.m_wStr += L" : ";
	CProtocol.m_wStr += szMsg;

	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}


void cCliGame::RecvPartyExp(void)
{
	GS2C_NTF_PARTY_EXP_UPDATE recv;
	pop(recv.m_nTamerGainExp);
	pop(recv.m_nTamerBonusExp);
	pop(recv.m_nTamerExp);
	pop(recv.m_nDigimonUID);	// 소유 디지몬이 여러 마리일 경우 처리를 위함
	pop(recv.m_nDigimonGainExp);
	pop(recv.m_nDigimonBonusExp);
	pop(recv.m_nDigimonExp);	// 
	pop(recv.m_nSkillExp);
	pop(recv.m_szOfferTamerName);	// 제공자 테이머 이름
	GAME_EVENT_ST.OnEvent( EVENT_CODE::NTF_PARTY_EXP_UPDATE, &recv );
}

//////////////////////////////////////////////////////////////////////////
// 파티 초대 거부 패킷
//////////////////////////////////////////////////////////////////////////
void cCliGame::SendPartyInviteResult( int const& nResultType, std::wstring const& targetName )
{
	newp(pParty::InviteResult);
	push(nResultType);
	push(targetName);
	endp(pParty::InviteResult);
	send();
}
// 
// void cCliGame::SendPartyInviteImpossible(int const& nResultType, std::wstring const& targetName)
// {
// 	newp(pParty::InviteResult);
// 		push(-3);	// 다른 작업으로 인해 파티 요청 처리 불가능
// 		push(m_szPartyInviteTargetTamerName);
// 	endp(pParty::InviteResult);
// 	send();
// }
// 
// //////////////////////////////////////////////////////////////////////////
// // 파티 초대 수락 패킷
// //////////////////////////////////////////////////////////////////////////
// void cCliGame::SendPartyInviteAccept(void)
// {
// 	newp(pParty::InviteResult);
// 		push(1);	// 요청에 대한 수락 응답
// 		push(m_szPartyInviteTargetTamerName);
// 	endp(pParty::InviteResult);
// 	send();
// }
// 
// 
// void cCliGame::SendPartyInviteReject(void)
// {
// 	newp(pParty::InviteResult);
// 		push(-1);	// 요청에 대한 거부 응답
// 		push(m_szPartyInviteTargetTamerName);
// 	endp(pParty::InviteResult);
// 	send();
// }


void cCliGame::RecvPartyInvite(void)	// 파티 초대 요청 받음 
{
	GS2C_RECV_PARTY_MEMBER_INVITE_REQUEST recv;
	pop(recv.m_wsInviterTamerName);	// 요청자 테이머 이름
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PARTY_INVITE_ASK, &recv );
}


void cCliGame::RecvPartyInviteResult(void)
{
	GS2C_RECV_PARTY_MEMBER_INVITE_RESULT recv;
	pop(recv.m_nResultType);
	pop(recv.m_wsTargetTamerName);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PARTY_INVITE_RESULT, &recv );
}


void cCliGame::RecvPartyCreated(void)
{
	GS2C_RECV_PARTY_MAKE recv;
	pop(recv.m_nPartyIDX);
	pop(recv.m_nCropType );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PARTY_MAKE_ME, &recv );	

// 	// 본인의 슬롯 번호는 0번
// 	// 파티장의 슬롯 번호 : 0번 ==> 본인이 파티장임
// 	g_pDataMng->GetParty()->SetPartyID( nPartyNo );
// 	g_pDataMng->GetParty()->SetHostSlotIndex( 0 );
// 	g_pDataMng->GetParty()->SetMySlotIndex( 0 );
// 
// 	g_pDataMng->GetParty()->SetPartyCropType( nCrop::Base );
// 	g_pDataMng->GetParty()->SetPartyCropRate( 0 );
// 	g_pDataMng->GetParty()->SetPartyCropDispRate( nCrop::RareRate::Lv1 );
// 	cPrintMsg::PrintMsg( 30083, _LAN( "선행 공격자 획득" ) );
// 	cPrintMsg::PrintMsg( 30085 );
}


void cCliGame::RecvPartyJoin(void)
{
	GS2C_RECV_ADD_NEW_PARTY_MEMBER recv;

	// 새로 가입한 멤버에 대한 정보를 처리
	n4 nSlot;
	pop(nSlot);			// 몇번째 슬롯인지
	recv.m_memberInfo.m_nSlotNo = nSlot;
	pop(recv.m_memberInfo.m_TamerInfo.m_nType);
	pop(recv.m_memberInfo.m_TamerInfo.m_nLevel);	
	pop(recv.m_memberInfo.m_TamerInfo.m_Name);
	pop(recv.m_memberInfo.m_DigimonInfo.m_nType);
	pop(recv.m_memberInfo.m_DigimonInfo.m_nLevel);
	pop(recv.m_memberInfo.m_DigimonInfo.m_Name);
	pop(recv.m_memberInfo.m_nMapNo);
	pop(recv.m_memberInfo.m_nChannelNo);
	pop(recv.m_memberInfo.m_TamerInfo.m_nUID);		// 값이 0인 경우 해당 맵에 존재하지 아니함
	pop(recv.m_memberInfo.m_DigimonInfo.m_nUID);	// 값이 0인 경우 해당 맵에 존재하지 아니함

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PARTY_ADD_NEW_MEMBER, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 파티 아이템 루팅 정보 변경
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvPartyChangeCropType(void)
{
	GS2C_RECV_PARTY_LOOTING_INFO_CHANGE recv;

	n4 nCropType;
	pop(nCropType);
	recv.m_lootingInfo.m_nCropType = nCropType;
	pop(recv.m_lootingInfo.m_nRareRate);
	pop(recv.m_lootingInfo.m_nDispRareGrade);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PARTY_ITEM_LOOTING_INFO_CHANGE, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 파티 아이템 루팅 정보 변경
//////////////////////////////////////////////////////////////////////////
void cCliGame::SendPartyChangeCropType(int nCropType, n1 nItemClass, n1 nItemDispGrade)	// pParty.h ==> nCrop::Base, nCrop::Free, nCrop::Order, nCrop::RareRate
{
// 	// 거래 중이면 실패
// 	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
// 	{
// 		cPrintMsg::PrintMsg( 30024 );
// 		return;
// 	}
// 
// 	// 파티 아닐때는 패스
// 	if( g_pDataMng->GetParty()->IsJoinParty() == false )
// 	{
// 		cPrintMsg::PrintMsg( 30078 );
// 		return;
// 	}
// 
// 	// 지금 아이템 분배 방식이면 패스
// 	if( g_pDataMng->GetParty()->GetPartyCropType() == nCropType &&
// 		g_pDataMng->GetParty()->GetPartyCropRate() == nItemClass &&
// 		g_pDataMng->GetParty()->GetPartyCropDispRate() == nItemDispGrade )
// 		return;

	// 파티장만 사용 가능
	// nCropType == nCrop::Base  ==> 선행 공격자 우선 습득
	// nCropType == nCrop::Free  ==> 파티원 누구나 습득 nItemClass 0 
	// nCropType == nCrop::Free  ==> 해당 레어도 이상 아이템은 주사위 처리 nItemClass 1~4
	// nCropType == nCrop::Order ==> 순서대로 줍기	

	newp(pParty::ChangeCropType);
		push(nCropType);
		push(nItemClass);	// 아이템 레어도
		push(nItemDispGrade);
	endp(pParty::ChangeCropType);

	send();
}


void cCliGame::RecvPartyMemberList(void)
{
	GS2C_RECV_PARTY_INFO recv;
	pop(recv.m_nPartyIDX);		// 파티 식별자	
	pop(recv.m_nMySlotNo);
	pop(recv.m_nMasterSlotNo);
	
	// 파티 루팅 정보
	pop(recv.m_lootingInfo.m_nCropType);
	pop(recv.m_lootingInfo.m_nRareRate);
	pop(recv.m_lootingInfo.m_nDispRareGrade);

	n4 nSlotNo;
	pop(nSlotNo);

	while(nSlotNo != -1)
	{
		TPartyMember memberInfo;
		memberInfo.m_nSlotNo = nSlotNo;

		pop(memberInfo.m_TamerInfo.m_nType);
		pop(memberInfo.m_TamerInfo.m_nLevel);
		pop(memberInfo.m_TamerInfo.m_Name);

		pop(memberInfo.m_DigimonInfo.m_nType);
		pop(memberInfo.m_DigimonInfo.m_nLevel);
		pop(memberInfo.m_DigimonInfo.m_Name);

		pop(memberInfo.m_nMapNo);
		pop(memberInfo.m_nChannelNo);

		pop(memberInfo.m_TamerInfo.m_nUID);		// 0이면 현재 맵에 존재하지 아니함
		pop(memberInfo.m_DigimonInfo.m_nUID);	// 0이면 현재 맵에 존재하지 아니함

		recv.m_listMemberInfo.push_back( memberInfo );

		pop(nSlotNo);		
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PARTY_INFO, &recv );
}


void cCliGame::RecvPartyDie(void)
{
	n1 nSlotNo;
	pop(nSlotNo);
}

// 파티원 위치 갱신 패킷
void cCliGame::RecvPartySyncPosition(void)
{
	GS2C_NTF_PARTY_MEMBER_POSITION_UPDATE recv;

	pop(recv.m_nSlotNo);
	pop(recv.m_tPos);
	pop(recv.m_dPos);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::NTF_PARTY_MEMBER_POSITION_UPDATE, &recv );
}


// 파티원 정보 갱신 패킷
void cCliGame::RecvPartySyncInfo(void)	// hp, mhp, ds, mds 동기화
{
	GS2C_NTF_PARTY_MEMBER_INFO_CHANGE recv;

	pop(recv.m_nSlotNo);
	pop(recv.m_nDigimonType);

	pop(recv.m_TamerBaseInfo.m_nHP);
	pop(recv.m_TamerBaseInfo.m_nMHP);
	pop(recv.m_TamerBaseInfo.m_nDS);
	pop(recv.m_TamerBaseInfo.m_nMDS);

	pop(recv.m_DigimonBaseInfo.m_nHP);
	pop(recv.m_DigimonBaseInfo.m_nMHP);
	pop(recv.m_DigimonBaseInfo.m_nDS);
	pop(recv.m_DigimonBaseInfo.m_nMDS);

	pop(recv.m_nTamerLevel);
	pop(recv.m_nDigimonLevel);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::NTF_PARTY_MEMBER_INFO_CHANGE, &recv );

}

//////////////////////////////////////////////////////////////////////////
// 파티원 맵 이동
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvPartyChangeMap(void)
{
	GS2C_NTF_PARTY_MEMBER_MAP_CHANGE recv;	
	pop(recv.m_nSlotNo);
	pop(recv.m_nMapNo);
	pop(recv.m_nChannelNo);

	pop(recv.m_nTamerUID);
	pop(recv.m_nDigimonUID);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_PARTY_MEMBER_MAP_CHANGE, &recv);
}

// 파티장 변경 패킷
void cCliGame::SendPartyChangeMaster(int nNextMasterSlotNo)
{
	newp(pParty::ChangeMaster);
		push(nNextMasterSlotNo);
	endp(pParty::ChangeMaster);
	send();
}


void cCliGame::RecvPartyChangeMaster(void)
{
	// 자기 자신에게도 들어옴
	GS2C_RECV_PARTY_MASTER_CHANGE recv;
//	n4 nMasterSlotNo;
	pop(recv.m_nSlotNo);
	
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PARTY_CHANGE_MASTER, &recv );
}

// 파티원 디지몬 변경
void cCliGame::RecvPartyChangeDigimon(void)
{
	GS2C_NTF_PARTY_MEMBER_DIGIMON_CHANGE recv;

	pop(recv.m_nSlotNo);
	pop(recv.m_nDigimonType);
	pop(recv.m_szDigimonName);

	u2 nHP, nDS, nMHP, nMDS;
	pop(nHP);
	pop(nMHP);
	pop(nDS);
	pop(nMDS);

	recv.m_DigimonBaseInfo.m_nHP = nHP;
	recv.m_DigimonBaseInfo.m_nMHP = nMHP;
	recv.m_DigimonBaseInfo.m_nDS = nDS;
	recv.m_DigimonBaseInfo.m_nMDS = nMDS;

	GAME_EVENT_ST.OnEvent( EVENT_CODE::NTF_PARTY_MEMBER_DIGIMON_CHANGE, &recv );
}

// 파티원 접속 종료
void cCliGame::RecvPartyDisconnected(void)
{
 	GS2C_NTF_PARTY_MEMBER_ONOFFLINE recv;
 	pop(recv.m_nSlotNo);	 // 해당 슬롯 유저는 접속 종료되었음
 	// 현재는 파티원이 접속 종료하게 되면 파티에서 탈퇴 시킨다.
 	recv.m_bOnline = false;
 	GAME_EVENT_ST.OnEvent( EVENT_CODE::NTF_PARTY_MEMBER_ONOFFLINE, &recv );
}

// 파티원 버프 갱신
void cCliGame::RecvPartySyncBuff(void)
{
	GS2C_NTF_PARTY_MEMBER_BUFF_CHANGE recv;
	pop(recv.m_nSlotNo);

	n2 nBuffCount;
	pop(nBuffCount);

	for( int i =0; i < nBuffCount; ++i )
	{
		TPartyMemberBuffInfo addInfo;
		pop(addInfo.m_nStatus);
		pop(addInfo.m_nBuffCode);
		recv.m_listBuffInfo.push_back( addInfo );
	}
	GAME_EVENT_ST.OnEvent( EVENT_CODE::NTF_PARTY_MEMBER_BUFF_CHANGE, &recv );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void cCliGame::SendBuddyList(void)	// 버디(친구 및 차단 포함) 목록 요청
{
	newp(pBuddy::List);
	endp(pBuddy::List);
	send();
}


void cCliGame::SendBuddyAdd(std::wstring const& szTamerName)
{
	newp(pBuddy::Add);
		push( szTamerName );
	endp(pBuddy::Add);
	send();
}


void cCliGame::SendBuddyDelete(std::wstring const& szTamerName)
{
	newp(pBuddy::Delete);
		push( szTamerName );
	endp(pBuddy::Delete);
	send();
}


void cCliGame::SendBuddyMemo(std::wstring const& szTamer, std::wstring const& szMemo)
{
	newp(pBuddy::Memo);
		push( szTamer );
		push( szMemo );
	endp(pBuddy::Memo);

	send();
}


void cCliGame::RecvBuddyAdd(void)		// 친구 등록 성공
{
	GS2C_RECV_ADDINFO addfriend;
	pop(addfriend.s_bOnlineState);
	pop(addfriend.s_szName);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_ADD_FRIEND, &addfriend);
}


void cCliGame::RecvBuddyBan(void)
{
	std::wstring addShutout;
	pop(addShutout);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_ADD_SHUTOUT, &addShutout);
}


void cCliGame::RecvBuddyOn(void)
{
	std::wstring szTamerName;
	pop(szTamerName);
	pair<bool, std::wstring> friendOnOffInfo = pair<bool, std::wstring>(true, szTamerName);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_FRIEND_ON_OFF, &friendOnOffInfo);
}


void cCliGame::RecvBuddyOff(void)
{
	std::wstring szTamerName;
	pop(szTamerName);
	pair<bool, std::wstring> friendOnOffInfo= pair<bool, std::wstring>(false, szTamerName);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_FRIEND_ON_OFF, &friendOnOffInfo);
}


void cCliGame::RecvBuddyList(void)
{
	u2 nfCnt;
	pop(nfCnt);	// 친구 개수
	GS2C_RECV_FRIENDLIST recvFriendList;
	for(int i=0; i<nfCnt; i++)
	{
		nsFriendInfo::sFriendInfo friendinfo;
		pop(friendinfo.s_eOnlineState);
		pop(friendinfo.s_szName);
		pop(friendinfo.s_szMemo);
		recvFriendList.m_FriendList.push_back(friendinfo);
	}
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_FRIENDLIST, &recvFriendList);
	u2 nsCnt;
	pop(nsCnt);	// 차단 개수
	GS2C_RECV_SHUTOUTLIST recvShutout;
	for(int i=0; i<nsCnt; i++)
	{
		nsShutout::sShutoutInfo shutoutinfo;
		pop(shutoutinfo.s_szName);
		pop(shutoutinfo.s_szMemo);
		recvShutout.m_ShoutoutList.push_back(shutoutinfo);
	}
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_SHUTOUTLIST, &recvShutout);
}



void cCliGame::RecvBuddyNotFound(void)	// 존재하지 않는 이름입니다., pBuddy::Add, pBuddy::Ban 의 결과
{
	GS2C_RECV_NOTFOUND NotFoundTamer;
	pop(NotFoundTamer.s_szName);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_NOT_FOUND, &NotFoundTamer);
}

void cCliGame::RecvBuddyChangeName(void)
{
	GS2C_RECV_NAME_CHANGE_FREIND_SHUTOUT changeName;
	
	pop( changeName.s_bIsban );
	pop( changeName.s_szPreName );
	pop( changeName.s_szNextName );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CHANGE_NAME_FRIEND_SHUTOUT, &changeName);
}
void cCliGame::SendBuddyBan(std::wstring const& szTamerName)	// 기존에 친구 목록에 있는넘도 같은 메세지로 처리
{
	newp(pBuddy::Ban);
		push( szTamerName );
	endp(pBuddy::Ban);
	send();
}











