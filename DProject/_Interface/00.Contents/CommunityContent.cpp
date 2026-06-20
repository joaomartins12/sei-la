#include "StdAfx.h"
#include "CommunityContent.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptTutorialQuest.h"

#include <atlconv.h>

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

cCommunityContent::cCommunityContent(void): m_bNoticeShow(false),m_bShowOfflinePerson(true),m_AdaptTutorialQuest(NULL)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_GUILDINFO, this, &cCommunityContent::GetGuildInfo);						// 길드 정보 가져오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_GUILD_MYINFO, this, &cCommunityContent::GetMyInfo);						// 내 길드 정보 가져오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_SET_PASIVESKILL_COLOR, this, &cCommunityContent::GetSetPassiveSkill);	// 길드 패시브 스킬의 색을 설정하고 가져온다.
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_GUILD_PERSON_INFO, this, &cCommunityContent::GetPersonInfo);					// 길드원 정보 가져오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_GUILDRANK_NAME, this, &cCommunityContent::GetRankName);					// 길드 랭크 네임 가져오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_GUILDRANK_DECRIPT, this, &cCommunityContent::GetRankDesc);				// 길드 랭크 설명 가져오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_ISACTIVESKILL, this, &cCommunityContent::GetIsActiveSkill);				// 액티브 스킬 소유 여부 가져오기

	GAME_EVENT_ST.AddEvent( EVENT_CODE::CHECK_INCREASE_GUILDMEMBERMAX, this, &cCommunityContent::CheckIncreaseMemberMax);	// 길드원 최대수 증가 아이템 사용전 검사
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CHECK_GUILD_DESTROY, this, &cCommunityContent::CheckGuildDestory);			// 길드 삭제전 재질문
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SET_GUILD_PERSONLEVEL, this, &cCommunityContent::SetPersonLevel);		// 길드원 레벨 변경

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILDINFO, this, &cCommunityContent::RecvGuildInfo);					// 길드 정보 대부분을 설정
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_MEMBERDELETE, this, &cCommunityContent::RecvGuildDelete);		// 길드원 강퇴 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILDMEMBER_MAXINC, this, &cCommunityContent::RecvGuildMemberMAXIncrease);// 최대 길드원수 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_MAP_NO, this, &cCommunityContent::RecvMapNo);							// 맵번호 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_LEAVE, this, &cCommunityContent::RecvGuildLeave);				// 길드 떠나기(자기자신) 받기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_CREATE, this, &cCommunityContent::RecvGuildCreate);				// 길드 개설 받기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILDNOTICE_CHANGE, this, &cCommunityContent::RecvGuildNoticeChange);	// 길드 공지 변경
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_HISTORY, this, &cCommunityContent::RecvGuildHistory);			// 길드 활동 기록 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_MEMBER_LEVELUP, this, &cCommunityContent::RecvGuildMemberLevelUp);// 길드원 레벨 업 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_DESTORY_BC, this, &cCommunityContent::RecvGuildDestroyBC);		// 길드 삭제 주변인 모두 받음(BC 브로드캐스트)
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_ADD, this, &cCommunityContent::RecvGuildAdd);					// 길드 추가
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_INVITEFAIL, this, &cCommunityContent::RecvGuildInviteFail);		// 길드 초대 실패 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_REJECT, this, &cCommunityContent::RecvGuildReject);				// 길드 초대 거절
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILDMEMBER_DELETE, this, &cCommunityContent::RecvGuildMemberDelete);	// 길드 멤버 삭제
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_TAMERNAME_CHANGE, this, &cCommunityContent::RecvGuildTamerNameChange);// 이름 변경 길드에 적용 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_ON, this, &cCommunityContent::RecvGuildOn);						// 길드 ON 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_OFF, this, &cCommunityContent::RecvGuildOff);					// 길드 OFF 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_CHANGE_CLASSNAME, this, &cCommunityContent::RecvGuildChangeClassName);// 길드랭크명 변경 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILDRANK_SET, this, &cCommunityContent::RecvGuildRankSet);				// 길드 등급 임명 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILDSKILL_SET, this, &cCommunityContent::RecvGuildSkillSet);				// 길드 스킬 사용여부 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_SUMMON, this, &cCommunityContent::RecvGuildSummon);			// 길마(부길마) 소환 받음

#ifdef GUILD_RENEWAL
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_SKILLPOINT, this, &cCommunityContent::RecvGuildSkillPoint);			// 길드 스킬포인트 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_CHANGEMEMO, this, &cCommunityContent::RecvGuildChangeMemo);			// 길드원 메모 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_LEVELUP, this, &cCommunityContent::RecvGuildLevelUp);			// 길드 레벨 업 받음

	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_USINGSKILLVALUE, this, &cCommunityContent::GetUsingSkillValue);			// 적용 중인 스킬로 인한 변화값 가져오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_USINGSKILL_MAP, this, &cCommunityContent::GetUsingSkillMap);			// 사용중인 스킬이 저장된 MAP가져 오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_USING_SKILL, this, &cCommunityContent::GetUsingSkill);					// 사용중인 스킬 가져오기
	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_GUILDMEMO, this, &cCommunityContent::SendGuildMemo);				// 길드 메모 보내기

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GUILD_SKILL_RESET, this, &cCommunityContent::RecvGuildSkillReset);		
#endif

	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_GUILD_LEAVE, this, &cCommunityContent::SendGuildLeave);				// 길드 떠나기(자기자신) 보내기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_GUILD_INVITE, this, &cCommunityContent::SendGuildInvite);				// 길드 초대 보내기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_GUILD_DESTROY, this, &cCommunityContent::SendGuildDestroy);			// 길드 폐쇄 보내기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_GUILD_BAN, this, &cCommunityContent::SendGuildBan);					// 길드 탈퇴 시키기(길드장만 가능)
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_GUILD_CHANGECLASSNAME, this, &cCommunityContent::SendGuildChangeClassName);// 길드 등급명 변경 보내기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_GUILDRANK_SET, this, &cCommunityContent::SendGuildRankSet);				// 길드 등급 임명 보내기


	EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Guild_Channel, this );

}
cCommunityContent::~cCommunityContent(void)
{
	m_AdaptTutorialQuest = 0;
	GAME_EVENT_STPTR->DeleteEventAll(this);
	EventRouter()->UnRegisterAll( this );
}

int const cCommunityContent::IsContentsIdentity(void)
{
	return E_CT_COMMUNITY;
}

int const cCommunityContent::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool cCommunityContent::Initialize(void)
{
	return true;
}

void cCommunityContent::UnInitialize(void)
{
}

