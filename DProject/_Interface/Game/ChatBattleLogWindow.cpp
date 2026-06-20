#include "StdAfx.h"

#include "../../ContentsSystem/ContentsSystem.h"
#include "../../ContentsSystem/ContentsSystemDef.h"


#include "ChatContents.h"
#include "ChatBase.h"
#include "ChatBattleLogWindow.h"

ChatBattleLogWindow::ChatBattleLogWindow(void)
{
	SetStringPos( CsPoint(8, 12));
	SetMovePosY(3);	
	//SetOutWindow(true);
}

ChatBattleLogWindow::~ChatBattleLogWindow(void)
{
	SaveUIOption(XML_CHAT_BATTLELOG);

	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void ChatBattleLogWindow::Destroy()
{	
	for( InfoIter = m_pBattleLogList.begin(); InfoIter != m_pBattleLogList.end(); ++InfoIter )
	{
		SAFE_NIDELETE( *InfoIter );	
	}

	m_pBattleLogList.clear();
	ChatBase::Destroy();
}

void ChatBattleLogWindow::DeleteResource()
{
	ChatBase::DeleteResource();
}

void ChatBattleLogWindow::ResetDevice()
{
	ChatBase::ResetDevice();
}

void ChatBattleLogWindow::ResetMap()
{
	ChatBase::ResetMap();
}

bool ChatBattleLogWindow::Close( bool bSound /* = true  */ )
{
	ChatBase::Close( bSound );
	return true;
}

void ChatBattleLogWindow::Create( int nValue /*=0*/ )
{
	LoadUIOption(XML_CHAT_BATTLELOG);

	ChatBase::Create( nValue );
}

void ChatBattleLogWindow::Update(float const& fDeltaTime)
{
	ChatBase::Update(fDeltaTime);

	for( InfoIter = m_pBattleLogList.begin(); InfoIter != m_pBattleLogList.end();  )
	{
		(*InfoIter)->s_fRemainTime -= g_fDeltaTime;
		if( (*InfoIter)->s_fRemainTime <= 0.0f )
		{
			_UpdateText( (*InfoIter)->s_szBattleLog, (*InfoIter)->s_Color );
			SAFE_NIDELETE( *InfoIter );
			InfoIter = m_pBattleLogList.erase( InfoIter );
			continue;
		}	
		++InfoIter;
	}
}

cBaseWindow::eMU_TYPE ChatBattleLogWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = ChatBase::Update_ForMouse();
	return muReturn;
}

void ChatBattleLogWindow::Render()
{
	 ChatBase::Render();
}

void ChatBattleLogWindow::StringListRender()
{
	CsPoint pos = m_ptRootClient + CsPoint( GetStringPos().x, GetStringPos().y );

	m_String.Render(pos, m_MovePosY);
}

void ChatBattleLogWindow::SetAlpha(float alpha)
{
}

void ChatBattleLogWindow::ControlUpdate(int SclrollSize)
{
	while( m_String.GetSize() > MAX_BATTLELOG )
	{
		m_String.RemoveHead();			
	}

	m_pScrollBar->SetRange( CsPoint( 0, m_String.GetSize() ) );

	if( m_pScrollBar->IsEnableScroll() )
	{	
		m_nCurIdx += SclrollSize;
		m_pScrollBar->SetCurPosIndex( m_nCurIdx );
	}
	else
	{		
		m_pScrollBar->SetCurPosIndex( m_nCurIdx );
	}
}

void ChatBattleLogWindow::_MoveUi( void * sender, void* pData )
{
	_UpdateMoveWindow_ForMouse();
}

void ChatBattleLogWindow::_ChageBGSize( void * sender, void* pData )
{
	ChatBase::_ChageBGSize(NULL, pData);
}

void ChatBattleLogWindow::_ChageBGUpdate( void * sender, void* pData )
{

}

