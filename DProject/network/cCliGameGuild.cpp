
#include "stdafx.h"

#include "cCliGame.h"

#include "common_vs2019/pGuild.h"
#include "common_vs2019/pLength.h"
#include "common_vs2019/pGuild.h"

void cCliGame::RecvGuildRankChange(void)
{
	u2 nNextRank = 0;	// 현재 랭킹
	pop(nNextRank);

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::GUILD_RANK;
	CProtocol.m_wStr = UISTRING_TEXT( "COMMUNITY_CHANGE_GUILD_RANK" );
	DmCS::StringFn::Replace( CProtocol.m_wStr, L"#Num#", nNextRank );
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

 
bool cCliGame::SendChangeGuildNotice(wchar const * msg)
{
	std::wstring sendMsg(msg);

	// msg 문자열 길이는 250자를 넘지 아니한다.
	// msg 는 한문자라도 존재 해야만 한다.
	newp(pGuild::ChangeNotice);
		push( sendMsg );
	endp(pGuild::ChangeNotice);
	send();
	return true;
}

void cCliGame::RecvGuildChangeNotice(void)
{
	std::wstring msg;
	pop(msg);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDNOTICE_CHANGE, &msg );
}


void cCliGame::SendGuildCreate(TCHAR const*title, u2 nInvenSlot, u4 nNpcIDX)	// 새로운 길드를 생성함
{
	if( NULL == title )
		return;

	std::wstring sendTitle(title);
	newp(pGuild::Create);
		push( sendTitle );
		push(nInvenSlot);
		push(nNpcIDX);
	endp(pGuild::Create);
	send();

	// RecvGuildCreate에서 성공인 경우에만 아이템 제거
}

void cCliGame::RecvGuildCreate(void) //길드 생성은 주변 모든 유저에게 전달된다.
{
	std::wstring szTamerName;
	pop(szTamerName);

	//테이머가 본인이라면
	if( 0 == szTamerName.compare( g_pCharMng->GetTamerUser()->GetName() ) )
	{
		n4 nSlotNo;
		pop(nSlotNo);	// 지워질 아이템의 인벤토리 위치
		
		bool bSuccess = false;
		if(nSlotNo < 0)	// 0보다 작은 값이면 실패
		{
			// 길드 생성 실패 - 이미 사용중인 이름	
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_CREATE, 0, &bSuccess );					
			return;
		}

		// 길드 생성 성공
		// 인벤토리의 nSlotNo 위치에 존재하는 아이템 1개를 제거한다.
		bSuccess = true;
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_CREATE, 0, &bSuccess );		
	}	

	std::wstring szGuildName;
	pop(szGuildName);

	// 길드 정보 갱신
	CTamer* pTamer = (CTamer*)g_pMngCollector->GetObject( RTTI_TAMER, szTamerName.c_str() );
	SAFE_POINTER_RET(pTamer);
	pTamer->GetCharOption()->SetGuildName( szGuildName.c_str() );
}


void cCliGame::SendGuildHistory(void)
{
	newp(pGuild::History);
	endp(pGuild::History);
	send();
}


void cCliGame::RecvGuildHistory(void)
{
	GS2C_GUILD_HISTORY historyList;

	n1 nType;
	pop(nType);
	while(nType)
	{
		GuildInfo::sGuild_History historyElement;
		historyElement.nType = nType;

		pop(historyElement.nTimeTS);
		pop(historyElement.nMasterClass);
		pop(historyElement.szMasterName);
		pop(historyElement.nMemberClass);
		pop(historyElement.szMemberName);
		historyElement.nTime = net::game->GetTimeTS() - historyElement.nTimeTS;

		historyList.m_GuildHistoryList.push_back(historyElement);

		pop(nType);
	}
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_HISTORY, &historyList );
}


void cCliGame::SendGuildInfo(void)	// 가급적 자주 호출 하지 아니한다.
{	
	cPacket p;
	p.newp(pGuild::Info);
	p.endp(pGuild::Info);
	DoSend(p);
}


