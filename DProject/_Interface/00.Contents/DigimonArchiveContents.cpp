#include "StdAfx.h"
#include "DigimonArchiveContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

CDigimonArchiveContents::DigimonInArchive::DigimonInArchive()
:miSlotIndex(0),
mdwModelID(0),
mbIsInIncubator(false),
mbIsInTranscend(false),
mpData(NULL)
{


}
CDigimonArchiveContents::DigimonInArchive::~DigimonInArchive(void)
{
	miSlotIndex =0;
	mdwModelID =0;
	mbIsInIncubator=false;
	mbIsInTranscend=false;
	SAFE_DELETE(mpData);
}
const int& CDigimonArchiveContents::DigimonInArchive::GetSlotIndex(void)
{
	return miSlotIndex;
}
const DWORD& CDigimonArchiveContents::DigimonInArchive::GetModelID(void)
{
	return mdwModelID;
}
cData_PostLoad::sDATA* CDigimonArchiveContents::DigimonInArchive::GetDigimonData(void)
{
	return mpData;
}
void CDigimonArchiveContents::DigimonInArchive::Initialize(int SlotID, cData_PostLoad::sDATA* pkDigimonData)
{
	miSlotIndex = SlotID;
	mpData = pkDigimonData;
}
//////////////////////////////////////////////////////////////////////////
void CDigimonArchiveContents::DigimonIncuSlotInfo::ResetAll(void)
{
	RegisteredDigimonID = 0;
	RegisteredDigimonArchiveSlotIdx = 0;
	RegisteredItemInvenSlotIdx = -1;
	RemainTime = 0;
	isActiated = false;

}
bool	CDigimonArchiveContents::DigimonIncuSlotInfo::isRemainTime(void)
{
	int nRemainTime = (int)( RemainTime - _TIME_TS );
	if( nRemainTime < 1 )//시간 다되면
	{
		return false;
	}
	return true;
}

std::wstring CDigimonArchiveContents::DigimonIncuSlotInfo::GetRemainTime(void)
{
	std::wstring wsTime;
	if(isRemainTime() == false)
	{
		DmCS::StringFn::Format( wsTime, L"%d %s", 0, UISTRING_TEXT( "COMMON_TXT_SECOND" ).c_str() );
		return wsTime;
	}

	int nRemainTime = (int)( RemainTime - _TIME_TS );

	int nSec = nRemainTime;
	int nMin = nRemainTime / 60;
	int nHour = nMin / 60;
	int nDay = nHour / 24;

	if( nDay > 0 )
	{
		DmCS::StringFn::Format( wsTime, L"%d %s %d %s", 
			nDay, UISTRING_TEXT( "COMMON_TXT_DAY" ).c_str(), 
			nHour%24, UISTRING_TEXT( "COMMON_TXT_SHORT_HOUR" ).c_str() );
	}
	else if( nHour > 0 )
	{
		DmCS::StringFn::Format( wsTime, L"%d %s %d %s", 
			nHour%24, UISTRING_TEXT( "COMMON_TXT_SHORT_HOUR" ).c_str(), 
			nMin%60, UISTRING_TEXT( "COMMON_TXT_SHORT_MIN" ).c_str() );
	}
	else if( nMin > 0 )
	{
		DmCS::StringFn::Format( wsTime, L"%d %s", nMin%60, UISTRING_TEXT( "COMMON_TXT_MINUTE" ).c_str() );
	}
	else if( nSec > 0 )
	{
		DmCS::StringFn::Format( wsTime, L"%d %s", nSec%60, UISTRING_TEXT( "COMMON_TXT_SECOND" ).c_str() );
	}

	return wsTime;
}
//----------------------------------------------------------------------------------------------------------------------------------
int const CDigimonArchiveContents::IsContentsIdentity(void)
{
	return E_CT_DIGIMON_ARCHIVE_CONTENTS;
}

CDigimonArchiveContents::CDigimonArchiveContents(void)
:MaxArchiveCount(0),
OpenedArchiveCount(0),
FilterMode(0),
SortMode(0),
mpkSelectedDigimon(NULL),
EMPTY_SLOT(-1),
EMPTY_INCU_DIGIMON_SLOT(0),
Draged_Src_AccompanySlotID(-1),
Draged_Src_ArchiveSlotID(-1),
Draged_Src_IncuSlotID(-1),
Draged_Dest_AccompanySlotID(-1),
Draged_Dest_ArchiveSlotID(-1),
Draged_Dest_IncuSlotID(-1),
LastCursorInfo_Index(-1),
LastCursorInfo_Count(-1),
Be_Change_IncuSlotIdx(-1),
Be_Regist_ArchiveSlotIdx(-1),
m_bisReceiveData(false),
m_pOpenNPC(NULL),
mOpenedPos(0.0f, 0.0f),
m_nPotableIndex(0),
SelectedDigimonIdx(-1)
,m_bTranscendenceOverExp(false)
{
	mDigimons.clear();
	mSorted.clear();
	//Packet
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ARCHIEVE_DIGIMONS, this, &CDigimonArchiveContents::OnRecvArchiveInfo );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_END_ARCHIEVE_DIGIMONS, this, &CDigimonArchiveContents::OnRecvEndArchiveInfo );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INCUBATOR_INFO, this, &CDigimonArchiveContents::OnRecvIncubarotInfo );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHANGE_DIGIMONS_ARCHIVE, this, &CDigimonArchiveContents::OnRecvChangeResult );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_MOVE_DIGIMONS_IN_ARCHIVE, this, &CDigimonArchiveContents::OnRecvMoveResultInArchive );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHANGE_INCUBATOR, this, &CDigimonArchiveContents::OnRecvIncubatorChanged );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_START_INCUBATING_INFO, this, &CDigimonArchiveContents::OnRecv_Start_Incubating );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_STOP_INCUBATING_INFO, this, &CDigimonArchiveContents::OnRecv_Stop_Incubating );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ON_EXTENT_ARCHIVE, this, &CDigimonArchiveContents::OnRecvExtendArchiveCount );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ARCHIVE_OPENCNT, this, &CDigimonArchiveContents::OnRecvExtendArchiveCount );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ON_EXTENT_ACCOMPANY_SLOT, this, &CDigimonArchiveContents::OnRecvExtendAccompanySlot );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ON_CHANGE_PARTNER_DIGIMON, this, &CDigimonArchiveContents::OnChangePartnerDigimonInfo );

	//Click
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_START_INCUBATING, this, &CDigimonArchiveContents::Req_StartIncubating);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_STOP_INCUBATING, this, &CDigimonArchiveContents::Req_StopIncubating);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_UNREGISTER_INCUBATED_DIGIMON, this, &CDigimonArchiveContents::Req_UnRegister_Incubated_Digimon);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_REGISTER_DIGIMON_TO_ACTIVATE_INCUSLOT, this, &CDigimonArchiveContents::Req_Register_Digimon_To_ActivatedIncuslot);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_CHANGE_DIGIMON_IN_ACTIVATE_INCUSLOT, this, &CDigimonArchiveContents::Req_Change_Digimon_In_ActivatingIncuslot);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_SHOWVIEWER_BY_NPC, this, &CDigimonArchiveContents::OnClick_Show_by_NPC);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_SHOWVIEWER_BT_ITEM, this, &CDigimonArchiveContents::OnClick_Show_by_ITEM);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_CHANGE_DIGIMON, this, &CDigimonArchiveContents::Req_Change_Digimon);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_HIDE_VIEWER, this, &CDigimonArchiveContents::Req_CloseViewer);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::ON_CANCEL_INCUBATING, this, &CDigimonArchiveContents::OnCancelIncubating);


	//////////////////////////////////////////////////////////////////////////
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_DIGIMON_TRANSCENDENCE_CHARGE_EXP, this, &CDigimonArchiveContents::RecvExpCharge );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_DIGIMON_TRANSCENDENCE_HATCH_UP, this, &CDigimonArchiveContents::RecvDigimonHatchUp );
}

CDigimonArchiveContents::~CDigimonArchiveContents(void)
{
	ClearAllRegistered();
	GAME_EVENT_ST.DeleteEventAll( this );
}

int const CDigimonArchiveContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CDigimonArchiveContents::Initialize(void)
{
	return true;
}

void CDigimonArchiveContents::UnInitialize(void)
{

}

bool CDigimonArchiveContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void CDigimonArchiveContents::Update(float const& fElapsedTime)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE ) == true )
	{
		UpdateIncuActivating();
	}
}

void CDigimonArchiveContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CDigimonArchiveContents::MakeMainActorData(void)
{
	
}

void CDigimonArchiveContents::ClearMainActorData(void)
{
	DigimonsIter kIter = mDigimons.begin();
	for(;kIter!= mDigimons.end();){
		SAFE_DELETE(kIter->second);
		kIter = mDigimons.erase(kIter);
	}
	mDigimons.clear();

	IncuSlotIter kIncuInfo = mIncuSlotInfos.begin();
	for(;kIncuInfo!= mIncuSlotInfos.end();){
		SAFE_DELETE(kIncuInfo->second);
		kIncuInfo = mIncuSlotInfos.erase(kIncuInfo);
	}
	mIncuSlotInfos.clear();

	mSorted.clear();
	mInitIncuInfo.clear();

	SelectedDigimonIdx = -1;
	mpkSelectedDigimon= NULL;
	mpkDragedDigimon = NULL;
	m_bisReceiveData = false;
	m_nPotableIndex = 0;
	mOpenedPos = NiPoint2::ZERO;
}

void CDigimonArchiveContents::ClearWorldData(void)
{
}

void CDigimonArchiveContents::MakeWorldData(void)
{
}

void CDigimonArchiveContents::OnRecvExtendArchiveCount(void* pkData)
{
	SAFE_POINTER_RET( pkData );
	GS2C_RECV_EXTEND_ARCHIVE* pkRecvData = static_cast<GS2C_RECV_EXTEND_ARCHIVE*>(pkData);
	OpenedArchiveCount = pkRecvData->GetExtendArchiveCnt();
	Notify(Update_Viewer);
}

void CDigimonArchiveContents::OnShowViewer(void)
{
	if(m_pOpenNPC != NULL){
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_TACTICSHOUSE);
		Notify(OnShow_ByNPC);
	}
	else if( mOpenedPos != NiPoint2::ZERO){
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_TACTICSHOUSE);
		Notify(OnShow_ByItem);
	}
	else
	{
		m_bisReceiveData = true;
		return;
	}
	UpdateIncuActivating();
	Notify(Initailze_Archive);
	OnShowPartnerDigimonInfo();
	UpdateSlotData();
	Notify(Update_Viewer);
}

void CDigimonArchiveContents::OnRecvEndArchiveInfo(void* pkData)
{
	m_bisReceiveData = true;

	if(m_pOpenNPC != NULL){
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_TACTICSHOUSE);
		Notify(OnShow_ByNPC);
	}
	else if( mOpenedPos != NiPoint2::ZERO){
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_TACTICSHOUSE);
		Notify(OnShow_ByItem);
	}
// 	else
// 	{
// 		m_bisReceiveData = true;
// 		return;
// 	}
	Notify(Initailze_Archive);
	OnShowPartnerDigimonInfo();

	//데이터 수신중 메세지
	cMessageBox::DelMsg( 10019, false );
}


void CDigimonArchiveContents::OnRecvArchiveInfo(void* pkData)
{
	ArchiveInitInfo* kRecvData = static_cast<ArchiveInitInfo*>(pkData);
	if(kRecvData->GetSlotType() == 0)
	{
		MaxArchiveCount = kRecvData->GetMaxArchiveCnt();
		OpenedArchiveCount = kRecvData->GetOpenedArchiveCnt();

		mInitIncuInfo.clear();
		const ArchiveInitInfo::IncuInfo* pkIncuInfo = kRecvData->GetRecvIncuInfo();
		if(pkIncuInfo != NULL)
		{
			ArchiveInitInfo::IncuInfo::const_iterator kIter = pkIncuInfo->begin();
			for(; kIter != pkIncuInfo->end(); ++kIter){
				mInitIncuInfo.insert( ArchiveInitInfo::IncuInfo::value_type(kIter->first, kIter->second) );
			}
		}
	}

	ArchiveInitInfo::DigimonsConstIter kIter = kRecvData->GetRecvInfo()->begin();
	for(; kIter != kRecvData->GetRecvInfo()->end(); ++kIter)
	{
		DigimonInArchive* pkInfo = new DigimonInArchive();
		pkInfo->miSlotIndex = kIter->first;
		pkInfo->mdwModelID =-1;
		pkInfo->mpData = kIter->second;

		ArchiveInitInfo::IncuInfo::const_iterator kIncuIter = mInitIncuInfo.find(kIter->first + 1000);
		if(kIncuIter == mInitIncuInfo.end())
			pkInfo->mbIsInIncubator = false;
		else
			pkInfo->mbIsInIncubator = true;

		mDigimons.insert(Digimons::value_type(kIter->first, pkInfo));
	}
	UpdateSlotData();

}
void CDigimonArchiveContents::OnRecvMoveResultInArchive(void* pkDtata)
{
	GS2C_RECV_Archive_Internal_Move* pkRecvData = static_cast<GS2C_RECV_Archive_Internal_Move*>(pkDtata);
	SAFE_POINTER_RET(pkRecvData);

	SwapDigimonInArchive(pkRecvData->nSlot1 - 1000, pkRecvData->nSlot2 - 1000);
	UpdateSlotData();
	Notify(Update_Viewer);
}
void CDigimonArchiveContents::OnRecvChangeResult(void* pkData)
{
	GS2C_RECV_ARCHIVE_CHANGE* pkRecvData = static_cast<GS2C_RECV_ARCHIVE_CHANGE*>(pkData);
	// 조그레스 퀘스트 진행중이면 
	// 파트너 창고이동이 성공후 조그레스 퀘스트는 "취소"상태가 된다.
	if(0 != pkRecvData->Dst)
	{
		DigimonInArchive* pkDigimonInfo = GetArchiveSlotInfo(pkRecvData->Dst);
		if(pkDigimonInfo != NULL && pkDigimonInfo->GetDigimonData() != NULL)
		{
			CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( pkDigimonInfo->GetDigimonData()->s_dwBaseDigimonID );	
			if(!pFTEvolInfo)
				return;
			for( int i=1; i<nLimit::EvoUnit; ++i )
			{
				CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( i );
				if( pFTEvolObj == NULL )
					continue;	
				if( pFTEvolObj->m_nChipsetType != 0 && pFTEvolObj->m_nOpenQuest != 0 ){
					if(g_pDataMng->GetQuest()->IsProcess(pFTEvolObj->m_nOpenQuest))
					{
						g_pDataMng->GetQuest()->DropProcQuest( pFTEvolObj->m_nOpenQuest );
					}			
				}
			}
		}
	}

	// 돈감소
	g_pDataMng->GetInven()->SetMoney( g_pDataMng->GetInven()->GetMoney().GetInt() - pkRecvData->Price, false );

	// 데이터 교환
	SwapDigimon(pkRecvData->Src, pkRecvData->Dst);
	UpdateSlotData();
	Notify(Update_Viewer);
}

