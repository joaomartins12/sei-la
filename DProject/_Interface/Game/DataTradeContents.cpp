#include "StdAfx.h"
#include "DataTradeContents.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

//////////////////////////////////////////////////////////////////////////
////////////////////	DataTradeContents	//////////////////////
//////////////////////////////////////////////////////////////////////////
int const DataTradeContents::IsContentsIdentity(void)
{
	return E_CT_DATA_TRADE_SYSTEM;
}

DataTradeContents::DataTradeContents(void) : m_eState(eTRADE_NONE), m_bEffectOn(false), m_bPickLeft(false), m_bPickRight(false)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::START_TRADE_DATA, this, &DataTradeContents::StartTradeData); 
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_TRADE_SELECT, this, &DataTradeContents::OpenSelect); 

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_DATA_SCALE, this, &DataTradeContents::RecvTradeScale); 
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_DATA_UPGRADE, this, &DataTradeContents::RecvTradeUpgrade); 
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_DATA_EVOLUTION, this, &DataTradeContents::RecvTradeEvolution);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_DATA_FAILURE, this, &DataTradeContents::RecvTradeFailure );
}

DataTradeContents::~DataTradeContents(void)
{
	GAME_EVENT_ST.DeleteEventAll( this );
}

int const DataTradeContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool DataTradeContents::Initialize(void)
{

	return true;
}

void DataTradeContents::UnInitialize(void)
{
}

bool DataTradeContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void DataTradeContents::Update(float const& fElapsedTime)
{
}

void DataTradeContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void DataTradeContents::MakeMainActorData(void)
{
}

void DataTradeContents::ClearMainActorData(void)
{
}

void DataTradeContents::OpenSelect(void*	pData)
{
	// 데이터 교환 이펙트 실행중이면 키 먹지 않음.
	if( IsTradeDataEffect() )
		return;

	CloseTrade();

	SetState(eTRADE_SELECT);

	// 예외처리 혹시 열려 있는 경우 닫고 다시 염.
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DATA_TRADE_SELECT ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DATA_TRADE_SELECT );

	cBaseWindow* pWin = g_pGameIF->GetDynamicIF( cBaseWindow::WT_DATA_TRADE_SELECT );
	if( pWin )
		pWin->SetShowWindow( true );

	Notify(eCenter_Reset);
}
void DataTradeContents::CloseSelect(bool	bFlow)
{
	CloseTrade();

	FLOWMGR_ST.PopFlow( Flow::CFlow::FLW_DATATRADECENTER );
	SetState(eTRADE_NONE);
}
void DataTradeContents::OpenTrade(int	state)
{
	eSTATE	opneState = GetIntToState(state);

	SetState(opneState);

	cBaseWindow* pWin = g_pGameIF->GetDynamicIF( cBaseWindow::WT_DATA_TRADE_SELECT );
	if( pWin )
		pWin->SetShowWindow( false );

	switch (opneState)
	{
	case eTRADE_SCALE:
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_DATA_TRADE_DIGIMON_SCALE );
		break;

	case eTRADE_UPGRADE:
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_DATA_TRADE_DIGIMON_UPGRADE );
		break;

	case eTRADE_EVOLUTION:
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_DATA_TRADE_DIGIMON_EVOLUTION );
		break;
	default:
		assert_cs( "OpenTrade state Error" );
		return;
	}	
	Notify(eCenter_Reset);	
}
void DataTradeContents::CloseTrade()
{
	switch (m_eState)
	{
	case eTRADE_SCALE:
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DATA_TRADE_DIGIMON_SCALE );
		break;

	case eTRADE_UPGRADE:
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DATA_TRADE_DIGIMON_UPGRADE );
		break;

	case eTRADE_EVOLUTION:
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DATA_TRADE_DIGIMON_EVOLUTION );
		break;
	case eTRADE_SELECT:
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DATA_TRADE_SELECT );
		break;
	default:
		assert_cs( "CloseTrade Error" );
		break;
	}	
}

