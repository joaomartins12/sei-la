#include "StdAfx.h"
#include "ChannelContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptCommunity.h"

int const ChannelContents::IsContentsIdentity(void)
{
	return E_CT_CHANELLSELECT;
}

ChannelContents::ChannelContents(void)
{
	//각 채널의 상태 정보 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHANNEL_LIST_INFO, this, &ChannelContents::ReceiveChannel);
	//현재 접속 채널 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CURRENT_CHANNELINDEX, this, &ChannelContents::ReceiveChannelIndex);
	//채널정보 참조
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_CHANNELINFO, this, &ChannelContents::GetChannelInfo);
	//선택 채널 이동 확인 팝업
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_CHANGE_CHANNEL_POPUP, this, &ChannelContents::PopupChangeChannel);
	//채널 이동 정보 채팅창 표시
	//GAME_EVENT_ST.AddEvent( EVENT_CODE::DISPLAY_CHANGE_CHANNEL, this, &ChannelContents::DisplayChangeChannel);
	//채널 이동 실패 해당 채널 폐쇄
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CHANGE_CHANNEL_FAIL, this, &ChannelContents::ChangeFail );
	//채널 변경 패킷 전송 및 채팅 메세지 세트
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_CHANGE_CHANNEL, this, &ChannelContents::SendChangeChannel );
	//SEND_CHANGE_CHANNEL 동일 GM에 의한 호출
	//GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_CHANGE_CHANNEL_BYMASTER, this, &ChannelContents::SendChangeChannelByMaster );
}

ChannelContents::~ChannelContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const ChannelContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool ChannelContents::Initialize(void)
{

	return true;
}

void ChannelContents::UnInitialize(void)
{
}

bool ChannelContents::IntraConnection(ContentsSystem& System)
{
	return true; 
}

void ChannelContents::Update(float const& fElapsedTime)
{
}

void ChannelContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void ChannelContents::MakeWorldData(void)
{
	channelInfo.ResetData();
}

void ChannelContents::ClearWorldData(void)
{
	channelInfo.ResetData();
}

void ChannelContents::MakeMainActorData(void)
{	
	RequestChannelList();
}

void ChannelContents::ClearMainActorData(void)
{
	channelInfo.ResetData();	

	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CHANNEL );
}

bool ChannelContents::RequestChannelList(void)
{
	SAFE_POINTER_RETVAL( net::game, false );
	net::game->SendChannelInfo();
	return true;
}

void ChannelContents::ReceiveChannel(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CHANNEL_LISTINFO* pCastData = static_cast<GS2C_RECV_CHANNEL_LISTINFO*>(pData);

	memset(channelInfo.channel, 0xFF, sizeof(channelInfo.channel));
	bool bIsMakeChannelWindow = false;
	for( int i = 0; i < nLimit::Channel; ++i)
	{
		channelInfo.channel[i] = pCastData->channel[i];
		if( channelInfo.channel[ i ] != 0xFF || channelInfo.channel[ i ] != -1 )
			bIsMakeChannelWindow = true;
	}

	if( bIsMakeChannelWindow )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CHANNEL ) )
			Notify( eSpreadList );
		else
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_CHANNEL );
	}
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CHANNEL );

}
ChannelContents::CHANNELINFO const & ChannelContents::GetChannelInfo()
{
	return channelInfo;
}

void ChannelContents::ReceiveChannelIndex(void* pData)
{
	SAFE_POINTER_RET(pData);
	// Source is GS2C_RECV_CURRENT_CHANNEL::channel_idx, which is n2 (2 bytes)
	// when COMPAT_487 is defined and uint (4 bytes) otherwise. Cast accordingly
	// — reading 4 bytes from a 2-byte field returns adjacent stack garbage,
	// which appears in the dropdown's selected-text as a huge number.
#ifdef COMPAT_487
	short* pCastData = static_cast<short*>(pData);
#else
	int* pCastData = static_cast<int*>(pData);
#endif
	channelInfo.channel_idx = static_cast<unsigned int>(*pCastData);

	Notify( eSetChannelTxt );
}