bool cCommunityContent::IntraConnection(ContentsSystem& System)
{
	m_AdaptTutorialQuest = new AdaptTutorialQuest;
	if( m_AdaptTutorialQuest && !m_AdaptTutorialQuest->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;
	return true; 
}

void cCommunityContent::Update(float const& fElapsedTime)
{
	// 보낸지 10초가 지나면 목록에서 삭제
	std::list< sNAME >::iterator it = m_listInviteName.begin();
	std::list< sNAME >::iterator itEnd = m_listInviteName.end();
	while( it!=itEnd )
	{
		if( ( it->s_fLifeTime -= g_fDeltaTime ) < 0.0f )
			it = m_listInviteName.erase( it );
		else
			++it;
	}

	LIST_GUILD_IT it2 = m_ListPersonInfo.begin();
	LIST_GUILD_IT it2End = m_ListPersonInfo.end();
	for( ; it2!=it2End; ++it2 )
	{
		if( it2->s_bInit && ( it2->s_bOnline == false ) )
			it2->s_Tick += fElapsedTime;

		if( it2->s_Tick > 13.0f )
		{
			it2->s_bInit = false;
			it2->s_Tick = 0;
			SetGuildOnOff( false, &(it2->s_szName), 0, 0, it2->s_nPersonLevel );

			
			cPrintMsg::PrintMsg( 30125, (TCHAR*)it2->s_szName.c_str() );
		}	
	}
}
void cCommunityContent::NotifyEvent(int const& iNotifiedEvt)
{
	return;
}
void cCommunityContent::MakeMainActorData(void)
{
}
void cCommunityContent::ClearMainActorData(void)
{
	//닷본 내부로 이동할때 리셋 되버리면 나올때 받을수 없다.
	//맵, ch 이동시 매번 데이터를 받으면서 지우고 다시 세팅함으로 굳이 ClearMainActorData에서 지울 필요 없다
	//Delete();
}
void cCommunityContent::MakeWorldData(void)
{
	return;
}
void cCommunityContent::ClearWorldData(void)
{
	m_bNoticeShow = false;
	Delete();
	return;
}
const cCommunityContent::sGUILD_INFO* cCommunityContent::GetGuildInfo() const
{
	return &m_GuildInfo;
}
void cCommunityContent::GetGuildInfo(void* pData)
{
	SAFE_POINTER_RET(pData);
	sGUILD_INFO* pCastData = static_cast<sGUILD_INFO*>(pData);
	*pCastData = m_GuildInfo;
}
// 길드원 최대수 증가 아이템 사용전 검사
void cCommunityContent::CheckIncreaseMemberMax(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int, bool>* pCastData = static_cast<std::pair<int, bool>*>(pData);
	bool bReturn = false;
	
	if( IsHaveMyInfo() == true && GetMyInfo().s_eRank == GR_NONE )
	{
		cPrintMsg::PrintMsg( 30155 );
		bReturn = true;
	}

	if( IsHaveMyInfo() == true && GetMyInfo().s_eRank != GR_MASTER )
	{				
		
		cPrintMsg::PrintMsg( 30158, (TCHAR*)GetRankName(GR_MASTER)->c_str() );
		bReturn = true;
	}

	int nIncMax = nsCsFileTable::g_pBaseMng->GetGuildInfo( m_GuildInfo.s_nGuildLevel )->s_nIncMember;
	if( pCastData->first + m_GuildInfo.s_nIncMember > nIncMax )
	{
		cPrintMsg::PrintMsg( 30200 );
		bReturn = true;
	}

	(*pCastData).second = bReturn;
}
// 사용중인 길드 스킬 정보

#ifdef GUILD_RENEWAL
void cCommunityContent::SetUsingSkill( bool bUsing , u4 nSkillCode, u4 nEndTime, u4 nUseTime )
{
	if( bUsing )	// 사용중으로 들어온 스킬
	{
		if( nUseTime != 0 )
		{
			if(nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode ) == NULL || nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode )->GetInfo() == NULL)
				return;
			int nCoolDown = (int)nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode )->GetInfo()->s_fCooldownTime;
			int nRemainTime = (int)nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode )->GetInfo()->s_fDamageTime;

			if( nCoolDown != 0 )
				nEndTime = nUseTime + nCoolDown;
			else
				nEndTime = nUseTime + nRemainTime;
			
		}
		//해당 스킬을 찾아서 갱신한다 없으면 추가
		UsingSkillMapItr Itr = m_mapUsingSkill.find(nSkillCode);
		if (Itr == m_mapUsingSkill.end())
		{
			sGUILDSKILL_INFO sInfo;
			sInfo.s_nSkillCode = nSkillCode;
			sInfo.s_nEndTime = nEndTime;

			std::pair< uint, sGUILDSKILL_INFO > element(nSkillCode, sInfo);
			m_mapUsingSkill.insert(element);
		}
		else
		{
			Itr->second.s_nEndTime = nEndTime;
		}
	}
	else		// 사용중인 스킬 지우기
	{
		UsingSkillMapItr Itr = m_mapUsingSkill.find(nSkillCode);
		if (Itr != m_mapUsingSkill.end())
		{
			m_mapUsingSkill.erase(nSkillCode);
		}
	}

}
// 레벨에 따라 Base길드스킬 -> 적용 스킬 리스트에 넣기
void cCommunityContent::SetBaseSkill( int nLevel )
{
	// 레벨에 따라 Base길드스킬 -> 적용 스킬 리스트에 넣기
	if(nsCsFileTable::g_pBaseMng->GetGuildSkill(0) == NULL || nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList() == NULL)
		return;
	std::list< CsGuild_Skill::sINFO* >::iterator it = nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList()->begin();
	std::list< CsGuild_Skill::sINFO* >::iterator itEnd = nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList()->end();
	// Base스킬 중 적용 중인놈이 있으면 없애주기(중첩 관리)
	UsingSkillMapItr itUse = m_mapUsingSkill.begin();
	UsingSkillMapItr itUsingSkillMapEnd = m_mapUsingSkill.end();
	for( ; itUse != itUsingSkillMapEnd ; itUse++ )
	{
		for( ; it != itEnd ; it++ )
		{
			if( (*it)->s_nSkillCode == itUse->second.s_nSkillCode )
			{
				if( itUse->second.s_nEndTime != 0 )
				{
					UsingSkillMapItr itFindItr = m_mapUsingSkill.find(itUse->second.s_nSkillCode);
					if (itFindItr != itUsingSkillMapEnd)
						itFindItr->second.s_nCount = 0;
				}
				else
				{
					UsingSkillMapItr itFindItr = m_mapUsingSkill.find(itUse->second.s_nSkillCode);
					if (itFindItr != itUsingSkillMapEnd)
						m_mapUsingSkill.erase(itFindItr);
				}
				break;
			}
		}
		it = nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList()->begin();
	}

	it = nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList()->begin();
	for( ; it != itEnd ; it++ )
	{
		if( (*it)->s_nReqLevel > nLevel )
			continue;

		UsingSkillMapItr itFindItr = m_mapUsingSkill.find((*it)->s_nSkillCode);
		// 이미 같은 스킬코드를 가지는 스킬이 존재 한다면 중첩 수 셋팅을 하고 쿨타임은 그대로 두어서 이어 받는다.
		if (itFindItr != itUsingSkillMapEnd)	
			itFindItr->second.s_nCount = itFindItr->second.s_nCount + 1;
		else if (itFindItr == itUsingSkillMapEnd)
		{
			sGUILDSKILL_INFO sInfo;
			sInfo.s_nSkillCode = (*it)->s_nSkillCode;
			sInfo.s_nEndTime = 0;

			std::pair< uint, sGUILDSKILL_INFO > elementPair((*it)->s_nSkillCode, sInfo);
			m_mapUsingSkill.insert(elementPair);
		}
	}
	CheckPasiveSkill();
}
NiColor cCommunityContent::CheckPasiveSkill()
{
	// Base스킬 중 패시브 스킬 능력치 적용
	int nAddLv = 0;

	int nColorLv = 0;	// 색 변경 우선순위. 색 변경 스킬 효과 추가 시 우선순위 체크
	NiColor pColor = DEFAULT_NAMECOLOR;

	UsingSkillMapItr it = m_mapUsingSkill.begin();
	UsingSkillMapItr itEnd = m_mapUsingSkill.end();

	for( ; it != itEnd ; it++ )
	{
		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( it->second.s_nSkillCode );
		if (pSkill == NULL)
			continue;

		CsSkill::sINFO* pSkillInfo = pSkill->GetInfo();
		if (pSkillInfo == NULL)
			continue;
		
		for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
		{
			if( pSkillInfo->s_Apply[i].s_nID == 0 )
				break;
			switch( pSkillInfo->s_Apply[ i ].s_nA )
			{
			case GUILD_MEMBERUP:	// 길드 인원 확장
				nAddLv += ( pSkillInfo->s_Apply[ i ].s_nB * it->second.s_nCount );	// 중첩 수 계산
				break;
			case GUILD_NAMECOLOR:	// 색 변경
				if( nColorLv < 1 )
				{
					float fR = (float)( pSkillInfo->s_Apply[i].s_nB/100 );
					float fG = (float)( (pSkillInfo->s_Apply[i].s_nB/10) % 10 );
					float fB = (float)( pSkillInfo->s_Apply[i].s_nB%10 );
					pColor = NiColor( fR / 10.0f, fG / 10.0f, fB / 10.0f );
					nColorLv = 1;
				}
				break;
			}
		}
	}

	// GUILD_MEMBERUP(52) - 길드 인원 확장된 만큼 추가
	m_GuildInfo.s_nIncMember = nAddLv;

	// GUILD_NAMECOLOR(54) - 이름색 지정
	g_pCharMng->GetTamerUser()->GetCharOption()->SetNameColor( pColor );

	return pColor;
}
bool cCommunityContent::IsActSkillExist()
{
	UsingSkillMapItr it = m_mapUsingSkill.begin();
	UsingSkillMapItr itEnd = m_mapUsingSkill.end();

	int nVal = 0;
	for(; it != itEnd; it++)
	{
		if( nsCsFileTable::g_pBaseMng->_IsGuildBaseSkill( it->second.s_nSkillCode ) == false )
			return true;	//Base 스킬이 아닌 스킬이면 Active 스킬이지
	}
	return false; // Active 스킬이 없음
}
int cCommunityContent::CheckActSkill( int nSkillCode )// 리턴값 - ( 0 : 같은그룹 아님, 1: 같은 그룹의 상위 존재, 스킬코드: 같은 그룹이지만 하위 스킬 존재 )
{
	// 사용하려는 스킬보다 상위 스킬이 적용되어 있는가
	UsingSkillMapItr it = m_mapUsingSkill.begin();
	UsingSkillMapItr itEnd = m_mapUsingSkill.end();
	// 지금 사용하려는 스킬
	CsSkill* pCurSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode );
	if( pCurSkill == NULL )
		return false;

	int nCurSkillCode = 0;
	int nVal = 0;

	CsSkill* pSkill;	// 적용 중인 스킬
	for( ; it != itEnd ; it++ )
	{
		// 적용 중인 스킬
		pSkill = nsCsFileTable::g_pSkillMng->GetSkill( it->second.s_nSkillCode );
		if( pSkill == NULL || pSkill->GetInfo() == NULL )
			continue;
		CsSkill::sINFO* pSkillInfo = pSkill->GetInfo();

		for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
		{
			if( pSkillInfo->s_Apply[i].s_nID == 0 )
				break;
			// 효과가 같으면서
			if( pCurSkill->GetInfo()->s_Apply[i].s_nA == pSkillInfo->s_Apply[i].s_nA )
			{
				if( nVal < pSkillInfo->s_Apply[i].s_nB )
				{
					nCurSkillCode = pSkillInfo->s_dwID;
					nVal = pSkillInfo->s_Apply[i].s_nB;
				}
				// 효과값이 크거나 같은 스킬이 적용 중이면
				if( pCurSkill->GetInfo()->s_Apply[i].s_nB <= pSkillInfo->s_Apply[i].s_nB )
				{
					return 1;	// 같은 그룹의 상위 스킬 존재
				}
			}
		}
	}
	if( nCurSkillCode > 0 )
		return nCurSkillCode;	// 같은 그룹이지만 하위 스킬 존재
	else
		return 0;	// 같은그룹 아님
}
int cCommunityContent::GetUsingSkillValue( int nApply )
{
	UsingSkillMapItr it = m_mapUsingSkill.begin();
	UsingSkillMapItr itEnd = m_mapUsingSkill.end();

	CsSkill* pSkill;	// 적용 중인 스킬
	int nVal = 0;
	for( ; it != itEnd ; it++ )
	{
		// 적용 중인 스킬
		pSkill = nsCsFileTable::g_pSkillMng->GetSkill( it->second.s_nSkillCode );

		if( pSkill == NULL )
			continue;

		for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
		{
			if( pSkill->GetInfo()->s_Apply[i].s_nID == 0 )
				break;
			// 효과가 같으면서
			if( pSkill->GetInfo()->s_Apply[i].s_nA == nApply )
			{
				nVal += ( pSkill->GetInfo()->s_Apply[i].s_nB * it->second.s_nCount );	// 중첩 수 계산
			}
		}
	}
	return nVal;
}
#endif
void cCommunityContent::SetRankName( eRANK eRank, std::wstring* szName )
{
	if ( eRank >= GR_END)
		return;
	RankStrMapItr itr = m_GuildRankNameMap.find(eRank);
	if (itr != m_GuildRankNameMap.end())
		itr->second = *szName;
	else
		m_GuildRankNameMap.insert(make_pair(eRank, *szName));
}
std::wstring* cCommunityContent::GetRankName( eRANK eRank )
{
	if ( eRank >= GR_END)
		return NULL;
	RankStrMapItr itr = m_GuildRankNameMap.find(eRank);
	if (itr != m_GuildRankNameMap.end())
		return &(itr->second);
	return NULL;
}
void cCommunityContent::SetRankDesc( eRANK eRank, std::wstring* szName )
{
	if ( eRank >= GR_END)
		return;
	RankStrMapItr itr = m_GuildRankDescMap.find(eRank);
	if (itr != m_GuildRankDescMap.end())
		itr->second = *szName;
	else
		m_GuildRankDescMap.insert(make_pair(eRank, *szName));
}
std::wstring* cCommunityContent::GetRankDesc( eRANK eRank )
{
	if ( eRank >= GR_END)
		return NULL;
	RankStrMapItr itr = m_GuildRankDescMap.find(eRank);
	if (itr != m_GuildRankDescMap.end())
		return &(itr->second);
	return NULL;
}