void DataTradeContents::SendTradeData(void*	pData)
{
	SAFE_POINTER_RET(pData);
	ContentsStream kStream = *static_cast<ContentsStream*>(pData);

	kStream.SetRp(0);	// 혹시 모를 일을 대비해 처음부터 읽는다.

	int L_Slot= 1;		// 최소 슬롯 번호가 1이다. 서버 기준으로 맞춰준 설정.
	int R_Slot= 1;

	wstring	wStr;
	kStream>>L_Slot>>R_Slot>>wStr;

	if(L_Slot ==1 && R_Slot == 1)
		return;

	nDigimon::eDataChangeType	eDC_Type;

	switch(m_eState)
	{
	case eTRADE_SCALE:
		eDC_Type = nDigimon::eDataChangeType_Size;
		break;

	case eTRADE_UPGRADE:
		eDC_Type = nDigimon::eDataChangeType_Inchant;
		break;

	case eTRADE_EVOLUTION:
		eDC_Type = nDigimon::eDataChangeType_EvoSlot;
		break;
	}

	//MessageToNotify(MESSAGE_COMPLETE, (TCHAR*)wStr.c_str());

	if( net::game )
		net::game->SendDigimonDataChange(DATS_NPC_ID, eDC_Type, L_Slot, R_Slot);		

}
void DataTradeContents::RecvTradeScale(void*	pData)
{
	SAFE_POINTER_RET(pData);
	ContentsStream kStream = *static_cast<ContentsStream*>(pData);

	kStream.SetRp(0);	// 혹시 모를 일을 대비해 처음부터 읽는다.

	u1	nLDigimonArrIDX=0;
	u1	nRDigimonArrIDX=0;

	u2	nLNewScale=0;
	u2	nRNewScale=0;

	u1	nLHatchLevel=0;
	u1	nRHatchLevel=0;

	kStream>>nLDigimonArrIDX>>nRDigimonArrIDX>>nLNewScale>>nRNewScale>>nLHatchLevel>>nRHatchLevel;

	// 좌. 우 정보 생성.
	// 서버는 0번이 디지몬 유저// 1~3번이 택틱스 보관소 번호.
	// 클라는 0~2번이 택틱스 보관소이기 때문에 -1을 감소 시켜준다.
	cData_PostLoad::sDATA*	LeftData = g_pDataMng->GetTactics()->GetTactics(nLDigimonArrIDX-1);
	cData_PostLoad::sDATA*	RightData = g_pDataMng->GetTactics()->GetTactics(nRDigimonArrIDX-1);
	LeftData->s_fScale = (float)nLNewScale/10000.f;
	RightData->s_fScale = (float)nRNewScale/10000.f;

	LeftData->s_HatchLevel = nLHatchLevel;
	RightData->s_HatchLevel = nRHatchLevel;

	// 성공 메시지 - 계열체 이름 필요
	std::wstring wsTypeName;
	CsTacticsExplain* pExplain = nsCsFileTable::g_pTacticsMng->GetTacticsExplain( LeftData->s_dwBaseDigimonID );
	if( pExplain )
	{
		CsTacticsExplain::sINFO* pInfo = pExplain->GetInfo();
		if( pInfo )
			wsTypeName = pInfo->s_szTacticsName;
	}
	if( !wsTypeName.empty() )
		MessageToNotify( MESSAGE_COMPLETE, const_cast<TCHAR*>(wsTypeName.c_str()) );

	DecreaseItemRequire(eTRADE_SCALE);

	if( net::game )
		net::game->SendQueryMoneyInven();	// 인벤에 금액이 일치하는지. 인벤 정보가 일치 하는지 검사.

	Notify(eScale_Reset, kStream);
}
void DataTradeContents::RecvTradeUpgrade(void*	pData)
{
	SAFE_POINTER_RET(pData);
	ContentsStream kStream = *static_cast<ContentsStream*>(pData);

	kStream.SetRp(0);	// 혹시 모를 일을 대비해 처음부터 읽는다.

	u1	nLDigimonArrIDX=0;
	u1	nRDigimonArrIDX=0;

	u2 nLEnchantLevel = 0;
	u2 nLExtendAttribute[MaxExtendStat] = {0,};
	u2 nLExtendAttributeLV[MaxExtendStat] = {0,};

	u2 nREnchantLevel = 0;
	u2 nRExtendAttribute[MaxExtendStat] = {0,};
	u2 nRExtendAttributeLV[MaxExtendStat] = {0,};

	kStream>>nLDigimonArrIDX>>nRDigimonArrIDX>>nLEnchantLevel>>nLExtendAttribute>>nLExtendAttributeLV
		>>nREnchantLevel>>nRExtendAttribute>>nRExtendAttributeLV;

	// 좌. 우 정보 생성.
	cData_PostLoad::sDATA*	LeftData = g_pDataMng->GetTactics()->GetTactics(nLDigimonArrIDX-1);
	cData_PostLoad::sDATA*	RightData = g_pDataMng->GetTactics()->GetTactics(nRDigimonArrIDX-1);

	// 좌
	LeftData->s_nEnchantLevel	= nLEnchantLevel;
	memcpy( LeftData->s_ExtendAttribute, nLExtendAttribute, sizeof(u2)*MaxExtendStat );
	memcpy( LeftData->s_ExtendAttributeLV, nLExtendAttributeLV, sizeof(u2)*MaxExtendStat );


	// 우
	RightData->s_nEnchantLevel	= nREnchantLevel;
	memcpy( RightData->s_ExtendAttribute, nRExtendAttribute, sizeof(u2)*MaxExtendStat );
	memcpy( RightData->s_ExtendAttributeLV, nRExtendAttributeLV, sizeof(u2)*MaxExtendStat );

	// 성공 메시지 - 계열체 이름 필요
	std::wstring wsTypeName;
	CsTacticsExplain* pExplain = nsCsFileTable::g_pTacticsMng->GetTacticsExplain( LeftData->s_dwBaseDigimonID );
	if( pExplain )
	{
		CsTacticsExplain::sINFO* pInfo = pExplain->GetInfo();
		if( pInfo )
			wsTypeName = pInfo->s_szTacticsName;
	}
	if( !wsTypeName.empty() )
		MessageToNotify( MESSAGE_COMPLETE, const_cast<TCHAR*>(wsTypeName.c_str()) );

	DecreaseItemRequire(eTRADE_UPGRADE);

	if( net::game )
		net::game->SendQueryMoneyInven();

	Notify(eUpgrade_Reset, kStream);
}
void DataTradeContents::RecvTradeEvolution(void*	pData)
{
	SAFE_POINTER_RET(pData);
	ContentsStream kStream = *static_cast<ContentsStream*>(pData);

	kStream.SetRp(0);	// 혹시 모를 일을 대비해 처음부터 읽는다.

	u1 nLDigimonArrIDX = 0;
	u1 nRDigimonArrIDX = 0;

	u1 nLMaxEvoUnit = 0;
	cEvoUnit kLEvoUnit[nLimit::EvoUnit];

	u1 nRMaxEvoUnit = 0;
	cEvoUnit kREvoUnit[nLimit::EvoUnit];

	kStream>>nLDigimonArrIDX>>nRDigimonArrIDX>>nLMaxEvoUnit>>kLEvoUnit>>nRMaxEvoUnit>>kREvoUnit;

	// 좌. 우 정보 생성.
	cData_PostLoad::sDATA*	LeftData = g_pDataMng->GetTactics()->GetTactics(nLDigimonArrIDX-1);
	cData_PostLoad::sDATA*	RightData = g_pDataMng->GetTactics()->GetTactics(nRDigimonArrIDX-1);

	// 좌.&kLEvoUnit[1]
	LeftData->s_nMaxEvoUnit = nLMaxEvoUnit;
	//memcpy( LeftData->s_EvoUnit, kLEvoUnit, sizeof( cEvoUnit)*nLimit::EvoUnit );
	//memcpy( &LeftData->s_EvoUnit[1], &kLEvoUnit[1], sizeof( cEvoUnit)*nLMaxEvoUnit );

	for(int i=0; i<nLMaxEvoUnit; ++i)
		LeftData->s_EvoUnit[1+i] = kLEvoUnit[1+i];

	// 우.
	RightData->s_nMaxEvoUnit = nRMaxEvoUnit;
	//memcpy( RightData->s_EvoUnit, kREvoUnit, sizeof( cEvoUnit)*nLimit::EvoUnit );
	//memcpy( &RightData->s_EvoUnit[1], &kREvoUnit[1], sizeof( cEvoUnit)*nRMaxEvoUnit );

	for(int i=0; i<nRMaxEvoUnit; ++i)
		RightData->s_EvoUnit[1+i] = kREvoUnit[1+i];

	// 성공 메시지 - 계열체 이름 필요
	std::wstring wsTypeName;
	CsTacticsExplain* pExplain = nsCsFileTable::g_pTacticsMng->GetTacticsExplain( LeftData->s_dwBaseDigimonID );
	if( pExplain )
	{
		CsTacticsExplain::sINFO* pInfo = pExplain->GetInfo();
		if( pInfo )
			wsTypeName = pInfo->s_szTacticsName;
	}
	if( !wsTypeName.empty() )
		MessageToNotify( MESSAGE_COMPLETE, const_cast<TCHAR*>(wsTypeName.c_str()) );

	DecreaseItemRequire(eTRADE_EVOLUTION);

	if( net::game )
		net::game->SendQueryMoneyInven();

	Notify(eEvolution_Reset, kStream);
}