void CDigimonArchiveContents::OnRecvIncubatorChanged(void* pkData)
{
	GS2C_RECV_INCUBATOR_CHANGE* pkRecvData = static_cast<GS2C_RECV_INCUBATOR_CHANGE*>(pkData);
	if(pkRecvData != NULL)
	{
		IncuSlotIter kIter = mIncuSlotInfos.find(pkRecvData->Incu_slotIdx);
		if(kIter != mIncuSlotInfos.end())
		{
			//성장촉진 디지몬을 그냥 취소한 경우
			if(pkRecvData->Archive_slotIdx == 0)
			{
				UnRegistDigimon(pkRecvData->Incu_slotIdx);
			}
			//비어있지않고 활성화 되어있는 성장촉진 슬롯에 보관소의 다른 디지몬과 교체한경우
			//비어있지만 활성화 되어있는 성장촉진 슬롯에 보관소의 디지몬을 등록한경우
			else
			{
				UnRegistDigimon(pkRecvData->Incu_slotIdx);
				RegistDigimonToIncubator(pkRecvData->Incu_slotIdx, pkRecvData->Archive_slotIdx - 1000);
			}
		}
	}
	Notify(Update_IncuViewer);
	cMessageBox::DelMsg( 10019, false );
}

void CDigimonArchiveContents::OnRecvEndIncubating(void* pkData)
{

}

void CDigimonArchiveContents::OnRecvIncubarotInfo(void* pkData)
{
	mIncuSlotInfos.clear();
	GS2C_RECV_INCUBATOR_INFO* pkRecvData = static_cast<GS2C_RECV_INCUBATOR_INFO*>(pkData);
	if(pkRecvData != NULL)
	{
		std::map<int, IncubatorInfo>::iterator kIter = pkRecvData->mIncuRegists.begin();
		for(; kIter != pkRecvData->mIncuRegists.end(); ++kIter)
		{
			DigimonIncuSlotInfo* pkInfo = new DigimonIncuSlotInfo();
			if(kIter->second.remaintime > 0)
				pkInfo->SetisActivated(true);
			pkInfo->SetRemianTime(kIter->second.remaintime);
			pkInfo->SetReg_DigimonArchiveSlotID(kIter->second.archiveslotidx);
			pkInfo->SetReg_ItemType(kIter->second.itemtype);
			pkInfo->SetReg_ItemInvenSlotIdx(-1);
// 			cData_PostLoad::sDATA* pData = GetDigimonDataInArchive( kIter->second.archiveslotidx );
// 			if( pData )
// 				pData->s_nLevel = kIter->second.digimonLv;

			mIncuSlotInfos.insert(IncuSlotInfo::value_type(kIter->first, pkInfo));
		}
	}
	Notify(OnShow_IncuViewer);
	cMessageBox::DelMsg( 10019, false );
}

void CDigimonArchiveContents::OnRecv_Start_Incubating(void* pkData)
{
	GS2C_RECV_START_INCUBATING_INFO* pkRecvData = static_cast<GS2C_RECV_START_INCUBATING_INFO*>(pkData);
	IncuSlotIter kIncuIter = mIncuSlotInfos.find(pkRecvData->Incu_slotIdx);
	if(kIncuIter != mIncuSlotInfos.end())
	{
		DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
		if(pkRegisteredInfo != NULL)
		{
			//보관소 디지몬 촉진 락킹
			DigimonInArchive* pkArchiveInfo =GetArchiveSlotInfo(pkRegisteredInfo->GetReg_DigimonArchiveSlotID());
			if(pkArchiveInfo != NULL){
				pkArchiveInfo->mbIsInIncubator = true;
			}
			//인큐베이터 슬롯 활성화
			pkRegisteredInfo->SetisActivated(true);
			pkRegisteredInfo->SetRemianTime(pkRecvData->RemainTime);
			//인벤의 아이템 언락킹 및 카운트 감소
			g_pDataMng->ItemUnlock(pkRegisteredInfo->GetReg_ItemInvenSlotIdx() );
			g_pDataMng->GetInven()->GetData( pkRegisteredInfo->GetReg_ItemInvenSlotIdx() )->DecreaseCount( 1 );
			pkRegisteredInfo->SetReg_ItemInvenSlotIdx(EMPTY_SLOT);
			ResetDragInfo();
			Notify(Update_IncuViewer);
		}
	}	
	//데이터 수신중 메세지
	cMessageBox::DelMsg( 10019, false );
}
void CDigimonArchiveContents::OnRecv_Stop_Incubating(void* pkData)
{
	GS2C_RECV_STOP_INCUBATING_INFO* pkRecvData = static_cast<GS2C_RECV_STOP_INCUBATING_INFO*>(pkData);
	if(pkRecvData != NULL)
	{
		IncuSlotIter kIncuIter = mIncuSlotInfos.find(pkRecvData->Incu_slotIdx);
		if(kIncuIter != mIncuSlotInfos.end())
		{
			DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
			if(pkRegisteredInfo != NULL)
			{	
				//인큐베이팅 정보 초기화
				UnRegistDigimon(pkRecvData->Incu_slotIdx);
				UnLockRegisteredItem(pkRecvData->Incu_slotIdx);
				pkRegisteredInfo->ResetAll();
				UpdateSlotData();
				ResetDragInfo();
				Notify(Update_IncuViewer);
			}
		}
	}
	//데이터 수신중 메세지
	cMessageBox::DelMsg( 10019, false );

}
void CDigimonArchiveContents::OnRecvArchiveOpenCnt(void* pkData)
{

}
void CDigimonArchiveContents::OnRecvExtendAccompanySlot(void* pkData)
{
	GS2C_RECV_EXTEND_ACCOMPANY_SLOT* pkRecvData = static_cast<GS2C_RECV_EXTEND_ACCOMPANY_SLOT*>(pkData);
	SAFE_POINTER_RET(pkRecvData);
	Notify(Update_Viewer);
}
void CDigimonArchiveContents::OnChangePartnerDigimonInfo(void* pkData)
{
	cData_PostLoad::sDATA* pkDigimonData = g_pDataMng->GetPostLoad()->GetDigimonData();
	if(pkDigimonData != NULL){
		//현재 선택된 디지몬이 파트너 디지몬이라면
		if(pkDigimonData == mpkSelectedDigimon){
			mpkSelectedDigimon = NULL;
			//정보 갱신후에 출력
			OnShowPartnerDigimonInfo();
		}
	}

}
void CDigimonArchiveContents::OnClick_Show_by_NPC(void* pkData)
{
	SAFE_POINTER_RET(pkData);
	CsC_AvObject* pkNPC = static_cast<CsC_AvObject*>(pkData);
	if(pkNPC != NULL)
	{
		int NPCLoadID =  pkNPC->GetLoadID();
		int InvenID = 0;
		m_pOpenNPC = pkNPC;
		mOpenedPos = NiPoint2::ZERO;
		m_nPotableIndex = 0;
		if(m_bisReceiveData == false){
			if(!g_pGameIF->IsActiveWindow(cBaseWindow::WT_TACTICSHOUSE))
				g_pGameIF->GetDynamicIF( cBaseWindow::WT_TACTICSHOUSE);
			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::TACTICSHOUSE_LIST, 0, &NPCLoadID, &InvenID );
			cPrintMsg::PrintMsg( 10019 );
			/*m_bisReceiveData = true;*/
		}
		else
		{	
			OnShowViewer();
		}
	}
}
void CDigimonArchiveContents::OnClick_Show_by_ITEM(void* pkData)
{
	SAFE_POINTER_RET(pkData);
	
	std::pair<int, NiPoint2> pairData = *static_cast<std::pair<int, NiPoint2>*>(pkData);

	m_pOpenNPC = NULL;

	mOpenedPos = pairData.second;
	m_nPotableIndex = pairData.first;

	int NPCLoadID =  0;
	int InvenID = m_nPotableIndex;
	if(m_bisReceiveData == false){
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::TACTICSHOUSE_LIST, 0, &NPCLoadID, &InvenID );
		cPrintMsg::PrintMsg( 10019 );
		m_bisReceiveData = true;
	}
	else{	
		OnShowViewer();
	}
}


void CDigimonArchiveContents::OnClick_SetFilteringMode(void* pData)
{
	int* pkFilterMode = static_cast< int* >(pData);
	SetFilterMode(*pkFilterMode);
	UpdateSlotData();
	Notify(Update_Viewer);
}
void CDigimonArchiveContents::OnClick_SetFilteringKeyword(void* pData)
{
	wstring* kSerchingKeyword = static_cast<wstring*>(pData);
	SetSerchingKeyword(*kSerchingKeyword);
	UpdateSlotData();
	Notify(Update_Viewer);
}

//
//
void CDigimonArchiveContents::OnClick_SetSortingMode(void* pData)
{
	int* pkSortmode = static_cast< int* >(pData);
	SetSortMode(*pkSortmode);
	UpdateSlotData();
	Notify(Update_Viewer);
}
void CDigimonArchiveContents::Req_ShowIncubatorViewer(void)
{
	UnRegistAllDigimonToTranscend();
	net::game->SendIncubaterList();
	cPrintMsg::PrintMsg( 10019 );
	if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRANSCENDENCE_HELP ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TRANSCENDENCE_HELP );
}

void CDigimonArchiveContents::Req_ShowArchiveViewer(void)
{
	UnLockRegisteredItem();
	UnRegistDigimonAll();
	UnRegistAllDigimonToTranscend();
	Notify(Update_Viewer);
	ReleaseSelectedDigimon();

	if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRANSCENDENCE_HELP ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TRANSCENDENCE_HELP );
}

void CDigimonArchiveContents::Req_ShowTranscendViewer(void)
{
	UnLockRegisteredItem();
	UnRegistDigimonAll();
	UnRegistAllDigimonToTranscend();
	Notify(OnShow_TranscendViewer);
	ReleaseSelectedDigimon();
}

void	CDigimonArchiveContents::OnClick_StartIncubating(int iIncuSlotID)
{
	IncuSlotIter kIncuIter = mIncuSlotInfos.find(iIncuSlotID);
	if(kIncuIter != mIncuSlotInfos.end())
	{
		DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
		if(pkRegisteredInfo != NULL)
		{
			//이미 촉진중인가
			if(pkRegisteredInfo->GetisActivated() == false)
			{
				//촉진 중이 아닌 슬롯에서 정보가 올바르게 들어가 있는경우
				if(pkRegisteredInfo->GetReg_ItemType() > 0 &&
					pkRegisteredInfo->GetReg_DigimonArchiveSlotID() > EMPTY_INCU_DIGIMON_SLOT)
				{
					SetDragInfo(CDigimonArchiveContents::SRC_INCU, iIncuSlotID);
					cPrintMsg::PrintMsg(30625);//성장 촉진을 시작하시겠습니까?
				}
			}
			else
			{
				//촉진 중인 슬롯에서 정보가 올바르게 들어가 있는 경우 멈춤
				//촉진 중인 슬롯에서 디지몬만 빠져 있는경우에도 멈춤
				if(pkRegisteredInfo->GetReg_ItemType() > 0 )
				{
					SetDragInfo(CDigimonArchiveContents::SRC_INCU, iIncuSlotID);
					cPrintMsg::PrintMsg(30626);//성장 촉진을 멈추시겠습니까?
				}
			}
		}
	}
}
void	CDigimonArchiveContents::Req_StopIncubating(void* pData)
{
	if(Draged_Src_IncuSlotID != EMPTY_SLOT)
	{
		IncuSlotIter kIncuIter = mIncuSlotInfos.find(Draged_Src_IncuSlotID);
		if(kIncuIter != mIncuSlotInfos.end())
		{
			DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
			if(pkRegisteredInfo)
			{
				//멈추기
				net::game->SendIncubatorExtraction( (u1)Draged_Src_IncuSlotID );
				//데이터 수신중 메세지
				cPrintMsg::PrintMsg( 10019 );
			}
		}
	}

}
void CDigimonArchiveContents::Req_StartIncubating(void* pData)
{
	if(Draged_Src_IncuSlotID != EMPTY_SLOT)
	{
		IncuSlotIter kIncuIter = mIncuSlotInfos.find(Draged_Src_IncuSlotID);
		if(kIncuIter != mIncuSlotInfos.end())
		{
			DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
			if(pkRegisteredInfo)
			{
				net::game->SendIncubatorFacilitation( (u1)Draged_Src_IncuSlotID, 
					(u4)(pkRegisteredInfo->GetReg_DigimonArchiveSlotID()), 
					(u4)pkRegisteredInfo->GetReg_ItemInvenSlotIdx());

				//데이터 수신중 메세지
				cPrintMsg::PrintMsg( 10019 );
			}
		}
	}
}
void	CDigimonArchiveContents::Req_Start_Extraction(void)
{

	cPrintMsg::PrintMsg( 10019 );

}
void	CDigimonArchiveContents::Req_UnRegister_Incubated_Digimon(void* pData)
{
	if(Be_Change_IncuSlotIdx != EMPTY_SLOT)
	{
		IncuSlotIter kIncuIter = mIncuSlotInfos.find(Be_Change_IncuSlotIdx);
		if(kIncuIter != mIncuSlotInfos.end())
		{
			DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
			if(pkRegisteredInfo)
			{
				int SelectedIncuSlotIdx = Be_Change_IncuSlotIdx;
				net::game->SendIncubaterSlotChange( SelectedIncuSlotIdx, 0 );
				//데이터 수신중 메세지
				cPrintMsg::PrintMsg( 10019 );
			}
		}
	}
}
const std::wstring CDigimonArchiveContents::GetSerchingKeyword(void)
{
	return SerchingKeyword;
}

void CDigimonArchiveContents::UpdateSlotData()
{
	Digimons kFiltered;
	std::wstring szKey;
	cData_PostLoad::sDATA* pData = NULL;
	DWORD CurrModelIdx = 0;
//Filtered---------------------------------------------------------------------------------------------
	DigimonsIter kIter = mDigimons.begin();
	for(; kIter != mDigimons.end(); ++kIter)
	{
		cData_PostLoad::sDATA* pkInfo = kIter->second->GetDigimonData();
		if(pkInfo != NULL){
			if(FilterMode == 0)
			{
				kFiltered.insert(Digimons::value_type(kIter->first, kIter->second ));
			}
			else
			{
				CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pkInfo->s_dwBaseDigimonID )->GetInfo();
				if(pFTDigimon != NULL)
				{
					//디지몬 속성 필터
					if(FilterMode <= nsCsDigimonTable::eATTRIBUTE_TYPE::AT_UNIDENTIFIED)
					{
						int eAttr = pFTDigimon->s_eAttributeType - nsCsDigimonTable::AT_NONE + 1;
						if(eAttr == FilterMode)
						{
							kFiltered.insert(Digimons::value_type(kIter->first, kIter->second ));
						}
					}
					//자연 속성 필터
					else
					{
						if( pFTDigimon->s_eBaseNatureTypes[0] != 0 )
						{
							int eAttr = pFTDigimon->s_eBaseNatureTypes[0];/* - nsCsDigimonTable::NT_ICE;*/
							if(eAttr == FilterMode + 10)
							{
								kFiltered.insert(Digimons::value_type(kIter->first, kIter->second ));
								continue;
							}
						}
						if( pFTDigimon->s_eBaseNatureTypes[1] != 0 )
						{
							int eAttr = pFTDigimon->s_eBaseNatureTypes[1];// - nsCsDigimonTable::NT_ICE;
							if(eAttr == FilterMode + 10)
							{
								kFiltered.insert(Digimons::value_type(kIter->first, kIter->second ));
								continue;
							}
						}
						if( pFTDigimon->s_eBaseNatureTypes[2] != 0 )
						{
							int eAttr = pFTDigimon->s_eBaseNatureTypes[2];// - nsCsDigimonTable::NT_ICE;
							if(eAttr == FilterMode + 10)
							{
								kFiltered.insert(Digimons::value_type(kIter->first, kIter->second ));
								continue;
							}
						}
					}
				}
			}
		}
	}