// 1. 본인 요청시 수신됨
// 2. 본인이 길드 가입이 될 경우 수신됨
// 3. 맵이동시 수신됨 (게임 init 포함)
void cCliGame::RecvGuildInfo(void)
{

	GS2C_GUILDINFO guildInfo;
	//std::wstring wsGuildName;
	pop(guildInfo.wsGuildName);
	pop(guildInfo.nGuildIDX);
	pop(guildInfo.nGuildLevel);
	pop(guildInfo.nGuildExp);

	pop(guildInfo.wsGuildNotice);
	pop(guildInfo.nIncMaxMember);

#ifdef GUILD_RENEWAL
	pop( guildInfo.nGSP );
	/************************************************************************/
	/*					사용중인 길드 스킬 정보(Base포함)					*/
	/************************************************************************/
	// Active스킬 확인, 사용중인걸 받아오자
	u1 nCnt = 1;
	pop(nCnt);
	while( nCnt )
	{
		u4 nSkillCode = 2600001;
		u4 nStartTime = _TIME_TS;// 액티브스킬 : 사용 시간  베이스스킬 : 스킬 시간
		std::pair<unsigned int, unsigned int> guildSkilInfoElement;
		pop(guildSkilInfoElement.first);
		pop(guildSkilInfoElement.second);
		guildInfo.m_GuildSkilList.push_back(guildSkilInfoElement);
		nCnt--;
	}
#endif	// GUILD_RENEWAL
	for(int i=0; i<nLimit::GuildGrade; i++)
	{
		// name / descript
		std::pair<std::wstring, std::wstring> guildGradeInfoElement;

		pop(guildGradeInfoElement.first);	// 0:마스터 1:부마스터 2:닷트길드원 3:일반길드원 4:신입길드원
		pop(guildGradeInfoElement.second);	

		guildInfo.m_GuildGradeInfoList.push_back(guildGradeInfoElement);
	}
	
	u1 nClass;
	pop(nClass);
	while(nClass)
	{
		GuildInfo::sGuild_PersonInfo guildPersonInfo;
		guildPersonInfo.nClass = nClass;
		u1 nTamerTypeO;
		pop(nTamerTypeO);
		guildPersonInfo.nTamerType = nTamerTypeO + 80000;

		pop(guildPersonInfo.wsName);
		pop(guildPersonInfo.nMemberExp);	// 길드 멤버의 길드에 대한 기여도
		pop(guildPersonInfo.nTamerLevel);	// 테이머 레벨, 레벨이 0인 경우 offline 처리

		pop(guildPersonInfo.nMapNO);		// nMapNO 값이 0 인 경우는 무시
		if(guildPersonInfo.nMapNO)
			pop(guildPersonInfo.nChannelNO);	// 채널은 맵번호가 존재하는 경우에만 수신됨
#ifdef GUILD_RENEWAL
		pop(guildPersonInfo.nRank);
		pop(guildPersonInfo.nLastTime);
		pop( guildPersonInfo.wsMemo );
#endif

		pop(nClass);
		guildInfo.m_GuildPersonInfoList.push_back(guildPersonInfo);
	}
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDINFO, &guildInfo );
}


void cCliGame::SendGuildChat(TCHAR *msg)
{
	nBase::rtrim(msg);

	cPacket p;
	p.newp(pGuild::Chat);
		p.push( msg );
	p.endp(pGuild::Chat);

	DoSend(p);
}


void cCliGame::RecvGuildLevelUp(void)
{
#ifdef GUILD_RENEWAL
	GS2C_GUILD_LEVELUP guildLevelUp;
	pop( guildLevelUp.nTamerUID );
	pop( guildLevelUp.nLevel );
	pop( guildLevelUp.nSkillCode );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_LEVELUP, &guildLevelUp );
#endif	// GUILD_RENEWAL
}

void cCliGame::RecvGuildMemberLevelUp(void)
{
	GS2C_GUILD_MEMBERLEVELUP memberLevelUp;
	
	pop(memberLevelUp.wsTamerName);
	pop(memberLevelUp.nTamerLevel);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_MEMBER_LEVELUP, &memberLevelUp );
}


void cCliGame::RecvGuildChat(void)
{
	std::wstring szFromTamerName;
	std::wstring szMessage;
	pop(szFromTamerName);
	pop(szMessage);	

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::GUILD;

	CProtocol.m_wStr = szFromTamerName;
	CProtocol.m_wStr += _T(" : ");
	CProtocol.m_wStr += szMessage;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}