void DataTradeContents::RecvTradeFailure(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nResult = *static_cast< int* >( pData );

	MessageToNotify( nResult );
}

void DataTradeContents::StartTradeData(void*	pData)
{
	Notify(eCenter_CreateEffect);

	OnTradeDataEffect();
}

void DataTradeContents::EndTradeData()
{
	switch (m_eState)
	{
	case eTRADE_SCALE:
		Notify(eScale_Trade);
		break;

	case eTRADE_UPGRADE:
		Notify(eUpgrade_Trade);
		break;

	case eTRADE_EVOLUTION:
		Notify(eEvolution_Trade);
		break;
	default:
		assert_cs( "EndTradeData Error" );
		return;
	}	

	OffTradeDataEffect();
}
void DataTradeContents::OnTradeDataEffect()
{
	m_bEffectOn = true;
}
void DataTradeContents::OffTradeDataEffect()
{
	m_bEffectOn = false;
}
bool DataTradeContents::IsTradeDataEffect()
{
	return m_bEffectOn;
}
void DataTradeContents::CreateTradeCenterModelLeft(int ID)
{
	ContentsStream kStream;
	kStream<<ID;
	Notify(eCenter_CreateDigimonLeft, kStream);
}
void DataTradeContents::CreateTradeCenterModelRight(int ID)
{
	ContentsStream kStream;
	kStream<<ID;
	Notify(eCenter_CreateDigimonRight, kStream);
}
void DataTradeContents::DeleteTradeCenterModelLeft()
{
	Notify(eCenter_DeletDigimonLeft);
}
void DataTradeContents::DeleteTradeCenterModelRight()
{
	Notify(eCenter_DeletDigimonRight);
}