void cCommunityContent::SetGuildName( std::wstring* szGuildName )
{
	assert_cs( szGuildName != NULL );

	// 길드 정보 init
	Delete();

	m_GuildInfo.s_szGuildName = *szGuildName;

	if( g_pCharMng->GetTamerUser() )
		g_pCharMng->GetTamerUser()->GetCharOption()->SetGuildName( m_GuildInfo.s_szGuildName.c_str() );
}
void cCommunityContent::Delete()
{	
	memset( &m_GuildInfo, 0 , sizeof( sGUILD_INFO ) );

	DeleteHistory();

	m_listInviteName.clear();
	m_ListPersonInfo.clear();	

#ifdef GUILD_RENEWAL
	m_mapUsingSkill.clear();
#endif
}
void cCommunityContent::DeleteHistory()
{
	m_HistoryInfoList.clear();
}

// 길드 정보 대부분을 설정
void cCommunityContent::RecvGuildInfo(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILDINFO* pCastData = static_cast<GS2C_GUILDINFO*>(pData);
	
#ifdef GUILD_RENEWAL
	SetGuildName( &(pCastData->wsGuildName) );
	SAFE_POINTER_RET(pData);
	m_mapUsingSkill.clear();
	// Base 부터 확인. 길드 레벨로..
	SetBaseSkill(pCastData->nGuildLevel);
	//first skilcode second time
	std::list<std::pair<unsigned int, unsigned int>>::iterator skillListItr = pCastData->m_GuildSkilList.begin();
	std::list<std::pair<unsigned int, unsigned int>>::iterator skillListEndItr = pCastData->m_GuildSkilList.end();
	for (;skillListItr != skillListEndItr; ++skillListItr)
	{
		// 시작시간을 줘서 종료시간으로 바꾸자
		u4 nEndTime;			
		// 해당 스킬 정보
		CsSkill::sINFO* pSkillInfo = nsCsFileTable::g_pSkillMng->GetSkill( skillListItr->first )->GetInfo();

		if( pSkillInfo->s_fDamageTime == 0 )// Base 스킬.. 쿨타임 존재함
			nEndTime = skillListItr->second + (u4)pSkillInfo->s_fCooldownTime;	// 종료시간 = 시작시간 + 쿨타임
		else	// 적용시간이 존재함. Active스킬
			nEndTime = skillListItr->second + (u4)pSkillInfo->s_fDamageTime;		// 종료시간 = 시작시간 + 적용 기간
		SetUsingSkill( true, skillListItr->first, nEndTime );
	}
#endif
	//name / descript
	std::list<std::pair<std::wstring, std::wstring>>::iterator gradeItr = pCastData->m_GuildGradeInfoList.begin();
	std::list<std::pair<std::wstring, std::wstring>>::iterator gradeEndItr = pCastData->m_GuildGradeInfoList.end();

	for (int i = 0; gradeItr != gradeEndItr; ++gradeItr, ++i)
	{
		SetRankName( (cCommunityContent::eRANK)(i + 1), &(gradeItr->first) );
		SetRankDesc( (cCommunityContent::eRANK)(i + 1), &(gradeItr->second) );
	}
#ifndef GUILD_RENEWAL
	SetGuildName(&(pCastData->wsGuildName));
#endif
	m_GuildInfo.s_nIncMember = pCastData->nIncMaxMember;
	m_GuildInfo.s_nExp = pCastData->nGuildExp;
	m_GuildInfo.s_nGuildLevel = pCastData->nGuildLevel;

#ifdef GUILD_RENEWAL
	//SetGuildName에서 Guild정보 Init해버려서 GSP 여기서 넣음
	m_GuildInfo.s_nGuildSkillPoint = pCastData->nGSP;

	//사용중인 스킬 중 패시브 있는지 확인하고 처리
	CheckPasiveSkill();
#endif	// GUILD_RENEWAL
	SetNotice( &(pCastData->wsGuildNotice), false );
	g_pCharMng->GetTamerUser()->GetCharOption()->SetGuildUID( pCastData->nGuildIDX );

	std::list<GuildInfo::sGuild_PersonInfo>::iterator Itr = pCastData->m_GuildPersonInfoList.begin();
	std::list<GuildInfo::sGuild_PersonInfo>::iterator EndItr = pCastData->m_GuildPersonInfoList.end();

	for(; Itr != EndItr; ++Itr)
	{
		sGUILD_PERSON_INFO sPerson;
		sPerson.s_eRank = (eRANK)Itr->nClass;
		sPerson.s_nPersonLevel = Itr->nTamerLevel;
		sPerson.s_nContribution = Itr->nMemberExp;	// 기여도
		sPerson.s_bOnline = (Itr->nMapNO != 0);
		sPerson.s_dwMapID = Itr->nMapNO;
		sPerson.s_nChannel = Itr->nChannelNO;
		sPerson.s_dwTamerType = Itr->nTamerType;
#ifdef GUILD_RENEWAL
		sPerson.s_nUserRank = Itr->nRank;
		if( sPerson.s_bOnline )
			sPerson.s_nLastPlayTime = INT_MAX;		//온라인
		else
			sPerson.s_nLastPlayTime = Itr->nLastTime;	//로그아웃 시간

		sPerson.s_szGuildMemo = Itr->wsMemo;
#endif
		sPerson.s_szName = Itr->wsName;
		SetGuildPersonInfo( &sPerson );
	}
	SetGuildInfoData();

}
void cCommunityContent::SetNotice( std::wstring* szNotice, bool bChat /* = true  */)	// 길드 공지.
{
	if( szNotice->empty() == true )
		m_GuildInfo.s_szNotice = UISTRING_TEXT( "COMMUNITY_WRITE_GUILD_NOTICE" );
	else
	{
		m_GuildInfo.s_szNotice = *szNotice;

		std::wstring wsNotice;
		DmCS::StringFn::Format( wsNotice, L"%s : %s", UISTRING_TEXT( "COMMUNITY_GUILD_NOTICE" ).c_str(), szNotice->c_str() );
		if( bChat == false )
		{
			if( m_bNoticeShow == false )
			{
				m_bNoticeShow = true;
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::GUILD_NOTICE;
				CProtocol.m_wStr = wsNotice;
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
			}
		}
		else
		{
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::GUILD_NOTICE;
			CProtocol.m_wStr = wsNotice;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}
	}
	ContentsStream kStream;
	kStream << &(m_GuildInfo.s_szNotice);
	Notify(SetNoticeStr, kStream);
}

void cCommunityContent::SetGuildPersonInfo( sGUILD_PERSON_INFO* pPerson )
{ 
	if( pPerson->s_bOnline )
		pPerson->s_bInit = true;

	if( pPerson->s_eRank == GR_MASTER )
		m_GuildInfo.s_szMasterName = pPerson->s_szName;

	m_ListPersonInfo.push_back( *pPerson );
}
void cCommunityContent::SetGuildInfoData()
{ 
	m_GuildInfo.s_nCurPersonCount = 0;
	m_GuildInfo.s_n2AndMasterNum = 0;
	m_GuildInfo.s_nMaxPersonCount = nsCsFileTable::g_pBaseMng->GetGuildInfo( m_GuildInfo.s_nGuildLevel )->s_nMaxGuildPerson + m_GuildInfo.s_nIncMember;
	m_GuildInfo.s_nPersonCount = m_ListPersonInfo.size();

	LIST_GUILD_IT it = m_ListPersonInfo.begin();
	LIST_GUILD_IT itEnd = m_ListPersonInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->s_bOnline == true )
			m_GuildInfo.s_nCurPersonCount++;

		if( it->s_eRank == GR_2NDMASTER )
			m_GuildInfo.s_n2AndMasterNum++;
	}

	Notify(SetGuildInfo);
}
// 길드 삭제 받음
void cCommunityContent::RecvGuildDelete(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::wstring wsTamerName = *(static_cast<std::wstring*>(pData));

	//길드 강퇴자가 본인이면
	if( wsTamerName.compare( g_pCharMng->GetTamerUser()->GetName() ) == 0 )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SHOUT_CASH ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SHOUT_CASH );
		if( cMessageBox::IsMessageBox( 30153 ) == false )
		{
			std::wstring name = m_GuildInfo.s_szGuildName;
			cPrintMsg::PrintMsg( 30153, (TCHAR*)name.c_str() );
		}

		Delete();
		g_pCharMng->GetTamerUser()->GetCharOption()->DeleteGuildName();

		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_COMMUNITY );
#ifdef GUILD_RENEWAL
		g_pCharMng->GetTamerUser()->GetCharOption()->SetNameColor( DEFAULT_NAMECOLOR );
#endif
		cPrintMsg::PrintMsg( 30131, (TCHAR*)wsTamerName.c_str() );	
	}
	else
	{
		//해당 테이머의 길드 정보 갱신
		CTamer* pTamer = (CTamer*)g_pMngCollector->GetObject( RTTI_TAMER, wsTamerName.c_str() );
		if( pTamer != NULL )
		{
			pTamer->GetCharOption()->DeleteGuildName();
#ifdef GUILD_RENEWAL
			pTamer->GetCharOption()->SetNameColor( DEFAULT_NAMECOLOR );
#endif
		}

		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_BAN, 0 );

		SetLeavePerson( &wsTamerName, 1 );
	}
	Notify(SetGuildInfo);
}
void cCommunityContent::SetLeavePerson( std::wstring* szTamerName, int nType )
{
	if( IsHaveMyInfo() == true && GetMyInfo().s_eRank == GR_NONE )
		return;

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szTamerName, nIdx );
	if( pInfo == NULL )
		return;

	switch( nType )
	{
	case 0: //본인이 탈퇴한경우
		cPrintMsg::PrintMsg( 30132, (TCHAR*)szTamerName->c_str() );
		break;
	case 1: //강퇴당한 경우
		cPrintMsg::PrintMsg( 30131, (TCHAR*)szTamerName->c_str() );
		break;
	default:
		assert_cs( false );
		break;
	}						

	SetGuildInfoData();
	LIST_GUILD_IT it = m_ListPersonInfo.begin();
	LIST_GUILD_IT itEnd = m_ListPersonInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->s_szName.compare( *szTamerName ) == 0 )
		{
			m_ListPersonInfo.erase( it );			
			break;
		}
	}	

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{	
		ContentsStream kstreamPopup;
		kstreamPopup << nIdx;
		Notify(CloseGuildMenuPopup, kstreamPopup);
		
		ContentsStream kstream;
		kstream << cCommunity::TAB_GUILD;
		Notify(SetUITab, kstream);
	}
}
cCommunityContent::sGUILD_PERSON_INFO* cCommunityContent::_GetPersonInfo( std::wstring* szName, int& nIdx )
{
	LIST_GUILD_IT it = m_ListPersonInfo.begin();
	LIST_GUILD_IT itEnd = m_ListPersonInfo.end();

	for( ; it!=itEnd; ++it, ++nIdx )
	{
		if( it->s_szName.compare( *szName ) == 0 )
			return &(*it);
	}	
	return NULL;
}
// 최대 길드원수 받음
void cCommunityContent::RecvGuildMemberMAXIncrease(void* pData)
{
	SAFE_POINTER_RET(pData);
	int nMaxGuildMemberByItem = *(static_cast<int*>(pData));
	m_GuildInfo.s_nIncMember = nMaxGuildMemberByItem;
	SetGuildInfoData();
	int nPerson = m_GuildInfo.s_nMaxPersonCount;
	cPrintMsg::PrintMsg( 30201, &nPerson );	
}
// 내 길드 정보 가져오기
void cCommunityContent::GetMyInfo(void* pData)
{
	SAFE_POINTER_RET(pData);
	SAFE_POINTER_RET(IsHaveMyInfo());
	sGUILD_PERSON_INFO* pCastData = static_cast<sGUILD_PERSON_INFO*>(pData);
	*pCastData = GetMyInfo();
}