void cCliGame::SendGuildClose(void)	// 자신의 길드를 폐쇄함
{
	newp(pGuild::Close);
	endp(pGuild::Close);
	send();	
}

void cCliGame::RecvGuildClose(void) //주변에 있는 모든 유저가 수신하게 됨
{
	std::wstring szGuildName;
	pop(szGuildName);	// 제거되는 길드명
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_DESTORY_BC, &szGuildName );
}

//모두 간다
void cCliGame::RecvGuildAdd(void)		// 해당 테이머가 길드에 추가됨
{
	GS2C_GUILD_ADD guildAdd;

	pop(guildAdd.nGuildClass);	// 길드 내에서의 자신의 위치 - master, sub-master, member

	if(guildAdd.nGuildClass == 0)
	{
		// 가입되는 길드원 주변 테이머들에게 수신됨, 길드원 본인에게는 수신되지 아니함
		pop(guildAdd.szTamerName);	// 길드에 가입 처리 완료됨	
		pop(guildAdd.szGuildName);
#ifdef GUILD_RENEWAL
		pop( guildAdd.nSkillCode );
#endif
		GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_ADD, &guildAdd );
		return;
	}
	u1 nTamerTypeO;
	pop(nTamerTypeO);
	guildAdd.nTamerType = 80000 + nTamerTypeO;

	pop(guildAdd.szTamerName);	// 길드에 가입 처리 완료됨
	pop(guildAdd.nTamerLevel);
	pop(guildAdd.nMapNo);
	pop(guildAdd.nChannelNo);
	pop(guildAdd.szGuildName);   // 길드명

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_ADD, &guildAdd );
}

//길드가입요청 ( Sender )
bool cCliGame::SendGuildReqAdd(TCHAR const*szTamerName)
{
	// 본인이 길드에 가입 요청 권한이 있고,
	// 상대(szTamerName) 에게 가입을 요청함

	std::wstring sendTamerName(szTamerName);
	cPacket p;
	p.newp(pGuild::ReqAdd);
		p.push( sendTamerName );
	p.endp(pGuild::ReqAdd);
	DoSend(p);

	return true;
}

void cCliGame::RecvGuildReqAddFailure(void)
{
	GS2C_GUILD_REQADDFAILURE addFail;
	//길드 가입 요청 받은사람이 없거나 로그아웃상태일때
	pop(addFail.nCode);
	pop(addFail.wsTamerName);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_INVITEFAIL, &addFail );
}

void cCliGame::RecvGuildReject(void)
{
	std::wstring szTamerName;
	pop(szTamerName);	// 거부한 테이머 이름

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_REJECT, &szTamerName );
}

void cCliGame::SendGuildReqAddFailure(TCHAR const*szTamerName)
{
	// 길드 가입 메세지를 중첩으로 받는 경우
	// 뭔가 작업중으로 처리가 힘든 경우

	std::wstring tamerName(szTamerName);
	cPacket p;
	p.newp(pGuild::ReqAddFailure);
		p.push(tamerName);
	p.endp(pGuild::ReqAddFailure);
	DoSend(p);
}

void cCliGame::RecvGuildDeletedTamer(void)
{
	std::wstring name;
	pop(name);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDMEMBER_DELETE, &name );
}

void cCliGame::RecvGuildChangeTamerName(void)
{
	std::pair<std::wstring, std::wstring> tamerNameChange;
	
	pop(tamerNameChange.first);
	pop(tamerNameChange.second);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_TAMERNAME_CHANGE, &tamerNameChange );
}