bool DataTradeContents::IsRequireItem(eSTATE eType)
{
	CsData_Exchange::MAP _CsDataExchange = nsCsFileTable::g_pDigimonMng->GetData_ExchangeMap();

	CsData_Exchange::MAP_Iter	miter = _CsDataExchange.find(  eType );

	if( miter == _CsDataExchange.end())
	{
		assert_cs("GetRequireItemCount error");
		return false;
	}

	// 인벤 아이템 갯수
	int ItemCount = GetInvenItemCount(eType);


	CsData_Exchange::LIST_INFO_CITER	List_Iter = miter->second.begin();

	for(List_Iter; List_Iter!= miter->second.end(); ++List_Iter)
	{
		CsData_Exchange::sINFO	info = *List_Iter;

		if(info.Item_Type == eType && ItemCount >= info.Req_Num )
		{
			return true;
		}
	}

	return false;
}

int	DataTradeContents::GetItemCodeCount(eSTATE	eType)					// 현재 타입의 아이템_코드가 몇개 있는지 얻는다.
{
	CsData_Exchange::MAP _CsDataExchange = nsCsFileTable::g_pDigimonMng->GetData_ExchangeMap();

	CsData_Exchange::MAP_Iter	miter = _CsDataExchange.find(  eType );

	if( miter == _CsDataExchange.end())
	{
		assert_cs("GetRequireItemCount error");
		return	-1;
	}

	CsData_Exchange::LIST_INFO_CITER	List_Iter = miter->second.begin();

	for(List_Iter; List_Iter!= miter->second.end(); ++List_Iter)
	{
		CsData_Exchange::sINFO	info = *List_Iter;

		if(info.Item_Type == eType )
			return	miter->second.size();
	}

	return 0;
}
int	DataTradeContents::GetItemCode(eSTATE	eType, int typecount)		// 현재 타입의 아이템_코드를 얻는다.
{
	CsData_Exchange::MAP _CsDataExchange = nsCsFileTable::g_pDigimonMng->GetData_ExchangeMap();

	CsData_Exchange::MAP_Iter	miter = _CsDataExchange.find(  eType );

	if( miter == _CsDataExchange.end())
	{
		assert_cs("GetRequireItemCount error");
		return	0;
	}

	CsData_Exchange::LIST_INFO_CITER	List_Iter = miter->second.begin();

	for(List_Iter; List_Iter!= miter->second.end(); ++List_Iter)
	{
		CsData_Exchange::sINFO	info = *List_Iter;

		if(info.Item_Type == eType && info.Use_Rank-1 == typecount)
			return	info.Item_Code;
	}
	return 0;
}

