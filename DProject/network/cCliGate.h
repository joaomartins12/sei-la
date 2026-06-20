


#pragma once


#include "nlib/cClient.h"
//#include "cNetwork.h"

#include "common_vs2019/pLength.h"
#include "common_vs2019/pNetwork.h"

#include "common_vs2019/pTamer.h"
#include "common_vs2019/pDigimon.h"

class cCliGate : public cClient
{
public:
	cCliGate(void);
   ~cCliGate(void);

public:
	bool start(void);

public:
	void OnConnected(void);
	void OnExecute(void);
	void OnDisconnected(char *msg);


public:
	char ip[Language::pLength::ip];
	uint port;
	uint m_nIsChangeServer;

	
/////////////////////////////////////////////////////////
// receive
private:
	void RecvChangeServer(void);
	void RecvTamerList(void);

	void RecvTamerCreateSuccess(void);
	void RecvTamerCreateFailure(void);

	void RecvTamerSelectSuccess(void);
	void RecvTamerSelectFailure(void);

	void RecvCheckDoubleName(void);
	void RecvBanUser(void);
	void RecvDiscUser(void);
	void RecvRemoveTamer(void);
	
	void RecvTamerCreateWaitTime(void);	// 테이머 생성시 대기 시간이 필요한 경우
	void RecvHandshake487(void);
#ifdef SERVER_WAITING_LINE
	void RecvWaitTime(void);
#endif

	void Recv_NProtectCheck();
/////////////////////////////////////////////////////////
// send
public:
	void SendAccessCode(void);
	void SendChangeServer(void);

	bool SendCreate(nNet::CreateData &data);
	void SendSelect(uint nSlotNo);
	void SendRemove(uint nSlotNo, wchar const*ssn2); // ssn2: 주민번호 뒷자리
	//void SendRemoveGSP(uint nSlotNo, wchar *eMail);


	bool SendCheckDoubleName(wchar const *name);

	void ACK_Send_nProtect( UINT unUniqueIdx, BYTE* byPacket, DWORD dwSize );
	void NTF_Send_nProtect_BotDetect( UINT unUniqueIdx, BYTE* byBotPacket, DWORD dwSize);

/////////////////////////////////////////////////////////
// send debug
public:

	//////////////////////////////////////////////////////////////////////////
	// Xigncode
	void NTF_Send_Xigncode( char* pBuffer, SIZE_T BufferSize );
	void Recv_XignCodeCheck(void);
	//////////////////////////////////////////////////////////////////////////


};