//길드 가입요청 ( Receiver )
void cCliGame::RecvGuildReqAdd(void)
{
	std::wstring szTamerName;
	std::wstring szGuildName;
	u4 nCertifiedCode;

	pop(szTamerName);	// 가입 요청자
	pop(nCertifiedCode);
	pop(szGuildName);	// 길드 이름

	// 현재 상황에서는 길드초대받기 되지 않는다. ( Npc이용, 전투중, 거래중, 진화중, 파트너몬 교체중 )
	if( g_pGameIF->IsOpenNpcWindow() || g_pCharMng->GetDigimonUser( 0 )->IsBattle() || g_pDataMng->GetServerSync()->IsChageDigimon() )	
	{	
		SendGuildReqAddFailure( szTamerName.c_str() );
		return;
	}
	// 메세지 박스 띄워 있는중
	else if( cMessageBox::IsMessageBox() )
	{	
		SendGuildReqAddFailure( szTamerName.c_str() );
		return;
	}
	// 튜토리얼중
	else
	{
		bool bTutorialPlaying = false;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_IS_TUTORIAL_PLAYING, &bTutorialPlaying );
		if( bTutorialPlaying )
		{
			SendGuildReqAddFailure( szTamerName.c_str() );
			return;
		}
	}


#ifdef GUILD_INVITATION
	//자동거절 활성화 
	if( g_pResist->m_Global.s_bGuildInvitation == false )
	{
		SendGuildReject(szTamerName.c_str(), nCertifiedCode);
		return;
	}
#endif

	cPrintMsg::PrintMsg( 30126, const_cast<TCHAR*>(szGuildName.c_str()) );
	cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
	pMBox->SetUintValue1( (UINT)nCertifiedCode );	
	pMBox->SetCharValue1( (LPTSTR)szTamerName.c_str() );

	//	SendGuildAllow(szTamerName, nCertifiedCode);	// 길드 가입 요청에 대한 수락
	//	SendGuildReject(szTamerName, nCertifiedCode);	// 길드 가입 요청에 대한 거부

}

void cCliGame::SendGuildAllow(TCHAR const*szTamerName, u4 nCertifiedCode)	// pGuild::ReqAdd의 응답 - 허가
{
	std::wstring tamerName(szTamerName);
	newp(pGuild::Allow);
		push(nCertifiedCode);
		push( tamerName );
	endp(pGuild::Allow);
	send();	
}


void cCliGame::SendGuildReject(TCHAR const*szTamerName, u4 nCertifiedCode)	// pGuild::ReqAdd의 응답 - 거부
{
	std::wstring tamerName(szTamerName);
	newp(pGuild::Reject);
		push(nCertifiedCode);
		push( tamerName );
	endp(pGuild::Reject);
	send();
}

void cCliGame::RecvGuildAllowFailure(void)
{
	// 네트웍 또는 길드 요청자 오류로 가입 처리 실패
	
	cPrintMsg::PrintMsg( 30139 );
}

void cCliGame::RecvGuildOn(void)	// online
{	
	GS2C_GUILD_ON guildOn;

	pop(guildOn.szTamerName);
	pop(guildOn.nTamerLevel);
	pop(guildOn.nMapNo);
	pop(guildOn.nChannelNo);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_ON, &guildOn );
}

void cCliGame::RecvGuildOff(void)	// offline
{
	// 아직 길드 정보를 수신하지 않은 상태라면, 해당 프로시져 무시
	std::wstring szTamerName;
	pop(szTamerName);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_OFF, &szTamerName );
}
void cCliGame::RecvGuildLeave(void)		// 주변에 있는 모든 유저 수신
{
	std::wstring szTamerName;
	pop(szTamerName);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_LEAVE, &szTamerName );
}

void cCliGame::SendGuildLeave(void)	// 길드에서 스스로 탈퇴하는 경우
{
	newp(pGuild::Leave);
	// 자신에 대한 정보는 없어도 무관
	endp(pGuild::Leave);
	send();
}
void cCliGame::RecvGuildDeleted(void)	// 주변에 있는 모든 유저 수신
{
	std::wstring szTamerName;

	pop(szTamerName);	// 강퇴 당한 테이머 이름

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_MEMBERDELETE, &szTamerName );
}

void cCliGame::SendGuildDelete(std::wstring* szTamerName)	// 상대를 강퇴함(자신이 길드장이어야 함)
{
	std::wstring delTamerName(*szTamerName);

	newp(pGuild::Delete);
		push( delTamerName );
	endp(pGuild::Delete);
	send();
}

