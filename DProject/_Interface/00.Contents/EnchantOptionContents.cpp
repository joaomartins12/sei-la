#include "stdafx.h"
#include "EnchantOptionContents.h"
#include "..\..\..\LibProj\CsFunc\CrashLogger.h"

#if !COMMON_LIB_FIXED
struct GS2C_RECV_ENCHANT_ITEM_IDENTIFY
{
	u2 nInvenPos;
	u1 nRate;
	u1 nLevel;
	u2 nAccOption[ nLimit::MAX_ACCESSORY_OPTION ];
	u2 nAccValues[ nLimit::MAX_ACCESSORY_OPTION ];
};

struct GS2C_RECV_ENCHANT_ITEM_ENCHANT
{
	u1 nResult;
	u2 nInvenPos;
	u1 nRate;
	u1 nLevel;
	u2 nAccOption[ nLimit::MAX_ACCESSORY_OPTION ];
	u2 nAccValues[ nLimit::MAX_ACCESSORY_OPTION ];
};
#endif

cEnchantOptionContents::cEnchantOptionContents() :
m_eUIState( eEnchantUI::eEnchantUI_None ),
m_eEnchantState( eEnchantState::eES_None )
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ENCHANT_OPTION_IDENTIFY_RESULT, this, &cEnchantOptionContents::RecvIdentifyItem );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ENCHANT_OPTION_ENCHANT_RESULT, this, &cEnchantOptionContents::RecvEnchantItem );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::START_ENCHANT_OPTION_ENCHANT, this, &cEnchantOptionContents::StartEnchantItem );

	if( EventRouter() )
	{
		EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Inventory_UseItem, this );
	}
}

cEnchantOptionContents::~cEnchantOptionContents()
{
	GAME_EVENT_ST.DeleteEventAll( this );
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
}

int const cEnchantOptionContents::IsContentsIdentity(void)
{
	return E_CT_ENCHANT_OPTION_CONTENTS;
}

int const cEnchantOptionContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool cEnchantOptionContents::Initialize(void)
{
	return true;
}

void cEnchantOptionContents::UnInitialize(void)
{

}

bool cEnchantOptionContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void cEnchantOptionContents::Update(float const& fElapsedTime)
{

}

void cEnchantOptionContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void cEnchantOptionContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Inventory_UseItem:
		{
			int nInvenIndex = 0;
			kStream >> nInvenIndex;
			RegistEnchantItem( nInvenIndex );
		}
		break;
	}
}

void cEnchantOptionContents::MakeMainActorData(void)
{

}

void cEnchantOptionContents::ClearMainActorData(void)
{
	CloseEnchant();
}

void cEnchantOptionContents::MakeWorldData(void)
{

}

void cEnchantOptionContents::ClearWorldData(void)
{
	CloseEnchant();
}

void cEnchantOptionContents::RegistEnchantItem(int const& nInvenIndex)
{
	cItemInfo* pInvenItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RET( pInvenItem );

	eEnchantType eAccType = _CheckEnchantItemType( pInvenItem->GetType() );
	switch( eAccType )
	{
	case eEnchantType::eAT_Accessory:
		_SetEnchantItem( pInvenItem, nInvenIndex );
		break;
	case eEnchantType::eAT_Enchant:
		_SetEnchantStone( pInvenItem, nInvenIndex );
		break;
	case eEnchantType::eAT_None:
		{
			if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCHANT_OPTION ) )
				cPrintMsg::PrintMsg( 11014 ); //해당 슬롯에 등록할 수 없습니다
		}
		break;
	}
}

void cEnchantOptionContents::RecvIdentifyItem(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_ENCHANT_ITEM_IDENTIFY* pRecv = static_cast< GS2C_RECV_ENCHANT_ITEM_IDENTIFY* >( pData );

	m_eEnchantState = eEnchantState::eES_None;
	cItemInfo* pInvenItem = GetInvenItem( pRecv->nInvenPos );
	SAFE_POINTER_RET( pInvenItem );

	// 감정 결과 저장
	pInvenItem->m_nRate = pRecv->nRate;
	pInvenItem->m_nLevel = pRecv->nLevel;
	for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION; ++i )
	{
		pInvenItem->m_nAccOption[ i ] = pRecv->nAccOption[ i ];
		pInvenItem->m_nAccValues[ i ] = pRecv->nAccValues[ i ];
	}

	m_sIdentify.ResetData();

	cPrintMsg::PrintMsg( 30601 ); // 디지털 월드의 신비로운 힘이 악세서리에 적용되었습니다.

	CloseEnchant();
}