void ChatBattleLogWindow::SetLog( CsC_AvObject* pHitter, CsC_AvObject* pTarget, float nRemainTime, int nTotalDem, sRECV_HITTER_INFO* pHitType, sRECV_TARGET_INFO* pDemType )
{
	if( !IsShowWindow() )
		return;

	sBattleInfo* pBattleInfo = NiNew sBattleInfo;
	pBattleInfo->s_fRemainTime = nRemainTime;		

	//hitter가 나일때
	if( pHitter && pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER )
	{
		switch( pDemType->s_DamageType )
		{
		case CsC_AttackProp::DT_Normal:
			{
				switch( pHitType->s_HitType )
				{
				case CsC_AttackProp::HT_Normal:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_ATTACK" );
						DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 
				case CsC_AttackProp::HT_Critical:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_CRITICAL_ATTACK" );
						DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 
				case CsC_AttackProp::HT_Miss:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_MISS" );
						DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 
				case CsC_AttackProp::HT_Skill:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_SKILL_DAMAGE" );
						std::wstring wsSkill;
						cSkill* pSkillMng = ( (CDigimon*)pHitter )->GetSkillMng();
						if( pSkillMng )
						{
							CsSkill* pFTSkill = pSkillMng->GetFTSkill( pHitType->s_nSkillIndex );
							if( pFTSkill )
							{
								CsSkill::sINFO* pSkillInfo = pFTSkill->GetInfo();
								if( pSkillInfo )
									wsSkill = pSkillInfo->s_szName;
								if( wsSkill.empty() )
									wsSkill = L"Unknown Skill";
							}
						}
						DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
						DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 	
					// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
				case CsC_AttackProp::HT_Absorb:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_ATTACK_ABSORB_ALL" );
						std::wstring wsSkill;
						CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
						if( pFTSkill )
						{
							CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
							if( pSkillInfo )
								wsSkill = pSkillInfo->s_szName;
							if( wsSkill.empty() )
								wsSkill = L"Unknown Buff";
						}
						DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
						DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 
				}		
			}
			break;
		case CsC_AttackProp::DT_Miss:
		case CsC_AttackProp::DT_Block:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_MISS" );
				DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break; 			
		case CsC_AttackProp::DT_Dead:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_KILL" );
				DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break; 	
		case CsC_AttackProp::DT_Damage_Dot:		
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_SKILL_DAMAGE" );
				std::wstring wsSkill;
				CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
				if( pFTSkill )
				{
					CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
					if( pSkillInfo )
						wsSkill = pSkillInfo->s_szName;
					if( wsSkill.empty() )
						wsSkill = L"Unknown Buff";
				}
				DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
				DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
		case CsC_AttackProp::DT_Recover:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_SKILL_RECOVERY_TARGET" );
				std::wstring wsSkill;
				CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
				if( pFTSkill )
				{
					CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
					if( pSkillInfo )
						wsSkill = pSkillInfo->s_szName;
					if( wsSkill.empty() )
						wsSkill = L"Unknown Buff";
				}
				DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
				DmCS::StringFn::Replace( wsMsg, L"#Recover#", nTotalDem );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
			// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
		case CsC_AttackProp::DT_Absorb:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_ATTACK_ABSORB_DAMAGE" );
				std::wstring wsSkill;
				CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
				if( pFTSkill )
				{
					CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
					if( pSkillInfo )
						wsSkill = pSkillInfo->s_szName;
					if( wsSkill.empty() )
						wsSkill = L"Unknown Buff";
				}
				DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
		}	

		pBattleInfo->s_Color = NiColor( 1.0f, 1.0f, 0.0f );
		m_pBattleLogList.push_back( pBattleInfo );
	}
	else if( pTarget && pTarget->GetLeafRTTI() == RTTI_DIGIMON_USER ) //내가 Target일때
	{	
		switch( pDemType->s_DamageType )
		{
		case CsC_AttackProp::DT_Normal:
			{
				switch( pHitType->s_HitType )
				{
				case CsC_AttackProp::HT_Normal:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_DAMAGED" );
						DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break;
				case CsC_AttackProp::HT_Critical:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_CRITICAL_DAMAGED" );
						DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break;
				case CsC_AttackProp::HT_Miss:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_AVOID" );
						DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break;
				case CsC_AttackProp::HT_Skill:
				case CsC_AttackProp::HT_Skill_NoAni:	
					if( RTTI_MONSTER == pHitter->GetLeafRTTI())
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_DAMAGED" );
						DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					else
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_SKILL_DAMAGED" );
						std::wstring wsSkill;
						cSkill* pSkillMng = ( (CDigimon*)pHitter )->GetSkillMng();
						if( pSkillMng )
						{
							CsSkill* pFTSkill = pSkillMng->GetFTSkill( pHitType->s_nSkillIndex );
							if( pFTSkill )
							{
								CsSkill::sINFO* pSkillInfo = pFTSkill->GetInfo();
								if( pSkillInfo )
									wsSkill = pSkillInfo->s_szName;
								if( wsSkill.empty() )
									wsSkill = L"Unknown Skill";
							}
						}
						DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
						DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break;		
					// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
				case CsC_AttackProp::HT_Absorb:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_ATTACK_ABSORB_ALL" );
						std::wstring wsSkill;
						CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
						if( pFTSkill )
						{
							CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
							if( pSkillInfo )
								wsSkill = pSkillInfo->s_szName;
							if( wsSkill.empty() )
								wsSkill = L"Unknown Buff";
						}
						DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
						DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 
				}		
			}
			break;
		case CsC_AttackProp::DT_Miss:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_AVOID" );
				DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
#if (defined VERSION_KOR || defined VERSION_USA)
		case CsC_AttackProp::DT_Block:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_BLOCK" );
				DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
#endif
		case CsC_AttackProp::DT_Dead:			
			_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, UISTRING_TEXT( "CHAT_BATTLELOG_PARTNER_FELL_DOWN" ).c_str() );	
			break;	
		case CsC_AttackProp::DT_Damage_Dot:		
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_SKILL_DOT_DAMAGED" );
				std::wstring wsSkill;
				CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
				if( pFTSkill )
				{
					CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
					if( pSkillInfo )
						wsSkill = pSkillInfo->s_szName;
					if( wsSkill.empty() )
						wsSkill = L"Unknown Buff";
				}
				DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
		case CsC_AttackProp::DT_Recover:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_SKILL_RECOVERY" );
				std::wstring wsSkill;
				CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
				if( pFTSkill )
				{
					CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
					if( pSkillInfo )
						wsSkill = pSkillInfo->s_szName;
					if( wsSkill.empty() )
						wsSkill = L"Unknown Buff";
				}
				DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Recover#", nTotalDem );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
			// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
		case CsC_AttackProp::DT_Absorb:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_ATTACK_ABSORB_DAMAGE" );
				std::wstring wsSkill;
				CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
				if( pFTSkill )
				{
					CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
					if( pSkillInfo )
						wsSkill = pSkillInfo->s_szName;
					if( wsSkill.empty() )
						wsSkill = L"Unknown Buff";
				}
				DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
		}	

		pBattleInfo->s_Color = NiColor( 1.0f, 128.0f / 255.0f, 0.0f );
		m_pBattleLogList.push_back( pBattleInfo );
	}	
	if( pHitter && pHitter->GetLeafRTTI() == RTTI_TAMER_USER )
	{
		CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( pHitType->s_nSkillIndex, -1 );
		CsSkill::sINFO* pFTSkillInfo = pFTSkillInfo = nsCsFileTable::g_pSkillMng->GetSkill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode )->GetInfo();

		switch( pDemType->s_DamageType )
		{
		case CsC_AttackProp::DT_Normal:
			{
				switch( pHitType->s_HitType )
				{
				case CsC_AttackProp::HT_Normal:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_ATTACK" );
						DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 
#if (defined VERSION_KOR || defined VERSION_USA)
				case CsC_AttackProp::HT_Critical:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_CRITICAL_SKILL" );
						DmCS::StringFn::Replace( wsMsg, L"#Skill#", pFTSkillInfo->s_szName );
						DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 
#endif
				case CsC_AttackProp::HT_Miss:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_MISS" );
						DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 
				case CsC_AttackProp::HT_Skill:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_SKILL_DAMAGE" );
						std::wstring wsSkill = pFTSkillInfo->s_szName;
						if( wsSkill.empty() )
							wsSkill = L"Unknown Skill";
						DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
						DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
						DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 		
					// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
				case CsC_AttackProp::HT_Absorb:
					{
						std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_ATTACK_ABSORB_ALL" );
						std::wstring wsSkill;
						CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
						if( pFTSkill )
						{
							CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
							if( pSkillInfo )
								wsSkill = pSkillInfo->s_szName;
							if( wsSkill.empty() )
								wsSkill = L"Unknown Buff";
						}
						DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
						DmCS::StringFn::Replace( wsMsg, L"#Hitter#", pHitter->GetName() );
						_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
					}
					break; 
				}		
			}
			break;
		case CsC_AttackProp::DT_Miss:	
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_MISS" );
				DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break; 			
		case CsC_AttackProp::DT_Dead:			
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_KILL" );
				DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break; 	
		case CsC_AttackProp::DT_Damage_Dot:		
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_SKILL_DAMAGE" );
				std::wstring wsSkill;
				CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
				if( pFTSkill )
				{
					CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
					if( pSkillInfo )
						wsSkill = pSkillInfo->s_szName;
					if( wsSkill.empty() )
						wsSkill = L"Unknown Buff";
				}
				DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
				DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
		case CsC_AttackProp::DT_Recover:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_SKILL_RECOVERY_TARGET" );
				std::wstring wsSkill;
				CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
				if( pFTSkill )
				{
					CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
					if( pSkillInfo )
						wsSkill = pSkillInfo->s_szName;
					if( wsSkill.empty() )
						wsSkill = L"Unknown Buff";
				}
				DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
				DmCS::StringFn::Replace( wsMsg, L"#Recover#", nTotalDem );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
			// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
		case CsC_AttackProp::DT_Absorb:
			{
				std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_ATTACK_ABSORB_DAMAGE" );
				std::wstring wsSkill;
				CsBuff* pFTSkill = nsCsFileTable::g_pBuffMng->GetBuff( pHitType->s_nSkillIndex );
				if( pFTSkill )
				{
					CsBuff::sINFO* pSkillInfo = pFTSkill->GetInfo();
					if( pSkillInfo )
						wsSkill = pSkillInfo->s_szName;
					if( wsSkill.empty() )
						wsSkill = L"Unknown Buff";
				}
				DmCS::StringFn::Replace( wsMsg, L"#Skill#", wsSkill.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Damage#", nTotalDem );
				_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
			}
			break;
		}	

		pBattleInfo->s_Color = NiColor( 1.0f, 1.0f, 0.0f );
		m_pBattleLogList.push_back( pBattleInfo );
	}
}	

