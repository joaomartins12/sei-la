

#pragma once

#include "nlib/cNetBase.h"
#include "common_vs2019/pLength.h"

extern bool IsNetOff(void);
extern void SetWindowTitle(int nIsOnline, int nMapID=-1);

#include "cCliAccount.h"
#include "cCliGate.h"
#include "cCliGame.h"

void CHAT_LOG(char *msg);
void CHAT_LOG(TCHAR *msg);
#define CHAT(x) CHAT_LOG(x)

namespace Cmd
{
	enum
	{
		None,
		ConnectGameServer,
		ConnectGateServer,
		ConnectAccountServer,
	};
};


namespace net
{
	namespace type
	{
		enum
		{
			game = 10,
			gate,
			account,
			none,
		};
	};


	extern cCliGame *game;
	extern cCliGame *game_tmp;

	extern cCliGate *gate;
	extern cCliGate *gate_tmp;

	extern cCliAccount *account;
	extern cCliAccount *account_tmp;

	extern cClient *net;
	extern bool receive_active;
	extern bool bInitGameDataReady;

	extern char ip[Language::pLength::ip+1];
	extern uint port;	// tmp ip and port
	extern uint cmd;

	extern uint cluster;

// 	extern uint ssn1;
// 	extern uint ssn2;	

	extern uint nIsMoveMapByCommand;

	inline bool IsMoveMapByCommand(void) { return nIsMoveMapByCommand ? true : false; }

	void start(void);
	void end(void);

	void load(void);
	void stop(void);
	void execute(void);
	void exit(uint code);

	bool start(int type, char const*ip, uint port);	


#ifdef DEF_CORE_NPROTECT
	void nProtect_BotDetect( UINT unUniqueIdx, const BYTE* Buffer, DWORD dwBufferSize );
	void nProtect_Send( UINT unUniqueIdx, const BYTE* Buffer, DWORD BufferSize ,DWORD dwGameServerIndex );
	void nProtect_BotDetect();
	void nProtect_Send();
#endif

	void Xigncode_SendPacket(char* Buffer, SIZE_T BufferSize);

	//void chat(char *fmt, ...); // chat 창이 있는 경우에만 출력
	//void chat(int type, char *fmt, ...); // chat 창이 있는 경우에만 출력



//////////////////////////////////////////////////
// current user's info
	extern uint account_idx; // current user's idx
	extern uint access_code; // user access code

	//extern char id[Language::pLength::id + 1]; // current user's id and pass
	//extern char pass[Language::pLength::pass+ 1];
	//extern char secondPass[Language::pLength::pass+ 1];	// 일단 임시로 여기에 저장 시킴
	//extern char MailAddress[Language::pLength::email + 1];	// 유져 메일 주소, string Type

	//extern char szUsrType[8];

	//AERIA
	//extern char Auth_Code[Language::pLength::str*2];				// binary Type
	//extern char cAccessToken[Language::pLength::AccessToken + 1];	// string Type
	//extern char cTicketingtime[MAX_ANAL_LEN];						// string Type
	//extern bool IsAeria;
	extern uint IsLogin;
	//AERIA

// 	extern bool IsWebLoginVersion;
// 
// 	static bool IsWebLogin()
// 	{
// 		return IsWebLoginVersion;		
// 	}	
// 
// 	extern int UsrType;
// 
// 	static bool IsChannelingConnet()
// 	{
// 		switch( UsrType )
// 		{
// 		case nLoginType::PlayNet:	// 플레이넷유저		
// 		case nLoginType::TooNiland:	// 투니랜드유저
// 		case nLoginType::IMBC:		// MBC게임즈 유저
// 		case nLoginType::MGAME:		// MGAME 유저
// 			return true;
// 		}
// 
// 		return false;
// 	}

//////////////////////////////////////////////////
// next map info
	extern uint next_map_no;
	extern uint prev_map_no;
	extern bool bPortalMove;
	extern bool bChangeChannel;

	//extern char account_ip[Language::pLength::ip+1];
	//extern uint account_port;

//////////////////////////////////////////////////
// network state
	extern int  svr_type;		
	//extern bool disable;	// network disable
};




namespace nErr
{
	enum
	{
		Bind = 199,
		Connect,
		Run,
	};
};