//Serching Filter---------------------------------------------------------------------------------------------
	DigimonsIter kFilterIter = kFiltered.begin();
	for(;kFilterIter != kFiltered.end(); )
	{
		cData_PostLoad::sDATA* pkData = kFilterIter->second->GetDigimonData();
		if(pkData == NULL || pkData->s_dwBaseDigimonID ==0)
			continue;

		CsTacticsExplain* pkExplain = nsCsFileTable::g_pTacticsMng->GetTacticsExplain( pkData->s_dwBaseDigimonID );
		if(pkExplain == NULL || pkExplain->GetInfo() == NULL)
			continue;

		wstring kName = pkData->s_szName;
		wstring kTypeName = pkExplain->GetInfo()->s_szTacticsName;
		DmCS::StringFn::Lwr(kName);
		DmCS::StringFn::Lwr(kTypeName);
		DmCS::StringFn::Lwr(SerchingKeyword);
		if(kName.size() < SerchingKeyword.size() && 
			kTypeName.size() < SerchingKeyword.size() )
		{
			kFilterIter = kFiltered.erase(kFilterIter);
		}
		else
		{
			bool isSame = false;
			if(kName.size() >= SerchingKeyword.size())
			{
				isSame= true;
				int iIndex = kName.find(SerchingKeyword);
				if(  iIndex < 0 )
				{
					isSame = false;
				}
			}

			if(isSame == false)
			{
				if(kTypeName.size() >= SerchingKeyword.size() )
				{
					isSame = true;
					int iIndex = kTypeName.find(SerchingKeyword);
					if(  iIndex < 0 )
					{
						isSame = false;
					}
				}
			}
			if(isSame == false)
			{
				kFilterIter = kFiltered.erase(kFilterIter);
			}
			else
			{
				++kFilterIter;
			}
		}
	}

//Sorted---------------------------------------------------------------------------------------------
	mSorted.clear();
	kFilterIter = kFiltered.begin();
	for(; kFilterIter != kFiltered.end(); ++kFilterIter)
	{
		cData_PostLoad::sDATA* pkInfo = kFilterIter->second->GetDigimonData();
		int iSlotIndex = kFilterIter->first;
		float fSlotIndex =(float)iSlotIndex *0.01;

		ViewInfo kViewInfo;
		kViewInfo.dwbaseDigimonID = pkInfo->s_dwBaseDigimonID;
		kViewInfo.iArchiveSlotIdx = iSlotIndex;

		if(SortMode == 0)
		{
			DmCS::StringFn::Format( szKey, L"L%.2f", fSlotIndex);
			mSorted.insert(	Sorted::value_type(szKey, kViewInfo));
		}
		else
		{
			{
				switch((SORTMODE)SortMode)
				{
				case SORTMODE::ASCENDINGLV:
					{
						int kCurrLV = pkInfo->s_nLevel;
						float fCurrLV = (float)kCurrLV * 0.01f;
						DmCS::StringFn::Format( szKey, L"L%.2f_%d_%.2f", fCurrLV, 
							kFilterIter->second->GetDigimonData()->s_dwBaseDigimonID, fSlotIndex);
					}
					break;
				case SORTMODE::DESCENDINGLV:
					{
						int kCurrLV = 100 - pkInfo->s_nLevel;
						float fCurrLV = (float)kCurrLV * 0.01f;
						DmCS::StringFn::Format( szKey, L"L%.2f_%d_%.2f", fCurrLV, 
							kFilterIter->second->GetDigimonData()->s_dwBaseDigimonID, fSlotIndex);
					}
					break;
				case SORTMODE::INCU:
					{
						int InIncubator = 1;
						if(kFilterIter->second->mbIsInIncubator == true)
						{
							InIncubator = 0;
						}
						DmCS::StringFn::Format( szKey, L"L%d_%d_%.2f", InIncubator, 
							kFilterIter->second->GetDigimonData()->s_dwBaseDigimonID, fSlotIndex);
					}
					break;
				case SORTMODE::EMPTY:
					{
						DmCS::StringFn::Format( szKey, L"L%.2f", fSlotIndex);
						mSorted.insert(	Sorted::value_type(szKey, kViewInfo));
					}
					break;

				case SORTMODE::ASCENDINGNAME:
					{
						DmCS::StringFn::Format( szKey, L"L%s_%d_%.2f", pkInfo->s_szName, 
							kFilterIter->second->GetDigimonData()->s_dwBaseDigimonID, fSlotIndex);
						DmCS::StringFn::Lwr(szKey);
					}
					break;
				}
				mSorted.insert(	Sorted::value_type(szKey, kViewInfo));
			}
		}
	}

	
}

const int CDigimonArchiveContents::GetOpenedAccompanySlotCnt(void)
{
	if(g_pDataMng != NULL && g_pDataMng->GetTactics() != NULL)
		return g_pDataMng->GetTactics()->_GetOpenSlot();

	return 0;
}
const bool CDigimonArchiveContents::isCompanyTactics(int iTacticsidx)
{
	if(g_pDataMng != NULL && g_pDataMng->GetTactics() != NULL)
		return g_pDataMng->GetTactics()->IsTactics(iTacticsidx);
	return false;
}
const DWORD CDigimonArchiveContents::GetUsingDigimon(void)
{
	return nsCsFileTable::g_pDigimonMng->GetDigimon( g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID() )->GetInfo()->s_dwModelID;
}
const DWORD CDigimonArchiveContents::GetCompanyID(int iIdx)
{
	if(g_pDataMng != NULL && g_pDataMng->GetTactics() != NULL){
		cData_PostLoad::sDATA* pData = g_pDataMng->GetTactics()->GetTactics( iIdx - 1 );
		if(pData != NULL){
			return pData->s_dwBaseDigimonID;
		}
	}
	return 0;
}
const DWORD CDigimonArchiveContents::GetModelID(int iSlotIdx)
{
	DigimonsIter kIter = mDigimons.find(iSlotIdx);
	if(kIter != mDigimons.end())
	{
		CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon(kIter->second->GetDigimonData()->s_dwBaseDigimonID)->GetInfo();
		if(pFTDigimon != NULL)
			return pFTDigimon->s_dwModelID;
	}
	return 0;
}
const DWORD CDigimonArchiveContents::GetBaseIDInArchive(int iSlotIdx)
{
	DigimonsIter kIter = mDigimons.find(iSlotIdx);
	if(kIter != mDigimons.end())
	{
		if(kIter->second->GetDigimonData() != NULL)
			return kIter->second->GetDigimonData()->s_dwBaseDigimonID;
	}
	return 0;
}


//파트너 디지몬이 선택됨
void	CDigimonArchiveContents::OnShowPartnerDigimonInfo(void)
{
	CDigimonUser* pkDigimonUser = g_pCharMng->GetDigimonUser(0);
	cData_PostLoad::sDATA* pkDigimonData = g_pDataMng->GetPostLoad()->GetDigimonData();
	if(pkDigimonUser && pkDigimonData != NULL)
	{
		//최신 정보로 갱신
		CDigimon::sBASE_STAT* pkBaseStat = pkDigimonUser->GetBaseStat(); 
		if(pkBaseStat){
			pkDigimonData->s_nLevel = pkDigimonUser->GetBaseStat()->GetLevel();
			pkDigimonData->s_nExp = pkDigimonUser->GetBaseStat()->GetExp();
		}
		CDigimon::sENCHENT_STAT* pEStat = pkDigimonUser->GetEnchantStat();
		if(pEStat){
			pkDigimonData->s_nEnchantLevel = pEStat->GetEnchantLevel() + 1;
			pkDigimonData->s_ExtendAttributeLV[ET_AT] = pEStat->GetEnchantLv( ET_AT );
			pkDigimonData->s_ExtendAttributeLV[ET_BL] = pEStat->GetEnchantLv( ET_BL );
			pkDigimonData->s_ExtendAttributeLV[ET_CR] = pEStat->GetEnchantLv( ET_CR );
			pkDigimonData->s_ExtendAttributeLV[ET_EV] = pEStat->GetEnchantLv( ET_EV );
		}
		pkDigimonData->s_fScale = pkDigimonUser->GetGameScale();
		memcpy( pkDigimonData->s_EvoUnit, pkDigimonUser->GetAttributeEvoUnit(), sizeof( cEvoUnit)*nLimit::EvoUnit );
		//////////////////////////
		if( mpkSelectedDigimon != pkDigimonData)
		{
			mpkSelectedDigimon = pkDigimonData;
			SelectedDigimonIdx = 0;
			ContentsStream kStream;
			kStream<<USING_DIGIMON;
			Notify(Selected_Digimon, kStream);
		}
	}
}
//보관소의 디지몬이 선택됨
void	CDigimonArchiveContents::OnShowDigimonInfoInArchieve(int ArchiveSlotIdx)
{
	DigimonsIter kIter =	mDigimons.find(ArchiveSlotIdx);
	if(kIter != mDigimons.end())
	{
		cData_PostLoad::sDATA* pkSelectedData = kIter->second->GetDigimonData();
		if(mpkSelectedDigimon != pkSelectedData)
		{
			mpkSelectedDigimon = pkSelectedData;
			SelectedDigimonIdx = ArchiveSlotIdx + 1;
			ContentsStream kStream;
			kStream<<ARCHIVE_DIGIMOM;
			Notify(Selected_Digimon, kStream);
		}
	}
	else
	{
		mpkSelectedDigimon = NULL;
		SelectedDigimonIdx = -1;
		ContentsStream kStream;
		kStream<<PART_END;
		Notify(Selected_Digimon, kStream);
	}
}

void CDigimonArchiveContents::ReleaseSelectedDigimon()
{
	SelectedDigimonIdx = -1;
	mpkSelectedDigimon = NULL;
	ContentsStream kStream;
	kStream<<PART_END;
	Notify(Selected_Digimon, kStream);
}

//동행중인 디지몬이 선택됨
void	CDigimonArchiveContents::OnShowDigimonInfoInAccompany(int AccompanySlotIdx)
{
	if(g_pDataMng->GetTactics()->IsTactics(AccompanySlotIdx) == true)
	{
		cData_PostLoad::sDATA* pkSelectedData = g_pDataMng->GetTactics()->GetTactics(AccompanySlotIdx);
		if(pkSelectedData->s_dwBaseDigimonID != 0 &&
			pkSelectedData != mpkSelectedDigimon )
		{
			mpkSelectedDigimon = pkSelectedData;
			SelectedDigimonIdx = AccompanySlotIdx + 1;
			ContentsStream kStream;
			kStream<<ACCOMPANY_DIGIMON;
			Notify(Selected_Digimon, kStream);
		}
	}
	else
	{
		mpkSelectedDigimon = NULL;
		ContentsStream kStream;
		kStream<<PART_END;
		Notify(Selected_Digimon, kStream);
	}
}
void	CDigimonArchiveContents::OnDragUpArchieveToAccompany(int AccompanySlotIdx, int ArchiveSlotIdx, CsC_AvObject* pNPC )
{
	ChangeDigimon(AccompanySlotIdx,ArchiveSlotIdx,pNPC);
}

bool CDigimonArchiveContents::ChangeDigimon( int iAccompanySlotIdx, int iArchieveSlotIdx , CsC_AvObject* pNPC )
{
	if(g_pDataMng->GetTactics()->_GetOpenSlot() <= iAccompanySlotIdx)
	{
		return false;

	}
	cData_PostLoad::sDATA* pkAchiveSlotData = GetDigimonDataInArchive(iArchieveSlotIdx + 1000 );
	DigimonInArchive* pkDigimonInfo = GetArchiveSlotInfo(iArchieveSlotIdx);
	if(pkDigimonInfo != NULL)
	{
		//인큐베이터 안에 있으면 교체 안됨
		if( pkDigimonInfo->mbIsInIncubator == true )
			return false;
	}

	bool isAccompanySlotEmpty = g_pDataMng->GetTactics()->IsTactics(iAccompanySlotIdx);
	bool isArchiveSlotEmpty = false;
	if( pkAchiveSlotData != NULL)
		isArchiveSlotEmpty = true;

	// 조그레스 진화중이면.
	// 조그레스 상태인가?
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );	
	if(pDUser->IsJointProgressing() || pDUser->IsCurJointProgress())
	{
		cPrintMsg::PrintMsg( 30513 );
		return false;
	}

#ifdef SDM_TAMER_XGUAGE_20180628
	if(pDUser->GetFTDigimon()->isX_AntiJogress())
	{
		cPrintMsg::PrintMsg( 30513 );
		return true;
	}