int DataTradeContents::GetRequireItemCount(eSTATE	eType, int typecount)
{
	CsData_Exchange::MAP _CsDataExchange = nsCsFileTable::g_pDigimonMng->GetData_ExchangeMap();

	CsData_Exchange::MAP_Iter	miter = _CsDataExchange.find(  eType );

	if( miter == _CsDataExchange.end())
	{
		assert_cs("GetRequireItemCount error");
		return	-1;
	}

	CsData_Exchange::LIST_INFO_CITER	List_Iter = miter->second.begin();

	for(List_Iter; List_Iter!= miter->second.end(); ++List_Iter)
	{
		CsData_Exchange::sINFO	info = *List_Iter;
		
		if(info.Item_Type == eType && info.Use_Rank-1 == typecount)
			return	info.Req_Num;
	}

	return 0;
}

int DataTradeContents::GetUseRank(eSTATE eType, int code)
{
	CsData_Exchange::MAP _CsDataExchange = nsCsFileTable::g_pDigimonMng->GetData_ExchangeMap();

	CsData_Exchange::MAP_Iter	miter = _CsDataExchange.find(  eType );

	if( miter == _CsDataExchange.end())
	{
		assert_cs("GetRequireItemCount error");
		return -1;
	}

	CsData_Exchange::LIST_INFO_CITER	List_Iter = miter->second.begin();

	for(List_Iter; List_Iter!= miter->second.end(); ++List_Iter)
	{
		CsData_Exchange::sINFO	info = *List_Iter;

		if(info.Item_Type == eType && info.Item_Code == code)
			return	info.Use_Rank;
	}

	return -1;
}
int DataTradeContents::GetInvenItemCount(eSTATE	eType)
{
	int codecount = GetItemCodeCount(eType);	// 전체 타입의 갯수

	cData_Inven*	pInven = g_pDataMng->GetInven();

	int ItemCount = 0;
	for(int i=0; i<codecount; ++i)
	{
		ItemCount = ItemCount + pInven->GetTypeToConut( GetItemCode(eType, i) );
	}
	return	ItemCount;
}

DataTradeContents::eSTATE DataTradeContents::GetIntToState(int state)
{
	if(state == 0)
		return eTRADE_SCALE;

	if(state == 1)
		return eTRADE_UPGRADE;

	if(state == 2)
		return eTRADE_EVOLUTION;

	return eTRADE_NONE;
}

DataTradeContents::eSTATE DataTradeContents::GetState()	const
{
	return m_eState;
}
void   DataTradeContents::SetState(eSTATE	state)
{
	m_eState = state;
}
// 알림창 메세지
void DataTradeContents::MessageToNotify(int	Message, TCHAR*	text1)
{
	switch(Message)
	{
	case MESSAGE_LACK:
		{
			// 필요 아이템 이미지의 번호 보관하기.
			int nKindNeedItemCount = GetItemCodeCount( m_eState );
			if( nKindNeedItemCount < 1 )
				return;

			int nNeedItemCode = GetItemCode( m_eState, nKindNeedItemCount - 1 );
			CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( nNeedItemCode );
			SAFE_POINTER_RET(pItem);

			CsItem::sINFO* pFTInfo = pItem->GetInfo();
			SAFE_POINTER_RET(pFTInfo);

			cPrintMsg::PrintMsg( MESSAGE_LACK,	pFTInfo->s_szName );
		}
		break;

	case MESSAGE_REGISTER:
	case MESSAGE_MISMATCH:
	case MESSAGE_PARTNERMON:
#if COMMON_LIB_FIXED
	case MESSAGE_SERVER_ERROR:
	case MESSAGE_DISTANCE_ERROR:
#endif
	case MESSAGE_MISMATCH_HATCHLV:
		cPrintMsg::PrintMsg( Message);
		break;

	case MESSAGE_COMPLETE:
	case MESSAGE_ACTION:
	case MESSAGE_SAME:
		{
			if(m_eState == eTRADE_SCALE)
				cPrintMsg::PrintMsg( Message, text1, (TCHAR*)UISTRING_TEXT( "DATA_TRADE_TITLE_SCALE" ).c_str());

			else if(m_eState == eTRADE_UPGRADE)
				cPrintMsg::PrintMsg( Message, text1, (TCHAR*)UISTRING_TEXT( "DATA_TRADE_TITLE_UPGRADE" ).c_str());

			else if(m_eState == eTRADE_EVOLUTION)
				cPrintMsg::PrintMsg( Message, text1, (TCHAR*)UISTRING_TEXT( "DATA_TRADE_TITLE_EVOLUTION" ).c_str());

		}
		break;
	}
}

