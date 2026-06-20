

#pragma once


#include <math.h>


namespace nSync
{
	// 이동 동기화
	struct Pos // last or target position
	{
		int m_nX;
		int m_nY;
//		int m_nDirect;
//		int m_nTouchTime;

		Pos(void) { }
		Pos(unsigned int x, unsigned int y) { m_nX = x, m_nY = y; }
	   ~Pos(void) { }

		bool operator==(Pos &pos)
		{
			if(m_nX!=pos.m_nX) return false;
			if(m_nY!=pos.m_nY) return false;
			return true;
		}
		bool operator!=(Pos &pos)
		{
			return ( ( (m_nX==pos.m_nX)&&(m_nY==pos.m_nY) ) == false );
		}

		Pos operator-(Pos &pos)
		{
			Pos p(m_nX-pos.m_nX, m_nY-pos.m_nY);
			return p;
		}

		int Length(){ return (int)sqrt( (double)( m_nX*m_nX ) + (double)( m_nY*m_nY ) ); }

	}; // struct Pos

	struct fPos // last or target position
	{
		float x;
		float y;
		//		int m_nDirect;
		//		int m_nTouchTime;

		fPos(void){}
		fPos( float sx, float sy ) { x = sx, y = sy; }
		fPos( Pos pos ){ x = (float)pos.m_nX; y = (float)pos.m_nY; }
		~fPos(void){}
		
		void operator=(const Pos& src){ x = (float)src.m_nX; y = (float)src.m_nY; }
		bool operator==(fPos d)	{ return (x == d.x)&&(y == d.y); }
		bool operator!=(fPos d)	{ return (x != d.x)||(y != d.y); }
		fPos operator-(fPos d)	{ return fPos( x-d.x, y-d.y ); }	
		fPos operator+(fPos d)	{ return fPos( x+d.x, y+d.y ); }
		fPos operator*(float f)	{ return fPos( (x*f), (y*f) ); }
		fPos operator/(float f)	{ return fPos( (x/f), (y/f) ); }
		void operator*=(float f){ x *= f; y *= f; }

		float Length(){ return sqrtf( (float)( x*x ) + (float)( y*y ) ); }
		float Unitize(){
			float fLength = Length();
			if ( fLength > 1e-06f )
			{
				float fRecip = 1.0f / fLength;
				x *= fRecip;
				y *= fRecip;
			}
			else
			{
				x = 0.0f;
				y = 0.0f;
				fLength = 0.0f;
			}
			return fLength;
		}

	}; // struct fPos

}; // namespace nSync



namespace pSync
{
	enum eCommand
	{
		None,

		New,				// 새로 생성해야 할 때
		Delete,				// 포탈, 종료등으로 화면에서 사라진 경우
		In,					// 화면에 들어올때
		Out,				// 화면에서 나갈때

		Walk,				// 이동(걷기)
		Move,				// 이동(뛰기)

		MsgChat,			// chat
		MsgWhisper,			// whisper
		MsgAll,				// broadcast
		MsgSystem,			// system msg

		MsgShout,			// 외치기
		MsgAllByItem,		// 아이템으로 해당 서버 전체 접속자에게 보내는 메세지

		Condition,			// 컨디션 변경시 처리

		PartyBooster,		// party booster값이 변경됨을 core 서버에 알림
		Achievement,		// 업적 달성 - 동기화
		BuffSync,           // 다른 사람 버프 동기화
		State = 0x80,
		MsgGuildShout,		// 길드확성기
		Dummy = 0xffff,		// 쓰이지 않는 값
	}; // enum eCommand
};


namespace nSync
{
	enum eCondition // 
	{
		Ride				= 0x00000001,	// 디지몬을 타다
		Away				= 0x00000002,	// 자리 비움					
		Shop				= 0x00000004,	// 상점 - 테이머일 경우 개인 상점
		ShopReady			= 0x00000008,	// 상점 오픈 준비중

		PCBang				= 0x00000010,	// 오라 체크 pc방 컨디션
		Peace				= 0x00000020,	// 평화선언상태
		s7					= 0x00000040,	// 
		s8					= 0x00000080,	// 

		s9					= 0x00000100,	// 
		Scanner0			= 0x00000200,	// 1:스캐너 장착, 0:스캐너 미장착
		Scanner1			= 0x00000400,	// 1:자신의 소유로 등록된 스캐너, 0:자신의 소유로 등록되지 아니한 스캐너
		Scanner2			= 0x00000800,	// 1:한정판 스캐너, 0:일반판 스캐너

		s13					= 0x00001000,	// 
		s14					= 0x00002000,	// 
		s15					= 0x00004000,	// 
		s16					= 0x00008000,

		s17					= 0x00010000,	// 
		S18					= 0x00020000,	// 
		Guild				= 0x00040000,	// 길드 가입 여부
		S20					= 0x00080000,	// 경험치-레벨 랭커

		S21					= 0x00100000,	// 크기 랭커
		Invisible			= 0x00200000,	// 투명인간
		Fatigue				= 0x00400000,	// FP > HP
		Invinsible			= 0x00800000,	// 무적

		Run					= 0x10000000,	// 뛰기
		Rest				= 0x20000000,	// 휴식중
		Die					= 0x40000000,	// 죽음
		Battle				= 0x80000000,	// 전투중

		Return				= 0x02000000,	// 몬스터가 되돌아가는 상태, 무적과는 좀 다르게 표현

	}; // enum eCondition

	enum eAddExp // 경험치 가속 부스터 condition
	{
		PersnalExp			= 0x00000001,	// 개인 가속부스터 활성화 유무
		p1					= 0x00000002,	// 
		p2					= 0x00000004,	// 
		p3					= 0x00000008,	// 
		p4					= 0x00000010,	// 
		p5					= 0x00000020,	// 
		p6					= 0x00000040,	// 
		p7					= 0x00000080,	//
		PersnalExp_Total	= 0x000000ff,
		
		PartyExp			= 0x00000100,	// 본인이 사용한 파티 가속 부스터 유무 
		Mp1					= 0x00000200,	// 
		Mp2					= 0x00000400,	// 
		Mp3					= 0x00000800,	// 
		Mp4					= 0x00001000,	// 
		Mp5					= 0x00002000,	// 
		Mp6					= 0x00004000,	// 
		Mp7					= 0x00008000,
		PartyExp_Total		= 0x0000ff00,

		Pp1					= 0x00010000,	// 
		Pp2					= 0x00020000,	// 
		Pp3					= 0x00040000,	// 
		Pp4					= 0x00080000,	// 
		Pp5					= 0x00100000,	// 
		Pp6					= 0x00200000,	// 
		Pp7					= 0x00400000,	// 
		Pp8					= 0x00800000,	// 
		Pp_Total			= 0x00ff0000,

		Pe_Macro1			= 0x01000000,	// 매크로 처리시 사용1
		Pe_Macro2			= 0x02000000,	// 매크로 처리시 사용2
		Pp11				= 0x04000000,	// 
		Pp12				= 0x08000000,	// 
		Pp13				= 0x10000000,	// 
		Pp14				= 0x20000000,	// 
		Pp15				= 0x40000000,	// 
		Pp16				= 0x80000000,	// 
		PExt_Total			= 0xff000000,

		AddExp_Total		= 0xffffffff,

	}; // enum eCondition
}; // namespace nSync