#endif

	// 둘중 어느 한곳이 빈 상태라면 
	if( isAccompanySlotEmpty == false && isArchiveSlotEmpty == true) 
	{
		// 동기화
		if( net::game ){
			if( pNPC ){
				CsNpc* pFTNpc = ( (CNpc*)pNPC )->GetFTNpc();
				net::game->SendDigimonMoveTo( iAccompanySlotIdx + 1, iArchieveSlotIdx + 1000, pFTNpc->GetInfo()->s_dwNpcID, false );
			}
			else
			{
				if( m_nPotableIndex == -1 )
					net::game->SendDigimonMoveTo( iAccompanySlotIdx + 1, iArchieveSlotIdx + 1000, 0, true );
				else
					net::game->SendDigimonMoveTo( iAccompanySlotIdx + 1, iArchieveSlotIdx + 1000, 0, false );
			}
		}
	}
	//ACcompany to Archive
	else if (isAccompanySlotEmpty == true && isArchiveSlotEmpty == false)
	{
		if(g_pDataMng != NULL && g_pDataMng->GetTactics() != NULL && g_pDataMng->GetTactics()->IsTactics(iAccompanySlotIdx))
		{
			cData_PostLoad::sDATA* pkAccompanyInfo = GetDigimonDataInAccompany( iAccompanySlotIdx + 1 );
			if(pkAccompanyInfo)
			{
				u8 nCost = CalCostMoney( pkAccompanyInfo->s_nLevel );
				sMONEY money( nCost );
				cPrintMsg::PrintMsg( 30050, &money );
				cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
				pMBox->SetValue1( iAccompanySlotIdx);
				pMBox->SetValue2( iArchieveSlotIdx );
				pMBox->SetValue3( (int)nCost );
				pMBox->SetValuePointer1( pNPC );
			}
		}
	}
	// 디지몬 슬롯 맞 교환
	else 
	{
		cData_PostLoad::sDATA* pkAccompanyInfo = GetDigimonDataInAccompany( iAccompanySlotIdx + 1 );
		DigimonInArchive* pkArchiveInfo = GetArchiveSlotInfo( iArchieveSlotIdx );
		if(pkArchiveInfo && pkAccompanyInfo)
		{
			u8 nCost = CalCostMoney( pkAccompanyInfo->s_nLevel );
			sMONEY money( nCost );
			cPrintMsg::PrintMsg( 30050, &money );
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1( iAccompanySlotIdx);
			pMBox->SetValue2( iArchieveSlotIdx );
			pMBox->SetValue3( (int)nCost );
			pMBox->SetValuePointer1( pNPC );
		}
	}
	return true;
}
u8 CDigimonArchiveContents::CalCostMoney( int nLevel )
{
	float fBaseMoney = 2.2f;
	if( nLevel < 41 )			fBaseMoney = 1;
	else if( nLevel < 66 )		fBaseMoney = 1.3f;
	else if( nLevel < 76 )		fBaseMoney = 1.5f;
	else if( nLevel < 100 )		fBaseMoney = 1.8f;

	return CsFloat2Int( fBaseMoney*nLevel*nLevel );
}
cData_PostLoad::sDATA* CDigimonArchiveContents::GetDigimonDataInArchive(int SlotIdx)
{
	DigimonsIter kIter = mDigimons.find(SlotIdx -1000);
	if(kIter != mDigimons.end())
		return kIter->second->GetDigimonData();
	return NULL;
}
CDigimonArchiveContents::DigimonInArchive* CDigimonArchiveContents::GetArchiveSlotInfo(int SlotIdx)
{
	DigimonsIter kIter = mDigimons.find(SlotIdx);
	if(kIter != mDigimons.end())
		return kIter->second;
	return NULL;
}

cData_PostLoad::sDATA* CDigimonArchiveContents::GetDigimonDataInAccompany(int SlotIdx)
{
	if(g_pDataMng != NULL && g_pDataMng->GetTactics() != NULL){
		cData_PostLoad::sDATA* pData = g_pDataMng->GetTactics()->GetTactics( SlotIdx - 1 );
		if(pData != NULL){
			return pData;
		}
	}
	return NULL;
}
cData_PostLoad::sDATA* CDigimonArchiveContents::GetDigimonDataInIncubator(int IncuSlotIdx)
{
	IncuSlotIter kIter = mIncuSlotInfos.find(IncuSlotIdx);
	if(kIter != mIncuSlotInfos.end())
	{
		int RegDigimonID = kIter->second->GetReg_DigimonArchiveSlotID();
		if(RegDigimonID > EMPTY_INCU_DIGIMON_SLOT)
		{
			return GetDigimonDataInArchive(RegDigimonID);
		}
	}
	return NULL;
}


void	CDigimonArchiveContents::MoveDigimonsAccomToArchive(int iArchiveSlotIdx, cData_PostLoad::sDATA* pkData)
{
	int ArchiveSlotIdx  = iArchiveSlotIdx - 1000;
	DigimonsIter kIter = mDigimons.find(ArchiveSlotIdx);
	if(kIter == mDigimons.end())
	{
		DigimonInArchive* pkInfo = new DigimonInArchive();
		pkInfo->miSlotIndex = ArchiveSlotIdx;
		pkInfo->mdwModelID =-1;
		pkInfo->mpData = new cData_PostLoad::sDATA();

		cData_PostLoad::sDATA temp = *pkInfo->mpData;
		*pkInfo->mpData = *pkData;
		*pkData = temp;

		pkInfo->mbIsInIncubator = false;
		pkInfo->mbIsInTranscend = false;

		mDigimons.insert(Digimons::value_type(ArchiveSlotIdx, pkInfo));
	}
}
void	CDigimonArchiveContents::RemoveDigimonsInArchive(int iArchiveSlotIdx)
{
	int ArchiveSlotIdx  = iArchiveSlotIdx - 1000;
	DigimonsIter kIter = mDigimons.find(ArchiveSlotIdx);
	if(kIter != mDigimons.end())
	{
		SAFE_DELETE(kIter->second);
		mDigimons.erase(ArchiveSlotIdx);
	}
}
void	CDigimonArchiveContents::SaveLastCursorInfo(int iIndex, int iCount)
{
	LastCursorInfo_Index = iIndex;
	LastCursorInfo_Count =iCount;
}
bool	CDigimonArchiveContents::SwapDigimonInArchive(int Src_ArchiveIdx, int Dest_ArchiveIdx)
{
	DigimonsIter kSrcIter = mDigimons.find(Src_ArchiveIdx);
	if(kSrcIter == mDigimons.end())
	{
		return false;
	}
	DigimonInArchive* pkSrcDigimon = kSrcIter->second;
	DigimonsIter kDestIter = mDigimons.find(Dest_ArchiveIdx);

	// 디지몬을 빈 보관소로 옮기는 경우
	if(kDestIter == mDigimons.end())
	{
		DigimonsIter kEraseIter = mDigimons.erase(kSrcIter);
		mDigimons.insert(Digimons::value_type(Dest_ArchiveIdx, pkSrcDigimon));
	}
	else
	{	
		DigimonInArchive* pkTempInfo = kSrcIter->second;
		kSrcIter->second = kDestIter->second;
		kDestIter->second = pkTempInfo;
	}
	return true;
}


bool	CDigimonArchiveContents::SwapDigimon(int iAccompanySlotidx, int iArchiveSlotIdx)
{
	cData_PostLoad::sDATA* pkDigimonInAccompany =  GetDigimonDataInAccompany(iAccompanySlotidx);
	cData_PostLoad::sDATA* pkDigimonInArchive =  GetDigimonDataInArchive(iArchiveSlotIdx);

	//양쪽에 디지몬이 들어가있는 경우
	if(pkDigimonInArchive != NULL && g_pDataMng->GetTactics()->IsTactics(iAccompanySlotidx - 1) == true)
	{
		if( mpkSelectedDigimon && (mpkSelectedDigimon == pkDigimonInAccompany || mpkSelectedDigimon == pkDigimonInArchive) )
			ReleaseSelectedDigimon();

		cData_PostLoad::sDATA temp = *pkDigimonInAccompany;
		*pkDigimonInAccompany = *pkDigimonInArchive;
		*pkDigimonInArchive = temp;
	}
	//보관소에서 빈 AccomPany로 등록
	else if( pkDigimonInArchive != NULL && g_pDataMng->GetTactics()->IsTactics(iAccompanySlotidx - 1) == false)
	{
		if( mpkSelectedDigimon && mpkSelectedDigimon == pkDigimonInArchive) 
			ReleaseSelectedDigimon();

		*pkDigimonInAccompany = *pkDigimonInArchive;
		RemoveDigimonsInArchive(iArchiveSlotIdx);
	}
	//Accompany가 빈 보관소로 이동
	else if(pkDigimonInArchive == NULL && g_pDataMng->GetTactics()->IsTactics(iAccompanySlotidx - 1) == true)
	{
		if( mpkSelectedDigimon && mpkSelectedDigimon == pkDigimonInAccompany)
			ReleaseSelectedDigimon(); 
		
		MoveDigimonsAccomToArchive(iArchiveSlotIdx, pkDigimonInAccompany );
		g_pDataMng->GetTactics()->DeleteTactics(iAccompanySlotidx - 1);
	}

	g_pDataMng->GetTactics()->LinkTactics();
	return true;
}

void	CDigimonArchiveContents::SetDragInfo(DragedDigimonType eType, int SlotIdx)
{
	switch(eType)
	{
	case DragedDigimonType::SRC_ACCOMPANY:
		Draged_Src_AccompanySlotID = SlotIdx - 1;
		break;
	case DragedDigimonType::SRC_ARCHIVE:
		Draged_Src_ArchiveSlotID= SlotIdx;
		break;
	case DragedDigimonType::DEST_ACCOMPANY:
		Draged_Dest_AccompanySlotID = SlotIdx- 1;
		break;
	case DragedDigimonType::DEST_ARCHIVE:
		Draged_Dest_ArchiveSlotID= SlotIdx;
		break;
	case DragedDigimonType::SRC_INCU:
		Draged_Src_IncuSlotID= SlotIdx;
		break;
	case DragedDigimonType::DEST_INCU_DIGIMON:
		Draged_Dest_IncuSlotID= SlotIdx;
		break;
	case DragedDigimonType::SRC_INVEN_ITEM:
		Draged_Src_InvenSlotID = SlotIdx;
		break;

	}
}
void	CDigimonArchiveContents::ExcuteDragInfo(void)
{
	if(Draged_Src_IncuSlotID != EMPTY_SLOT && Draged_Dest_ArchiveSlotID != EMPTY_SLOT)
	{
		//인큐 To 보관소로
		OnDragUp_IncuToArchive(Draged_Src_IncuSlotID, Draged_Dest_ArchiveSlotID);
	}		
	else if(Draged_Src_AccompanySlotID != EMPTY_SLOT && Draged_Dest_ArchiveSlotID != EMPTY_SLOT)
	{
		//동행 디지몬 -> 보관소로 
		OnDragUpArchieveToAccompany(Draged_Src_AccompanySlotID, Draged_Dest_ArchiveSlotID, m_pOpenNPC);
	}
	else if(Draged_Src_ArchiveSlotID != EMPTY_SLOT && Draged_Dest_AccompanySlotID != EMPTY_SLOT)
	{
		//보관소-> 동행디지몬
		OnDragUpArchieveToAccompany(Draged_Dest_AccompanySlotID, Draged_Src_ArchiveSlotID, m_pOpenNPC);
	}
	else if ((Draged_Src_AccompanySlotID == EMPTY_SLOT && Draged_Dest_AccompanySlotID == EMPTY_SLOT)
			&& Draged_Src_ArchiveSlotID != Draged_Dest_ArchiveSlotID && 
			Draged_Src_ArchiveSlotID != EMPTY_SLOT && Draged_Dest_ArchiveSlotID != EMPTY_SLOT)
	{
		//보관소에 끼리 이동
		//Test Code
//#ifdef USE_MOVE_DIGIMON_IN_ARCHIVE
		OnDragUp_ArchiveToArchive(Draged_Src_ArchiveSlotID, Draged_Dest_ArchiveSlotID);
//#endif
		//SwapDigimonInArchive(Draged_Src_ArchiveSlotID, Draged_Dest_ArchiveSlotID);
		//UpdateSlotData();
		//Notify(Update_Viewer);*/
		//cPrintMsg::PrintMsg( 30053 );
	}
	else if ((Draged_Src_ArchiveSlotID == EMPTY_SLOT && Draged_Dest_ArchiveSlotID == EMPTY_SLOT ) &&
		Draged_Src_AccompanySlotID != Draged_Dest_AccompanySlotID && 
		Draged_Src_AccompanySlotID != EMPTY_SLOT && Draged_Dest_AccompanySlotID != EMPTY_SLOT)
	{
		//동행 디지몬끼리 이동
		cPrintMsg::PrintMsg( 30053 );
	}
	else
	{
		//제자리 이동, 이나 그 밖의 경우
		BHPRT("ExcuteDragInfo Please Check This");
	}
	ResetDragInfo();
}
void	CDigimonArchiveContents::ResetDragInfo()
{
	Draged_Src_AccompanySlotID = EMPTY_SLOT;
	Draged_Src_ArchiveSlotID = EMPTY_SLOT;
	Draged_Src_IncuSlotID = EMPTY_SLOT;
	Draged_Dest_AccompanySlotID = EMPTY_SLOT;
	Draged_Dest_ArchiveSlotID = EMPTY_SLOT;
	Draged_Dest_IncuSlotID = EMPTY_SLOT;
	LastCursorInfo_Index = EMPTY_SLOT;
	LastCursorInfo_Count= EMPTY_SLOT;
	//Draged_Src_InvenSlotID = EMPTY_SLOT;
}

void CDigimonArchiveContents::OnDragUp_ArchieveToIncubator(void)
{
	if( Draged_Dest_IncuSlotID != EMPTY_SLOT && Draged_Src_ArchiveSlotID != EMPTY_SLOT )
	{
		IncuSlotIter kIncuIter = mIncuSlotInfos.find(Draged_Dest_IncuSlotID);
		if(kIncuIter != mIncuSlotInfos.end())
		{
			cData_PostLoad::sDATA* pkDigimonInfo = GetDigimonDataInArchive(Draged_Src_ArchiveSlotID + 1000);
			if(pkDigimonInfo == NULL)
				return;
			//만일 현재 촉진중인 인큐베이터 라면
			if(kIncuIter->second->GetisActivated() == true)
			{
				Be_Change_IncuSlotIdx = Draged_Dest_IncuSlotID;
				Be_Regist_ArchiveSlotIdx = Draged_Src_ArchiveSlotID;
				//현재 디지몬이 들어가 있으면 교체
				if(kIncuIter->second->GetReg_DigimonArchiveSlotID() > EMPTY_INCU_DIGIMON_SLOT)
				{
					cPrintMsg::PrintMsg( 30628 );
				}
				//현재 비어있다면
				else
				{
					cPrintMsg::PrintMsg( 30630 );
				}
			}
			//그렇지 않으면 바로 등록
			else
			{
				RegistDigimonToIncubator(Draged_Dest_IncuSlotID, Draged_Src_ArchiveSlotID);
				ResetDragInfo();
				Notify(Update_IncuViewer);
			}
		}
	}
}
void CDigimonArchiveContents::OnDragUp_InvenToIncubator(void)
{	
	if(Draged_Src_InvenSlotID != EMPTY_SLOT 
		&& Draged_Dest_IncuSlotID != EMPTY_SLOT
		&& Draged_Src_ArchiveSlotID == EMPTY_SLOT)
	{
		cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( Draged_Src_InvenSlotID );
		if(pInfo != NULL)
		{
			CsItem* pkItem = nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() );
			if(pkItem)
			{
				CsItem::sINFO* pFTItemInfo = pkItem->GetInfo();
				if(pFTItemInfo != NULL)
				{
					if( pFTItemInfo->s_nType_L == nItem::Incubater )	// 촉진제 타입일때만
					{
						RegistItemToIncubator(Draged_Dest_IncuSlotID, Draged_Src_InvenSlotID);
						ResetDragInfo();
						Notify(Update_IncuViewer);
						return;
					}
					else
						cPrintMsg::PrintMsg( 11014 );	// 해당 슬롯에 등록할 수 없습니다	
				}
			}
		}
	}

}

