

#pragma once

#include "nlib/cClient.h"
//#include "cNetwork.h"
#include "common_vs2019/pLength.h"

class cCliAccount : public cClient
{
public:
	cCliAccount(void);
   ~cCliAccount(void);

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
	void RecvLoginWaitTime(void);
	void RecvLoginRequest(void);
	void RecvAccountBan(void);
	void RecvBirthday(void);
	void RecvHandshake487(void);
	//void RecvLoginSuccess(void);
	//void RecvLoginFailure(void);

	void RecvGateInfo(void);
	void RecvClusterList(void);

	//void RecvOnTheAnvil(void);
	void RecvVersionError(void);

	void RecvCheck2ndPass(void);
	void Recv2ndPassChange(void);
	void Recv2ndPassRegister(void);

	// GSP :
	//void RecvLoginFailureGSP(void);

	void Recv_NProtectCheck();
	void Recv_WeekShutDownData();

	void RecvHashValue();
	void RecvSelectCluster();
/////////////////////////////////////////////////////////
// send
public:
	bool SendLogin(char const* pUserID, char const* pUserPassword);
	bool SendLogin();
	bool SendSelectCluster( UINT const& nSendServerIdx );
	void SendReqClusterList(void);

	bool SendRegister2ndPass(char *pass);
	void SendChange2ndPass(char *szPrevPass, char *szNextPass);
	void SendSecondPassCertified(char const*pass);
	void SendSkipSecondPass(void);

	void ACK_Send_nProtect( UINT unUniqueIdx, BYTE* byPacket, DWORD dwSize );
	void NTF_Send_nProtect_BotDetect( UINT unUniqueIdx, BYTE* byBotPacket, DWORD dwSize);

	//////////////////////////////////////////////////////////////////////////
	// Xigncode
	void NTF_Send_Xigncode( char* pBuffer, SIZE_T BufferSize );
	void Recv_XignCodeCheck(void);
	//////////////////////////////////////////////////////////////////////////

private:
	bool m_bDisable;
};