const cCommunityContent::sGUILD_PERSON_INFO& cCommunityContent::GetMyInfo() const
{
	LIST_GUILD_CIT itr = m_ListPersonInfo.begin();
	LIST_GUILD_CIT endItr = m_ListPersonInfo.end();
	for (; itr != endItr; ++itr)
	{
		if( itr->s_szName.compare(g_pCharMng->GetTamerUser()->GetName()) == 0 )
			return *itr;
	}
}

bool cCommunityContent::IsHaveMyInfo()
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, false );

	std::wstring wsName = pUser->GetName();
	LIST_GUILD_CIT itr = m_ListPersonInfo.begin();
	LIST_GUILD_CIT endItr = m_ListPersonInfo.end();
	for (; itr != endItr; ++itr)
	{
		if( itr->s_szName.compare(wsName) == 0 )
			return true;
	}
	return false;
}
// 맵번호 받음
void cCommunityContent::RecvMapNo(void* pData)
{
	SAFE_POINTER_RET(pData);
	SAFE_POINTER_RET(IsHaveMyInfo());
	unsigned int pCastData = *(static_cast<unsigned int*>(pData));
	
	LIST_GUILD_IT itr = m_ListPersonInfo.begin();
	LIST_GUILD_IT endItr = m_ListPersonInfo.end();
	for (; itr != endItr; ++itr)
	{
		if( itr->s_szName.compare(g_pCharMng->GetTamerUser()->GetName()) == 0 )
			itr->s_dwMapID = pCastData;
	}
}

void cCommunityContent::SetMyChannel(const int nChannel)
{
	LIST_GUILD_IT itr = m_ListPersonInfo.begin();
	LIST_GUILD_IT endItr = m_ListPersonInfo.end();
	for (; itr != endItr; ++itr)
	{
		if( itr->s_szName.compare(g_pCharMng->GetTamerUser()->GetName()) == 0 )
			itr->s_nChannel = nChannel;
	}
}

cCommunityContent::LIST_GUILD* cCommunityContent::GetMap()
{ 
	return &m_ListPersonInfo;
}

cCommunityContent::sHISTORY_INFO cCommunityContent::GetHistoryInfo( int nIdx )
{
	sHISTORY_INFO findInfo;
	std::list< sHISTORY_INFO >::iterator Itr = m_HistoryInfoList.begin();
	std::list< sHISTORY_INFO >::iterator EndItr = m_HistoryInfoList.end();
	for (int i = 0; Itr != EndItr; ++i, ++Itr)
	{
		if (nIdx != i)
			continue;
		findInfo = *Itr;
	}
	return findInfo;
}

int cCommunityContent::GetHistoryInfoSize()
{
	return m_HistoryInfoList.size();
}

// 길드 떠나기(자기자신) 받기
void cCommunityContent::RecvGuildLeave(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::wstring wsTamerName = *(static_cast<wstring*>(pData));
	//길드 탈퇴자가 본인이면
	if( 0 == wsTamerName.compare( g_pCharMng->GetTamerUser()->GetName() ) )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SHOUT_CASH ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SHOUT_CASH );
		LeaveClose();
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_LEAVE, 0 );
		cPrintMsg::PrintMsg( 30141 );
	}
	else
	{
		//해당 테이머의 길드 정보 갱신
		CTamer* pTamer = (CTamer*)g_pMngCollector->GetObject( RTTI_TAMER, wsTamerName.c_str() );
		if( pTamer != NULL )
		{
			pTamer->GetCharOption()->DeleteGuildName();
#ifdef GUILD_RENEWAL
			pTamer->GetCharOption()->SetNameColor( DEFAULT_NAMECOLOR );
#endif
		}
		SetLeavePerson( &wsTamerName, 0 );
	}

	Notify(SetGuildInfo);
}
// 길드 떠나기(자기자신) 보내기
void cCommunityContent::SendGuildLeave(void* pData)
{
	if (IsHaveMyInfo() == false)
	{
		//길드에 소속된 테이머가 아닙니다.
		cPrintMsg::PrintMsg( 30155 );
		return;
	}
	if( GetMyInfo().s_eRank == cCommunityContent::GR_MASTER )
		return;
	net::game->SendGuildLeave();
	LeaveClose();
}
void cCommunityContent::LeaveClose()
{
	Delete();
#ifdef GUILD_RENEWAL
	CheckPasiveSkill();
#endif
	g_pCharMng->GetTamerUser()->GetCharOption()->DeleteGuildName();

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_COMMUNITY );			
	}	
}

void cCommunityContent::SendGuildInvite(void* pData)
{
	SAFE_POINTER_RET(pData);
	wstring* szTamerName = static_cast<wstring*>(pData);
	// 현재 상황에서는 초대가 되지 않는다. ( Npc이용, 전투중, 거래중, 진화중, 파트너몬 교체중 )
	if( g_pGameIF->IsOpenNpcWindow() || g_pCharMng->GetDigimonUser( 0 )->IsBattle() || g_pDataMng->GetServerSync()->IsChageDigimon() 
		|| cMessageBox::IsMessageBox() )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}

	else if( IsHaveMyInfo() == true && GetMyInfo().s_eRank != GR_MASTER && GetMyInfo().s_eRank != GR_2NDMASTER )
	{ 
		cPrintMsg::PrintMsg( 30158, (TCHAR*)GetRankName( GR_2NDMASTER )->c_str() );
		return;
	}

	// 나라면 패스
	
	else if( szTamerName->compare(g_pCharMng->GetTamerUser()->GetName()) == 0 )
	{
		cPrintMsg::PrintMsg( 11101 );
		return;
	}	
	//전투 확인
	else if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
	{
		cPrintMsg::PrintMsg( 30455 );
		return;
	}
	//기초대 목록 확인
	if( IsSendInvate( szTamerName ) == false )
	{
		bool bSend = true;

		if( Language::SUCCEEDED != Language::CheckTamerName( *szTamerName ) )
		{
			cPrintMsg::PrintMsg( 11101 );
			return;
		}
		// 서버에 전송
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_INVATE, 0, (TCHAR*)szTamerName->c_str() );	
			
	}
}
bool cCommunityContent::IsSendInvate( std::wstring* szTamerName )
{
	// 이미 보낸 목록에 존재 하는지 찾는다.
	std::list< sNAME >::iterator it = m_listInviteName.begin();
	std::list< sNAME >::iterator itEnd = m_listInviteName.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->s_szName.compare( *szTamerName ) == 0 )
		{
			cPrintMsg::PrintMsg( 30062 );	
			return true;
		}
	}

	// 보낸 목록에 저장
	sNAME pName;
	pName.s_szName = *szTamerName;
	pName.s_fLifeTime = 10.0f;
	m_listInviteName.push_back( pName );
	return false;
}
// 길드 개설 받기
void cCommunityContent::RecvGuildCreate(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<bool, int>* pCastData = static_cast<std::pair<bool, int>*>(pData);
	if( pCastData->first )
	{
		//길드생성 성공
		g_pDataMng->GetInven()->GetData( pCastData->second )->DecreaseCount( 1 );				
		cPrintMsg::PrintMsg( 30122 );
	}
	else
	{
		//길드생성 실패
		Delete();
		cPrintMsg::PrintMsg( 30120 );
	}			

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_COMMUNITY );			
	}	
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TALK );

}
// 길드 패시브 스킬의 색을 설정하고 가져온다.
void cCommunityContent::GetSetPassiveSkill(void* pData)
{
#ifdef GUILD_RENEWAL
	SAFE_POINTER_RET(pData);

	NiColor* pColor = static_cast<NiColor*>(pData);
	NiColor Color = CheckPasiveSkill();
	*pColor = Color;
#endif
}
void cCommunityContent::CheckGuildDestory(void* pData)
{
	if(m_ListPersonInfo.size() > 1)
		cPrintMsg::PrintMsg( 30159 );
	else
		cPrintMsg::PrintMsg( 30152 );
}
#ifdef GUILD_RENEWAL
void cCommunityContent::GetUsingSkill(void* pData)
{
	SAFE_POINTER_RET(pData);

	std::pair<u4, sGUILDSKILL_INFO>* pCastData = static_cast<std::pair<u4, sGUILDSKILL_INFO>*>(pData);
	if(IsHaveUsingSkill(pCastData->first) == true)
		(*pCastData).second = *GetUsingSkill(pCastData->first);
}
cCommunityContent::sGUILDSKILL_INFO const* cCommunityContent::GetUsingSkill( u4 nSkillCode ) const
{
	// 스킬 코드로 적용중인 스킬 얻기( 쿨타임/적용기간, 스킬 존재 유무 확인용 )
	UsingSkillMapCItr it = m_mapUsingSkill.find( nSkillCode );
	if( it != m_mapUsingSkill.end() )
		return &(it->second);
	return NULL;
}
bool cCommunityContent::IsHaveUsingSkill( u4 nSkillCode )
{
	// 스킬 코드로 적용중인 스킬 얻기( 쿨타임/적용기간, 스킬 존재 유무 확인용 )
	UsingSkillMapItr it = m_mapUsingSkill.find( nSkillCode );
	if( it == m_mapUsingSkill.end() )
		return false;

	return true;
}
// 사용중인 스킬이 저장된 MAP가져 오기
void cCommunityContent::GetUsingSkillMap(void* pData)
{
	SAFE_POINTER_RET(pData);
	UsingSkillMap* pCastData = static_cast<UsingSkillMap*>(pData);
	*pCastData = m_mapUsingSkill;
}