void CDigimonArchiveContents::OnDragUp_ArchiveToArchive(int Src_ArchiveSlotIdx, int Dest_ArchiveSlotIdx)
{
	DigimonInArchive* pkSrcDigimon = GetArchiveSlotInfo(Src_ArchiveSlotIdx);
	if(pkSrcDigimon != NULL && (pkSrcDigimon->mbIsInIncubator == true || pkSrcDigimon->mbIsInTranscend == true) )
	{
		cPrintMsg::PrintMsg( 30098 );
		return;
	}
	DigimonInArchive* pkDestDigimon = GetArchiveSlotInfo(Dest_ArchiveSlotIdx);
	if(pkDestDigimon != NULL && (pkDestDigimon->mbIsInIncubator == true || pkDestDigimon->mbIsInTranscend == true) )
	{
		cPrintMsg::PrintMsg( 30098 );
		return;
	}
	DWORD	dwNPCID = 0;
	if(m_pOpenNPC != NULL)
	{
		CsNpc* pFTNpc = ( (CNpc*)m_pOpenNPC )->GetFTNpc();
		dwNPCID = pFTNpc->GetInfo()->s_dwNpcID;
	}

	if( -1 == m_nPotableIndex )
		net::game->SendDigimonMoveBetweenArchive( Src_ArchiveSlotIdx+ 1000, Dest_ArchiveSlotIdx+ 1000, dwNPCID, true);
	else
		net::game->SendDigimonMoveBetweenArchive( Src_ArchiveSlotIdx+ 1000, Dest_ArchiveSlotIdx+ 1000, dwNPCID, false);
	return;
}

void CDigimonArchiveContents::OnDragUp_IncuToArchive(int Src_IncuSlotIdx, int Dest_ArchiveSlotidx)
{
	//인큐 슬롯에서
	IncuSlotIter kIter = mIncuSlotInfos.find(Src_IncuSlotIdx);
	if(kIter != mIncuSlotInfos.end())
	{
		if(kIter->second->GetReg_DigimonArchiveSlotID() == (Dest_ArchiveSlotidx + 1000)){
			return;//같은 디지몬으로 변경
		}

		//보관함에 슬롯 위에서 드래그 업 했을때
		DigimonInArchive* pkArchiveInfo = GetArchiveSlotInfo(Dest_ArchiveSlotidx);
		if(pkArchiveInfo != NULL)
		{
			//인큐베이터에 들어있지 않은 상태에서
			if(pkArchiveInfo->mbIsInIncubator == false)
			{
				//만일 인큐슬롯이 활성화된 상태이고
				if(kIter->second->GetisActivated() == true)
				{
					//교체의사를 물어본다
					Be_Change_IncuSlotIdx = Src_IncuSlotIdx;
					Be_Regist_ArchiveSlotIdx = Dest_ArchiveSlotidx;
					cPrintMsg::PrintMsg( 30628 );
				}
				else
				{
					//바로 교환
					UnRegistDigimon(Src_IncuSlotIdx);
					RegistDigimonToIncubator(Src_IncuSlotIdx, Dest_ArchiveSlotidx );
					Notify(Update_IncuViewer);
				}
			}
			//인큐베이터에 등록, 혹은 가등록 되어있는경우에는
			else
			{
				cPrintMsg::PrintMsg( 11014 );
			}

		}
		else
		{
			//만일 인큐슬롯이 활성화된 상태라면
			if(kIter->second->GetisActivated() == true)
			{
				Be_Change_IncuSlotIdx = Src_IncuSlotIdx;
				cPrintMsg::PrintMsg( 30629 );	// 성장 촉진 중인 디지몬을 인큐베이터에서 꺼내겠습니까
			}
			else
			{
				UnRegistDigimon(Src_IncuSlotIdx);
				Notify(Update_IncuViewer);
			}
			
		}
	}

}
void CDigimonArchiveContents::OnRClick_Incubator_Digimon(int IncuSlotIdx)
{
	IncuSlotIter kIter = mIncuSlotInfos.find(IncuSlotIdx);
	if(kIter != mIncuSlotInfos.end())
	{
		//디지몬이 등록되어 있는데
		if( kIter->second->GetReg_DigimonArchiveSlotID() > EMPTY_INCU_DIGIMON_SLOT )
		{
			//현재 성장 촉진 활성화 중인 경우
			if(kIter->second->GetisActivated() == true)
			{
				Be_Change_IncuSlotIdx = IncuSlotIdx;
				cPrintMsg::PrintMsg( 30629 );	// 성장 촉진 중인 디지몬을 인큐베이터에서 꺼내겠습니까
			}
			//현재 성장 촉진 활성화 하지 않는 경우
			else if (kIter->second->GetisActivated() == false)
			{
				UnRegistDigimon(IncuSlotIdx);
				Notify(Update_IncuViewer);
			}
		}
	}

}
void CDigimonArchiveContents::OnRClick_Incubator_Item(int IncuSlotIdx)
{
	IncuSlotIter kIter = mIncuSlotInfos.find(IncuSlotIdx);
	if(kIter != mIncuSlotInfos.end())
	{
		if(kIter->second->GetisActivated() == false)
		{
			UnLockRegisteredItem(IncuSlotIdx);
			Notify(Update_IncuViewer);
		}
		else if (kIter->second->GetisActivated() == true)
		{
			SetDragInfo(CDigimonArchiveContents::SRC_INCU, IncuSlotIdx);
			cPrintMsg::PrintMsg(30626);//성장 촉진을 멈추시겠습니까?
		}
	}
}
void CDigimonArchiveContents::OnRClick_AccompanyDigimon(int AccompanySlotIdx)
{
	//칸이 비지 않았으면
	if(isCompanyTactics(AccompanySlotIdx) == true)
	{
		int EmptyArchiveSlotIdx = GetEmptyArchiveSlotIdx();
		if(EmptyArchiveSlotIdx > -1)
		{
			ChangeDigimon(AccompanySlotIdx, EmptyArchiveSlotIdx, m_pOpenNPC);
			return;
		}
	}
}
void CDigimonArchiveContents::OnRClick_LockedArchiveSlot(int ArchiveSlotIdx)
{
	if(g_pDataMng && g_pDataMng->GetInven())
	{
		int InvenSlotIdx = g_pDataMng->GetInven()->GetTypeToIndex(nItem::Cash_DigimonCareSlot);
		if(InvenSlotIdx != cData_Inven::eINVEN_INDEX::INVALIDE_INVEN_INDEX  )
		{
			if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_EXTRA_TACTICSHOUSE ) == false )
			{
				cPrintMsg::PrintMsg( 10004 );
				return;
			}
			int nTHouseCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse;
			if(GetOpenedArchiveCount() >= nTHouseCount)
			{
				cPrintMsg::PrintMsg( 20024 );
				return;
			}

			cItemInfo* pItem = g_pDataMng->GetInven()->GetData( InvenSlotIdx );
			if(pItem == NULL)
				return;

			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
			if(pFTItem == NULL)
				return;

			CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
			if(pFTInfo == NULL)
				return;

			CsC_AvObject* pTarget = NULL;
			switch( pFTInfo->s_nUseCharacter )
			{
			case 0:		// 해당 사항 없음
			case 1:		// 둘다 사용
				pTarget = g_pCharMng->GetTamerUser();		
				break;
			case 2:		// 디지몬 사용 가능
				pTarget = g_pCharMng->GetDigimonUser( 0 );
				break;
			case 3:		// 테이머 사용 가능		
				pTarget = g_pCharMng->GetTamerUser();		
				break;		
			default:
				assert_cs( false );
			}
			if(pTarget)
			{
#ifdef ITEM_USE_TIME_PASS
				if(pItem->m_nEndTime == 0xFFFFFFF0)
				{
					switch( pFTInfo->s_nBelonging )
					{
					case 0:
					case 2:	// 그외 일 경우
						cPrintMsg::PrintMsg( 30177, pFTInfo->s_szName );
						cMessageBox::GetFirstMessageBox()->SetValue1( InvenSlotIdx );
						break;

					case 1:// 사용시 귀속인 아이템일 경우
						cPrintMsg::PrintMsg( 30176, pFTInfo->s_szName );
						cMessageBox::GetFirstMessageBox()->SetValue1( InvenSlotIdx );
						break;
					}				
					return;
				}
				else
				{
					g_pDataMng->SendItemUse( TO_INVEN_SID( InvenSlotIdx ) );
				}	
#else
				g_pDataMng->SendItemUse( TO_INVEN_SID( InvenSlotIdx ) );
#endif
			}
		}	
		else
		{
			cPrintMsg::PrintMsg( 10004 );
		}
	}

}
void CDigimonArchiveContents::OnRClick_Inventory_Item(const int ItemInvenSlotIdx)
{
	if(ItemInvenSlotIdx >= 0)
	{
		IncuSlotIter kIter = mIncuSlotInfos.begin();
		for(; kIter != mIncuSlotInfos.end(); ++kIter)
		{
			//아이템이 비어있고
			if( kIter->second->GetReg_ItemType() <= 0 ){
				if(kIter->second->GetReg_DigimonArchiveSlotID() > EMPTY_INCU_DIGIMON_SLOT)
				{
					RegistItemToIncubator(kIter->first, ItemInvenSlotIdx);
					ResetDragInfo();
					Notify(Update_IncuViewer);
					return;
				}
			}
		}
	}
}

void CDigimonArchiveContents::OnRClick_Archive_Digimon_In_InfoMode(int ArchiveSlotIdx)
{
	DigimonInArchive* pkArchiveInfo = GetArchiveSlotInfo(ArchiveSlotIdx);
	if(pkArchiveInfo != NULL)
	{
		// Archive To Accompany(빈슬롯이 있으면 자동으로 삽입)
		int OpenedAccompanyCnt = GetOpenedAccompanySlotCnt();
		for(int i = 0; i < OpenedAccompanyCnt; ++i)
		{
			if(isCompanyTactics(i) == false)			{
				ChangeDigimon(i, ArchiveSlotIdx, m_pOpenNPC);
				return;
			}
		}
	}
}

void CDigimonArchiveContents::OnRClick_Archive_Digimon_In_IncuMode(int ArchiveSlotIdx)
{
	DigimonInArchive* pkArchiveInfo = GetArchiveSlotInfo(ArchiveSlotIdx);
	//디지몬이 인큐베이팅 중이 아니면
	if(pkArchiveInfo != NULL && pkArchiveInfo->mbIsInIncubator == false)
	{
		IncuSlotIter kIter = mIncuSlotInfos.begin();
		for(; kIter != mIncuSlotInfos.end(); ++kIter)
		{
			//디지몬이 비어있는데 
			if( kIter->second->GetReg_DigimonArchiveSlotID() <= EMPTY_INCU_DIGIMON_SLOT ){
				//활성화되어있으면
				if( kIter->second->GetisActivated() == true){
					Be_Change_IncuSlotIdx = kIter->first;
					Be_Regist_ArchiveSlotIdx = ArchiveSlotIdx;
					cPrintMsg::PrintMsg( 30630 );// 해당 디지몬 보관소의 디지몬을 촉진 중인 인큐베이터에 등록 하시겠습니까
				}
				//비활성화되어 있으면
				else{
					RegistDigimonToIncubator(kIter->first, ArchiveSlotIdx);
					Notify(Update_IncuViewer);
				}
				break;
			}
		}
	}
	//디지몬이 인큐베이팅 중이면
	else
	{
		IncuSlotIter kIter = mIncuSlotInfos.begin();
		for(; kIter != mIncuSlotInfos.end(); ++kIter)
		{
			//인큐베이터에서 찾은다음에
			if(kIter->second->GetReg_DigimonArchiveSlotID() == (ArchiveSlotIdx + 1000))
			{
				//이게 활성화 되어있으면
				if(kIter->second->GetisActivated() == true)
				{
					//물어봅니다
					Be_Change_IncuSlotIdx  = kIter->first;
					cPrintMsg::PrintMsg( 30629 );	// 성장 촉진 중인 디지몬을 인큐베이터에서 꺼내겠습니까
				}
				//이게 활성화 안 되어있으면
				else
				{
					//바로 등록해지
					UnRegistDigimon(kIter->first);
					Notify(Update_IncuViewer);
				}
				break;

			}
		}
	}
}

void	CDigimonArchiveContents::UnRegistDigimonAll(void)
{
	//1.기존에 인큐베이터에 등록된 디지몬을 해제 하고
	IncuSlotIter kIter = mIncuSlotInfos.begin();
	for(;kIter != mIncuSlotInfos.end(); ++kIter)
	{
		if( kIter->second->GetReg_DigimonArchiveSlotID() > EMPTY_INCU_DIGIMON_SLOT)
		{
			DigimonsIter kOldIter =	mDigimons.find(kIter->second->GetReg_DigimonArchiveSlotID() - 1000);
			if(kOldIter != mDigimons.end())
			{
				if(kIter->second->GetisActivated() == false)
				{
					kOldIter->second->mbIsInIncubator = false;
					kIter->second->SetReg_DigimonArchiveSlotID(EMPTY_INCU_DIGIMON_SLOT);
				}
			}
		}
	}
}

void	CDigimonArchiveContents::UnRegistDigimon(int IncuSlotIdx)
{
	//1.기존에 인큐베이터에 등록된 디지몬을 해제 하고
	IncuSlotIter kIter = mIncuSlotInfos.find(IncuSlotIdx);
	if(kIter != mIncuSlotInfos.end())
	{
		if(kIter->second->GetReg_DigimonArchiveSlotID() > EMPTY_INCU_DIGIMON_SLOT)
		{
			DigimonsIter kOldIter =	mDigimons.find(kIter->second->GetReg_DigimonArchiveSlotID() - 1000);
			if(kOldIter != mDigimons.end())
			{
				kOldIter->second->mbIsInIncubator = false;
				kIter->second->SetReg_DigimonArchiveSlotID(EMPTY_INCU_DIGIMON_SLOT);
			}
		}
	}
}
void	CDigimonArchiveContents::RegistDigimonToIncubator(int IncuSlotIdx, int ArchiveSlotIdx)
{
	IncuSlotIter kIncuIter = mIncuSlotInfos.find(IncuSlotIdx);
	if(kIncuIter != mIncuSlotInfos.end())
	{
		if(kIncuIter->second->GetReg_DigimonArchiveSlotID() > EMPTY_INCU_DIGIMON_SLOT)
			UnRegistDigimon(IncuSlotIdx);

		DigimonsIter kRegisteredIter =	mDigimons.find(ArchiveSlotIdx);
		if(kRegisteredIter != mDigimons.end())
		{
			kRegisteredIter->second->mbIsInIncubator = true;
			kIncuIter->second->SetReg_DigimonArchiveSlotID(ArchiveSlotIdx + 1000);
		}
	}
}