void cCliGame::RecvGuildToMaster(void)
{
	std::pair<cData_ServerSync::eSYNC, std::wstring> rankdata;
	rankdata.first = cData_ServerSync::GUILD_AP_MASTER;
	pop(rankdata.second);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDRANK_SET, &rankdata );
}

void cCliGame::RecvGuildToSubMaster(void)
{
	std::pair<cData_ServerSync::eSYNC, std::wstring> rankdata;
	rankdata.first = cData_ServerSync::GUILD_AP_2MASTER;
	pop(rankdata.second);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDRANK_SET, &rankdata );
}

void cCliGame::RecvGuildToSubMember(void)
{
	std::pair<cData_ServerSync::eSYNC, std::wstring> rankdata;
	rankdata.first = cData_ServerSync::GUILD_AP_NEWBI;
	pop(rankdata.second);
	
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDRANK_SET, &rankdata );
}

void cCliGame::SendGuildChangeClassName(int nClass, wchar const* szName, wchar const* szDesc)
{
	std::wstring szRankName(szName);
	std::wstring szRankDesc(szDesc);
	newp(pGuild::ChangeClassName);
		push(nClass);
		push(szRankName);
		push(szRankDesc);	// 해당 직급에 대한 설명 영문64, 한글 32자 이내
	endp(pGuild::ChangeClassName);
	send();
}

void cCliGame::RecvGuildChangeClassName(void)
{
	GS2C_GUILD_CHANGE_CLASSNAME_RECV changeClassName;
	pop(changeClassName.nClass);
	pop(changeClassName.szName);
	pop(changeClassName.szDesc);	// 추가되었습니다.	
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_CHANGE_CLASSNAME, &changeClassName );

}


void cCliGame::RecvGuildToDatsMember(void)
{

	std::pair<cData_ServerSync::eSYNC, std::wstring> rankdata;
	rankdata.first = cData_ServerSync::GUILD_AP_DAT;
	pop(rankdata.second);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDRANK_SET, &rankdata );
}

void cCliGame::RecvGuildToMember(void)
{

	std::pair<cData_ServerSync::eSYNC, std::wstring> rankdata;
	rankdata.first = cData_ServerSync::GUILD_AP_NORMAL;
	pop(rankdata.second);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDRANK_SET, &rankdata );
}

void cCliGame::SendGuildToMaster(std::wstring* szNextMasterTamerName)
{
	std::wstring tamerName(*szNextMasterTamerName);
	newp(pGuild::ToMaster);
		push( tamerName );
	endp(pGuild::ToMaster);
	send();
}

void cCliGame::SendGuildToSubMaster(std::wstring* szNextSubMasterTamerName)
{
	std::wstring tamerName(*szNextSubMasterTamerName);
	newp(pGuild::ToSubMaster);
		push( tamerName );
	endp(pGuild::ToSubMaster);
	send();
}

void cCliGame::SendGuildToMember(std::wstring* szNextMemberTamerName)
{
	std::wstring tamerName(*szNextMemberTamerName);

	newp(pGuild::ToMember);
		push( tamerName );
	endp(pGuild::ToMember);
	send();
}

void cCliGame::SendGuildToSubMember(std::wstring* szNextMemberTamerName)
{
	std::wstring tamerName(*szNextMemberTamerName);
	newp(pGuild::ToSubMember);
	push( tamerName );
	endp(pGuild::ToSubMember);
	send();
}


void cCliGame::SendGuildToDatsMember(std::wstring* szNextMemberTamerName)
{
	std::wstring tamerName(*szNextMemberTamerName);
	newp(pGuild::ToDatsMember);
	push( tamerName );
	endp(pGuild::ToDatsMember);
	send();
}


void cCliGame::RecvGuildIncMember(void)
{
	int nMaxGuildMemberByItem;	// 아이템에 의해 추가된 최대 길드원 수
	pop(nMaxGuildMemberByItem);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDMEMBER_MAXINC, &nMaxGuildMemberByItem );
}

#ifdef GUILD_RENEWAL

void cCliGame::RecvGuildSkillPoint()
{
	u4 nGSP = 11;
	pop( nGSP );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_SKILLPOINT, &nGSP);
}

