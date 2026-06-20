#include "StdAfx.h"
#include "NeonDisplayContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

//----------------------------------------------------------------------------------------------------------------------------------
int const CNeonDisplayContents::IsContentsIdentity(void)
{
	return E_CT_NEON_DISPLAY_CONTENTS;
}

CNeonDisplayContents::CNeonDisplayContents(void)
{
}

CNeonDisplayContents::~CNeonDisplayContents(void)
{
	ClearAllRegistered();
}

int const CNeonDisplayContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CNeonDisplayContents::Initialize(void)
{
	return true;
}

void CNeonDisplayContents::UnInitialize(void)
{

}

bool CNeonDisplayContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void CNeonDisplayContents::Update(float const& fElapsedTime)
{
}

void CNeonDisplayContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CNeonDisplayContents::MakeMainActorData(void)
{

}

void CNeonDisplayContents::ClearMainActorData(void)
{
}

void CNeonDisplayContents::MakeWorldData(void)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_NEON_DISPLAY, this, &CNeonDisplayContents::NTF_NeonDisplayData );

// 	std::wstring msg = L"[ @<tc:990>네네넨네네넨넨@</tc> ] 님이 [ 우후후후 ] 을/를 사용하여 < 아군몬 >로 진화시켰습니다. 축하해주세요.";
// 	m_messageQue.push( msg );
// 	if( g_pGameIF )
// 	{
// 		g_pGameIF->GetDynamicIF( cBaseWindow::WT_PUBLICITY );
// 		Notify( eAddNeonDisplay );
// 	}
}

void CNeonDisplayContents::ClearWorldData(void)
{
	GAME_EVENT_ST.DeleteEventAll( this );
}


std::wstring CNeonDisplayContents::GetShowMessageAndPop()
{
	std::wstring msg;
	if( m_messageQue.empty() )
		return msg;

	msg = m_messageQue.front();
	m_messageQue.pop();
	return msg;
}

//////////////////////////////////////////////////////////////////////////
void CNeonDisplayContents::closeDisplayWindow()
{
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PUBLICITY );
}

std::wstring CNeonDisplayContents::GetItemName(DWORD const& dwItemCode)
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, L"" );
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemCode );
	SAFE_POINTER_RETVAL( pItem, L"" );
	SAFE_POINTER_RETVAL( pItem->GetInfo(), L"" );
	return pItem->GetInfo()->s_szName;
}

std::wstring CNeonDisplayContents::GetDigimonType( int const& nType ) const
{
	std::wstring strEvolType;
	if( nsCsFileTable::g_pBaseMng )
		strEvolType = nsCsFileTable::g_pBaseMng->GetEvoTypeName( nType );

	return strEvolType;
}

std::wstring CNeonDisplayContents::GetDigimonTableName( DWORD const& dwDigimonTBID ) const
{
	std::wstring wsName;
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, wsName );
	CsDigimon* pFindDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonTBID );
	SAFE_POINTER_RETVAL( pFindDigimon, wsName );
	CsDigimon::sINFO* pInfo = pFindDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, wsName );
	wsName = pInfo->s_szName;
	return wsName;
}
//////////////////////////////////////////////////////////////////////////
void CNeonDisplayContents::NTF_NeonDisplayData(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_NOTIFY_Neon_Display* pRecv = static_cast<GS2C_NOTIFY_Neon_Display*>(pData);

	std::wstring wsMsg;
	switch( pRecv->nNeonType )
	{
	case nNeonType::Neon_Evolution:	
		{
			wsMsg = UISTRING_TEXT( "NEON_DISPLAY_TYPE_DIGIMON_EVOLUTION" );	
			DmCS::StringFn::ReplaceAll( wsMsg, L"#TamerName#", pRecv->strTamerName );
			std::wstring wsItemName = GetItemName( pRecv->nType1 );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#ItemName#", wsItemName );
			std::wstring wsDigimonType = GetDigimonTableName( pRecv->nType2 );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#DigimonType#", wsDigimonType );
		}break;
	case nNeonType::Neon_Inchant:	
		{
			wsMsg = UISTRING_TEXT( "NEON_DISPLAY_TYPE_DIGIMON_INCHANT" );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#TamerName#", pRecv->strTamerName );
			std::wstring wsDigimonType = GetDigimonTableName( pRecv->nType1 );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#DigimonTableName#", wsDigimonType );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#InchantCount#", pRecv->nType2 );
		}break;
	case nNeonType::Neon_Scale:	
		{
			wsMsg = UISTRING_TEXT( "NEON_DISPLAY_TYPE_DIGIMON_SCALE" );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#TamerName#", pRecv->strTamerName );
			std::wstring wsDigimonType = GetDigimonTableName( pRecv->nType1 );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#DigimonType#", wsDigimonType );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#DigimonScale#", pRecv->nType2 / 100);
		}break;
	case nNeonType::Neon_Item:
		{
			wsMsg = UISTRING_TEXT( "NEON_DISPLAY_TYPE_GET_ITEM" );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#TamerName#", pRecv->strTamerName );
			std::wstring wsItemName1 = GetItemName( pRecv->nType1 );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#ItemName1#", wsItemName1 );
			std::wstring wsItemName2 = GetItemName( pRecv->nType2 );
			DmCS::StringFn::ReplaceAll( wsMsg, L"#ItemName2#", wsItemName2 );
		}break;
	default:
		return;
	}

	m_messageQue.push( wsMsg );

	SAFE_POINTER_RET( g_pGameIF );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_PUBLICITY ) )
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_PUBLICITY );
		Notify( eAddNeonDisplay );
	}
}