void	CDigimonArchiveContents::RegistItemToIncubator(int IncuSlotIdx, int ItemSlotIdx)
{
	IncuSlotIter kIncuIter = mIncuSlotInfos.find(IncuSlotIdx);
	if(kIncuIter != mIncuSlotInfos.end())
	{
		cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( ItemSlotIdx );
		if(pInfo != NULL){
			CsItem::sINFO* pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() )->GetInfo();
			if(pFTItemInfo != NULL){
				if( pFTItemInfo->s_nType_L == nItem::Incubater )	// 촉진제 타입일때만
				{
					if(kIncuIter->second->GetReg_ItemInvenSlotIdx() >= 0)
						g_pDataMng->ItemUnlock( kIncuIter->second->GetReg_ItemInvenSlotIdx() );

					kIncuIter->second->SetReg_ItemInvenSlotIdx(ItemSlotIdx);
					kIncuIter->second->SetReg_ItemType(pFTItemInfo->s_dwItemID);
					g_pDataMng->ItemLock( ItemSlotIdx ); 
				}
			}
		}
	}
}
void	CDigimonArchiveContents::ResetKeyword(void)
{
	SerchingKeyword =L"";
}


void	CDigimonArchiveContents::UnLockRegisteredItem(void)
{
	IncuSlotIter kIter = mIncuSlotInfos.begin();
	for(;kIter != mIncuSlotInfos.end(); ++kIter)
	{
		if(kIter->second->GetisActivated() == false)
		{
			if(kIter->second->GetReg_ItemInvenSlotIdx() >= 0)
			{
				g_pDataMng->ItemUnlock(kIter->second->GetReg_ItemInvenSlotIdx());
				kIter->second->SetReg_ItemInvenSlotIdx(EMPTY_SLOT);
			}
		}

	}
	ResetDragInfo();
}
void	CDigimonArchiveContents::UnLockRegisteredItem(int iIncuSlotIdx)
{
	IncuSlotIter kIter = mIncuSlotInfos.find(iIncuSlotIdx);
	if(kIter != mIncuSlotInfos.end())
	{
		if(kIter->second->GetReg_ItemInvenSlotIdx() >= 0)
		{
			g_pDataMng->ItemUnlock(kIter->second->GetReg_ItemInvenSlotIdx());
			kIter->second->SetReg_ItemInvenSlotIdx(EMPTY_SLOT);
		}
		if(kIter->second->GetReg_ItemType() > 0)
		{
			kIter->second->SetReg_ItemType(EMPTY_SLOT);
		}
	}
}
const bool CDigimonArchiveContents::CanStartIncubating(int iIncuSlotID)
{
	IncuSlotIter kIncuIter = mIncuSlotInfos.find(iIncuSlotID);
	if(kIncuIter != mIncuSlotInfos.end())
	{
		DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
		if(pkRegisteredInfo != NULL)
		{
			if(pkRegisteredInfo->GetisActivated() == false)
			{
				if(pkRegisteredInfo->GetReg_ItemInvenSlotIdx() >= 0 &&
					pkRegisteredInfo->GetReg_ItemType() > 0 &&
					pkRegisteredInfo->GetReg_DigimonArchiveSlotID() > EMPTY_INCU_DIGIMON_SLOT)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void	 CDigimonArchiveContents::Req_Register_Digimon_To_ActivatedIncuslot(void* pData)
{
	if(Be_Change_IncuSlotIdx != EMPTY_SLOT && Be_Regist_ArchiveSlotIdx != EMPTY_SLOT)
	{
		IncuSlotIter kIncuIter = mIncuSlotInfos.find(Be_Change_IncuSlotIdx);
		if(kIncuIter != mIncuSlotInfos.end())
		{
			DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
			if(pkRegisteredInfo)
			{
				net::game->SendIncubaterSlotChange( Be_Change_IncuSlotIdx, Be_Regist_ArchiveSlotIdx  + 1000);
				//데이터 수신중 메세지
				cPrintMsg::PrintMsg( 10019 );
			}
		}
	}
}
void	 CDigimonArchiveContents::Req_Change_Digimon_In_ActivatingIncuslot(void* pData)
{
	if(Be_Change_IncuSlotIdx != EMPTY_SLOT){
		IncuSlotIter kIncuIter = mIncuSlotInfos.find(Be_Change_IncuSlotIdx);
		if(kIncuIter != mIncuSlotInfos.end()){
			DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
			if(pkRegisteredInfo){
				net::game->SendIncubaterSlotChange( Be_Change_IncuSlotIdx, Be_Regist_ArchiveSlotIdx  + 1000);
				//데이터 수신중 메세지
				cPrintMsg::PrintMsg( 10019 );
			}
		}
	}
}
void	CDigimonArchiveContents::Req_Change_Digimon(void* pData)
{
	CDigimonArchiveContents::ChangeData* pkData = static_cast<CDigimonArchiveContents::ChangeData*>(pData);
	if(pkData != NULL)
	{
		if(pkData->changecost > g_pDataMng->GetInven()->GetMoney().GetInt())
		{
			cPrintMsg::PrintMsg( 11003 );//돈이 모자르다
			return;
		}
		// 동기화
		if( net::game )
		{
			if( m_pOpenNPC )
			{
				CsNpc* pFTNpc = ( (CNpc*)m_pOpenNPC )->GetFTNpc();
				net::game->SendDigimonMoveTo( pkData->accompanyslotidx + 1, pkData->archiveslotidx + 1000, 
					pFTNpc->GetInfo()->s_dwNpcID, false );
			}
			else
			{
				if( m_nPotableIndex == -1 )
					net::game->SendDigimonMoveTo( pkData->accompanyslotidx + 1, pkData->archiveslotidx + 1000, 0, true );
				else
					net::game->SendDigimonMoveTo( pkData->accompanyslotidx + 1, pkData->archiveslotidx + 1000, 0, false );
			}
		}			

	}
}
void	CDigimonArchiveContents::Req_CloseViewer(void* pData)
{
	ResetAllData();

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TACTICSHOUSE );

	if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRANSCENDENCE_HELP ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TRANSCENDENCE_HELP );
	//Notify(OnHide_Viewer);
}
void	CDigimonArchiveContents::ResetBuffer(void)
{
	Be_Change_IncuSlotIdx = EMPTY_SLOT;
	Be_Regist_ArchiveSlotIdx = EMPTY_SLOT;
}

void	CDigimonArchiveContents::ResetAllData(void)
{
	ResetDragInfo();
	ResetBuffer();
	Draged_Dest_IncuSlotID = EMPTY_SLOT;

	FilterMode =0;
	SortMode = 0;

	ResetKeyword();

	SelectedDigimonIdx = -1;
	mpkSelectedDigimon = NULL;
	mpkDragedDigimon = NULL;

	mOpenedPos =NiPoint2::ZERO;
	m_nPotableIndex = 0;
	m_pOpenNPC = NULL;
}

const int CDigimonArchiveContents::GetEmptyArchiveSlotIdx(void)
{
	for(int i =0 ; i < OpenedArchiveCount; ++i)
	{
		if(GetArchiveSlotInfo(i) == NULL)
		{
			return i;
		}
	}
	return -1;
}

void CDigimonArchiveContents::OnShowToolTip(SELECT_PART ePart, int SlotIdx)
{
	cData_PostLoad::sDATA* pkData = NULL;
	switch(ePart)
	{
	case USING_DIGIMON:
		pkData =  g_pDataMng->GetPostLoad()->GetDigimonData();
		break;
	case ACCOMPANY_DIGIMON:
		pkData = GetDigimonDataInAccompany(SlotIdx);
		break;
	case ARCHIVE_DIGIMOM:
		pkData = GetDigimonDataInArchive(SlotIdx);
		break;
	case INCUBATING_DIGIMON:
		OnShowToolTipIncubatorDigimon(SlotIdx);
		return;
	case INCUBATING_ITEM:
		OnShowToolTipIncubatorItem(SlotIdx);
		return;
	}

	SAFE_POINTER_RET( pkData );

	if(pkData->s_dwBaseDigimonID <= 0)
		return;

// 	std::wstring wsName;
// 	DmCS::StringFn::Format( wsName, _LAN( "디지몬이름 : %s \n" ), pkData->s_szName );
// 	std::wstring wsLv;
// 	DmCS::StringFn::Format( wsLv, _LAN( "디지몬레벨 : %d" ), pkData->s_nLevel );
// 	std::wstring wsTooltip;
// 	wsTooltip = wsName;
// 	wsTooltip += wsLv;
// 	TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, wsTooltip.c_str() );

	std::wstring wsTooltip = UISTRING_TEXT( "DIGIMON_ARCHIVE_TOOLTIP_DIGIMON_STATE" );
	DmCS::StringFn::Replace( wsTooltip, L"#Name#", pkData->s_szName );
	DmCS::StringFn::Replace( wsTooltip, L"#Level#", pkData->s_nLevel );
	DmCS::StringFn::Replace( wsTooltip, L"#HatchGrade#", pkData->s_HatchLevel );
	DmCS::StringFn::Replace( wsTooltip, L"#EnchantLevel#", pkData->s_nEnchantLevel - 1 );

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );
	pTooltip->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, wsTooltip.c_str() );
}

void CDigimonArchiveContents::OnShowToolTipIncubatorDigimon(int SlotIdx)
{
//	std::wstring ToolTipInfo;
	std::wstring wsTooltip = UISTRING_TEXT( "DIGIMON_ARCHIVE_TOOLTIP_DIGIMON_STATE" );
	cData_PostLoad::sDATA* pkData = GetDigimonDataInIncubator(SlotIdx);
	if( pkData != NULL)
	{
// 		std::wstring wsName;
// 		DmCS::StringFn::Format( wsName, _LAN( "디지몬이름 : %s \n" ), pkData->s_szName );
// 		std::wstring wsLv;
// 		DmCS::StringFn::Format( wsLv, _LAN( "디지몬레벨 : %d" ), pkData->s_nLevel );
// 		ToolTipInfo = wsName;
// 		ToolTipInfo += wsLv;

		DmCS::StringFn::Replace( wsTooltip, L"#Name#", pkData->s_szName );
		DmCS::StringFn::Replace( wsTooltip, L"#Level#", pkData->s_nLevel );
		DmCS::StringFn::Replace( wsTooltip, L"#HatchGrade#", pkData->s_HatchLevel );
		DmCS::StringFn::Replace( wsTooltip, L"#EnchantLevel#", pkData->s_nEnchantLevel - 1 );
	}
	else
	{
		//ToolTipInfo = UISTRING_TEXT("Digimon_Storage_Incubating_Empty_DigimonSlot");
		wsTooltip = UISTRING_TEXT("Digimon_Storage_Incubating_Empty_DigimonSlot");
	}

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );
	pTooltip->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, wsTooltip.c_str() );

	//TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, ToolTipInfo.c_str() );
}

void CDigimonArchiveContents::OnShowToolTipIncubatorItem(int SlotIdx)
{
	IncuSlotIter kIncuIter = mIncuSlotInfos.find(SlotIdx);
	if(kIncuIter == mIncuSlotInfos.end())
		return;
		
	DigimonIncuSlotInfo* pkRegisteredInfo = kIncuIter->second;
	SAFE_POINTER_RET( pkRegisteredInfo );

	if( !pkRegisteredInfo->GetisActivated() )
	{
		if(pkRegisteredInfo->GetReg_ItemInvenSlotIdx() >= 0)
		{
			cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( pkRegisteredInfo->GetReg_ItemInvenSlotIdx() );
			SAFE_POINTER_RET( pInfo );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->GetType(), cBaseWindow::WT_INVENTORY, 0, 0, pInfo );
		}
		else
		{
			std::wstring ToolTipInfo = UISTRING_TEXT("Digimon_Storage_Incubating_Empty_ItemSlot");
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, ToolTipInfo.c_str() );
		}
	}
	else
	{
		SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( pkRegisteredInfo->GetReg_ItemType() );
		SAFE_POINTER_RET( pItem );
		CsItem::sINFO* pItemInfo = pItem->GetInfo();
		SAFE_POINTER_RET( pItemInfo );

		std::wstring szExpApply;
		DmCS::StringFn::Format( szExpApply, L"%s : %d%%", UISTRING_TEXT( "DIGIMON_ARCHIVE_BOOSTED_EXP" ).c_str(), pItemInfo->s_nTypeValue );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, szExpApply.c_str() );
	}

}

void CDigimonArchiveContents::OnMouseMoveOut(void)
{
	ResetDragInfo();
}

void  CDigimonArchiveContents::OnCancelIncubating(void* pData)
{
	ResetDragInfo();
	ResetBuffer();
}

bool  CDigimonArchiveContents::UpdateIncuActivating()
{
	bool isChanged = false;
	const CDigimonArchiveContents::IncuSlotInfo* pkIncuSlotInfos = GetIncuSlotInfo();
	CDigimonArchiveContents::IncuSlotInfo::const_iterator kCIter = mIncuSlotInfos.begin();
	for(; kCIter != mIncuSlotInfos.end(); ++kCIter)
	{
		if(kCIter->second->GetisActivated() == true)
		{
			if(kCIter->second->isRemainTime() == false)
			{
				UnRegistDigimon(kCIter->first);
				UnLockRegisteredItem(kCIter->first);
				kCIter->second->ResetAll();
				isChanged = true;
			}
		}
	}
	if(isChanged)
		Notify(Update_IncuViewer);

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CDigimonArchiveContents::RegistDigimonToTranscend(int ArchiveSlotIdx)
{
	DigimonsIter kRegisteredIter =	mDigimons.find(ArchiveSlotIdx);
	if( kRegisteredIter == mDigimons.end() )
		return;

	// 이미 등록된 놈이면
	if( kRegisteredIter->second->mbIsInTranscend )
		return;

	kRegisteredIter->second->mbIsInTranscend = true;

	sTranscendMaterialInfo newRegist;
	newRegist.RegisteredDigimonArchiveSlotIdx = ArchiveSlotIdx + 1000;
	newRegist.RegisteredDigimonID = kRegisteredIter->second->mpData->s_dwBaseDigimonID;
	newRegist.RegisteredExp = GetMaterialDigimonExp(kRegisteredIter->second->mpData);
	mTransMaterialInfos.push_back( newRegist );	
}

void CDigimonArchiveContents::UnRegistAllDigimonToTranscend(void)
{
	TransMaterialIter it = mTransMaterialInfos.begin();
	for( ; it != mTransMaterialInfos.end(); ++it )
	{
		DigimonsIter kOldIter =	mDigimons.find(it->RegisteredDigimonArchiveSlotIdx - 1000);
		if(kOldIter == mDigimons.end())
			continue;

		kOldIter->second->mbIsInTranscend = false;
	}	

	mTransMaterialInfos.clear();
}

void CDigimonArchiveContents::UnRegistDigimonToTranscend(int materialSlotIdx)
{
	TransMaterialIter it = mTransMaterialInfos.begin();
	for( ; it != mTransMaterialInfos.end(); ++it )
	{
		if( materialSlotIdx != (*it).RegisteredDigimonArchiveSlotIdx - 1000 )
			continue;
		DigimonsIter kOldIter =	mDigimons.find((*it).RegisteredDigimonArchiveSlotIdx - 1000);
		if(kOldIter != mDigimons.end())
			kOldIter->second->mbIsInTranscend = false;

		mTransMaterialInfos.erase(it);
		break;
	}
}


void CDigimonArchiveContents::OnRClick_Archive_Digimon_In_TranscendMode( int ArchiveSlotIdx )
{
	// 선택된 디지몬이 없다
	if( NULL == mpkSelectedDigimon )
	{
		cPrintMsg::PrintMsg( 20115 );
		return;
	}

 	DigimonInArchive* pkArchiveInfo = GetArchiveSlotInfo(ArchiveSlotIdx);
	SAFE_POINTER_RET( pkArchiveInfo );

	// 성장 촉진 중인 디지몬은 재료 등록 못함.
	if( pkArchiveInfo->mbIsInIncubator )
		return;

 	if( pkArchiveInfo->mbIsInTranscend == false  )
	{
		if( mTransMaterialInfos.size() >= MAX_TRANSCENDENCE_MAX_MATERIAL_COUNT )
			return;

		// 현재 선택된 디지몬은 초월을 할 수 없는 상태인지 검사 함.
		if( !IsEnableTranscendDigimon(mpkSelectedDigimon))
			return;

		// 재료로 넣을려는 디지몬이 재료가 될수 있는지 검사함.
		if( !IsEnableTranscend_MaterialDigimon(pkArchiveInfo->mpData) )
			return;// 재료 아이템으로 등록 할 수 없다.

		if( !m_bTranscendenceOverExp )
		{
			DWORD dwCurrentExp = 0;
			DWORD dwMaxExp = 0;
			GetCurrentDigimonMaxNExp( mpkSelectedDigimon, dwCurrentExp, dwMaxExp );
			if( dwCurrentExp >= dwMaxExp )
			{	// 경험치를 더이상 넣을 수 없다
				cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Full_Exp );
				return;
			}
		}

		RegistDigimonToTranscend(ArchiveSlotIdx);// 재료 등록
	}
	else// 재료 등록 해제
		UnRegistDigimonToTranscend(ArchiveSlotIdx);

	Notify(Update_TranscendViewer);
}

