#include "stdafx.h"

#include "cCliGate.h"
#include "cNetwork.h"
#include "common_vs2019/pServer.h"

#include "common_vs2019/pGate.h"
#include "common_vs2019/pTamer.h"
#include "common_vs2019/pDigimon.h"
#include "common_vs2019/pSession.h"

#include "nlib/cSHA1.h"
#include "nlib/md5.h"
#include "common_vs2019/pPass2.h"

void cCliGate::SendAccessCode(void)
{
	xassert(net::access_code, "code is null");
	xassert(net::account_idx, "account idx is null");

	newp(pSvr::AccessCode);
		push(net::gate->port^net::access_code|net::account_idx);
		push(net::account_idx);
		push(net::access_code);
	endp(pSvr::AccessCode);
	send();
}

void cCliGate::SendChangeServer(void)
{
	bool bSendBlock = cClient::IsSendBlock();
	cClient::SetSendBlock(false);

	newp(pSvr::Change);
	endp(pSvr::Change);
	DoSend();

	cClient::SetSendBlock(bSendBlock);
}

// net::gate->SendCreate(data);
bool cCliGate::SendCreate(nNet::CreateData &data)
{
	if( Language::SUCCEEDED != Language::CheckTamerName( std::string( data.m_szTamerName ) ) )
	{
		cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_NAME_ERROR );
		 
		return false;
	}

	if( Language::SUCCEEDED != Language::CheckDigimonName( std::string( data.m_szDigimonName ) ) )
	{
		cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_NAME_ERROR );
		 
		return false;
	}

	newp(pTamer::Create);
		push(data);
	endp(pTamer::Create);
	send();

	return true;
}

void cCliGate::SendSelect(uint nSlotNo)
{
	newp(pTamer::Select);
	push(nSlotNo);
	endp(pTamer::Select);
	send();
}


// void cCliGate::SendRemoveGSP(uint nSlotNo, wchar *eMail)
// {
// 	newp(pTamer::Remove);
// 		push(nSlotNo);
// 		push(eMail);
// 	endp(pTamer::Remove);
// 	send();
// }


void cCliGate::SendRemove(uint nSlotNo, wchar const *ssn2)
{
	SAFE_POINTER_RET(ssn2);

	newp(pTamer::Remove);
	push(nSlotNo);

	std::string strPass;
	DmCS::StringFn::ToMB( ssn2, strPass );

	if( GLOBALDATA_ST.Is2ndPassEncry() )
	{
		char LastPass[1024] = {0,};
		md5( const_cast<char*>(strPass.c_str()), LastPass );
		strPass = LastPass;
	}
	
	push(strPass);	

// 	switch( GLOBALDATA_ST.Get2ndPassType() )
// 	{
// 	case GData::eEmail:			push(strPass);		break;
// 	case GData::eStringPass:	push(strPass);		break;
// 	case GData::eAccountPass:	
// 	case GData::e2ndNumberPass:
// 		{
// 			char LastPass[Language::pLength::SecondPass + 1] = {0,};
// 			if( GLOBALDATA_ST.IsCountry( GData::Thailand ) || GLOBALDATA_ST.IsCountry( GData::Aeria ) )
// 				strPass.copy( LastPass, sizeof(char)*Language::pLength::SecondPass );
// 			else
// 				md5( const_cast<char*>(strPass.c_str()), LastPass );
// 			push(LastPass);	
// 		}
// 		break;
// 	}

	endp(pTamer::Remove);
	send();
}

bool cCliGate::SendCheckDoubleName(wchar const *name)
{
	static char s_szName[255] = "";
	static uint s_nLastCheckTime = 0;

	if(name == null)
		return false;

	if(s_nLastCheckTime > nBase::GetTickCount())
	{
		if(stricmp(s_szName, nBase::w2m(name).c_str()) == 0)
		{
			return false;
		}
	}

	s_nLastCheckTime = nBase::GetTickCount() + 3333;

	int n  = GetCurrentACP();
	std::wstring TargetName( name );
	newp(pTamer::CheckDoubleName);
	push(TargetName);
	endp(pTamer::CheckDoubleName);
	send();

	return true;
}

void cCliGate::NTF_Send_nProtect_BotDetect( UINT unUniqueIdx, BYTE* byBotPacket, DWORD dwSize)
{
#ifdef DEF_CORE_NPROTECT
// 	bool bSendBlock = cClient::IsSendBlock();
// 	cClient::SetSendBlock(false);
// 
// 	BYTE bPacket[4096] = {0,};
// 	memcpy(bPacket, byPacket, dwSize*sizeof(char));
// 
// 	newp( pGameTwo::GameGuardCheck );
//  push( unUniqueIdx ); 
// 	push( bPacket,4096 );
// 	push( dwSize );
// 	endp( pGame::GameGuardCheck);
// 	send();
// 
// 	cClient::SetSendBlock( bSendBlock );	
#endif
}

void cCliGate::ACK_Send_nProtect( UINT unUniqueIdx, BYTE* byPacket, DWORD dwSize )
{
#ifdef DEF_CORE_NPROTECT
// 	bool bSendBlock = cClient::IsSendBlock();
// 	cClient::SetSendBlock(false);
// 
// 	BYTE bPacket[4096] = {0,};
// 	memcpy(bPacket, byPacket, dwSize*sizeof(char));
// 
// 	newp( pGameTwo::GameGuardCheck );
//  push( unUniqueIdx ); 
// 	push( bPacket,4096 );
// 	push( dwSize );
// 	endp( pGame::GameGuardCheck);
// 	send();
// 
// 	cClient::SetSendBlock( bSendBlock );	
#endif
}

void cCliGate::NTF_Send_Xigncode( char* pBuffer, SIZE_T BufferSize )
{
#ifdef SDM_DEF_XIGNCODE3_20181107
// 	C2GS_SEND_XignCodePacket sendPacket;
// 	sendPacket.nSize = BufferSize;
// 	memcpy( sendPacket.szPacketData, pBuffer, BufferSize );
// 
// 	newp( sendPacket.GetProtocol() );
// 	push( sendPacket.szPacketData, BufferSize );
// 	push( sendPacket.nSize );
// 	endp( sendPacket.GetProtocol() );
// 	send();
#endif
}