void ChatBattleLogWindow::SetLog( CsC_AvObject* pHitter, CsC_AvObject* pTarget, float nRemainTime )
{
	if( !IsShowWindow() )
		return;

	sBattleInfo* pBattleInfo = NiNew sBattleInfo;
	pBattleInfo->s_fRemainTime = nRemainTime + 0.1f;	

	//hitter가 나일때
	if( pHitter && pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER )
	{
		std::wstring wsMsg = UISTRING_TEXT( "CHAT_BATTLELOG_KILL" );
		DmCS::StringFn::Replace( wsMsg, L"#Target#", pTarget->GetName() );
		_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, wsMsg.c_str() );
		pBattleInfo->s_Color = NiColor( 1.0f, 1.0f, 0.0f );
		m_pBattleLogList.push_back( pBattleInfo );
	}
	else if( pTarget && pTarget->GetLeafRTTI() == RTTI_DIGIMON_USER ) //내가 Target일때
	{
		_stprintf_s( pBattleInfo->s_szBattleLog, BATTLELOG_LENGTH, UISTRING_TEXT( "CHAT_BATTLELOG_PARTNER_FELL_DOWN" ).c_str() );	
		pBattleInfo->s_Color = NiColor( 1.0f, 128.0f / 255.0f, 0.0f );
		m_pBattleLogList.push_back( pBattleInfo );
	}
}