void CDigimonArchiveContents::UnRegistAllTranscend()
{
	UnRegistAllDigimonToTranscend();
	Notify(Update_TranscendViewer);
}

// 초월을 할 수 있는 디지몬인지 조건 검사.
bool CDigimonArchiveContents::IsEnableTranscendDigimon( cData_PostLoad::sDATA const* pDigimonInfo )
{
	SAFE_POINTER_RETVAL( pDigimonInfo, false );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, false );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTacticsMng, false );

#ifdef CROSSWARS_SYSTEM
	// 크로스 워즈 디지몬은 초월 불가.
	if(nsCsFileTable::g_pDigimonMng->IsCrossDigimon(pDigimonInfo->s_dwBaseDigimonID))
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_EvoIdx );// 초월이 불가능한 디지몬입니다.
		return false;
	}
#endif

	// 분양 디지몬은 초월할 수 없다.
	if( nsCsFileTable::g_pDigimonParcelTBMng->IsParcelDigimon( pDigimonInfo->s_dwBaseDigimonID ) )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_DigimonTranscendenceParcelOut );
		return false;
	}

// 	// 파트너 디지몬 초월 할 수 없다.
//  파트너 디지몬의 Evo Type이 0이기 때문에 테이블에 진화 타입이 없기 때문에 예외처리를 하지 않는다.
// 	if( nsCsFileTable::g_pDigimonMng->IsStartDigimon( pDigimonInfo->s_dwBaseDigimonID ) )
// 	{
// 		cPrintMsg::PrintMsg( 20107 );
// 		return false;
// 	}
	
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pDigimonInfo->s_dwBaseDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, false );

	CsDigimon::sINFO* pTInfo = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pTInfo, false );
	
	sTranscendInfo const* pInfos = nsCsFileTable::g_pTacticsMng->GetTranscendInfo( pTInfo->s_eEvolutionType, pDigimonInfo->s_HatchLevel);
	if( NULL == pInfos )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_EvoIdx );// 초월이 불가능한 디지몬입니다.
		return false;
	}

	if( !pInfos->sNeedValue.IsEnableDigimonLv( pDigimonInfo->s_nLevel ) )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_TermsLevel );// 초월하려는 디지몬의 레벨이 부족합니다
		return false;// 디지몬 레벨 부족
	}

	if( !pInfos->sNeedValue.IsEnableScale( pDigimonInfo->s_fScale * 100 ) )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_TermsScale );// '초월하려는 대상의 크기가 부족
		return false;// 디지몬 크기 부족
	}

	if( !pInfos->sNeedValue.IsEnableEnchantLv( pDigimonInfo->s_nEnchantLevel ) )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_TermsDigiclone );//'초월하려는 대상의 강화 수치가 부족합니다.
		return false;// 디지몬 강화 수치 부족
	}
	return true;
}

// 초월에 필요한 재료 디지몬인지 검사
bool CDigimonArchiveContents::IsEnableTranscend_MaterialDigimon( cData_PostLoad::sDATA const* pDigimonInfo )
{
	SAFE_POINTER_RETVAL( mpkSelectedDigimon, false );// 디지몬

	SAFE_POINTER_RETVAL( pDigimonInfo, false );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, false );

	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pDigimonInfo->s_dwBaseDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, false );

	CsDigimon::sINFO* pTInfo = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pTInfo, false );

	// 파트너 디지몬은 재료로 넣을 수 없다.
	if( nsCsFileTable::g_pDigimonMng->IsStartDigimon( pDigimonInfo->s_dwBaseDigimonID ) )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_UseDigimonMaterial );// 초월하려는 대상의 재료로 사용할 수 없는 디지몬 타입입니다.
		return false;
	}

	// 분양 디지몬은 재료로 넣을 수 없다.
	if( nsCsFileTable::g_pDigimonParcelTBMng->IsParcelDigimon( pDigimonInfo->s_dwBaseDigimonID ) )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_DigimonMaterialParcelOut );
		return false;
	}

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTacticsMng, false );

	CsDigimon* pSelDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( mpkSelectedDigimon->s_dwBaseDigimonID );
	SAFE_POINTER_RETVAL( pSelDigimon, false );

	CsDigimon::sINFO* pSelTInfo = pSelDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pSelTInfo, false );

	sTranscendInfo const* pInfos = nsCsFileTable::g_pTacticsMng->GetTranscendInfo( pSelTInfo->s_eEvolutionType, mpkSelectedDigimon->s_HatchLevel);
	if( NULL == pInfos )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_UseAvailableDigimonMaterial );// '초월 하려는 대상의 재료로 테이블상에 없는 경우
		return false;
	}

	if( !pInfos->sNeedValue.IsEnableEvoType( pTInfo->s_eEvolutionType ) )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_UseDigimonMaterial );// 초월하려는 대상의 재료로 사용할 수 없는 디지몬 타입입니다.
		return false;// 진화 재료에 사용할 수 없는 디지몬 진화 타입
	}

	// 부화 단계 조건 확인
	if( !pInfos->sNeedValue.IsEnableHatchLv( pDigimonInfo->s_HatchLevel ) )
	{
		cPrintMsg::PrintMsg( 20116 );
		return false;
	}

	return true;
}

CDigimonArchiveContents::sSubMaterialInfo CDigimonArchiveContents::GetSubMaterial( DWORD const& dwDigimonIDX, int const& nType )
{
	sSubMaterialInfo sInfo;
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTacticsMng, sInfo );
	LIST_MATERIALS const* pSubMaterial = nsCsFileTable::g_pTacticsMng->GetTranscendEvo( dwDigimonIDX, nType );
	SAFE_POINTER_RETVAL( pSubMaterial, sInfo );
	LIST_MATERIALS_CIT it = pSubMaterial->begin();
	if( it != pSubMaterial->end() )
	{
		sInfo.dwItemType = (*it).dwItemType;
		sInfo.dwShowItemCode = nsCsFileTable::g_pItemMng->TypeLS_to_Disp( (*it).dwItemType / 100, (*it).dwItemType%100 );
		sInfo.nNeedCount = (*it).nNeedCount;
		sInfo.nHaveCount = GetInvenItemCount( (*it).dwItemType );
		sInfo.nMinAddExp = (*it).dwMinExp;
	}

	return sInfo;
}

DWORD CDigimonArchiveContents::GetInvenItemCount(DWORD const& dwItemType)
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, 0 );
	return pInven->GetCount_Item_TypeLS( dwItemType / 100, dwItemType%100 );
}

void CDigimonArchiveContents::LockSubMaterialItems()
{
}

DWORD CDigimonArchiveContents::GetMaterialDigimonExp( cData_PostLoad::sDATA const* pData )
{
	SAFE_POINTER_RETVAL( mpkSelectedDigimon, 0 );
	SAFE_POINTER_RETVAL( pData, 0);
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTacticsMng, 0 );

	// 경험치용 디지몬인지 검사
	bool bExpDigimon = nsCsFileTable::g_pTacticsMng->IsExpDigimon( pData->s_dwBaseDigimonID, pData->s_HatchLevel );
	if( bExpDigimon )
		return nsCsFileTable::g_pTacticsMng->GetExpDigimonFixedFigure( pData->s_dwBaseDigimonID, pData->s_HatchLevel );

	sTranscendCorrect const& pInfo = nsCsFileTable::g_pTacticsMng->GetTranscend_Correct();
	bool bSeamType = IsSameDigimonType( mpkSelectedDigimon, pData );
	float fGrowthCorrect = nsCsFileTable::g_pTacticsMng->GetSameCorrectValue( bSeamType, pData->s_HatchLevel );
	sSubMaterialInfo NeedSubMaterial = GetSubMaterial( mpkSelectedDigimon->s_dwBaseDigimonID, 1 );
	return (DWORD)nsTransExp::MaterialAddExp( pData->s_nLevel, pData->s_nEnchantLevel, pInfo.nEnchentDefaultCorrect, fGrowthCorrect, NeedSubMaterial.nMinAddExp );
}

bool CDigimonArchiveContents::IsSameDigimonType( cData_PostLoad::sDATA* pSel, cData_PostLoad::sDATA const* pDist )
{
	SAFE_POINTER_RETVAL( pSel, false );
	SAFE_POINTER_RETVAL( pDist, false );

	if( pSel->s_dwBaseDigimonID == pDist->s_dwBaseDigimonID )
		return true;

// 	CsDigimon* pSelDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwSrcDigimonIDx );
// 	SAFE_POINTER_RETVAL( pSelDigimon, false );
// 
// 	CsDigimon::sINFO* pSelTInfo = pSelDigimon->GetInfo();
// 	SAFE_POINTER_RETVAL( pSelTInfo, false );
// 
// 	CsDigimon* pDestDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDestDigimonIDx );
// 	SAFE_POINTER_RETVAL( pDestDigimon, false );
// 
// 	CsDigimon::sINFO* pDestTInfo = pDestDigimon->GetInfo();
// 	SAFE_POINTER_RETVAL( pDestTInfo, false );
// 
// 	if( pDestTInfo->s_eEvolutionType == pSelTInfo->s_eEvolutionType )
// 		return true;

	return false;
}

DWORD CDigimonArchiveContents::GetNextGrowthMaxExp( cData_PostLoad::sDATA const* pkDATA )
{
	SAFE_POINTER_RETVAL( pkDATA, 0 );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTacticsMng, 0 );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, 0 );

	CsDigimon* pSelDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pkDATA->s_dwBaseDigimonID );
	SAFE_POINTER_RETVAL( pSelDigimon, 0 );

	CsDigimon::sINFO* pSelTInfo = pSelDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pSelTInfo, 0 );

	sTranscendInfo const* pInfos = nsCsFileTable::g_pTacticsMng->GetTranscendInfo( pSelTInfo->s_eEvolutionType, pkDATA->s_HatchLevel);
	SAFE_POINTER_RETVAL( pInfos, 0 );

	return pInfos->dwMaxExp;
}

void CDigimonArchiveContents::GetCurrentDigimonMaxNExp( cData_PostLoad::sDATA const* pkDATA, DWORD & dwCurrentExp, DWORD & dwMaxpExp )
{
	dwCurrentExp = 0;
	dwMaxpExp = 0;
	SAFE_POINTER_RET( pkDATA );
	SAFE_POINTER_RET( nsCsFileTable::g_pTacticsMng );
	SAFE_POINTER_RET( nsCsFileTable::g_pDigimonMng );

	CsDigimon* pSelDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pkDATA->s_dwBaseDigimonID );
	SAFE_POINTER_RET( pSelDigimon );

	CsDigimon::sINFO* pSelTInfo = pSelDigimon->GetInfo();
	SAFE_POINTER_RET( pSelTInfo );

	sTranscendInfo const* pInfos = nsCsFileTable::g_pTacticsMng->GetTranscendInfo( pSelTInfo->s_eEvolutionType, pkDATA->s_HatchLevel);
	SAFE_POINTER_RET( pInfos );

	dwMaxpExp = pInfos->dwMaxExp;

	dwCurrentExp = pkDATA->s_nTranscendenceExp;
	DWORD dwRegistMaterialExp = GetRegistMaterialExp();	
	dwCurrentExp += dwRegistMaterialExp;
}

DWORD CDigimonArchiveContents::GetTranscendOriginNeedMoney( cData_PostLoad::sDATA const* pkDATA )
{
	SAFE_POINTER_RETVAL( pkDATA, 0 );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTacticsMng, 0 );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, 0 );

	CsDigimon* pSelDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pkDATA->s_dwBaseDigimonID );
	SAFE_POINTER_RETVAL( pSelDigimon, 0 );

	CsDigimon::sINFO* pSelTInfo = pSelDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pSelTInfo, 0 );

	sTranscendInfo const* pInfos = nsCsFileTable::g_pTacticsMng->GetTranscendInfo( pSelTInfo->s_eEvolutionType, pkDATA->s_HatchLevel);
	SAFE_POINTER_RETVAL( pInfos, 0 );

	return pInfos->dwTransCost;
}

DWORD CDigimonArchiveContents::GetTotalNeedMoney(cData_PostLoad::sDATA const* pkDATA)
{
	SAFE_POINTER_RETVAL( pkDATA, 0 );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTacticsMng, 0 );

	DWORD dwMaxExp = GetNextGrowthMaxExp( pkDATA );
	DWORD dwNeedMoney = GetTranscendOriginNeedMoney( pkDATA );
	if( dwMaxExp >= pkDATA->s_nTranscendenceExp )
		return dwNeedMoney;

	DWORD dwOverExp = pkDATA->s_nTranscendenceExp - dwMaxExp;
	float fDiscountRate = (float)dwOverExp / (float)dwMaxExp;
	if( fDiscountRate > 1.0f )
		fDiscountRate = 1.0f;

	dwNeedMoney -= (dwNeedMoney*fDiscountRate);
	return dwNeedMoney;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 경험치 충전 요청 패킷 