bool DataTradeContents::DataTradeCheck(stLoadData*	pL_Data, stLoadData*	pR_Data, wstring	wStr)
{
	// 데이터가 없는 경우
	if( pL_Data == NULL || pR_Data == NULL )
	{
		MessageToNotify(MESSAGE_REGISTER);
		return false;
	}

	// 데이터가 같은 경우 검사.
	bool	bSame = false;
	switch(m_eState)
	{
	case eTRADE_SCALE:
		{
			int LSize = CsFloat2Int(pL_Data->s_fScale*100.0f);
			int RSize = CsFloat2Int(pR_Data->s_fScale*100.0f);
			if( LSize== RSize)
				bSame = true;
		}		
		break;

	case eTRADE_UPGRADE:
		{
			if( pL_Data->s_ExtendAttributeLV[ET_AT] == pR_Data->s_ExtendAttributeLV[ET_AT] &&
				pL_Data->s_ExtendAttributeLV[ET_BL] == pR_Data->s_ExtendAttributeLV[ET_BL] &&
				pL_Data->s_ExtendAttributeLV[ET_CR] == pR_Data->s_ExtendAttributeLV[ET_CR] &&
				pL_Data->s_ExtendAttributeLV[ET_EV] == pR_Data->s_ExtendAttributeLV[ET_EV] 
#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
			&&	pL_Data->s_ExtendAttributeLV[ET_HP] == pR_Data->s_ExtendAttributeLV[ET_HP]
#endif
			)
				bSame = true;
		}
		break;

	case eTRADE_EVOLUTION:
		{
			int nSameCnt = 0;
			for(int i=0;i<nLimit::EvoUnit; ++i)
			{
				if(pL_Data->s_EvoUnit[i].m_nSlotState == pR_Data->s_EvoUnit[i].m_nSlotState)
					nSameCnt++;
			}
			if(nSameCnt == nLimit::EvoUnit)
				bSame = true;

		}
		break;
	default:
		assert_cs( "DataTradeCheck Error" );
		return false;
	}
	// 데이터가 같은 경우
	if(bSame)
	{
		MessageToNotify(MESSAGE_SAME, (TCHAR*)wStr.c_str() );
		return false;
	}

	// 필요 아이템이 부족한 경우
	if(	IsRequireItem(m_eState) == false)
	{		
		MessageToNotify(MESSAGE_LACK);
		return false;
	}

	// 데이터 교환 가능. 교환 여부 묻는 메시지.
	MessageToNotify(MESSAGE_ACTION, (TCHAR*)wStr.c_str());
	return true;
}

void DataTradeContents::SetPickLeftSlot(bool	bPick)
{
	m_bPickLeft = bPick;
}
void DataTradeContents::SetPickRightSlot(bool	bPick)
{
	m_bPickRight = bPick;
}
bool DataTradeContents::IsPickLeftSlot() const
{
	return m_bPickLeft;
}
bool DataTradeContents::IsPickRightSlot() const
{
	return m_bPickRight;
}
void DataTradeContents::DecreaseItemRequire(eSTATE eType)
{
	cData_Inven*	pInven = g_pDataMng->GetInven();

	int codecount = GetItemCodeCount(eType);	// 전체 타입의 갯수

	for(int i=0; i<codecount; ++i)
	{
		int Count = GetRequireItemCount(eType, i);		// 필요 개수

		int Code = GetItemCode(eType, i);				// 현재 아이템 번호

		int	ItemCount = pInven->GetTypeToConut(Code);	// 인벤 소지 개수

		if( Count > ItemCount)
			continue;

		while(Count>0)
		{
			pInven->DecreaseItem( Code, 1, false, false );

			Count--;
		}

		if(Count == 0)
		{
			break;
		}
	}
}