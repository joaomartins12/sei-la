

#pragma once 

namespace nsATTRIBUTE
{
	enum eNATURE{
		NAT_None		= 0,
		NAT_Fire		= 16,
		NAT_Thunder		= 17,
		NAT_Steel		= 18, 
		NAT_Wind		= 19,
		NAT_Wood		= 20,
		NAT_Water		= 21,
		NAT_Darkness	= 22,
		NAT_Earth		= 23,
		NAT_Ice			= 24,
		NAT_Light		= 25,
	};
}

class AdaptMainFrame;

class cSkill
{
public:
	enum eSCRIPTSKILL_TYPE{ NONE, TACTICSHOUSE, JUMPBOOSTER, };	// 아이템 사용형 스크립트 스킬 enum값
	struct sINFO : public NiMemObject
	{
		static sINFO*	NewInstance(){ return NiNew sINFO; }
		static void		DeleteInstance( sINFO* pInfo ){ NiDelete pInfo; }

		sINFO():fUseSkill(NULL),s_pFTSkill(NULL),s_eAnimation(ANI::INVALIDE_ID)
			,s_nLifeTime(0), s_nTacticsIndex(INT_MAX)
		{}

		CsSkill*		s_pFTSkill;
		ANI::eTYPE		s_eAnimation;
		CsCoolTimeSeq	s_CoolTime;
		u4				s_nLifeTime;
		u4				s_nTacticsIndex;
		// 스킬 예외 처리를 하기 위한 함수 포인터
		void			(cSkill::*fUseSkill)( sINFO* pSkillInfo );
	};

	CsVectorPB< sINFO* >	m_vpInfo;

protected:
	CsC_AvObject*			m_pParent;	
	std::map<int, u4>		m_mTamerSkill;	// 아이템 사용형 스크립트 스킬 사용 시 정보 저장
	int						m_nSkillIndex;	// 스킬 인덱스
public:
	cSkill();
	~cSkill();

	void			Delete();

	void			SetTamer( CsC_AvObject* pParent, CsTamer* pTamerFT );
	void			SetDigimon( CsC_AvObject* pParent, CsDigimon* pDigimonTB );
	void			__UseSkill( int nSkilIndex );

	CsSkill*		GetFTSkill( int nSkillIndex );
	CsSkill*		GetFTSkill( uint nFTDigimonID, int nSkillIndex );

	bool			IsSkill( int nSkillIndex );
	sINFO*			GetSkill( int nSkillIndex );
	int				GetSkillCount(){ return m_vpInfo.Size(); }
	void			SetSkillCooltime( u4 nSkillCode, u4 nCoolTime );

	bool			IsUseSkill( CsC_AvObject* pParent, int nSkillIndex );
	bool			CheckTamerToUseSkill();

public:
	sINFO*			GetTamerActiveSkill( int nIndex );
	int				GetTamerActiveSkillIndex( int nIndex );
	sINFO*			GetTamerPasiveSkill( int nIndex );
	int				GetTamerPasiveSkillIndex( int nIndex );
	sINFO*			GetTamerEmotionSkill( int nIndex );
	int				GetTamerEmotionSkillIndex( int nIndex );
	int				GetTamerSkill_Index( u4 nSkillCode );
	void			SetTamerSkill_Ani( int nTamerSqID, int nDigimonSqID );	// 테이머 시퀀스, 디지몬 시퀀스 ID	
	int				GetSkill_Inex() { return m_nSkillIndex; }
	void			SetSkill_Index( int nIndex ) { m_nSkillIndex = nIndex; }

	void				SetTamerSkillMap( int nWindow, u4 nSkillCode ) { m_mTamerSkill[nWindow] = nSkillCode; }
	std::map<int, u4>*	GetTamerSkillMap() { return &m_mTamerSkill; }

	void			_Tamer_Skill_UseCheck( int nAvailable, int nSkillIndex, TCHAR* rSkillName );

	void			SetTamerCashSkill( u4 nIndex, u4 nCoolTime, u4 nLifeTime );		// 테이머 Cash 스킬 설정
	void			DelTamerCashSkill( DWORD dwSkill_Index );						// 테이머 Cash 스킬 삭제
	int				CountCashSkill();												// 테이머 Cash 스킬 카운트 리턴

	void			SetDigimonCashSkill( u4 nSkillCode, u4 nCoolTime = 0);
	void			AddDigimonCashSkill( u4 nSkillCode, u4 nCoolTime = 0);
	void			DelDigimonCashSkill( u4 nSkillCode );
	SHORT			GetMySkill_Index( u4 nSkillCode );

protected:
	void			_Target_My_All_Digimon( int nSkilIndex );
	void			_Target_One_Enemy( int nSkilIndex );
	void			_Target_Tamer( int nSkilIndex );
	void			_Target_My_All( int nSkilIndex );
	void			_Target_My_Tamer( int nSkilIndex );
	void			_Target_My_Partner_Digimon( int nSkilIndex );

	void			_Target_Another_All( int nSkillIndex );

	void			_Target_My_Partner_Around( int nSkilIndex );

	void			_Target_One_Digimon_Party( int nSkilIndex );	// 자신의 파티에 속한 디지몬 한개체
	void			_Target_All_Digimon_Party( int nSkilIndex );	// 자신의 파티에 속한 디지몬 전체

	// 스킬 예외 처리
protected:
	bool			__IsAttackObject( CsC_AvObject* pTarget );

	void			__Wait_Digimon( sINFO* pSkillInfo );
	void			__Follow_Digimon( sINFO* pSkillInfo );
	void			__Attack_Digimon( sINFO* pSkillInfo );
	void			__Pickup_Digitama( sINFO* pSkillInfo );
		
// 	void			__BattleTag_Digimon1( sINFO* pSkillInfo );
// 	void			__BattleTag_Digimon2( sINFO* pSkillInfo );
public:
//	void			__BattleTag_Digimon3( sINFO* pSkillInfo );	//디지몬 교체 스킬.. 디지몬슬롯 추가로 하나 더 만듬

	void			__BattleTag_Digimon( sINFO* pSkillInfo );

	void			__Away( sINFO* pSkillInfo );

	void			__Normal_Attack_Monster( sINFO* pInfo ) { __Attack_Digimon( pInfo ); }

private:
	TOwnershipGuard< AdaptMainFrame >		m_AdaptMainFrame;
};