void cEnchantOptionContents::RecvEnchantItem(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_ENCHANT_ITEM_ENCHANT* pRecv = static_cast< GS2C_RECV_ENCHANT_ITEM_ENCHANT* >( pData );

	m_eEnchantState = eEnchantState::eES_None;
	cItemInfo* pInvenItem = GetInvenItem( pRecv->nInvenPos );
	// ui 끄기
	if( !pInvenItem )
	{
		CloseEnchant();
		return;
	}

	switch( m_sEnchant.nStoneType )
	{
	case nItem::DigiPower://디지터리 파워 증가/감소
		{
			if( !m_sEnchant.IsRegistEnchantItem() )
				break;

			int nOldRate = m_sEnchant.cEnchantItem.m_nRate;
			int nCurRate = pRecv->nRate;

			cPrintMsg::PrintMsg( 30603, &nOldRate, &nCurRate ); // 디지터리 파워가 [int]%에서 [int]%로 변경되었습니다.
		}
		break;
	case nItem::Renewal://디지어블 파워 리뉴얼 횟수 증가/감소
		{
			if( !m_sEnchant.IsRegistEnchantItem() )
				break;

			int nOldLv = m_sEnchant.cEnchantItem.m_nLevel;
			int nCurLv = pRecv->nLevel;

			cPrintMsg::PrintMsg( 30612, &nOldLv, &nCurLv ); // 리뉴얼 변경 횟수가 [int]회에서 [int]회로 변경되었습니다.
		}
		break;
	case nItem::AccOption://악세서리 옵션 랜덤 변경
		{
			if( !m_sEnchant.IsRegistEnchantItem() )
				break;

			//원래 속성
			std::wstring wsOldOp;
			for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION; ++i )
			{
				std::wstring wsOption = GetOptionName( m_sEnchant.cEnchantItem.m_nAccOption[ i ] );
				wsOldOp.append( wsOption.c_str() );
				if( ( i + 1 < nLimit::MAX_ACCESSORY_OPTION ) && 
					( m_sEnchant.cEnchantItem.m_nAccOption[ i + 1 ] != 0 ) )
					wsOldOp.append( L", " );
				else
					break;
			}

			//바뀐 속성
			std::wstring wsCurOp;
			for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION ; ++i )
			{
				std::wstring wsOption = GetOptionName( pRecv->nAccOption[ i ] );
				wsCurOp.append( wsOption.c_str() );
				if( ( i + 1 < nLimit::MAX_ACCESSORY_OPTION ) && 
					( pRecv->nAccOption[ i + 1 ] != 0 ) )
					wsCurOp.append( L", " );
				else
					break;
			}
			cPrintMsg::PrintMsg( 30606, const_cast<TCHAR*>(wsOldOp.c_str()), const_cast<TCHAR*>(wsCurOp.c_str()) ); // 디지어블 파워의 능력치가 [str]에서 [str]로 변경되었습니다.
		}
		break;
	case nItem::OptionValue://해당 악세서리 옵션 값 증가/감소
		{
			std::wstring wsOption = GetOptionName( pRecv->nAccOption[ m_sEnchant.nSelectOption ] );

			// 목걸이 추가 시 CD, AS 스텟 추가됨.
			if( pRecv->nAccOption[ m_sEnchant.nSelectOption ] == nItem::CD ||
				pRecv->nAccOption[ m_sEnchant.nSelectOption ] == nItem::AS
#if COMMON_LIB_FIXED
				|| ( pRecv->nAccOption[ m_sEnchant.nSelectOption ] >= nItem::AP_ATTRIBUTE_DA &&
				pRecv->nAccOption[ m_sEnchant.nSelectOption ] <= nItem::AP_ATTRIBUTE_ST )
#endif
				)
			{
				float fOldVal = m_sEnchant.cEnchantItem.m_nAccValues[ m_sEnchant.nSelectOption ] / 100.0f;
				float fCurVal = pRecv->nAccValues[ m_sEnchant.nSelectOption ] / 100.0f;

				std::wstring wsOldVal;
				DmCS::StringFn::Format( wsOldVal, L"%5.2f%%", fOldVal );
				std::wstring wsCurVal;
				DmCS::StringFn::Format( wsCurVal, L"%5.2f%%", fCurVal );

				//디지어블 [str]능력 수치가 [str]에서 [str]로 변경되었습니다.
				cPrintMsg::PrintMsg( 30615, const_cast<TCHAR*>(wsOption.c_str()), 
					const_cast<TCHAR*>(wsOldVal.c_str()), const_cast<TCHAR*>(wsCurVal.c_str()) );
			}
			else
			{
				int nOldVal = m_sEnchant.cEnchantItem.m_nAccValues[ m_sEnchant.nSelectOption ];
				int nCurVal = pRecv->nAccValues[ m_sEnchant.nSelectOption ];

				//디지어블 [str]능력 수치가 [int]에서 [int]로 변경되었습니다.
				cPrintMsg::PrintMsg( 30609, const_cast<TCHAR*>(wsOption.c_str()), &nOldVal, &nCurVal );
			}
		}
		break;
	}

	// 강화 결과로 데이터 갱신 - 인벤 & 등록 아이템
	pInvenItem->m_nRate = pRecv->nRate;
	pInvenItem->m_nLevel = pRecv->nLevel;
	m_sEnchant.cEnchantItem.m_nRate = pRecv->nRate;
	m_sEnchant.cEnchantItem.m_nLevel = pRecv->nLevel;
	for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION; ++i )
	{
		pInvenItem->m_nAccOption[ i ] = pRecv->nAccOption[ i ];
		pInvenItem->m_nAccValues[ i ] = pRecv->nAccValues[ i ];
		m_sEnchant.cEnchantItem.m_nAccOption[ i ] = pRecv->nAccOption[ i ];
		m_sEnchant.cEnchantItem.m_nAccValues[ i ] = pRecv->nAccValues[ i ];
	}

	// 강화 아이템 1 감소
	cItemInfo* pInvenStone = GetInvenItem( m_sEnchant.nStoneIndex );
	if( pInvenStone )
		pInvenStone->DecreaseCount( 1 );
	// 강화 아이템 지우고
	m_sEnchant.ResetEnchantStone();

	Notify( eSuccessEnchant );
}