void cCommunityContent::RecvGuildSkillReset(void* pData)
{
	CsGuild_Skill* pGuildSkill = nsCsFileTable::g_pBaseMng->GetGuildSkill(0);
	std::list<CsGuild_Skill::sINFO*>::iterator it_Base = pGuildSkill->GetList()->begin();

	for( int i = 0 ; i < IF_BASESKILL_COUNT ; i++ )
	{
		int nSkillCode = nsCsFileTable::g_pSkillMng->GetSkill( (*it_Base)->s_nSkillCode )->GetInfo()->s_dwID;
		UsingSkillMapItr skillInfo = m_mapUsingSkill.find( nSkillCode );
		UsingSkillMapItr endItr = m_mapUsingSkill.end();
		if (skillInfo != endItr)
		{
			if (skillInfo->second._IsCoolTime())
			{
				skillInfo->second.s_nEndTime = 0;
			}
		}
		it_Base++;
	}

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = _T("길드스킬 쿨타임 초기화 효과 발동");
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

#endif
// 길드원 정보 가져오기
void cCommunityContent::GetPersonInfo(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int, sGUILD_PERSON_INFO*>* pCastData = static_cast<std::pair<int, sGUILD_PERSON_INFO*>*>(pData);
	(*pCastData).second = GetPersonInfo(pCastData->first);

}
// 길드원 정보 가져오기
cCommunityContent::sGUILD_PERSON_INFO* cCommunityContent::GetPersonInfo( int nDataIdx )
{
 	LIST_GUILD_IT it = m_ListPersonInfo.begin();
 	int nListSize = m_ListPersonInfo.size();
 
 	for(int i = 0; i < nListSize && i <= nDataIdx ; ++it, ++i)
 	{
 		if( nDataIdx == i )
 			return &(*it);
 	}
	assert_cs( false );
	return NULL;
}
// 길드네임 가져오기
void cCommunityContent::GetRankName(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<eRANK, std::wstring*>* pCastData = static_cast<std::pair<eRANK, std::wstring*>*>(pData);
	(*pCastData).second = GetRankName(pCastData->first);
}
// 길드 랭크 설명 가져오기
void cCommunityContent::GetRankDesc(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<eRANK, std::wstring*>* pCastData = static_cast<std::pair<eRANK, std::wstring*>*>(pData);
	(*pCastData).second = GetRankDesc(pCastData->first);
}
// 액티브 스킬 소유 여부 가져오기
void cCommunityContent::GetIsActiveSkill(void* pData)
{
#ifdef GUILD_RENEWAL
	SAFE_POINTER_RET(pData);
	bool* isActiveSkill = static_cast<bool*>(pData);
	*isActiveSkill = IsActSkillExist();
#endif
}

bool CompGuildLevel(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_nPersonLevel > second.s_nPersonLevel)
		return true;
	else
		return false;
}
bool CompGuildLevelReverse(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_nPersonLevel < second.s_nPersonLevel)
		return true;
	else
		return false;
}
bool CompGuildName(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_szName.compare(second.s_szName) > 0)
		return true;
	else
		return false;
}
bool CompGuildNameReverse(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_szName.compare(second.s_szName) < 0)
		return true;
	else
		return false;
}
bool CompGuildRank(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_eRank > second.s_eRank)
		return true;
	else
		return false;
}
bool CompGuildRankReverse(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_eRank < second.s_eRank)
		return true;
	else
		return false;
}
bool CompGuildContribution(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_nContribution > second.s_nContribution)
		return true;
	else
		return false;
}
bool CompGuildContributionReverse(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_nContribution < second.s_nContribution)
		return true;
	else
		return false;
}
bool CompGuildMap(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_dwMapID > second.s_dwMapID)
		return true;
	else
		return false;
}
bool CompGuildMapReverse(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
	if(first.s_dwMapID < second.s_dwMapID)
		return true;
	else
		return false;
}

bool CompGuildUserRank(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
#ifdef GUILD_RENEWAL
	if(first.s_nUserRank > second.s_nUserRank)
		return true;
	else
#endif
		return false;
}
bool CompGuildUserRankReverse(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
#ifdef GUILD_RENEWAL
	if(first.s_nUserRank < second.s_nUserRank)
		return true;
	else
#endif
		return false;
}
bool CompGuildLastTime(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
#ifdef GUILD_RENEWAL
	if(first.s_nLastPlayTime > second.s_nLastPlayTime)
		return true;
	else
#endif
		return false;
}
bool CompGuildLastTimeReverse(cCommunityContent::sGUILD_PERSON_INFO first, cCommunityContent::sGUILD_PERSON_INFO second)
{
#ifdef GUILD_RENEWAL
	if(first.s_nLastPlayTime < second.s_nLastPlayTime)
		return true;
	else
#endif
		return false;
}
#ifdef GUILD_RENEWAL

void cCommunityContent::_PersonSort( int nButton, bool bSort )
{
	if(bSort == true)
	{
		switch(nButton)
		{
		case 0://level
			m_ListPersonInfo.sort(CompGuildLevelReverse);
			break;
		case 1://name
			m_ListPersonInfo.sort(CompGuildNameReverse);
			break;
		case 2://rank
			m_ListPersonInfo.sort(CompGuildRankReverse);
			break;
		case 3://
			m_ListPersonInfo.sort(CompGuildUserRankReverse);
			break;
		case 4://접속시간
			m_ListPersonInfo.sort(CompGuildLastTimeReverse);
			break;
		default:
			break;
		}
	}
	else
	{
		switch(nButton)
		{
		case 0:
			m_ListPersonInfo.sort(CompGuildLevel);
			break;
		case 1:
			m_ListPersonInfo.sort(CompGuildName);
			break;
		case 2:
			m_ListPersonInfo.sort(CompGuildRank);
			break;
		case 3:
			m_ListPersonInfo.sort(CompGuildUserRank);
			break;
		case 4:
			m_ListPersonInfo.sort(CompGuildLastTime);
			break;
		default:
			break;
		}
	}
}
#else
void cCommunityContent::PersonSort( int nType, int nButton, bool bSort )
{
	if(bSort == true)
	{
		switch(nButton)
		{
		case 0://level
			m_ListPersonInfo.sort(CompGuildLevelReverse);
			break;
		case 1://name
			m_ListPersonInfo.sort(CompGuildNameReverse);
			break;
		case 2://rank
			switch( nType )
			{
			case cCommunity::INFO:
				// 위치
				m_ListPersonInfo.sort(CompGuildMapReverse);
				break;
			case cCommunity::ACTIVE:
				// 기여등급
				m_ListPersonInfo.sort(CompGuildContributionReverse);
				break;
			case cCommunity::RANK:
				// 직급
				m_ListPersonInfo.sort(CompGuildRankReverse);
				break;
			default:
				assert_cs( false );
				break;
			}
			
			break;
		case 3://
			m_ListPersonInfo.sort(CompGuildContributionReverse);
			break;
		default:
			break;
		}
	}
	else
	{
		switch(nButton)
		{
		case 0:
			m_ListPersonInfo.sort(CompGuildLevel);
			break;
		case 1:
			m_ListPersonInfo.sort(CompGuildName);
			break;
		case 2:
			switch( nType )
			{
			case cCommunity::INFO:
				// 위치
				m_ListPersonInfo.sort(CompGuildMap);
				break;
			case cCommunity::ACTIVE:
				// 기여등급
				m_ListPersonInfo.sort(CompGuildContribution);
				break;
			case cCommunity::RANK:
				// 직급
				m_ListPersonInfo.sort(CompGuildRank);
				break;
			default:
				assert_cs( false );
				break;
			}
			m_ListPersonInfo.sort(CompGuildRank);
			break;
		case 3:
			m_ListPersonInfo.sort(CompGuildContribution);
			break;
		default:
			break;
		}
	}
}
#endif	// GUILD_RENEWAL
// 적용 중인 스킬로 인한 변화값 가져오기

// 길드 공지 변경
void cCommunityContent::RecvGuildNoticeChange(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::wstring wsNotice = *(static_cast<std::wstring*>(pData));
	if( wsNotice.size() == 0 )
		m_GuildInfo.s_szNotice = UISTRING_TEXT( "COMMUNITY_WRITE_GUILD_NOTICE" );
	else
	{
		if( wsNotice.compare(m_GuildInfo.s_szNotice) != 0)
		{
			m_GuildInfo.s_szNotice = wsNotice;
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::GUILD_NOTICE;
			DmCS::StringFn::Format( CProtocol.m_wStr, L"%s : %s", UISTRING_TEXT( "COMMUNITY_GUILD_NOTICE" ).c_str(), wsNotice.c_str() );
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}
	}
	ContentsStream kStream;
	kStream << wsNotice;
	Notify(SetNoticeStr, kStream);
}
// 길드 활동 기록 받음
void cCommunityContent::RecvGuildHistory(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_HISTORY* pCastData = static_cast<GS2C_GUILD_HISTORY*>(pData);
	m_HistoryInfoList.clear();

	std::list<GuildInfo::sGuild_History>::iterator Itr = pCastData->m_GuildHistoryList.begin();
	std::list<GuildInfo::sGuild_History>::iterator EndItr = pCastData->m_GuildHistoryList.end();
	for(; Itr != EndItr; ++Itr)
	{
		sHISTORY_INFO sInfo;

		sInfo.s_nType = Itr->nType;
		sInfo.s_nMasterClass = Itr->nMasterClass;
		sInfo.s_nMemberClass = Itr->nMemberClass;
		sInfo.s_szMasterName = Itr->szMasterName;
		sInfo.s_szMemberName = Itr->szMemberName;

		if( Itr->nTime < (uint)1 )
			sInfo.s_nTime = 0;
		else
			sInfo.s_nTime = Itr->nTime;

		m_HistoryInfoList.push_back(sInfo);
		//SetHistory( sInfo );
	}

	Notify(MakeHistory);
}
void cCommunityContent::SetHistory( sHISTORY_INFO sInfo )
{
	if( m_HistoryInfoList.size() == 0 )
	{
		m_HistoryInfoList.push_back( sInfo ); 
	}
	else
	{
		bool bInsert = false;
		std::list< sHISTORY_INFO >::iterator Itr = m_HistoryInfoList.begin();
		std::list< sHISTORY_INFO >::iterator EndItr = m_HistoryInfoList.end();

		for( int idx = 0; Itr != EndItr; ++Itr, idx++ )
		{
			if( sInfo.s_nTime < (*Itr).s_nTime )
			{
				m_HistoryInfoList.insert( Itr, sInfo );
				bInsert = true;
				break;
			}
		}	

		if( bInsert == false )
			m_HistoryInfoList.push_back( sInfo );
	}
}

