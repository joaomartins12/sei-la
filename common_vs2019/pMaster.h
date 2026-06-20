

#pragma once


#include "nScope.h"


namespace pMaster
{
	enum
	{
		Start = nScope::Master,

		Jump,					// 순간 이동 처리

		CreateItem,
		CreateMonster,
		CreateMonsterF,

		SetMoveSpeed,
		SetAttackSpeed,			// 사용되지 아니함

		SetMoney,
		SetParameter,			// digimon parameter setting	(사용안함)
		SetParameter2,			// monster parameter setting	(사용안함)

		SetLevel,				// level 강제 변경
		SetForceFirstPosition,	// 시작 위치로 이동				(사용안함)

		QuestOff,				// 퀘스트 미수행 상태로 변경
		QuestOn,				// 퀘스트 수행 완료 상태로 변경

		SetHP,
		SetDS,
		SetFP,
		SetFS,

		MoveMap,
		CloseAllConnection,		// 모든 연결을 종료한다.

		UserInfo,
		UserInfoNotFound,		// 해당 유저가 접속중이지 않거나, 존재하지 않는 유저임
		UserList,
		DiscUser,
		TamerInfo,				// 테이머 이름으로 현재 접속 상태를 알아냄

		MakeInvincible,			// 무적 모드
		SetSkillPoint,			// 스킬 포인트값 갱신	(사용안함)

		SetSkillReadyTime,		// 스킬 시전 시간	(사용안함)
		SetSkillSpeed,			// 스킬 이동 속도	(사용안함)
		SetSkillAcceleration,	// 스킬 이동 가속도 (사용안함)

		QuestReset,				// 퀘스트 초기화
		QuestComplete,			// 퀘스트 완료

		CheatLevel,				// 치트 클래스 변경 0:일반유저, 1:외부관계자, 2:내부관계자 및 운영자, 3:개발자

		MsgAll,					// broadcasting, 해당 맵에 접속중인 사용자 전원 대상
		MsgSystem,				// system message, 해당 클러스터내의 전 인원 대상

		ClearAllSession,		// 해당 클러스터에 붙어있는 모든 세션 제거

		JumpToTamer,			// 대상 테이머 근처 위치로 이동
		JumpToTamerReturn,		// 응답

		ConnCountInfo,			// 맵,채널 별 서버 접속자 수 목록

		BanUser,				// 강제 종료합니다. 처리됨
		DiscTamer,				// 해당 테이머를 접속 종료시킨다. ==> 잠시 접속 종료합니다. 처리됨

		InitSkillStat,			// 스킬 포인트 초기화

		PartnerScale,			// 파트너 용병의 현재 스케일을 임시 조정한다.

		InitPosition,			// 테이머의 위치를 마을로 강제 이동 시킨다.

		InsertItemToShop,		// 캐쉬샵에 아이템을 추가한다.

		Init2ndPass,			// 2차 비밀번호를 초기화한다.

		UserCheatLevel,			// 해당유저의 치트레벨을 변경한다.

		ChargeEventCash,		// 충전 이벤트 캐시

		DeleteAllItem,			// 소지하고 있는 아이템 모두 삭제
		CreateSetItem,			// 세트 아이템 생성

		Visible,				// 투명 모드로 전환, 또는 투명 모드 해제
		Channel,				// 채널 변경
		SlotMax,				// 인벤 / 창고 / 디지몬 보관소 확장
		AttendanceReset,		// 출석체크 리셋
		SetPCBang,				// PC방 설정
		PCBangExpire,			// PC방 만료 설정
		ShopWait,				// 위탁 상점 영업 정지
		Invisible,				// 투망
		LieKillMonster,			// 가상으로 많은 몬스터를 죽인 것처럼
		MonsterHP,				// 몬스터 HP 변경 %
		PlayTime,				// 플레이시간 변경
		Cinfo,					// 캐릭터 정보 로그출력
		CReload,				// 유저 테이머 & 디지몬 Reload
		MasterConn,				// 마스터 치트로 현재 접속자수 확인 요청
		MasterPayCash,			// 마스터 치트로 현재 결제 금액 확인 요청
		GMHatch,				// 디지몬 부화
		GMHatchTry,				// 디지몬 부화 시도 결과
		GMBuff,					// 버프 부여
		PCBangItemClear,		// PC방 아이템 제거
		GMBuffRemove,			// 버프 강제 제거
		GMUserOut,				// 유저 강제로 로그 오프 시킴
		GMServiceChange,		// 게임 서비스 상태 변경
		GMQuest,				// 퀘스트 강제 수신
		GMEnchant,				// 강화
		GMSummon,				// 대상 소환
		GMCoolReSet,			// 쿨타임 리셋
		GmCheckBuff,			// 상태 상태 교정
		GmFriendlyEvent,		// 친구 초대 이벤트 처리
		GmInShop,				// 인 게임샵 아이템 구매
		RareMachineReset,		//레어머신 초기화 기능 
		CheatDeny,				// 코어에서 게임서버에 치트 불가 메세지 전달
		MachineRun,				//레어아이템 나올때까지 돌림
		RankItemRun,			//특정횟수만큼 랭크 아이템 돌림
		ChatBanUser,			//채팅금지유저
		ChatBanClear,			//채팅금지 해제
		TamerMoneySave,			//코어서버 메모리에 있는 테이머 머니 디비에 저장 
		GMGuildlv,				// 길드레벨 업
		GMGuildSkillPoint,		// 길드스킬포인트
		GuildExp,				// 길드명성치(Exp) 감소/증가
		CommissionShopForceClose,	// 위탁상점 강제 닫기
		GMGuildSkillReset,		// 기드 스킬 리셋
		TranscendenceChangeChargeEXP,
				
		End
	};



};


namespace nCheat
{
	enum
	{
		Level0 = 0,	// 일반 유저
		Level1 = 1, // 외부 관계자
		Level2 = 2, // 내부 관계자 및 운영자
		Level3 = 3,	// 개발자
		Level9 = 9	// 모든 치트 불허
	};
};