void cEnchantOptionContents::StartEnchantItem(void* pData)
{
	m_eEnchantState = eEnchantState::eES_Processing;
	Notify( eStartEnchant );
}

cEnchantOptionContents::eEnchantType cEnchantOptionContents::_CheckEnchantItemType(uint nItemType)
{
	CsItem::sINFO* pFTInfo = GetFTItemInfo( nItemType );
	SAFE_POINTER_RETVAL( pFTInfo, eAT_None );

	switch( pFTInfo->s_nType_L )
	{
	case nItem::Ring:
	case nItem::Necklace:
	case nItem::Earring:
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
	case nItem::Digivice:
#endif
		return eAT_Accessory;

	case nItem::AccStone:
		return eAT_Enchant;
	}

	return eAT_None;
}

void cEnchantOptionContents::_SetEnchantItem(cItemInfo* pInvenItem, uint nInvenIndex)
{
	SAFE_POINTER_RET( pInvenItem );

	// 패킷 대기 중 또는 프로세싱
	if( eEnchantState::eES_None != m_eEnchantState )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	switch( m_eUIState )
	{
	// 감정 중
	case eEnchantUI::eEnchantUI_Identify:
		cPrintMsg::PrintMsg( 10004 );
		return;

	// 아무 창도 안 열려 있으면
	case eEnchantUI::eEnchantUI_None:
		{
			if( g_pGameIF && g_pGameIF->IsOpenNpcWindow() )
				return;

			// 이미 감정한 아이템이면
			if( 0 != pInvenItem->m_nRate )
				return;

			CsItem::sINFO* pFTItem = GetFTItemInfo( pInvenItem->GetType() );
			SAFE_POINTER_RET( pFTItem );
			if( nsCsItemTable::OPTIONCODE != pFTItem->s_nSkillCodeType )
				return;

			// 감정 아이템 정보 저장
			m_sIdentify.SetAccessory( pInvenItem, nInvenIndex );
			m_eUIState = eEnchantUI::eEnchantUI_Identify;
			m_eEnchantState = eEnchantState::eES_Processing;

			if( g_pGameIF )
				g_pGameIF->GetDynamicIF( cBaseWindow::WT_ENCHANT_OPTION );

			Notify( eIdentifyItem );
		}
		break;

	case eEnchantUI::eEnchantUI_Enchant:
		_RegistEnchantItem( pInvenItem, nInvenIndex );
		break;
	}

	if( g_pNpcMng )
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_ENCHANT_OPTION );
}

void cEnchantOptionContents::_SetEnchantStone(cItemInfo* pInvenItem, uint nInvenIndex)
{
	SAFE_POINTER_RET( pInvenItem );

	// 패킷 대기 중 또는 프로세싱
	if( eEnchantState::eES_None != m_eEnchantState )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	switch( m_eUIState )
	{
	// 감정 중
	case eEnchantUI::eEnchantUI_Identify:
		cPrintMsg::PrintMsg( 10004 );
		return;

	// 아무 창도 안 열려 있으면
	case eEnchantUI::eEnchantUI_None:
		{
			SAFE_POINTER_RET( g_pGameIF );
			if( g_pGameIF->IsOpenNpcWindow() )
				return;

			m_eUIState = eEnchantUI::eEnchantUI_Enchant;
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_ENCHANT_OPTION );
		}	// 창만 추가로 열어주면 되기 때문에 break 안 한 것임
	case eEnchantUI::eEnchantUI_Enchant:
		_RegistEnchantStone( pInvenItem, nInvenIndex );
		break;
	}

	if( g_pNpcMng )
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_ENCHANT_OPTION );
}

