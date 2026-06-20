#pragma once

struct stRELOCATE_REQUEST
{
	WORD		wType;			// 패킷 타입

	INT64 		idxUser;		// 웹 사용자 인덱스
	INT			iOldServer;		// 기존 서버 코드
	INT64		iOldTamerIdx;	// 기존 테이머 인덱스
	WCHAR		szTamerName[32];	// 기존 테이머명

	INT			iNewServer;		// 이전 할 서버 코드
	WCHAR		szNewName[32];	// 이전 할 서버의 테이머명
};

struct stRELOCATE_NAME_CHECK
{
	WORD		wType;			// 패킷 타입

	INT			iNewServer;		// 이전 할 서버 코드
	WCHAR		szNewName[32];	// 이전 할 서버의 테이머명
};