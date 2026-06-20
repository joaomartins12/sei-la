

#pragma once


#include "nlib/base.h"
#include "pDigimon.h"


class cDigimonSource
{
public:
	cDigimonSource(void);
   ~cDigimonSource(void);

public:
	uint get(uint nAttr);
	void set(uint nAttr, uint nVal);

public:
	uint GetIDX(void) { return m_nIDX; }
	char *GetName(void) { return m_szName; }

public:
	void SetIDX(uint idx) { m_nIDX = idx; }
	void SetName(char *name) { strcpy_s(m_szName, name); }

#ifdef LJS_NEW_ATTRIBUTE_SYSTEM
	bool SetNatual( u1 nPos, u1 nType )
	{
		switch( nPos )
		{
		case 0:
			{
				if( nType > NewAttribute::Fire || nType < NewAttribute::Ice )
				{
					if( nType != NewAttribute::MaxNatualType )				
					{
						return false;
					}
				}
			} break;
		case 1:
			{
				if( nType > NewAttribute::Wood || nType < NewAttribute::Earth )
				{
					if( nType != NewAttribute::MaxNatualType )				
					{
						return false;
					}
				}
			} break;
		case 2:
			{
				if( nType > NewAttribute::Steel || nType < NewAttribute::Light )
				{
					if( nType != NewAttribute::MaxNatualType )				
					{
						return false;
					}
				}
			} break;
		default:
			{
				return false;
			}
		}
		m_nNatual[ nPos ] = nType;
		return true;
	}
	u1 GetNatual( u1 nPos ) { return m_nNatual[ nPos ]; }
#endif

#ifdef HSS_NEW_CROSS_WARS_20130611
	void SetRequireCodeCrown(n4 nCodeCrown) { m_nRequireCodeCrown = nCodeCrown; }
	void SetCrossWarsType(u1 nCrossWarsType) { m_nCrossWarsType = nCrossWarsType; }
	void SetCrossWarsDigimonRank(u1 nDigimonRank) { m_nCrossWarsDigimonRank = nDigimonRank; }
	void SetCrossWarsCrownHeal(n4 nHeal) { m_nCrownHeal = nHeal; }
	bool SetCrossWarsDigimonType(u1 nPos, u4 nDigimonType)
	{
		if ( nLimit::MAX_CROSS_RANK_COUNT <= nPos || 0 > nPos )
		{
			return false;
		}

		m_nCrossWarsDigimonType[nPos] = nDigimonType;
		return true;
	}

	n4 GetRequireCodeCrown(void) { return m_nRequireCodeCrown; }
	u1 GetCrossWarsType(void) { return m_nCrossWarsType; }
	u1 GetCrossWarsDigimonRank(void) { return m_nCrossWarsDigimonRank; }
	n4 GetCrossWarsCrownHeal(void) { return m_nCrownHeal; }
	u4 GetCrossWarsDigimonType(u1 nPos)
	{
		if ( nLimit::MAX_CROSS_RANK_COUNT <= nPos || 0 > nPos )
		{
#ifdef _DEBUG
			DBG(false, "nPos(%d)", nPos);
#endif
			return 0xFFFFFFFF;	// 실패 값
		}

		return m_nCrossWarsDigimonType[nPos];	
	}
#endif

//////////////////////////////////////////////////////
// internal data
private:
	uint m_nIDX;
	char m_szName[64];

private:
	uint m_nAttribute[nDigimon::MaxBaseAttribute];

#ifdef LJS_NEW_ATTRIBUTE_SYSTEM
private:
	u1 m_nNatual[ NewAttribute::NatualExpCount ];
#endif

#ifdef HSS_NEW_CROSS_WARS_20130611
private:
	n4 m_nRequireCodeCrown;	// 코드 크라운 요구 수치
	n4 m_nCrownHeal;		// 코드 크라운 사용량
	u1 m_nCrossWarsType;	//크로스 타입
	u1 m_nCrossWarsDigimonRank;	//디지몬 랭크 // 디지몬 합체를 위해 필요한 숫자
	u4 m_nCrossWarsDigimonType[nLimit::MAX_CROSS_RANK_COUNT];	// 요구되는 디지몬 종류-맥스치 11
#endif

};

class cSkillSource;
typedef struct sNativeSkillInfo
{
	u4 nSkillCode;
	u2 nOpenLevel;
	cSkillSource *pSkillSource;
}sNATIVESKILL, *psNATIVESKILL;

class cDigimonNativeSkillSource
{
private:
	u4 m_nCode;					// 클래스 그룹 코드 
	u2 m_nCount;				// 총 스킬 개수

	sNATIVESKILL m_nNativeSkill[nDigimon::MAX_NATIVE_SINGLE_SKILL_COUNT];	// 클래스 or 패시브 스킬 정보
public:
	cDigimonNativeSkillSource();
	~cDigimonNativeSkillSource();

	void SetGroupCode(u4 nCode) { m_nCode = nCode; }
	void SetSkillInfo(u2 count, psNATIVESKILL data) { m_nCount = count; memcpy(m_nNativeSkill,data,sizeof(m_nNativeSkill)); }
public:
	bool IsClassSkill()	{ return m_nCode > 100 ? true : false; }	//(100 이하 클래스 스킬 /100 이상 패시브 스킬 )
	bool IsOpenLevelCheck(u2 nIndex,u2 nLevel);			// 스킬 오픈 요건 체크
public:
	u4 GetClassGroupCode()	{ return m_nCode; }
	u2 GetSkillCount()		{ return m_nCount; }
	psNATIVESKILL GetNativeSkill(u2 nIndex);
	u4 GetIndexToNativeSkillCode(u2 nIndex);
	cSkillSource *GetIndexToSkillSource(u2 nIndex);
	void CheckSkillOpenState(u2 nLevel, int *nStat);	
};