void cEnchantOptionContents::_RegistEnchantItem(cItemInfo* pInvenItem, uint nInvenIndex)
{
	SAFE_POINTER_RET( pInvenItem );

	CsItem::sINFO* pFTInfo = GetFTItemInfo( pInvenItem->GetType() );
	SAFE_POINTER_RET( pFTInfo );

	if( nsCsItemTable::OPTIONCODE != pFTInfo->s_nSkillCodeType )
	{
		cPrintMsg::PrintMsg( 11014 ); //해당 슬롯에 등록할 수 없습니다
		return;
	}

	// 감정이 안 된 아이템
	if( 0 == pInvenItem->m_nRate )
	{
		cPrintMsg::PrintMsg( 11014 ); //해당 슬롯에 등록할 수 없습니다
		return;
	}

	DWORD dwTotalItemType = (pFTInfo->s_nType_L * 100) + pFTInfo->s_nType_S;

	switch( dwTotalItemType )
	{
	case nItem::Tutorialnecklace:
	case nItem::TutorialRing:
	case nItem::TutorialEaring:
		cPrintMsg::PrintMsg( 31003 );//해당 슬롯에 등록할 수 없습니다	
		return;
	}

	// 이미 등록되어 있는 악세서리가 존재하면
	if( m_sEnchant.IsRegistEnchantItem() )
		m_sEnchant.ResetEnchantItem();

	m_sEnchant.SetEnchantItem( pInvenItem, nInvenIndex );

	ContentsStream kSend;
	kSend << m_sEnchant.cEnchantItem.m_nType;
	Notify( eRegistEnchantItem, kSend );
}

void cEnchantOptionContents::_RegistEnchantStone(cItemInfo* pInvenItem, uint nInvenIndex)
{
	SAFE_POINTER_RET( pInvenItem );

	CsItem::sINFO* pFTInfo = GetFTItemInfo( pInvenItem->GetType() );
	SAFE_POINTER_RET( pFTInfo );
	CsAccessory_Enchant* pEnchant = nsCsFileTable::g_pItemMng->GetAccessoryEnchant( pFTInfo->s_dwSkill );
	SAFE_POINTER_RET( pEnchant );
	CsAccessory_Enchant::sINFO* pEInfo = pEnchant->GetInfo();
	SAFE_POINTER_RET( pEInfo );

	// 이미 등록되어 있는 인첸 스톤이 존재하면
	if( m_sEnchant.IsRegistEnchantStone() )
		m_sEnchant.ResetEnchantStone();

	m_sEnchant.SetEnchantStone( pInvenItem, nInvenIndex, pEInfo->s_nOpt );

	ContentsStream kSend;
	kSend << m_sEnchant.cEnchantStone.m_nType << m_sEnchant.nStoneType;
	Notify( eRegistEnchantStone, kSend );
}

//////////////////////////////////////////////////////////////////////////
void cEnchantOptionContents::CloseEnchant()
{
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ENCHANT_OPTION );

	if( g_pNpcMng )
		g_pNpcMng->ResetZoomNpc();

	m_eUIState = eEnchantUI::eEnchantUI_None;

	if( eEnchantState::eES_WaitRecv == m_eEnchantState )
		return;

	m_eEnchantState = eEnchantState::eES_None;

	m_sEnchant.ResetEnchantStone();
	m_sEnchant.ResetEnchantItem();
	m_sIdentify.ResetData();
}

void cEnchantOptionContents::SuccessIdentifyItem()
{
	SAFE_POINTER_RET( net::game );

	if( m_eEnchantState == eEnchantState::eES_WaitRecv )
		return;

	if( !m_sIdentify.cIdentifyItem.IsEnable() )
		return;

	uint nUID = GetItemUserUID( m_sIdentify.cIdentifyItem.m_nType );
	if( 0 == nUID )
		return;

	net::game->SendAccessoryCheck( nUID, m_sIdentify.nInvenIndex );

	m_eEnchantState = eEnchantState::eES_WaitRecv;
}

