

#pragma once

#include "pLimit.h"
#include "nScope.h"
#include "cNpcSource.h"

namespace pQuest
{
	enum
	{
		Begin = nScope::Quest,

		ChangeInfo,		// 퀘스트 정보 변경
						// server->client 몬스터 제거 정보
						// client->server 클라이언트 관련 정보

		Require,		// 요청
		Cancel,			// 취소
		Complete,		// 완료
		Result,			// 결과

		DailyUpdate,	// 일일퀘스트를 초기화 해주세요.
		Achievement,	// 업적

		Canceled,		// 취소됨

		End
	};
};


namespace nQuest
{
	enum
	{
		Error,		// 오류
		None,		// 봉인 상태
		Execute,	// 수행중
		Complete	// 완료
	};

	enum
	{
		MaxCondition =  5,	// 완료 체크시 체크할 조건의 수
		MaxExecute   = 20,	// 한번에 수행할 수 있는 퀘스트 수
	};

	enum
	{
		DigimonMaxCondition = 2,
		DigimonMaxExecute	= 2,
	};

	enum
	{
		TypeSub			= 0,	// 서브 퀘스트
		TypeMain		= 1,	// 메인 퀘스트
		TypeDaily		= 2,	// 1일 퀘스트
		TypeRepeat		= 3,	// 반복형 퀘스트
		TypeRepeat2		= 4,	// 이벤트 반복형 퀘스트
		TypeAchievement	= 5,	// 업적 퀘스트
		TypeCombine		= 6,	// 합체 퀘스트
		TypeTutorial	= 7	// 튜토리얼 퀘스트
	};

	enum eRequireType
	{
		PrecedingQuestComplete  = 0,  // 선행 퀘스트를 완료하였는가?
		TamerLevel              = 1,  // 테이머 레벨
		ItemHold                = 2,  // 아이템을 보유하고 있는가?
		DigimonLevel            = 3,  // 디지몬레벨
		DigimonType             = 4,  // 디지몬타입
		AreaEntry               = 5,  // 영역돌입
		EvoSlot				    = 6,  // 진화슬롯
		NotOpenEvoSlot		    = 7,  // 오픈금지진화슬롯
		DigimonFamily		    = 8,  // 디지몬 계열체
		TamerType			    = 9,  // 테이머 타입
		QuestProcessNotProceed  = 10, // 해당 퀘스트 진행시 진행하지 않음
		QuestCompleteNotProceed = 11, // 해당 퀘스트 완료시 진행하지 않음
	};

	struct sPreSupply	// 퀘스트 진행시 선지급 내용물
	{
		uint m_nType;
		// 아이템 이외의 것은 고민 필요

		uint m_nItemIDX;
		uint m_nItemCount;
	};

	struct sRequire		// 퀘스트 요구 조건
	{
		uint m_nType;	// 0 퀘스트 완료, 1 레벨 충족, 2 아이템
		//32 bytes so  far
		union
		{
			uint m_nQuestIDX;	// 선행 퀘스트 번호
			uint m_nLevel;		// 레벨
			uint m_nItemIDX;	// 보유 아이템 번호			36 ?
//#ifdef LJS_NEW_COMBINE_EVO_2012_09_07
//			uint m_nCurEvo;
//#endif			
		};
		uint m_nItemCount;
	};	

	struct sComplete	// 퀘스트 완료 조건
	{
		// 0 몬스터, 1 아이템, 2 클라이언트 정보, 3 디지몬, 4 npc와의 대화, 5 아이템 사용
		// 6 npc에게 아이템 사용, 7 몬스터에게 아이템 사용, 8 지역에서 아이템 사용, 9 레벨에 도달, 10 디지몬소환
		enum
		{
			MONSTER			= 0, 
			ITEM			= 1, 
			CLIENT			= 2, 
			REGION			= 3, 
			NPCTALK			= 4,									//40
			USE_ITEM		= 5, 
			USE_ITEM_NPC	= 6, 
			USE_ITEM_MONSTER= 7, 
			USE_ITEM_REGION	= 8, 
			T_LEVEL			= 9, 
			D_TYPE			= 10,
		};

		uint m_nType;												//44