void ChannelContents::GetChannelInfo(void* pData)
{
	SAFE_POINTER_RET(pData);
	CHANNELINFO* pGetData = static_cast<CHANNELINFO*>(pData);

	*pGetData = channelInfo;
}

void ChannelContents::PopupChangeChannel(void* pData)
{
	SAFE_POINTER_RET(pData);
	int nActive1 = *static_cast<int*>(pData);

	// 전투 중인지체크
	if( null != g_pCharMng->GetTamerUser() && g_pCharMng->GetTamerUser()->IsBattle() == true )
	{
		cPrintMsg::PrintMsg( 16002 );
		return;
	}

	if( channelInfo.channel_idx != nActive1 )
	{
#ifdef CHANGE_CHANNEL_LIMIT
		if( channelInfo.channel[ nActive1 ] >= 20 )	// 포화 상태
		{
			cPrintMsg::PrintMsg( 10016 );	// 채널 변경을 할 수 없습니다.\n 잠시후에 다시 시도해 주시기 바랍니다.
		}
#endif
		TCHAR sz[ 32 ];
		_stprintf_s( sz, 32, _T( "%d " ), nActive1 );
		cPrintMsg::PrintMsg( 10017, sz );
		cMessageBox* pMsgBox = cMessageBox::GetMessageBox( 10017 );
		if( pMsgBox )
			pMsgBox->SetValue1( nActive1 );
	}	
}
void ChannelContents::DisplayChangeChannel(void* pData)
{
	char msg[222];
	for(int i=0; i<nLimit::Channel; i++)
	{
		if(channelInfo.channel[i])
		{
			sprintf(msg, " - - 채널 [%d] 가동중", i);
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
			CProtocol.m_wStr = nBase::m2w(msg);
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}
	}

	sprintf(msg, "현재 [%d]번째 채널에 접속중", channelInfo.channel_idx);
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = nBase::m2w(msg);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

unsigned int const ChannelContents::GetCurrentChannelIdx() const
{
	return channelInfo.channel_idx;
}
void ChannelContents::ChangeFail(void* pData)
{
	channelInfo.channel[ channelInfo.channel_Tryidx ] = (char)0xff;
}
void ChannelContents::SendChangeChannel(void* pData)
{
	SAFE_POINTER_RET(pData);
	int nChannelIDX = *static_cast<int*>(pData);
	if( net::game != null && Set_ChannelTryIndexNChat(nChannelIDX) )
		net::game->SendChangeChannel(nChannelIDX);
}
void ChannelContents::SendChangeChannelByMaster(void* pData)
{
// 	SAFE_POINTER_RET(pData);
// 	int nChannelIDX = *static_cast<int*>(pData);
// 	if( net::game != null && Set_ChannelTryIndexNChat(nChannelIDX) )
// 		net::game->SendChangeChannelByMaster(nChannelIDX);
}
bool ChannelContents::Set_ChannelTryIndexNChat(int nChannelIDX)
{
	assert( channelInfo.channel_idx != nChannelIDX );
	if(net::game == null || g_pGameIF == null || nChannelIDX < 0)
	{
		return false;
	}
	if( net::game->m_bPortalRequesting )
	{		
		return false;
	}
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
	{
		cPrintMsg::PrintMsg( 30028 );
		return false;
	}
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
	{
		cPrintMsg::PrintMsg( 30357 );
		return false;
	}

	if(nChannelIDX >= nLimit::Channel)
	{
		DisplayChangeChannel(null);
		return false;
	}

#ifndef _GIVE
	char msg[222];
	sprintf(msg, "[%d] 번째 채널로 이동합니다.", nChannelIDX);
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = nBase::m2w(msg);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
#endif

	net::bChangeChannel = true;
	channelInfo.channel_Tryidx = nChannelIDX;

	ContentsStream kStream;
	kStream << nChannelIDX;
	NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_Guild_Channel, kStream );
	return true;
}