void cEnchantOptionContents::SuccessEnchantItem()
{
	SAFE_POINTER_RET( net::game );

	if( m_eEnchantState == eEnchantState::eES_WaitRecv )
		return;

	if( !m_sEnchant.IsRegistEnchantStone() )
		return;

	if( !m_sEnchant.IsRegistEnchantItem() )
		return;

	uint nUID = 0;
	if( g_pCharMng )
	{
		CTamerUser* pUser = g_pCharMng->GetTamerUser();
		if( pUser )
			nUID = pUser->GetUniqID();
	}

	switch( m_sEnchant.nStoneType )
	{
	case nItem::DigiPower:
	case nItem::AccOption:
	case nItem::Renewal:
		{
			//Send - UIDX, 강화아이템 인벤슬롯, 악세서리 인벤슬롯
			net::game->SendAccessoryEnchant( nUID, m_sEnchant.nStoneIndex, m_sEnchant.nItemIndex );
		}
		break;
	case nItem::OptionValue:
		{
			//Send - UIDX, 강화아이템 인벤슬롯, 악세서리 인벤슬롯, 선택 옵션 (몇번째인지)
			net::game->SendAccessoryEnchant( nUID, m_sEnchant.nStoneIndex, m_sEnchant.nItemIndex, m_sEnchant.nSelectOption );
		}
		break;
	default:// 잘못된 타입임
		return;
	}

	m_eEnchantState = eEnchantState::eES_WaitRecv;
}

void cEnchantOptionContents::SelectOptionValue(uint nSelectIndex)
{
	if( !m_sEnchant.IsRegistEnchantStone() )
		return;

	if( m_sEnchant.nStoneType != nItem::OptionValue )
		return;

	m_sEnchant.nSelectOption = nSelectIndex;
}

bool cEnchantOptionContents::ResetEnchantStone()
{
	if( eEnchantState::eES_None != m_eEnchantState )
		return false;

	m_sEnchant.ResetEnchantStone();
	return true;
}

bool cEnchantOptionContents::ResetEnchantItem()
{
	if( eEnchantState::eES_None != m_eEnchantState )
		return false;

	m_sEnchant.ResetEnchantItem();
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool cEnchantOptionContents::IsEnableRegist(uint nInvenIndex, uint nItemCount) const
{
	cItemInfo* pInvenItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RETVAL( pInvenItem, false );

	if( pInvenItem->GetCount() != nItemCount )
	{
		cPrintMsg::PrintMsg( 20030 ); // 분리한 아이템은 바로 회수할 수 없습니다. 인벤토리에 옮긴 후 회수하시기 바랍니다
		return false;
	}

	return true;
}

bool cEnchantOptionContents::IsProcessingEnchant() const
{
	if( eEnchantState::eES_Processing == m_eEnchantState )
		return true;

	return false;
}

cEnchantOptionContents::eEnchantUI cEnchantOptionContents::GetUIState() const
{
	return m_eUIState;
}

std::wstring cEnchantOptionContents::GetEnchantItemTypeName() const
{
	std::wstring wsTypeName = UISTRING_TEXT( "COMMON_TXT_ACCESSORY" );
	if( !m_sEnchant.IsRegistEnchantItem() )
		return wsTypeName;

	CsItem::sINFO* pFTItem = GetFTItemInfo( m_sEnchant.cEnchantItem.m_nType );
	SAFE_POINTER_RETVAL( pFTItem, wsTypeName );

	switch( pFTItem->s_nType_L )
	{
	case nItem::Digivice:		// 디지바이스 - 나중에 이펙트 만들어주면 추가
		wsTypeName = UISTRING_TEXT( "COMMON_TXT_DIGIVICE" );
		break;
	case nItem::Ring:			// 반지
		wsTypeName = UISTRING_TEXT( "COMMON_TXT_RING" );
		break;
	case nItem::Necklace:		// 목걸이
		wsTypeName = UISTRING_TEXT( "COMMON_TXT_NECKLACE" );
		break;
	case nItem::Earring:		// 귀걸이
		wsTypeName = UISTRING_TEXT( "COMMON_TXT_EARRING" );
		break;
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
		wsTypeName = UISTRING_TEXT( "COMMON_TXT_BRACELET" );
		break;
#endif
	}

	return wsTypeName;
}

uint cEnchantOptionContents::GetEnchantStoneEnchantType() const
{
	if( !m_sEnchant.IsRegistEnchantStone() )
		return 0;

	return m_sEnchant.nStoneType;
}

uint cEnchantOptionContents::GetEnchantStoneItemType() const
{
	if( !m_sEnchant.IsRegistEnchantStone() )
		return 0;

	return m_sEnchant.cEnchantStone.m_nType;
}

uint cEnchantOptionContents::GetEnchantItemType() const
{
	if( !m_sEnchant.IsRegistEnchantItem() )
		return 0;

	return m_sEnchant.cEnchantItem.m_nType;
}

uint cEnchantOptionContents::GetItemUserUID(uint nItemType) const
{
	CsItem::sINFO* pFTInfo = GetFTItemInfo( nItemType );
	SAFE_POINTER_RETVAL( pFTInfo, 0 );

	CsC_AvObject* pUser = NULL;

	switch( pFTInfo->s_nUseCharacter )
	{
	case 0:		// 해당 사항 없음
	case 1:		// 둘다 사용
		pUser = g_pCharMng->GetTamerUser();
		break;
	case 2:		// 디지몬 사용 가능
		pUser = g_pCharMng->GetDigimonUser( 0 );
		break;
	case 3:		// 테이머 사용 가능		
		pUser = g_pCharMng->GetTamerUser();
		break;
	}

	SAFE_POINTER_RETVAL( pUser, 0 );
	return pUser->GetUniqID();
}

const cItemInfo* cEnchantOptionContents::GetEnchantStone() const
{
	if( !m_sEnchant.IsRegistEnchantStone() )
		return NULL;

	return &m_sEnchant.cEnchantStone;
}

const cItemInfo* cEnchantOptionContents::GetEnchantItem() const
{
	if( !m_sEnchant.IsRegistEnchantItem() )
		return NULL;

	return &m_sEnchant.cEnchantItem;
}

cItemInfo* cEnchantOptionContents::GetInvenItem(uint nInvenIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, NULL );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, NULL );
	cItemInfo* pInvenItem = pInven->GetData( nInvenIndex );
	SAFE_POINTER_RETVAL( pInvenItem, NULL );
	if( !pInvenItem->IsEnable() )
		return NULL;

	return pInvenItem;
}