		union
		{
			uint m_nMonsterIDX;
			uint m_nItemIDX;										//48
			uint m_nClientInfo;
			uint m_nDigimonIDX;
		};
		uint m_nItemCount;											//56
		uint m_nTargetID;
	};

	struct sResult		// 퀘스트 보상 내용
	{
		uint m_nType;	// 0 money, 1 경험치, 2 아이템	

		uint m_nSelect;	// 지급 방식 0:모두 지급, 1:1중에 택일, 2:2중에 택일, 3:3중에 택일
		union
		{
			uint m_nExp;
			uint m_nMoney;
			uint m_nItemIDX;	// item idx, 경험치양, 돈 양
			uint m_nEvoSlot;	//오픈시킬 슬롯 번호
		};
		uint m_nItemCount;		// item인 경우, 양, 갯수				56+16 - 72
	};

///////////////////////////////////////////////////////////////////////////
// quest source
	class cSource
	{
	public:
		cSource(void);
	   ~cSource(void);

	public:
		void SetIDX(uint nIDX) { m_nIDX = nIDX; }
		uint GetIDX(void) { return m_nIDX; }

	public:
		void InitRequire(uint nNum);
		void InitPreSupply(uint nNum);
		void InitComplete(uint nNum);
		void InitResult(uint nNum);

	///// internal data
	public:
		uint m_nIDX;		// quest idx
		uint m_nType;		// quest type
		char m_szTitle[256];// quest title

	public:
		cNpcSource *m_pNpcStart;	// 퀘스트를 주거나, 보상해주는 NPC
		cNpcSource *m_pNpcEnd;		// 완료시 보상 NPC, 값이 0이면 m_nNpcStart에서 보상 받음

	public:
		uint m_nLevel;		// 테이머 레벨과 비교해서 경험치 등 처리

	public:
		uint m_nRequire;	// 선행조건 (요구조건)
		uint m_nPreSupply;	// 선지급 아이템
		uint m_nComplete;	// 완료조건 (완료조건)
		uint m_nResult;		// 보상     (결과값)
		uint m_nCompleteTarget;	// 퀘스트 완료 타겟, 0:시작 타겟, 1:대상 npc, 2:완료즉시 보상

	public:
		sRequire	*m_pRequire;	// 선행조건
		sPreSupply	*m_pPreSupply;	// 선지급
		sComplete	*m_pComplete;	// 완료조건
		sResult		*m_pResult;		// 보상

		bool		m_bIsReset;		//클리어하자마자 초기화 여부
	};



///////////////////////////////////////////////////////////////////////////
// quest 저장 정보
#pragma pack(push)
#pragma pack(1)
	struct sExeInfo
	{
		u2 m_nIDX; // 0이 아니면 퀘스트 존재함
		u1 m_nCondition[nQuest::MaxCondition];
	};

	class cInfo // 퀘스트 수행중 상태 정보
	{
	public:
		cInfo(void);
	   ~cInfo(void);

	public:
		void on(uint nQuestIDX);
		void off(uint nQuestIDX);

	public:
		void Clear(void) { memset(this, 0, sizeof(cInfo)); }
		uint Check(uint nQuestIDX); // nQuestIDX == 0 인 경우는 return nQuest::Complete;
			// nQuest::Complete	: 완료
			// nQuest::Execute	: 수행
			// nQuest::None		: 봉인
		uint CheckAchieve(uint nQuestIDX );
			// nQuest::Complete	: 완료
			// nQuest::None		: 봉인

		nQuest::sExeInfo &GetExeInfo(int nArrIDX) { return m_pExecute[nArrIDX]; }

	public:
		bool CheatClear(uint nQuestIDX);		// 퀘스트 진행 이전 상태로 되돌림, GM명령에서만 사용됨
		bool CheatComplete(uint nQuestIDX);		// 퀘스트 진행 상태를 완료로 만듦, GM명령에서만 사용됨

		bool Begin(uint nQuestIDX);		// 퀘스트 시작
		bool Cancel(uint nQuestIDX);	// 퀘스트 취소
		bool Complete(uint nQuestIDX);	// 퀘스트 완료
		bool CompleteAchievement(uint nQuestIDX);	// 업적 완료