//////////////////////////////////////////////////////////////////////////
bool CDigimonArchiveContents::IsEnableExpCharge( int const& nChargeType )
{
	// 현재 선택된 디지몬은 초월을 할 수 없는 상태인지 검사 함.
	if( !IsEnableTranscendDigimon(mpkSelectedDigimon))
		return false;

	// 재료 아이템이 없다
	if( mTransMaterialInfos.empty() )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_UseAvailableDigimonMaterial );// 초월시 필요한 보조 아이템이 부족한 경우		
		return false;
	}

	std::list<u2> materialDigimonSlotIdx;	
	TransMaterialConstIter it = mTransMaterialInfos.begin();
	for( ; it != mTransMaterialInfos.end(); ++it )
	{
		int nArchiveSlotIdx = (*it).RegisteredDigimonArchiveSlotIdx - 1000;
		DigimonsIter kRegisteredIter =	mDigimons.find(nArchiveSlotIdx);
		if( kRegisteredIter == mDigimons.end() )
			return false;// 디지몬이 없다		

		DigimonInArchive* pMaterial = kRegisteredIter->second;
		SAFE_POINTER_RETVAL( pMaterial, false );
		if( pMaterial->mbIsInIncubator )
			return false;

		if( !pMaterial->mbIsInTranscend )
			return false;

		// 재료로 넣을려는 디지몬이 재료가 될수 있는지 검사함.
		if( !IsEnableTranscend_MaterialDigimon(pMaterial->mpData) )
			return false;// 재료 아이템으로 등록 할 수 없다.

		materialDigimonSlotIdx.push_back( nArchiveSlotIdx );
	}

	int nMaterialCount = materialDigimonSlotIdx.size();
	sSubMaterialInfo NeedSubMaterial = GetSubMaterial( mpkSelectedDigimon->s_dwBaseDigimonID, nChargeType );
	if( NeedSubMaterial.nNeedCount * nMaterialCount > NeedSubMaterial.nHaveCount )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::ItemCountShortage );// 초월시 필요한 보조 아이템이 부족한 경우		
		return false;// 보조 재료 부족
	}
	return true;
}

void CDigimonArchiveContents::_UpdateTranscendenceExp( u1 const& nTacticsPos, u8 const& totalExp )
{
	SAFE_POINTER_RET( g_pDataMng );

	if( 0 == nTacticsPos )
	{
		cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();
		SAFE_POINTER_RET( pPostLoad );
		cData_PostLoad::sDATA* pkDigimonData = pPostLoad->GetDigimonData();
		SAFE_POINTER_RET( pkDigimonData );
		pkDigimonData->s_nTranscendenceExp = totalExp;
	}
	else
	{
		cData_Tactics* pTactics = g_pDataMng->GetTactics();
		SAFE_POINTER_RET( pTactics );
		if( !pTactics->IsTactics( nTacticsPos - 1 ) )
			return;

		cData_PostLoad::sDATA* pkDigimonData = pTactics->GetTactics( nTacticsPos - 1 );
		SAFE_POINTER_RET( pkDigimonData );
		pkDigimonData->s_nTranscendenceExp = totalExp;
	}
}

bool CDigimonArchiveContents::_UpdateTranscendenceHatchLv( u1 const& nTacticsPos, u1 const& nHatchLv )
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );

	if( 0 == nTacticsPos )
	{
		cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();
		SAFE_POINTER_RETVAL( pPostLoad, false );
		cData_PostLoad::sDATA* pkDigimonData = pPostLoad->GetDigimonData();
		SAFE_POINTER_RETVAL( pkDigimonData, false );
		pkDigimonData->s_HatchLevel = nHatchLv;
	}
	else
	{
		cData_Tactics* pTactics = g_pDataMng->GetTactics();
		SAFE_POINTER_RETVAL( pTactics, false );
		if( !pTactics->IsTactics( nTacticsPos - 1 ) )
			return false;

		cData_PostLoad::sDATA* pkDigimonData = pTactics->GetTactics( nTacticsPos - 1 );
		SAFE_POINTER_RETVAL( pkDigimonData, false );
		pkDigimonData->s_HatchLevel = nHatchLv;
	}
	return true;
}

std::string CDigimonArchiveContents::GetDigimonImgFileName(DWORD dwBaseDigimonID)
{
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwBaseDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, "" );
	CsDigimon::sINFO* pDigimonInfo = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonInfo, "" );

	if( 0 != pDigimonInfo->s_dwModelID )
	{
		DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimonModelIdx( pDigimonInfo->s_dwModelID );
		SAFE_POINTER_RETVAL( g_pModelDataMng, "" );
		return g_pModelDataMng->GetLargeModelIconFile( dwModelID );
	}

	return "";
}

std::wstring CDigimonArchiveContents::GetDigimonFileTableName(DWORD dwBaseDigimonID)
{
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwBaseDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, L"" );
	CsDigimon::sINFO* pDigimonInfo = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonInfo, L"" );
	return pDigimonInfo->s_szName;
}

void CDigimonArchiveContents::OnClickTranscendenceHelp()
{
	SAFE_POINTER_RET( g_pGameIF );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRANSCENDENCE_HELP ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_TRANSCENDENCE_HELP );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TRANSCENDENCE_HELP );
}

bool CDigimonArchiveContents::SendExpCharge(int const& nChargeType)
{
	SAFE_POINTER_RETVAL( net::game, false );

	// 현재 선택된 디지몬은 초월을 할 수 없는 상태인지 검사 함.
	if( !IsEnableTranscendDigimon(mpkSelectedDigimon))
		return false;

	// 재료 아이템이 없다
	if( mTransMaterialInfos.empty() )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::ItemCountShortage );// 초월시 필요한 보조 아이템이 부족한 경우		
		return false;
	}

	std::list<u2> materialDigimonSlotIdx;	
	TransMaterialConstIter it = mTransMaterialInfos.begin();
	for( ; it != mTransMaterialInfos.end(); ++it )
	{
		int nArchiveSlotIdx = (*it).RegisteredDigimonArchiveSlotIdx - 1000;
		DigimonsIter kRegisteredIter =	mDigimons.find(nArchiveSlotIdx);
		if( kRegisteredIter == mDigimons.end() )
			return false;// 디지몬이 없다		

		DigimonInArchive* pMaterial = kRegisteredIter->second;
		SAFE_POINTER_RETVAL( pMaterial, false );
		if( pMaterial->mbIsInIncubator )
			return false;

		if( !pMaterial->mbIsInTranscend )
			return false;

		// 재료로 넣을려는 디지몬이 재료가 될수 있는지 검사함.
		if( !IsEnableTranscend_MaterialDigimon(pMaterial->mpData) )
			return false;// 재료 아이템으로 등록 할 수 없다.

		materialDigimonSlotIdx.push_back( nArchiveSlotIdx );
	}

	int nMaterialCount = materialDigimonSlotIdx.size();
	sSubMaterialInfo NeedSubMaterial = GetSubMaterial( mpkSelectedDigimon->s_dwBaseDigimonID, nChargeType );
	if( NeedSubMaterial.nNeedCount * nMaterialCount > NeedSubMaterial.nHaveCount )
	{
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::ItemCountShortage );// 초월시 필요한 보조 아이템이 부족한 경우		
		return false;// 보조 재료 부족
	}

	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );
	std::map<int,cItemData> items;
	pInven->GetItemDatas_ItemType( NeedSubMaterial.dwItemType, NeedSubMaterial.nNeedCount * nMaterialCount, items );

	std::list<stTranscendenceItem> m_lItemList;	
	std::map<int,cItemData>::const_iterator it2 = items.begin();
	for( ; it2 != items.end(); ++it2 )
	{
		stTranscendenceItem addItem;
		addItem.m_u2ItemPos = it2->first;
		addItem.m_u2ItemCount = it2->second.GetCount();
		addItem.m_uItemType = it2->second.m_nType;
		m_lItemList.push_back( addItem );
	}

	bool bIsVipMode = false;
	DWORD dwNpcIDX = 0;
	int nItemInvenPos = 0;
	if( m_pOpenNPC )
	{
		CsNpc* pFTNpc = ( (CNpc*)m_pOpenNPC )->GetFTNpc();
		dwNpcIDX = pFTNpc->GetInfo()->s_dwNpcID;
	}
	else if( m_nPotableIndex != -1 )
		nItemInvenPos = m_nPotableIndex;
	else
		bIsVipMode = true;

	net::game->SendDigimonTranscendenceChargeExp( bIsVipMode, nItemInvenPos, dwNpcIDX, nChargeType, SelectedDigimonIdx, materialDigimonSlotIdx, m_lItemList );
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 경험치 충전 결과 패킷 
//////////////////////////////////////////////////////////////////////////
void CDigimonArchiveContents::RecvExpCharge(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_DigimonTranscendenceChargeEXP* pRecv = static_cast<GS2C_RECV_DigimonTranscendenceChargeEXP*>(pData);
	SetWindowLock(false);

	bool bSuccess = true;
	bool bOverExp = false;
	if( nsDigimonTranscendenceResult::Success != pRecv->m_nResult )
	{
		bSuccess = false;
		cPrintMsg::PrintMsg( pRecv->m_nResult );
		Notify( Update_Viewer );
		ContentsStream kTmp;
		kTmp << bSuccess << bOverExp;
		Notify( Recv_TranscendChargeExpResult, kTmp );	
		return;
	}

	//pRecv->m_u1Flag;							      // 선택 (일반 충전, 고급 충전)
	_UpdateTranscendenceExp( pRecv->m_u1DigimonTranscendencePos, pRecv->m_u8TotalEXP );

	cData_Inven* pInven = g_pDataMng->GetInven();
	std::list< nsItemDel::sItemInfo >::const_iterator itItem = pRecv->m_lItemList.begin();  // 재료 아이템 정보
	for( ; itItem != pRecv->m_lItemList.end(); ++itItem )
		pInven->UpdateInvenItem( (*itItem).u2InvenIdx, (*itItem).itemData, false );

	UnRegistAllTranscend();

	// 재료 디지몬 위치// 지울놈.
	std::list<u2>::const_iterator it = pRecv->m_listDigimonPos.begin();
	for( ; it != pRecv->m_listDigimonPos.end(); ++it )
		RemoveDigimonsInArchive( (*it) + 1000 );

	Notify( Update_Viewer );

	if( pRecv->m_u2SuccessRate > 1 )
		bOverExp = true;

	ContentsStream kTmp;
	kTmp << bSuccess << bOverExp;
	Notify( Recv_TranscendChargeExpResult, kTmp );	

	SetWindowLock( false );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 초월 요청 패킷
//////////////////////////////////////////////////////////////////////////
void CDigimonArchiveContents::SendDigimonTranscendence()
{
	SAFE_POINTER_RET( net::game );

	// 현재 선택된 디지몬은 초월을 할 수 없는 상태인지 검사 함.
	if( !IsEnableTranscendDigimon(mpkSelectedDigimon))
		return;

	DWORD dwCurrentExp = 0;
	DWORD dwMaxExp = 0;
	GetCurrentDigimonMaxNExp( mpkSelectedDigimon, dwCurrentExp, dwMaxExp );
	if( dwCurrentExp < dwMaxExp )
	{// 초월 하려는 디지몬의 경험치 부족
		cPrintMsg::PrintMsg( nsDigimonTranscendenceResult::Not_Exp );
		return ;
	}

	DWORD dwNeedMoney = GetTranscendOriginNeedMoney( mpkSelectedDigimon );

	if( m_bTranscendenceOverExp )
	{
		DWORD dwOverExp = dwCurrentExp - dwMaxExp;
		if( dwOverExp > 0 )
		{
			float fDiscountRate = (float)dwOverExp / (float)dwMaxExp;
			if( fDiscountRate > 1.0f )
				fDiscountRate = 1.0f;
			double dDiscount = DmCS::StringFn::FloatFloor( dwNeedMoney*fDiscountRate );
			dwNeedMoney -= dDiscount;
		}
	}

	sMONEY	myMoney = g_pDataMng->GetInven()->GetMoney();
	u8	MyPrice = myMoney.GetInt();
	if( dwNeedMoney > MyPrice )
	{	// 초월에 필요한 돈이 부족하다.
		cPrintMsg::PrintMsg( 11003 );
		return;
	}

	bool bIsVipMode = false;
	DWORD dwNpcIDX = 0;
	int nItemInvenPos = 0;
	if( m_pOpenNPC )
	{
		CsNpc* pFTNpc = ( (CNpc*)m_pOpenNPC )->GetFTNpc();
		dwNpcIDX = pFTNpc->GetInfo()->s_dwNpcID;
	}
	else if( m_nPotableIndex != -1 )
		nItemInvenPos = m_nPotableIndex;
	else
		bIsVipMode = true;

	net::game->SendDigimonTranscendence( bIsVipMode, nItemInvenPos, dwNpcIDX, SelectedDigimonIdx, dwNeedMoney );
	SetWindowLock(true);
}

//////////////////////////////////////////////////////////////////////////
// 초월 결과 패킷 
//////////////////////////////////////////////////////////////////////////
void CDigimonArchiveContents::RecvDigimonHatchUp(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_DigimonTranscendence * pRecv = static_cast<GS2C_RECV_DigimonTranscendence*>(pData);
	SetWindowLock(false);

	if( nsDigimonTranscendenceResult::Success != pRecv->m_nResult )
	{
		Notify( Update_Viewer );
		cPrintMsg::PrintMsg( pRecv->m_nResult );
		return;
	}

	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->SetMoney( pRecv->m_n8Money, false );
	}
	
	//pRecv->m_u1Flag;							      // 선택 (일반 충전, 고급 충전)
	_UpdateTranscendenceHatchLv( pRecv->m_u1DigimonTranscendencePos, pRecv->m_u1HatchLevel );
	Notify( Update_Viewer );

	DWORD dwBaseDigimonIDX = 0;
	if( 0 == pRecv->m_u1DigimonTranscendencePos )
		dwBaseDigimonIDX = g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID();
	else
		dwBaseDigimonIDX = GetCompanyID( pRecv->m_u1DigimonTranscendencePos );

	if( 0 == dwBaseDigimonIDX )
		return;	

	g_pTacticsAni->Set_bAniProcess(true);	// 부화 애니 시작위치 설정
	g_pTacticsAni->SetTranscendAni( true, GetDigimonImgFileName( dwBaseDigimonIDX ), GetDigimonFileTableName( dwBaseDigimonIDX ) );
	g_pTacticsAni->Start( dwBaseDigimonIDX, g_pCharMng->GetTamerUser() );	// 부화 애니메이션 활성화	
}

//////////////////////////////////////////////////////////////////////////

void CDigimonArchiveContents::SetWindowLock(bool bLock)
{
	ContentsStream ktmp;
	ktmp << !bLock;
	Notify( Set_Window_Lock, ktmp );
}

DWORD CDigimonArchiveContents::GetRegistMaterialExp() const
{
	DWORD dwMaterialTotalExp = 0;
	std::list<sTranscendMaterialInfo>::const_iterator it = mTransMaterialInfos.begin();
	for( ; it != mTransMaterialInfos.end(); ++it )
		dwMaterialTotalExp += (*it).RegisteredExp;

	return dwMaterialTotalExp;
}

bool CDigimonArchiveContents::IsEnableTranscendenceOverExp() const
{
	return m_bTranscendenceOverExp;
}