CsItem::sINFO* cEnchantOptionContents::GetFTItemInfo(uint nItemType) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, NULL );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemType );
	SAFE_POINTER_RETVAL( pFTItem, NULL );
	return pFTItem->GetInfo();
}

CsC_AvObject* cEnchantOptionContents::GetEnchantItemEffect() const
{
	if( !m_sEnchant.IsRegistEnchantItem() )
		return NULL;

	CsItem::sINFO* pFTInfo = GetFTItemInfo( m_sEnchant.cEnchantItem.m_nType );
	SAFE_POINTER_RETVAL( pFTInfo, NULL );

	SAFE_POINTER_RETVAL( g_pCharMng, NULL );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, NULL );
	CsC_EffectProp* pEffectProp = pUser->GetProp_Effect();
	SAFE_POINTER_RETVAL( pEffectProp, NULL );

	//강화 이펙트 테이머에 붙이고( 안보이게 땅속에 처박음; )
	switch( pFTInfo->s_nType_L )
	{
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
	case nItem::Digivice:		// 디지바이스
		pEffectProp->AddEffect( "system\\Accessory\\Digivice.nif", pUser->GetPos() + NiPoint3(0.0f, 0.0f, -800.0f), 1.0f );
		break;
#endif
	case nItem::Ring:			// 반지
		pEffectProp->AddEffect( "system\\Accessory\\Ring.nif", pUser->GetPos() + NiPoint3(0.0f, 0.0f, -800.0f), 1.0f );
		break;
	case nItem::Necklace:		// 목걸이
		pEffectProp->AddEffect( "system\\Accessory\\Necklace.nif", pUser->GetPos() + NiPoint3(0.0f, 0.0f, -800.0f), 1.0f );
		break;
	case nItem::Earring:	// 귀걸이
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
		pEffectProp->AddEffect( "system\\Accessory\\Earring.nif", pUser->GetPos() + NiPoint3(0.0f, 0.0f, -800.0f), 1.0f );
		break;
	}

	int nEffectIndex = pEffectProp->GetEffectCount() - 1;
	if( -1 == nEffectIndex )
		return NULL;

	return pEffectProp->GetEffect( nEffectIndex );
}