// 길드원 레벨 업 받음
void cCommunityContent::RecvGuildMemberLevelUp(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_MEMBERLEVELUP* pCastData = static_cast<GS2C_GUILD_MEMBERLEVELUP*>(pData);
	
	if( !(m_GuildInfo.s_szGuildName.size() > 0) )
		return;
	SetLevelUp( &(pCastData->wsTamerName), pCastData->nTamerLevel );
}
void cCommunityContent::SetLevelUp( std::wstring* szTamerName, int nLevel )
{
	if( IsHaveMyInfo() == false || GetMyInfo().s_eRank == GR_NONE )
		return;

	int nIdx = 0;
	cCommunityContent::sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szTamerName, nIdx );
	if( pInfo == NULL )
		return;
	pInfo->s_nPersonLevel = nLevel;

	ContentsStream kStream;
	kStream << nIdx;
	Notify(SetGuildPerson, kStream);
}
// 길드 폐쇄 보내기
void cCommunityContent::SendGuildDestroy(void* pData)
{
	if( IsHaveMyInfo() == true && GetMyInfo().s_eRank != cCommunityContent::GR_MASTER )
		return;
	net::game->SendGuildClose();
}
// 길드 삭제 주변인 모두 받음(BC 브로드캐스트)
void cCommunityContent::RecvGuildDestroyBC(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::wstring wsGuildName = *(static_cast<std::wstring*>(pData));
	
	//이름 밑에 길드이름 제거
	g_pCharMng->DeleteGuildName( wsGuildName.c_str() );
	//자기 자신의 길드이면
	if( wsGuildName.compare(m_GuildInfo.s_szGuildName ) == 0 )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SHOUT_CASH ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SHOUT_CASH );
		//길드 폐쇠
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_DESTROY, 0 );
		cPrintMsg::PrintMsg( 30123 );

		//길드창 갱신
		Delete();
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_COMMUNITY );
	}
}
// 길드 추가
void cCommunityContent::RecvGuildAdd(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_ADD* guildAdd = static_cast<GS2C_GUILD_ADD*>(pData);

	if(guildAdd->nGuildClass == 0)
	{
		CTamer* pTamer = (CTamer*)g_pMngCollector->GetObject( RTTI_TAMER, guildAdd->szTamerName.c_str() );
		if( pTamer != NULL )
			pTamer->GetCharOption()->SetGuildName( guildAdd->szGuildName.c_str() );

#ifdef GUILD_RENEWAL
		if( pTamer != NULL )
		{
			int nColorLv = 0;	// 색 변경 우선순위. 색 변경 스킬 효과 추가 시 우선순위 체크
			NiColor pColor;
			if( guildAdd->nSkillCode == 0 )
			{
				pColor = DEFAULT_NAMECOLOR;
			}
			else
			{
				CsSkill::sINFO* pInfo = nsCsFileTable::g_pSkillMng->GetSkill( guildAdd->nSkillCode )->GetInfo();
				for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
				{
					switch( pInfo->s_Apply[i].s_nA )
					{
					case GUILD_NAMECOLOR:	// 색 변경
						if( nColorLv < 1 )
						{
							float fR = (float)( pInfo->s_Apply[i].s_nB/100 );
							float fG = (float)( (pInfo->s_Apply[i].s_nB/10) % 10 );
							float fB = (float)( pInfo->s_Apply[i].s_nB%10 );
							pColor = NiColor( fR / 10.0f, fG / 10.0f, fB / 10.0f );
							break;
							nColorLv = 1;
						}
					default:
						break;
					}
				}
			}
			pTamer->GetCharOption()->SetNameColor( pColor );
		}
#endif
		return;
	}

	// nGuildClass 값은 nGuild::Member 로 가입됨	

	//길드 가입자가 본인이면
	if( _tcscmp( g_pCharMng->GetTamerUser()->GetName(), guildAdd->szTamerName.c_str() ) == 0 )
	{
		//본인 이면 길드 이름셋팅
		if( m_GuildInfo.s_szGuildName.size() > 0 )
		{
			g_pCharMng->GetTamerUser()->GetCharOption()->SetGuildName( guildAdd->szGuildName.c_str() );	
		}

		if( guildAdd->nGuildClass != 0 )
			cPrintMsg::PrintMsg( 30140 );

		//==========================================================================================================
		// 업적 체크
		//==========================================================================================================
		GS2C_RECV_CHECKTYPE recv;
		recv.nType = AchieveContents::CA_GuildJoin;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
	}
	else
	{
		cCommunityContent::sGUILD_PERSON_INFO sPerson;
		sPerson.s_eRank = (cCommunityContent::eRANK)guildAdd->nGuildClass;
		sPerson.s_bOnline = true;
		sPerson.s_dwTamerType = guildAdd->nTamerType;
		sPerson.s_dwMapID = guildAdd->nMapNo;
		sPerson.s_nChannel = guildAdd->nChannelNo;
		sPerson.s_nPersonLevel = guildAdd->nTamerLevel;
#ifdef GUILD_RENEWAL
		sPerson.s_nLastPlayTime = INT_MAX;	// 온라인
#endif
		sPerson.s_szName = guildAdd->szTamerName;
		SetJoinPerson( &sPerson );

		cPrintMsg::PrintMsg( 30127, (TCHAR*)guildAdd->szTamerName.c_str() );	
	}		

	RecvInvite( &(guildAdd->szTamerName) );
	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_INVATE, 0 );	

}
// 신규 길드 가입자일때
void cCommunityContent::SetJoinPerson( sGUILD_PERSON_INFO* pPerson )
{
	if( IsHaveMyInfo() == false || GetMyInfo().s_eRank == GR_NONE )
		return;

	SetGuildPersonInfo( pPerson );
	SetGuildInfoData();

	int nIdx = m_ListPersonInfo.size() - 1;
	ContentsStream kStream;
	kStream << nIdx;
	Notify(SetGuildPerson, kStream);
}
void cCommunityContent::RecvInvite( std::wstring* szName )
{
	// 나라면 패스
	
	if( szName->compare(g_pCharMng->GetTamerUser()->GetName()) == 0 )
	{		
		return;
	}

	if( IsHaveMyInfo() == true )
	{
		if( GetMyInfo().s_eRank == GR_MASTER || GetMyInfo().s_eRank == GR_2NDMASTER )
		{
			assert_cs( net::game );

			// 이미 보낸 목록에 존재 하는지 찾는다.
			std::list< sNAME >::iterator it = m_listInviteName.begin();
			std::list< sNAME >::iterator itEnd = m_listInviteName.end();
			for( ; it!=itEnd; ++it )
			{
				if( it->s_szName.compare( *szName ) == 0 )
				{
					m_listInviteName.erase( it );
					break;
				}
			}
		}
	}	
}
// 길드 초대 실패 받음
void cCommunityContent::RecvGuildInviteFail(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_REQADDFAILURE* pCastData = static_cast<GS2C_GUILD_REQADDFAILURE*>(pData);
	switch( pCastData->nCode )
	{
	case 1 :	// 이미 길드에 가입되어 있는 상대입니다.
		cPrintMsg::PrintMsg( 30129 );
		break; 
	case 2 :	// 접속중이지 않거나, 존재하지 않는 상대입니다.
		cPrintMsg::PrintMsg( 30137 );
		break;
	case 3:		// 상대방이 초대 받지 못하는 상황일때
		cPrintMsg::PrintMsg( 30143 );
		break;	
	case 4:		// 길드원이 FULL일때
		cPrintMsg::PrintMsg( 30130 );
		break;
	}

	RecvInvite( &(pCastData->wsTamerName) );
	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_INVATE, 0 );
}
// 길드 초대 거절
void cCommunityContent::RecvGuildReject(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::wstring* wsTamerName = static_cast<std::wstring*>(pData);
	RecvInvite( wsTamerName );
	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_INVATE, 0 );

	cPrintMsg::PrintMsg( 30128, (TCHAR*)wsTamerName->c_str() );
}
// 길드 멤버 삭제
void cCommunityContent::RecvGuildMemberDelete(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::wstring* wsTamerName = static_cast<std::wstring*>(pData);
	SetLeavePerson( wsTamerName, 0 );
}
// 이름 변경 길드에 적용 받음
void cCommunityContent::RecvGuildTamerNameChange(void* pData)
{
	SAFE_POINTER_RET(pData);
	//first fromName second toName
	std::pair<std::wstring, std::wstring>* pCastData = static_cast<std::pair<std::wstring, std::wstring>*>(pData);
	ReName( &(pCastData->first), &(pCastData->second) );

}
void cCommunityContent::ReName( std::wstring* szFrom, std::wstring* szTo )
{
	if( IsHaveMyInfo() == false || GetMyInfo().s_eRank == GR_NONE )
		return;

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szFrom, nIdx );	
	if( pInfo == NULL )
		return;
	pInfo->s_szName = *szTo;

	if( pInfo->s_eRank == GR_MASTER )
	{
		m_GuildInfo.s_szMasterName = *szTo;
		SetGuildInfoData();	
	}
	ContentsStream kstreamPopup;
	kstreamPopup << nIdx;
	Notify(CloseGuildMenuPopup, kstreamPopup);
	ContentsStream kstream;
	kstream << nIdx;
	Notify(SetGuildPerson, kstream);
}
// 길드 탈퇴 시키기(길드장만 가능)
void cCommunityContent::SendGuildBan(void* pData)
{
	SAFE_POINTER_RET(pData);
	TCHAR* pCastData = static_cast<TCHAR*>(pData);
	std::wstring wsTamerName(pCastData);

	if( pCastData == NULL )
		return;

	if( GetRank( &wsTamerName ) == GR_NONE )
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_BAN, 0 );
		cPrintMsg::PrintMsg( 30155 );
		return;
	}
	net::game->SendGuildDelete( &wsTamerName );
}