void ChatBattleLogWindow::_UpdateText( TCHAR* szText, NiColor vColor )
{
	cText::sTEXTINFO ti;		
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, vColor );					
	ti.s_bOutLine = true;
	ti.SetText( szText );	

	// 문자열을 길이이에 맞게 자름.
	int nSize = m_String.GetSize();
	g_pStringAnalysis->Cut_Parcing( &m_String, 348, szText, &ti );

	ControlUpdate(m_String.GetSize() - nSize);
}

void ChatBattleLogWindow::_AddSixLog( ContentsStream const& kStream )
{
	kStream.SetRp(0);	// 혹시 모를 일을 대비해 처음부터 읽는다.

	sRECV_HITTER_INFO HitterInfo;
	sRECV_TARGET_INFO TargetInfo;
	float	nRemainTime	= 0.0f;
	int		nTotalDem	= 0;
	bool	bLastAttack = false;

	kStream >> HitterInfo >> TargetInfo >> nRemainTime >> nTotalDem >> bLastAttack;

	if(g_pMngCollector == NULL) return;
	CsC_AvObject* pHitter = g_pMngCollector->GetObject( HitterInfo.s_nUID );
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( TargetInfo.s_nUID );

	SetLog( pHitter, pTarget, nRemainTime, nTotalDem, &HitterInfo, &TargetInfo);

	if(bLastAttack)
		SetLog( pHitter, pTarget, nRemainTime);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ChatBattleLogWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( NS_CHAT::BATTLE_TEXT, this );

	return true;
}

void ChatBattleLogWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case NS_CHAT::BATTLE_TEXT:	_AddSixLog(kStream);		break;
	}
}