std::wstring cEnchantOptionContents::GetOptionName(ushort nOption) const
{
	switch( nOption )
	{
	case nItem::AP:			return UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" );
	case nItem::DP:			return UISTRING_TEXT( "COMMON_TXT_STAT_DEFENSE" );
	case nItem::MAXHP:		return UISTRING_TEXT( "COMMON_TXT_MAX_HP" );
	case nItem::MAXDS:		return UISTRING_TEXT( "COMMON_TXT_DS" );
	case nItem::SkillAP:	return UISTRING_TEXT( "COMMON_TXT_SKILLAP" );
	case nItem::AttrAP:		return UISTRING_TEXT( "COMMON_TXT_ATTRIBUTEAP" );
	case nItem::CA:			return UISTRING_TEXT( "COMMON_TXT_CRITICAL" );
	case nItem::CD:			return UISTRING_TEXT( "COMMON_TXT_CRITICAL_DAMAGE" );
	case nItem::AS:			return UISTRING_TEXT( "COMMON_TXT_ATTACK_SPEED" );
	case nItem::EV:			return UISTRING_TEXT( "COMMON_TXT_AVOID" );
	case nItem::BL:			return UISTRING_TEXT( "COMMON_TXT_BLOCK" );
	case nItem::HT:			return UISTRING_TEXT( "COMMON_TXT_STAT_HIT_RATE" );

#if COMMON_LIB_FIXED
		// 속성 데미지 추가
	case nItem::AP_ATTRIBUTE_DA:	return UISTRING_TEXT( "COMMON_TXT_DATA_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_VA:	return UISTRING_TEXT( "COMMON_TXT_VACCINE_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_VI:	return UISTRING_TEXT( "COMMON_TXT_VIRUS_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_UN:	return UISTRING_TEXT( "COMMON_TXT_UNKNOWN_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_IC:	return UISTRING_TEXT( "COMMON_TXT_ICE_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_WA:	return UISTRING_TEXT( "COMMON_TXT_WATER_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_FI:	return UISTRING_TEXT( "COMMON_TXT_FIRE_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_EA:	return UISTRING_TEXT( "COMMON_TXT_EARTH_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_WI:	return UISTRING_TEXT( "COMMON_TXT_WIND_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_WO:	return UISTRING_TEXT( "COMMON_TXT_WOOD_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_LI:	return UISTRING_TEXT( "COMMON_TXT_LIGHT_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_DK:	return UISTRING_TEXT( "COMMON_TXT_DARKNESS_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_TH:	return UISTRING_TEXT( "COMMON_TXT_THUNDER_ATTRIBUTE_DAMAGE" );
	case nItem::AP_ATTRIBUTE_ST:	return UISTRING_TEXT( "COMMON_TXT_STEEL_ATTRIBUTE_DAMAGE" );
#endif
	}

	return UISTRING_TEXT( "COMMON_TXT_UNKNOWN_ATTRIBUTION" );
}

void cEnchantOptionContents::EnchantItem() 
{
	if( !m_sEnchant.IsRegistEnchantItem() )
		return;

	if( !m_sEnchant.IsRegistEnchantStone() )
		return;

	CsItem::sINFO* pFTStone = GetFTItemInfo( m_sEnchant.cEnchantStone.m_nType );
	SAFE_POINTER_RET( pFTStone );
	CsAccessory_Enchant* pEnchant = nsCsFileTable::g_pItemMng->GetAccessoryEnchant( pFTStone->s_dwSkill );
	SAFE_POINTER_RET( pEnchant );
	CsAccessory_Enchant::sINFO* pEnchantInfo = pEnchant->GetInfo();
	SAFE_POINTER_RET( pEnchantInfo );

	CsItem::sINFO* pFTAcc = GetFTItemInfo( m_sEnchant.cEnchantItem.m_nType );
	SAFE_POINTER_RET( pFTAcc );
	CsAccessory_Option* pOption = nsCsFileTable::g_pItemMng->GetAccessoryOption( pFTAcc->s_dwSkill );
	SAFE_POINTER_RET( pOption );
	CsAccessory_Option::sINFO* pOptionInfo = pOption->GetInfo();
	SAFE_POINTER_RET( pOptionInfo );

	switch( m_sEnchant.nStoneType )
	{
	case nItem::DigiPower://디지터리 파워 증가/감소
		{
			//현재 악세서리 파워 + 강화석 파워 > 200 ( 강화 시 최대치 넘어갈 경우 )
			if( ( m_sEnchant.cEnchantItem.m_nRate + pEnchantInfo->s_nVal ) > 200 ) 
			{
				//파워 최대치라서 강화 안됨
				cPrintMsg::PrintMsg( 30604 ); // 디지터리 파워가 최대치에 도달하여 더 이상 증가시킬 수 없습니다.
				return;
			}
			else
			{
				cPrintMsg::PrintMsg( 30602 ); // 디지털 스톤의 힘을 적용하여 디지터리 파워를 변경하시겠습니까?
				return;
			}
		}
		break;
	case nItem::Renewal://디지어블 파워 리뉴얼 횟수 증가/감소
		{
			// 리뉴얼 횟수 + 아이템의 추가 횟수 가 최대치를 넘어갈 경우
			if( ( m_sEnchant.cEnchantItem.m_nLevel + pEnchantInfo->s_nVal ) > pOptionInfo->s_nLimit 
				|| m_sEnchant.cEnchantItem.m_nLevel >= pOptionInfo->s_nLimit )
			{
				//최대 횟수만큼 가지고 있으니 더 증가 못시킴
				cPrintMsg::PrintMsg( 30613 ); // 리뉴얼 변경 횟수가 최대치에 도달하여 더 이상 증가시킬 수 없습니다.
				return;
			}
			else
			{
				cPrintMsg::PrintMsg( 30611 ); // 디지털 스톤의 힘을 적용하여 리뉴얼 횟수를 변경하시겠습니까?
				return;
			}
		}
		break;
	case nItem::AccOption://악세서리 옵션 랜덤 변경
		{
			if( m_sEnchant.cEnchantItem.m_nLevel <= 0 )
			{
				//강화횟수 다써서 더이상 강화 불가능
				cPrintMsg::PrintMsg( 30607 ); // 리뉴얼 변경 횟수가 부족하여 디지어블 능력치를 변경할 수 없습니다.
				return;
			}
			else
			{
				cPrintMsg::PrintMsg( 30605 ); // 디지털 스톤의 힘을 적용하여 디지어블 능력치를 변경하시겠습니까?
				return;
			}
		}
		break;
	case nItem::OptionValue://해당 악세서리 옵션 값 증가/감소
		{
			if( m_sEnchant.cEnchantItem.m_nLevel <= 0 )
			{
				//강화횟수 다써서 더이상 강화 불가능
				cPrintMsg::PrintMsg( 30610 ); // 리뉴얼 변경 횟수가 부족하여 디지어블 능력 수치를 변경할 수 없습니다.
				return;
			}
			else
			{
				cPrintMsg::PrintMsg( 30608 ); // 디지털 스톤의 힘을 적용하여 디지어블 능력 수치를 변경하시겠습니까?
				return;
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void cEnchantOptionContents::sIdentifyItem::ResetData()
{
	if( g_pDataMng )
		g_pDataMng->ItemUnlock( nInvenIndex );
	nInvenIndex = INT_MAX;
	cIdentifyItem.Clear();
}

void cEnchantOptionContents::sIdentifyItem::SetAccessory(cItemInfo* pItem, uint nIndex)
{
	SAFE_POINTER_RET( pItem );

	// 감정 아이템 정보 저장
	cIdentifyItem = *pItem;
	cIdentifyItem.SetCount( 1 );
	nInvenIndex = nIndex;

	// 아이템 락
	if( g_pDataMng )
		g_pDataMng->ItemLock( nInvenIndex );
}

//////////////////////////////////////////////////////////////////////////
bool cEnchantOptionContents::sEnchantItem::IsRegistEnchantItem() const
{
	if( !cEnchantItem.IsEnable() )
		return false;

	if( INT_MAX == nItemIndex )
		return false;

	return true;
}

bool cEnchantOptionContents::sEnchantItem::IsRegistEnchantStone() const
{
	if( !cEnchantStone.IsEnable() )
		return false;

	if( INT_MAX == nStoneIndex )
		return false;

	return true;
}

void cEnchantOptionContents::sEnchantItem::SetEnchantItem(cItemInfo* pItem, uint nIndex)
{
	SAFE_POINTER_RET( pItem );

	// 감정 아이템 정보 저장
	cEnchantItem = *pItem;
	cEnchantItem.SetCount( 1 );
	nItemIndex = nIndex;

	// 아이템 락
	if( g_pDataMng )
		g_pDataMng->ItemLock( nItemIndex );
}

void cEnchantOptionContents::sEnchantItem::SetEnchantStone(cItemInfo* pItem, uint nIndex, uint nType)
{
	SAFE_POINTER_RET( pItem );

	// 감정 아이템 정보 저장
	cEnchantStone = *pItem;
	cEnchantStone.SetCount( 1 );
	nStoneIndex = nIndex;
	nStoneType = nType;

	// 아이템 락
	if( g_pDataMng )
		g_pDataMng->ItemLock( nStoneIndex );
}

void cEnchantOptionContents::sEnchantItem::ResetEnchantItem()
{
	if( g_pDataMng )
		g_pDataMng->ItemUnlock( nItemIndex );
	nItemIndex = INT_MAX;
	cEnchantItem.Clear();
	nSelectOption = 0;
}

void cEnchantOptionContents::sEnchantItem::ResetEnchantStone()
{
	if( g_pDataMng )
		g_pDataMng->ItemUnlock( nStoneIndex );
	nStoneIndex = INT_MAX;
	cEnchantStone.Clear();
	nSelectOption = 0;
}
