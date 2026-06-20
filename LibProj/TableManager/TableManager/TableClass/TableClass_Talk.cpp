#include "stdafx.h"
#include "TableClass_Talk.h"

BEGIN_NAME_TABLE

// CTalk::CTalk()
// {
// }
// 
// CTalk::~CTalk()
// {
// }
// 
// void CTalk::InitTable(int const& nServiceType)
// {
// 	AddLoadTableEvent(L"Dm_LoadingTip.bin", this, &CTalk::Load_LoadingTips);
// 	AddLoadTableEvent(L"Dm_DigimonTip.bin", this, &CTalk::Load_DigimonTips);
// 	AddLoadTableEvent(L"Dm_DigimonTalk.bin", this, &CTalk::Load_DigimonTalks);
// 	AddLoadTableEvent(L"Dm_Message.bin", this, &CTalk::Load_Message);
// 	AddLoadTableEvent(L"Dm_EventTalk.bin", this, &CTalk::Load_EventTalks);
// 	AddLoadTableEvent(L"Dm_ToolTip.bin", this, &CTalk::Load_ToolTips);
// }
// 
// void CTalk::ReleaseTable()
// {
// 	m_kLoadingTips.clear();
// 	m_kDigimonTips.clear();
// 	m_kDigimonTalks.clear();
// 	m_kMessageTalks.clear();
// 	m_kEventTalks.clear();
// 	m_kTooltips.clear();
// }
// 
// bool CTalk::Load_LoadingTips(DataStream & pReader)
// {
// 	bool bSuccess = pReader.MoveFirst();
// 	while(bSuccess)
// 	{
// 		std::wstring wsTips;
// 		pReader.pop( wsTips );
// 		m_kLoadingTips.push_back( wsTips );
// 		bSuccess = pReader.MoveNext();
// 	}
// 
// 	return true;
// }
// 
// bool CTalk::Load_DigimonTips(DataStream & pReader)
// {
// 	bool bSuccess = pReader.MoveFirst();
// 	while(bSuccess)
// 	{
// 		std::wstring wsTips;
// 		pReader.pop( wsTips );
// 		m_kDigimonTips.push_back( wsTips );
// 		bSuccess = pReader.MoveNext();
// 	}
// 	return true;
// }
// 
// bool CTalk::Load_DigimonTalks(DataStream & pReader)
// {
// 	bool bSuccess = pReader.MoveFirst();
// 	while(bSuccess)
// 	{
// 		DWORD dwIdx = 0;
// 		pReader.pop( dwIdx );
// 		if( 0 == dwIdx )
// 			break;
// 
// 		sDigimonTalk info;
// 		pReader.pop( info.s_nType );
// 		pReader.pop( info.s_szText );
// 		pReader.pop( info.s_szList );
// 
// 		int nParam = 0;
// 		pReader.pop( nParam );
// 		if( nParam == 1 )
// 			info.s_dwParam |= sDigimonTalk::TP_QUEST;
// 
// 		nParam = 0;
// 		pReader.pop( nParam );
// 		if( nParam == 1 )
// 			info.s_dwParam |= sDigimonTalk::TP_NPC;
// 
// 		nParam = 0;
// 		pReader.pop( nParam );
// 		if( nParam == 1 )
// 			info.s_dwParam |= sDigimonTalk::TP_DIGIMON;
// 
// 		nParam = 0;
// 		pReader.pop( nParam );
// 		if( nParam == 1 )
// 			info.s_dwParam |= sDigimonTalk::TP_SKILL;
// 
// 		m_kDigimonTalks.insert( std::make_pair( dwIdx, info ) );
// 		bSuccess = pReader.MoveNext();
// 	}
// 	return true;
// }
// 
// bool CTalk::Load_Message(DataStream & pReader)
// {
// 	bool bSuccess = pReader.MoveFirst();
// 	while(bSuccess)
// 	{
// 		DWORD dwMessageIdx = 0;
// 		pReader.pop( dwMessageIdx );
// 		if( 0 == dwMessageIdx )
// 			break;
// 
// 		Tb::sMessageTalk Infos;
// 		pReader.pop( Infos.s_MsgType );
// 		pReader.pop( Infos.s_Type );
// 		pReader.pop( Infos.s_TitleName );
// 		pReader.pop( Infos.s_Message );
// 		pReader.pop( Infos.s_dwLinkID );
// 
// 		m_kMessageTalks.insert( std::make_pair( dwMessageIdx, Infos ) );
// 		bSuccess = pReader.MoveNext();
// 	}
// 	return true;
// }
// 
// bool CTalk::Load_EventTalks(DataStream & pReader)
// {
// 	bool bSuccess = pReader.MoveFirst();
// 	while(bSuccess)
// 	{
// 		DWORD dwIdx = 0;
// 		pReader.pop( dwIdx );
// 		if( 0 == dwIdx )
// 			break;
// 
// 		std::wstring wsMessage;
// 		pReader.pop( wsMessage );
// 		m_kEventTalks.insert( std::make_pair( dwIdx, wsMessage ) );
// 		bSuccess = pReader.MoveNext();
// 	}
// 	return true;
// }
// 
// bool CTalk::Load_ToolTips(DataStream & pReader)
// {
// 	bool bSuccess = pReader.MoveFirst();
// 	while(bSuccess)
// 	{
// 		DWORD dwIdx = 0;
// 		pReader.pop( dwIdx );
// 		if( 0 == dwIdx )
// 			break;
// 
// 		sTooltips info;
// 		pReader.pop( info.s_TitleName );
// 		pReader.pop( info.s_Message );
// 		m_kTooltips.insert( std::make_pair( dwIdx, info ) );
// 		bSuccess = pReader.MoveNext();
// 	}
// 	return true;
// }
END_NAME_TABLE