// 길드 ON 받음
void cCommunityContent::RecvGuildOn(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_ON* pCastData = static_cast<GS2C_GUILD_ON*>(pData);
	SetGuildOnOff( true, &(pCastData->szTamerName), pCastData->nMapNo, pCastData->nChannelNo, pCastData->nTamerLevel );
}
// 길드 OFF 받음
void cCommunityContent::RecvGuildOff(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::wstring* szTamerName = static_cast<std::wstring*>(pData);
	int index = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo(szTamerName, index);
	SAFE_POINTER_RET(pInfo);
	pInfo->s_bOnline = false;
#ifdef GUILD_RENEWAL
	pInfo->s_nLastPlayTime = _TIME_TS;	// 접속시간 저장
#endif
}

// 길드 등급명 변경 보내기
void cCommunityContent::SendGuildChangeClassName(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_CHANGE_CLASSNAME_SEND* pChangeClassName = static_cast<GS2C_GUILD_CHANGE_CLASSNAME_SEND*>(pData);

	if( g_pCharMng->GetDigimonUser( 0 )->IsBattle() || g_pDataMng->GetServerSync()->IsChageDigimon() )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}
	// 메세지 박스 떠있는게 있다면 실패
	if( cMessageBox::IsMessageBox() )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}
#if ( defined VERSION_TW || defined VERSION_HK )
	// 사용해도 되는이름인지 검사	
	if( nsCsFileTable::g_pCuidMng->CheckID( pChangeClassName->wsName.c_str() ) == false )
	{
		cPrintMsg::PrintMsg( 10029 );
		return;
	}
#endif
	
	if( pChangeClassName->wsName.compare( *GetRankName( (cCommunityContent::eRANK)pChangeClassName->nClass )) == 0 )
	{
		cPrintMsg::PrintMsg( 30157 );
		return;
	}

	if( IsHaveMyInfo() == true && GetMyInfo().s_eRank != GR_MASTER )
		return;
	
	if (net::game == NULL)
		return;

	

	//길드 등급명 변경	
	if (pChangeClassName->baseWindowType == cBaseWindow::WT_GUILDRANK_NAME)
	{
		switch( Language::CheckGuildClassName( pChangeClassName->wsName ) )
		{
		case Language::STRING_EMPTY:
		case Language::STRING_SIZE_MIN:
		case Language::STRING_SIZE_MAX:
		case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10024 );	return;
		}

		std::wstring wsRankDesc(*GetRankDesc( (cCommunityContent::eRANK)pChangeClassName->nClass));
		switch( Language::CheckGuildClassNameDesc( wsRankDesc ) )
		{
		case Language::STRING_EMPTY:
		case Language::STRING_SIZE_MIN:
		case Language::STRING_SIZE_MAX:
		case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10024 );	return;
		}
		net::game->SendGuildChangeClassName( pChangeClassName->nClass, pChangeClassName->wsName.c_str(), wsRankDesc.c_str() );
	}

	else if (pChangeClassName->baseWindowType == cBaseWindow::WT_GUILDRANK_EXPLAIN)
	{
		std::wstring wsRankName( *GetRankName( (cCommunityContent::eRANK)pChangeClassName->nClass ) );
		switch( Language::CheckGuildClassName( wsRankName ) )
		{
		case Language::STRING_EMPTY:
		case Language::STRING_SIZE_MIN:
		case Language::STRING_SIZE_MAX:
		case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10024 );	return;
		}

		switch( Language::CheckGuildClassNameDesc( pChangeClassName->wsName ) )
		{
		case Language::STRING_EMPTY:
		case Language::STRING_SIZE_MIN:
		case Language::STRING_SIZE_MAX:
		case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10024 );	return;
		}
		net::game->SendGuildChangeClassName( pChangeClassName->nClass, wsRankName.c_str(), pChangeClassName->wsName.c_str() );
	}
}
// 길드랭크 변경 받음
void cCommunityContent::RecvGuildChangeClassName(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_CHANGE_CLASSNAME_RECV* pChangeClassName = static_cast<GS2C_GUILD_CHANGE_CLASSNAME_RECV*>(pData);

	SetRankName( (eRANK)pChangeClassName->nClass, &(pChangeClassName->szName) );
	SetRankDesc( (eRANK)pChangeClassName->nClass, &(pChangeClassName->szDesc) );

	ContentsStream kstream;
	kstream << cCommunity::TAB_GUILD;
	Notify(SetUITab, kstream);
}
void cCommunityContent::SetClassRefresh( std::wstring* szTamerName, eRANK nClass )
{
	if( IsHaveMyInfo() == false || GetMyInfo().s_eRank == GR_NONE )
		return;

	switch( nClass )
	{
	case GR_MASTER:		_ChangeGuildMaster( szTamerName );	break;
	case GR_2NDMASTER:	_Change2ndMaster( szTamerName );	break;
	case GR_NORMAL:		_ChangeNormal( szTamerName );		break;
	case GR_DAT:		_ChangeDatMember( szTamerName );	break;
	case GR_NEW:		_ChangeNewbi( szTamerName );		break;
	default:
		assert_cs( false );
		break;
	}
}
void cCommunityContent::_ChangeGuildMaster( std::wstring* szName )
{
	LIST_GUILD_IT it = m_ListPersonInfo.begin();
	LIST_GUILD_IT itEnd = m_ListPersonInfo.end();
	int nMasterIdx = 0;
	for( ; it!=itEnd; ++it, ++nMasterIdx )
	{
		if( it->s_eRank == GR_MASTER )
		{
			it->s_eRank = GR_NORMAL;					

			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
			{	
				// 기존 길마가 본인이었다면
				if( it->s_szName.compare( g_pCharMng->GetTamerUser()->GetName() ) == 0 )
				{
					g_pGameIF->GetCommunity()->SetNoticeBtn( false );
					if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILDADMIN )
						g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_GUILDADMIN );
				}	
				ContentsStream kStream;
				kStream << nMasterIdx;
				Notify(SetGuildPerson, kStream);
			}
			break;
		}
	}	

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	pInfo->s_eRank = GR_MASTER;
	m_GuildInfo.s_szMasterName = *szName;

	if( g_pGameIF && g_pGameIF->GetCommunity() )
	{
		// 정보창 갱신
		g_pGameIF->GetCommunity()->MakeGuildInfo();

		// 새로운 길마가 본인이라면
		
		if( szName->compare(g_pCharMng->GetTamerUser()->GetName()) == 0 )
		{
			g_pGameIF->GetCommunity()->SetNoticeBtn( true );
			if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILDADMIN )
			{
				ContentsStream kstream;
				kstream << cCommunity::TAB_GUILDADMIN;
				Notify(SetUITab, kstream);
			}
		}	

		ContentsStream kStream;
		kStream << nIdx;
		Notify(SetGuildPerson, kStream);
	}
}

void cCommunityContent::_Change2ndMaster( std::wstring* szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	pInfo->s_eRank = GR_2NDMASTER;

	m_GuildInfo.s_n2AndMasterNum++;
	ContentsStream kStream;
	kStream << nIdx;
	Notify(SetGuildPerson, kStream);
}

void cCommunityContent::_ChangeDatMember( std::wstring* szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	assert_cs( pInfo != NULL );
	if( pInfo->s_eRank == GR_2NDMASTER )
		m_GuildInfo.s_n2AndMasterNum--;

	pInfo->s_eRank = GR_DAT;			

	ContentsStream kStream;
	kStream << nIdx;
	Notify(SetGuildPerson, kStream);
}

void cCommunityContent::_ChangeNormal( std::wstring* szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	assert_cs( pInfo != NULL );
	if( pInfo->s_eRank == GR_2NDMASTER )
		m_GuildInfo.s_n2AndMasterNum--;

	pInfo->s_eRank = GR_NORMAL;			

	ContentsStream kStream;
	kStream << nIdx;
	Notify(SetGuildPerson, kStream);
}

void cCommunityContent::_ChangeNewbi( std::wstring* szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	assert_cs( pInfo != NULL );
	if( pInfo->s_eRank == GR_2NDMASTER )
		m_GuildInfo.s_n2AndMasterNum--;

	pInfo->s_eRank = GR_NEW;			

	ContentsStream kStream;
	kStream << nIdx;
	Notify(SetGuildPerson, kStream);
}