void cCliGame::SendGuildMemoAdd( TCHAR const* szMemo )
{
	SAFE_POINTER_RET( szMemo );

	newp(pGuild::Memo);
		std::wstring guildMemo(szMemo);
		push( szMemo );
	endp(pGuild::Memo);
	send();
}

void cCliGame::SendUseGuildSkill( int nType, void* pData )
{
	switch( nType )
	{
	case GUILD_SHOUT:	// 길드 확성기(51)
		{
			TCHAR sz[Language::pLength::str+1];
			_tcscpy_s( sz, Language::pLength::str, (TCHAR*)pData );
			nsCsFileTable::g_pCuidMng->CheckChat(sz);	

			newp( pGuild::Shout );
			push( sz );
			endp( pGuild::Shout );
			send();
		}
		break;
	case GUILD_SUMMON:	// 길드원 소환(53)
		{
			newp( pGuild::Summon );
			n4* nSkillCode = (n4*)pData;
			push( *nSkillCode );		// 소환 스킬 코드	
// 			u2 nCount = (u2)g_pDataMng->GetGuild()->GetOnlineCount();
// 			push( nCount );			// 접속중인 인원 수
			push( (u1)0 );
			endp( pGuild::Summon );
			send();
		}
		break;
	case GUILD_NATUREUP:	// 속성 경험치 추가(61)
	case GUILD_SCANREDUCE:	// 스캔 비용 감소(62)
	case GUILD_SCANBITUP:	// 회수 시 Bit 추가 획득(63)
	case GUILD_HATCHUP:	// 부화 확률 증가(64)
	case GUILD_CROPBITUP:	// 사냥 시 Bit 추가 획득(65)
	case GUILD_MOVEUP:	// 이동 속도 증가(66)
	case GUILD_INCHAPAYREDUCE:	// 강화 비용 감소(67)
	case GUILD_INCHARATEUP:	// 강화 확률 증가(68)
		{
			newp( pGuild::GSkill );
			u4* nSkillCode = (u4*)pData;
			push( *nSkillCode );
			endp( pGuild::GSkill );
			send();
		}
		break;
	}
}

void cCliGame::RecvUseGuildSkill()
{
	GS2C_GUILD_SKILL skillInfo;
	pop( skillInfo.nSkillCode );
	skillInfo.bUsing = true;
	skillInfo.nUseTime = _TIME_TS;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDSKILL_SET, &skillInfo );
}

void cCliGame::RecvRemoveUseGuildSkill()
{
	GS2C_GUILD_SKILL skillInfo;
	pop( skillInfo.nSkillCode );
	skillInfo.bUsing = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILDSKILL_SET, &skillInfo );
}

void cCliGame::SendGuildSkill_Summon( u4 nSkillCode )
{
	// 길드원이 소환 수락 할 때
	newp( pGuild::Summon );
	push( (n4)nSkillCode );		// 소환 스킬 코드
	push( (u1)1 );			// 길드원이 소환 수락 할 때
	endp( pGuild::Summon );
	send();
}

void cCliGame::RecvGuildSkill_Summon()
{
	// 길드장이 썼을 때 길드원이 수신
	u4 nSkillCode = 2600003;
	pop( nSkillCode );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_SUMMON, &nSkillCode );
}

void cCliGame::RecvGuildSkillError()
{
	u1 nErrCode = 0;
	pop( nErrCode );
	switch( nErrCode )
	{
	case 0:	// 채널 인원 제한(100명)으로 소환 스킬 실패
		cPrintMsg::PrintMsg( 30210 );	// @<tc:900>현재 맵에 인원이 많아 길드 소환 스킬을 사용할 수 없습니다.%@</tc>
		break;
	case 1:	// 여러 클라에서 스킬 동시 사용 시 스킬 사용 실패 유저
		cPrintMsg::PrintMsg( 30219 );	// @<tc:900>스킬@</tc>이\n이미 사용 되었습니다.%
		break;
	default:
		break;
	}
}
// 길드 메모 수정 되면 모두 받음.
void cCliGame::RecvGuildChangeMemo()
{
 	GS2C_GUILD_CHAGE_MEMO chageMemo;
 	pop(chageMemo.wsTamerName);
 	pop(chageMemo.wsMemo);
 	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_CHANGEMEMO, &chageMemo );
}
#endif