		bool Clear(uint nQuestIDX){ return CheatClear( nQuestIDX ); }		// 반복퀘, 일일퀘 에서 사용


		// 클라이언트 필요 함수
		nQuest::sExeInfo* GetExeInfo_ByID( u2 nIDX )
		{
			for( int i=0; i<nQuest::MaxExecute; ++i )
			{ if( m_pExecute[ i ].m_nIDX == nIDX ){ return &m_pExecute[ i ]; } }
			return NULL;
		}

		uint GetExecuteCount(void)
		{
			uint nReturn = 0;
			for(uint i=0; i<nQuest::MaxExecute; ++i)
			{
				if(m_pExecute[ i ].m_nIDX)
				{
					++ nReturn;
				}
			}
			 
			return nReturn;
		}
		
		uint* GetQuestCompletionInfo() { return m_nComplete;}
		sExeInfo* GetQuestExecuteInfo() { return m_pExecute; }
	private:
/*		
		uint m_nComplete[nLimit::Quest/32];
		// 전체 퀘스트 중 완료 정보,   32bit * 32ea == 1024 == nLimit::Quest
		// 0번 퀘스트는 사용하지 아니한다.
*/
		uint m_nComplete[nLimit::Quest/32 * nLimit::MaxQuestSetCount];	// 퀘스트 확장으로 증가처리
		
		sExeInfo m_pExecute[nQuest::MaxExecute];
		// 수행중 퀘스트 정보


	public:	// DEBUG PROCESSING
		void OutputDebugInfo(void);
		
	};

	class cDigimonQuestInfo // 퀘스트 수행중 상태 정보
	{
	public:
		cDigimonQuestInfo(void);
		~cDigimonQuestInfo(void);

	public:
		void on(uint nQuestIDX);
		void off(uint nQuestIDX);

	public:
		void Clear(void) { memset(this, 0, sizeof(cDigimonQuestInfo)); }
		uint Check(uint nQuestIDX); // nQuestIDX == 0 인 경우는 return nQuest::Complete;
		// nQuest::Complete	: 완료
		// nQuest::Execute	: 수행
		// nQuest::None		: 봉인
		uint CheckAchieve(uint nQuestIDX );
		// nQuest::Complete	: 완료
		// nQuest::None		: 봉인

		nQuest::sExeInfo &GetExeInfo(int nArrIDX) { return m_pExecute[nArrIDX]; }

	public:
		bool CheatClear(uint nQuestIDX);		// 퀘스트 진행 이전 상태로 되돌림, GM명령에서만 사용됨
		bool CheatComplete(uint nQuestIDX);		// 퀘스트 진행 상태를 완료로 만듦, GM명령에서만 사용됨

		bool Begin(uint nQuestIDX);		// 퀘스트 시작
		bool Cancel(uint nQuestIDX);	// 퀘스트 취소
		bool Complete(uint nQuestIDX);	// 퀘스트 완료
		bool CompleteAchievement(uint nQuestIDX);	// 업적 완료

		bool Clear(uint nQuestIDX){ return CheatClear( nQuestIDX ); }		// 반복퀘, 일일퀘 에서 사용


		// 클라이언트 필요 함수
		nQuest::sExeInfo* GetExeInfo_ByID( u2 nIDX )
		{
			for( int i=0; i<nQuest::DigimonMaxExecute; ++i )
			{ if( m_pExecute[ i ].m_nIDX == nIDX ){ return &m_pExecute[ i ]; } }
			return NULL;
		}

		uint GetExecuteCount(void)
		{
			uint nReturn = 0;
			for(uint i=0; i<nQuest::DigimonMaxExecute; ++i)
			{
				if(m_pExecute[ i ].m_nIDX)
				{
					++ nReturn;
				}
			}
			return nReturn;
		}

	private:
		uint m_nComplete[nLimit::DigimonQuest/32];
		// 전체 퀘스트 중 완료 정보,   32bit * 32ea == 1024 == nLimit::Quest
		// 0번 퀘스트는 사용하지 아니한다.

		sExeInfo m_pExecute[nQuest::DigimonMaxExecute];
		// 수행중 퀘스트 정보


	public:	// DEBUG PROCESSING
		void OutputDebugInfo(void);
	};


#pragma pack(pop)


};