// 길드 등급 임명 받음
void cCommunityContent::RecvGuildRankSet(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<cData_ServerSync::eSYNC, std::wstring>* pCastData = static_cast<std::pair<cData_ServerSync::eSYNC, std::wstring>*>(pData);

	wstring wsRankName = pCastData->second;

	switch(pCastData->first)
	{
	case cData_ServerSync::eSYNC::GUILD_AP_MASTER:
		{
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_MASTER, 0 );
			SetClassRefresh( &wsRankName, cCommunityContent::GR_MASTER );
			cPrintMsg::PrintMsg( 30133, (TCHAR*)wsRankName.c_str(), (TCHAR*)GetRankName( cCommunityContent::GR_MASTER )->c_str() );
		}
		break;
	case cData_ServerSync::eSYNC::GUILD_AP_2MASTER:
		{
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_2MASTER, 0 );
			SetClassRefresh( &wsRankName, cCommunityContent::GR_2NDMASTER );
			cPrintMsg::PrintMsg( 30133, (TCHAR*)wsRankName.c_str(), (TCHAR*)GetRankName( cCommunityContent::GR_2NDMASTER )->c_str() );
		}
		break;
	case cData_ServerSync::eSYNC::GUILD_AP_DAT:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SHOUT_CASH ) )
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SHOUT_CASH );
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_DAT, 0 );
			SetClassRefresh( &wsRankName, cCommunityContent::GR_DAT );
			cPrintMsg::PrintMsg( 30133, (TCHAR*)wsRankName.c_str(), (TCHAR*)GetRankName( cCommunityContent::GR_DAT )->c_str() );
		}
		break;
	case cData_ServerSync::eSYNC::GUILD_AP_NORMAL:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SHOUT_CASH ) )
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SHOUT_CASH );
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_NORMAL, 0 );
			SetClassRefresh( &wsRankName, cCommunityContent::GR_NORMAL );
			cPrintMsg::PrintMsg( 30133, (TCHAR*)wsRankName.c_str(), (TCHAR*)GetRankName( cCommunityContent::GR_NORMAL )->c_str() );
		}
		break;
	case cData_ServerSync::eSYNC::GUILD_AP_NEWBI:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SHOUT_CASH ) )
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SHOUT_CASH );
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_NEWBI, 0 );
			SetClassRefresh( &wsRankName, cCommunityContent::GR_NEW );
			cPrintMsg::PrintMsg( 30133, (TCHAR*)wsRankName.c_str(), (TCHAR*)GetRankName( cCommunityContent::GR_NEW )->c_str() );
		}
		break;
	}
}
// 길드 등급 임명 보내기
void cCommunityContent::SendGuildRankSet(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<cData_ServerSync::eSYNC, TCHAR*>* pCastData = static_cast<std::pair<cData_ServerSync::eSYNC, TCHAR*>*>(pData);
	wstring wsRankName(pCastData->second);
#ifdef TAIWAN_NAME_LENGTH
	if( wsRankName.size() > NAME_MAX_LEN )
		return;
#else
	if( wsRankName.size() > Language::pLength::name )
		return;
#endif

	switch(pCastData->first)
	{
	case cData_ServerSync::eSYNC::GUILD_AP_MASTER:
		{
			if( GetRank( &wsRankName ) == GR_NONE )
			{		
				cPrintMsg::PrintMsg( 30155 );
				g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_MASTER, 0 );
				return;
			}
			net::game->SendGuildToMaster( &wsRankName );
		}
		break;
	case cData_ServerSync::eSYNC::GUILD_AP_2MASTER:
		{
			if( GetRank( &wsRankName ) == GR_NONE )
			{		
				cPrintMsg::PrintMsg( 30155 );
				g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_2MASTER, 0 );
				return;
			}
			net::game->SendGuildToSubMaster( &wsRankName );
		}
		break;
	case cData_ServerSync::eSYNC::GUILD_AP_DAT:
		{
			if( GetRank( &wsRankName ) == GR_NONE )
			{		
				cPrintMsg::PrintMsg( 30155 );
				g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_DAT, 0 );
				return;
			}
			net::game->SendGuildToDatsMember( &wsRankName );
		}
		break;
	case cData_ServerSync::eSYNC::GUILD_AP_NORMAL:
		{
			if( GetRank( &wsRankName ) == GR_NONE )
			{		
				cPrintMsg::PrintMsg( 30155 );
				g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_NORMAL, 0 );
				return;
			}
			net::game->SendGuildToMember( &wsRankName );
		}
		break;
	case cData_ServerSync::eSYNC::GUILD_AP_NEWBI:
		{
			if( GetRank( &wsRankName ) == GR_NONE )
			{		
				cPrintMsg::PrintMsg( 30155 );
				g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GUILD_AP_NEWBI, 0 );
				return;
			}
			net::game->SendGuildToSubMember( &wsRankName );
		}
		break;
	}
}
cCommunityContent::eRANK cCommunityContent::GetRank( std::wstring* szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );	
	if( pInfo != NULL )
	{
		return pInfo->s_eRank;
	}
	return GR_NONE;
}
// 길드 스킬 사용여부 받음
void cCommunityContent::RecvGuildSkillSet(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_SKILL* pCastData = static_cast<GS2C_GUILD_SKILL*>(pData);

#ifdef GUILD_RENEWAL
	SetUsingSkill(pCastData->bUsing, pCastData->nSkillCode, pCastData->nEndTime, pCastData->nUseTime);
#endif

}

void cCommunityContent::SetGuildOnOff( bool bValue, std::wstring* szTamerName, DWORD nMapID, int nChannel, int nLevel )
{
// 	if( IsHaveMyInfo() == false )
// 		return;

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szTamerName, nIdx );
	SAFE_POINTER_RET(pInfo);

	assert_cs( GetMyInfo().s_eRank != GR_NONE );

	if( bValue )
		m_GuildInfo.s_nCurPersonCount++;
	else
		m_GuildInfo.s_nCurPersonCount--;

	//assert_cs( pInfo != NULL );
	//SAFE_POINTER_RET(pInfo);
	pInfo->s_bOnline = bValue;
	if (bValue)
	{
		pInfo->s_dwMapID = nMapID;
		pInfo->s_nChannel = nChannel;	
		pInfo->s_nPersonLevel = nLevel;
	}
	
#ifdef GUILD_RENEWAL
	if( pInfo->s_bOnline )
		pInfo->s_nLastPlayTime = INT_MAX;	//온라인
	else
		pInfo->s_nLastPlayTime = _TIME_TS;	//로그아웃 시간
#endif

	if( ( pInfo->s_bInit == false ) && ( bValue == true ) )
	{
		cPrintMsg::PrintMsg( 30124, (TCHAR*)pInfo->s_szName.c_str() );
		pInfo->s_bInit = true;
	}
	ContentsStream kStream;
	kStream << nIdx;
	Notify(SetGuildPerson, kStream);
}

#ifdef GUILD_RENEWAL
void cCommunityContent::GetUsingSkillValue(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int, int>* pCastData = static_cast<std::pair<int, int>*>(pData);
	(*pCastData).second = GetUsingSkillValue( pCastData->first );
}
// 길드 메모 보내기
void cCommunityContent::SendGuildMemo(void* pData)
{
	SAFE_POINTER_RET(pData);
	SAFE_POINTER_RET(IsHaveMyInfo());
	std::wstring MemoInput = *(static_cast<std::wstring*>(pData));

	if( 0 != MemoInput.compare(GetMyInfo().s_szGuildMemo) )
	{
		LIST_GUILD_IT itr = m_ListPersonInfo.begin();
		LIST_GUILD_IT endItr = m_ListPersonInfo.end();
		for (; itr != endItr; ++itr)
		{
			if( itr->s_szName.compare(g_pCharMng->GetTamerUser()->GetName()) == 0 )
			{
				itr->s_szGuildMemo = MemoInput;
				net::game->SendGuildMemoAdd( MemoInput.c_str() );
			}
		}
	}
}
// 길드 스킬포인트 받음
void cCommunityContent::RecvGuildSkillPoint(void* pData)
{
	SAFE_POINTER_RET(pData);
	u4 nGSP = *(static_cast<u4*>(pData));
	m_GuildInfo.s_nGuildSkillPoint = nGSP;
	Notify(SetGuildSkill);
}
bool cCommunityContent::sGUILDSKILL_INFO::_IsCoolTime() const
{
	return _TIME_TS < s_nEndTime;	// ( 현재시간 < 쿨타임 종료 시간 ) true : 쿨타임 중 , false : 쿨타임 아님 
}
// 길드원 메모 받음
void cCommunityContent::RecvGuildChangeMemo(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_CHAGE_MEMO* pCastData =  static_cast<GS2C_GUILD_CHAGE_MEMO*>(pData);
	int index = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo(&(pCastData->wsTamerName), index);
	pInfo->s_szGuildMemo = pCastData->wsMemo;
}
// 길드 레벨 업 받음
void cCommunityContent::RecvGuildLevelUp(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_GUILD_LEVELUP* pCastData = static_cast<GS2C_GUILD_LEVELUP*>(pData);

	CsC_AvObject* pTamer = g_pMngCollector->GetObject( pCastData->nTamerUID );
	if( pTamer == NULL )
		return;

	switch( pTamer->GetLeafRTTI() )
	{
	case RTTI_TAMER:	// 옆 테이머 길드 레벨업( 색 변경 시에만 올 것임 )
		{
			int nColorLv = 0;	// 색 변경 우선순위. 색 변경 스킬 효과 추가 시 우선순위 체크
			NiColor pColor;
			if( pCastData->nSkillCode == 0 )
			{
				pColor = DEFAULT_NAMECOLOR;
			}
			else
			{
				CsSkill::sINFO* pInfo = nsCsFileTable::g_pSkillMng->GetSkill( pCastData->nSkillCode )->GetInfo();
				for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
				{
					switch( pInfo->s_Apply[i].s_nA )
					{
					case GUILD_NAMECOLOR:	// 색 변경
						if( nColorLv < 1 )
						{
							float fR = (float)( pInfo->s_Apply[i].s_nB/100 );
							float fG = (float)( (pInfo->s_Apply[i].s_nB/10) % 10 );
							float fB = (float)( pInfo->s_Apply[i].s_nB%10 );
							pColor = NiColor( fR / 10.0f, fG / 10.0f, fB / 10.0f );
							break;
							nColorLv = 1;
						}
					default:
						break;
					}
				}
			}
			( (CTamer*)pTamer )->GetCharOption()->SetNameColor( pColor );
		}
		break;
	case RTTI_TAMER_USER:	// 본인 길드 레벨업
		{
			m_GuildInfo.s_nGuildLevel = pCastData->nLevel;
			Notify(SetGuildInfo);
			SetBaseSkill( pCastData->nLevel );
			SetGuildInfoData();
			Notify(SetGuildPersonCnt);
		}
		break;
	default:
		break;
	}
}

#endif

// 길마(부길마) 소환 받음
void cCommunityContent::RecvGuildSummon(void* pData)
{
	SAFE_POINTER_RET(pData);
	int nSkillCode = *(static_cast<int*>(pData));
	if(FLOWMGR_ST.GetCurTopFlowID() == Flow::CFlow::FLW_DATSCENTER)
		return;
	if(g_pCharMng->GetDigimonUser( 0 )->GetDigimonState() == CDigimon::DIGIMON_DIE)
		return;
	// 스킬 수락 여부 메세지.
	cPrintMsg::PrintMsg(30209);
	cMessageBox::GetFirstMessageBox()->SetValue1( nSkillCode );

}
void cCommunityContent::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Guild_Channel:
		{
			SAFE_POINTER_RET(IsHaveMyInfo());
			kStream.SetRp(0);
			int nChannelIndex;
			kStream >> nChannelIndex;
			SetMyChannel(nChannelIndex);
		}
		break;
	}
}
// 길드원 레벨 변경
void cCommunityContent::SetPersonLevel(void* pData)
{
	SAFE_POINTER_RET(pData);
	int	nLevel = *(static_cast<int*>(pData));
	
	LIST_GUILD_IT itr = m_ListPersonInfo.begin();
	LIST_GUILD_IT endItr = m_ListPersonInfo.end();
	for (; itr != endItr; ++itr)
	{
		if( itr->s_szName.compare(g_pCharMng->GetTamerUser()->GetName()) == 0 )
		{
			if( itr->s_eRank != cCommunityContent::GR_NONE )
				itr->s_nPersonLevel = nLevel;
		}
	}
}

void cCommunityContent::SetShowOfflinePerson(bool bValue)
{
	m_bShowOfflinePerson = bValue;
}

bool cCommunityContent::IsShowOfflinePerson() const
{
	return m_bShowOfflinePerson;
}