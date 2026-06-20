
#pragma once 

namespace nsBuff
{
	// 스킬코드가 없는 클라이언트 처리용 버프들..
	enum eDefine_Buff{
		BK_DARKTOWER			= 50101,
		BK_EQUIP_FRIENDSHIP		= 50104,
		BK_EQUIP_EXP			= 50105,		
		BK_BARCODE_WHITE		= 50106,  
		BK_BARCODE_BLACK		= 50107, 
		BK_CANDY_SIZE_CHANGE	= 50108, 
		BK_MACRO_EXP			= 50111, 
		BK_MACRO_DEF			= 50112, 
		BK_EQUIP_PC				= 50115,
		BK_PARTY_PC				= 50116,
		BK_PLAYNET				= 50117,
		BK_SPEED_UP				= 51050,	
		BK_BUFF_SPEED			= 40115,
		BK_MONSTER_PLAN_A		= 40161,		// 버프명: 플랜A 효과: 자신과 파티원 이속 잠시 증가. 사용여부 : 해외마다 다름.
#ifdef MONSTER_SKILL_GROWTH
		BK_MONSTER_GROWTH		= 40162,		//	몬스터버프 :: 성장
#endif
	};
}

class cBuffData : public NiMemObject
{
public:
	cBuffData();

public:	
	struct sBUFF_DATA
	{
		CsBuff*		s_pBuffTable;	// 포인터만 들고있음
		UINT		s_nTargetUID;	// 해당버프를 생성한 유저ID
		UINT		s_nEndTime;		// 버프 종료 시간
		UINT		s_nLifeTime;	// 버프 남은 시간
		UINT		s_nBuffCurLV;	// 적용중인 버프레벨		
		u4			s_dwSkillCode;	// 같은 버프코드로 인한 스킬코드 구분을 위해 변수 생성
		UINT		s_nTimeIverse;	// 최초 버프 남은 시간 (s_nLifeTime 과 다름 s_nLifeTime는 Update 되면서 시간이 변경 됨.)
#ifdef MONSTER_SKILL_GROWTH
		int			m_nStack;			//	버프 중첩 수
#endif

	};

	typedef std::list< sBUFF_DATA >							LIST_BUFF;
	typedef std::list< sBUFF_DATA >::iterator				LIST_BUFF_IT;

	enum eUNUSUAL_CONDITION
	{	
		UNUSUAL_NONE			=   0x00000000,		/**< 이상없음 */		
		UNUSUAL_MOVE			=	0x00000001,		/**< 못움직이는상태 */
		UNUSUAL_CHANGE			=	0x00000010,		/**< 진화,교체불가상태 */	
		UNUSUAL_ITEM			=	0x00000100,		/**< 아이템사용불가상태 */			
		UNUSUAL_ATTACK			=	0x00001000,		/**< 어택불능상태 */
	};

protected:
	CsC_AvObject*					m_pObject;

	std::list< sBUFF_DATA >			m_ListBuff; 
	std::list< sBUFF_DATA >			m_ListDeBuff;
	std::list< sBUFF_DATA >			m_ListSystemBuff;

	DWORD							m_dwUnusualCond;

public:
	void	Init(CsC_AvObject* pObject);

	void	Delete();			
	void	Update();	
	CsC_AvObject*	GetObject(){ return m_pObject; }

public:
	void			SetBuff( USHORT nBuffID, UINT nID, UINT nLifeTime, int nLV, u4 dwSkillCode = 0 );		
	void			ReleaseBuff( USHORT nBuffID );
	
	bool			IsUnusualCond( eUNUSUAL_CONDITION eCond ) { return ( ( m_dwUnusualCond & eCond ) != 0 ); }
	DWORD			GetUnusualCond() { return m_dwUnusualCond; }

	std::list< sBUFF_DATA >*		GetBuff(){ return &m_ListBuff; }
	std::list< sBUFF_DATA >*		GetDeBuff(){ return &m_ListDeBuff; }
	std::list< sBUFF_DATA >*		GetSystemBuff(){ return &m_ListSystemBuff; }

	sBUFF_DATA*		GetBuffData( USHORT nID );	
	bool			IsBuffData( USHORT nID );		

protected:
	int				_GetUnusalCondLv();
	void			_SetUnusalCond( int nLv );	
	void			_ReleaseUnusalCond( int nLv );

protected:
	struct sENCY_BUFF_DATA{
		UINT		s_nEndTime;				// 버프 종료 시간
		USHORT		s_nOptIdx;				// 사용중인 덱의 옵션 인덱스
	};

	std::list< sENCY_BUFF_DATA >	m_ListEncyBuff;	// 적용중인 발동 버프 리스트

public:
	typedef std::list< sENCY_BUFF_DATA >			LIST_ENCY_BUFF;
	typedef std::list< sENCY_BUFF_DATA >::iterator	LIST_ENCY_BUFF_IT;

	void			_SetEncyBuff( u2 nIdx, u4 nEndTime );	// 덱 발동 버프 셋
	void			_ReleaseEncyBuff( u2 nIdx );			// 버프 릴리즈
	void			_ReleaseAllEncyBuff();					// 모든 발동 버프 릴리즈

	std::list< sENCY_BUFF_DATA >*	_GetEncyBuff(){ return &m_ListEncyBuff; }

protected:
	bool							m_bIsOnePlayEffect;

public:
	void ClearBuffLoopEffect();		// 파트너 체인지 시 삭제
	void SetOnePlayEffect(bool bFlag){m_bIsOnePlayEffect = bFlag;}

protected:// 스킬 추가 효과(Skill_Apply_2가 존재)와 ID에 대한 이펙트 처리
	void _SetBuffEffect(CsBuff* pBuff, USHORT nBuffID, UINT nTargetUID);
};







