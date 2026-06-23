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

namespace
{
	static std::string Md5BinaryToLowerHex(char const* pMd5Bytes, size_t nByteCount)
	{
		static const char* HEX = "0123456789abcdef";

		std::string result;
		result.resize(nByteCount * 2);

		for (size_t i = 0; i < nByteCount; ++i)
		{
			unsigned char value = static_cast<unsigned char>(pMd5Bytes[i]);

			result[i * 2] = HEX[(value >> 4) & 0x0F];
			result[i * 2 + 1] = HEX[value & 0x0F];
		}

		return result;
	}

	static bool IsValidCreateNameA_2_16(char const* name)
	{
		if (name == NULL)
			return false;

		const size_t len = strlen(name);

		if (len < 2 || len > 16)
			return false;

		for (size_t i = 0; i < len; ++i)
		{
			const char c = name[i];

			const bool isNumber = (c >= '0' && c <= '9');
			const bool isUpper = (c >= 'A' && c <= 'Z');
			const bool isLower = (c >= 'a' && c <= 'z');

			if (!isNumber && !isUpper && !isLower)
				return false;
		}

		return true;
	}
}

void cCliGate::SendAccessCode(void)
{
	xassert(net::access_code, "code is null");
	xassert(net::account_idx, "account idx is null");

	newp(pSvr::AccessCode);
	push(net::gate->port ^ net::access_code | net::account_idx);
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
bool cCliGate::SendCreate(nNet::CreateData& data)
{
	/*
	 * O validador antigo Language::CheckTamerName / CheckDigimonName
	 * limita o nome a 8 caracteres.
	 *
	 * Para criação nova, passamos a aceitar:
	 * mínimo 2
	 * máximo 16
	 * apenas letras e números
	 */
	if (!IsValidCreateNameA_2_16(data.m_szTamerName))
	{
		cPrintMsg::PrintMsg(cPrintMsg::CHARCREATE_NAME_ERROR);
		return false;
	}

	if (!IsValidCreateNameA_2_16(data.m_szDigimonName))
	{
		cPrintMsg::PrintMsg(cPrintMsg::CHARCREATE_NAME_ERROR);
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
//     newp(pTamer::Remove);
//     push(nSlotNo);
//     push(eMail);
//     endp(pTamer::Remove);
//     send();
// }

void cCliGate::SendRemove(uint nSlotNo, wchar const* ssn2)
{
	SAFE_POINTER_RET(ssn2);

	newp(pTamer::Remove);

	push(nSlotNo);

	std::string strPass;
	DmCS::StringFn::ToMB(ssn2, strPass);

	/*
		IMPORTANTE:
		O delete character estava a fazer MD5 quando GLOBALDATA_ST.Is2ndPassEncry() vinha true.

		Mas no nosso server atual, a secondary password já é validada no formato normal/texto.
		Se enviarmos MD5 aqui, o server compara:
			DB/SecondPassword = "631998"
		com:
			client = "md5(631998)"
		e responde como password errada.

		Por isso, para este server, enviamos a password exatamente como o jogador escreveu.
	*/

	char log[256] = { 0 };
	sprintf_s(
		log,
		"[CHAR_DELETE][SEND] slot=%u password_len=%d sending_plain_second_password\n",
		nSlotNo,
		(int)strPass.length()
	);
	OutputDebugStringA(log);

	push(strPass);

	endp(pTamer::Remove);
	send();
}

bool cCliGate::SendCheckDoubleName(wchar const* name)
{
	static char s_szName[255] = "";
	static uint s_nLastCheckTime = 0;

	if (name == null)
		return false;

	if (s_nLastCheckTime > nBase::GetTickCount())
	{
		if (stricmp(s_szName, nBase::w2m(name).c_str()) == 0)
		{
			return false;
		}
	}

	s_nLastCheckTime = nBase::GetTickCount() + 3333;

	int n = GetCurrentACP();

	std::wstring TargetName(name);

	newp(pTamer::CheckDoubleName);
	push(TargetName);
	endp(pTamer::CheckDoubleName);
	send();

	return true;
}

void cCliGate::NTF_Send_nProtect_BotDetect(UINT unUniqueIdx, BYTE* byBotPacket, DWORD dwSize)
{
#ifdef DEF_CORE_NPROTECT
	// bool bSendBlock = cClient::IsSendBlock();
	// cClient::SetSendBlock(false);
	//
	// BYTE bPacket[4096] = {0,};
	// memcpy(bPacket, byPacket, dwSize*sizeof(char));
	//
	// newp( pGameTwo::GameGuardCheck );
	// push( unUniqueIdx );
	// push( bPacket,4096 );
	// push( dwSize );
	// endp( pGame::GameGuardCheck);
	// send();
	//
	// cClient::SetSendBlock( bSendBlock );
#endif
}

void cCliGate::ACK_Send_nProtect(UINT unUniqueIdx, BYTE* byPacket, DWORD dwSize)
{
#ifdef DEF_CORE_NPROTECT
	// bool bSendBlock = cClient::IsSendBlock();
	// cClient::SetSendBlock(false);
	//
	// BYTE bPacket[4096] = {0,};
	// memcpy(bPacket, byPacket, dwSize*sizeof(char));
	//
	// newp( pGameTwo::GameGuardCheck );
	// push( unUniqueIdx );
	// push( bPacket,4096 );
	// push( dwSize );
	// endp( pGame::GameGuardCheck);
	// send();
	//
	// cClient::SetSendBlock( bSendBlock );
#endif
}

void cCliGate::NTF_Send_Xigncode(char* pBuffer, SIZE_T BufferSize)
{
#ifdef SDM_DEF_XIGNCODE3_20181107
	// C2GS_SEND_XignCodePacket sendPacket;
	// sendPacket.nSize = BufferSize;
	// memcpy( sendPacket.szPacketData, pBuffer, BufferSize );
	//
	// newp( sendPacket.GetProtocol() );
	// push( sendPacket.szPacketData, BufferSize );
	// push( sendPacket.nSize );
	// endp( sendPacket.GetProtocol() );
	// send();
#endif
}
