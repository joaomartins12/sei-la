

#include "cDigimonSource.h"



cDigimonSource::cDigimonSource(void)
{
	memset(m_nAttribute, 0, sizeof(m_nAttribute));
}


cDigimonSource::~cDigimonSource(void)
{



}


uint cDigimonSource::get(uint nAttr)
{
	xstop1(nAttr<nDigimon::MaxBaseAttribute, "(attr:%d)", nAttr);
	return m_nAttribute[nAttr];
}


void cDigimonSource::set(uint nAttr, uint nVal)
{
	xstop2(nAttr<nDigimon::MaxBaseAttribute, "(attr:%d, val:%d)", nAttr, nVal);
	m_nAttribute[nAttr] = nVal;
}

#ifdef LJS_NEW_ATTRIBUTTE_SYSTEM
//bool cDigimonSource::SetNatual( u1 nPos, u1 nType )
//{
	/*switch( nPos )
	{
	case 0:
		{
			if( nType > NewAttritude::Fire || nType < NewAttritude::Ice )
			{
				if( nType != NewAttritude::MaxNatualType )				
				{
					return false;
				}
			}
		} break;
	case 1:
		{
			if( nType > NewAttritude::Wood || nType < NewAttritude::Earth )
			{
				if( nType != NewAttritude::MaxNatualType )				
				{
					return false;
				}
			}
		} break;
	case 2:
		{
			if( nType > NewAttritude::Steel || nType < NewAttritude::Light )
			{
				if( nType != NewAttritude::MaxNatualType )				
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
	return true;*/
//}
#endif
//////////////////////////////////////////////////////////////////////////////////////////////
//	-----------------------------------------------------------------------------------------
//	디지몬 고유 스킬 소스
//////////////////////////////////////////////////////////////////////////////////////////////
cDigimonNativeSkillSource::cDigimonNativeSkillSource()
{

}
cDigimonNativeSkillSource::~cDigimonNativeSkillSource()
{

}
bool cDigimonNativeSkillSource::IsOpenLevelCheck(u2 nIndex,u2 nLevel)
{	// 현재 디지몬 클래스 & 패시브 스킬은 오픈 조건이 레벨만 있음 조건 추가시 이곳에 추가함
	if( nIndex >= m_nCount )
	{	// 이미 모든 스킬이 오픈된 경우		
		return false;
	}	

	if( m_nNativeSkill[nIndex].nSkillCode == 0 )
	{	// 스킬 Code 가 0인 경우 오류 일듯
		LOG("NATIVE SKILL CODE IS 0. CLASS CODE:%d INDEX:%d",m_nCode,nIndex);
		return false;
	}

	if( m_nNativeSkill[nIndex].nOpenLevel > nLevel )
	{
		return false;
	}

	return true;
}

psNATIVESKILL cDigimonNativeSkillSource::GetNativeSkill(u2 nIndex)
{
	if( nIndex >= m_nCount )
	{
		return NULL;
	}

	return &m_nNativeSkill[nIndex];
}

u4 cDigimonNativeSkillSource::GetIndexToNativeSkillCode(u2 nIndex)
{
	if( nIndex >= m_nCount )
	{
		return 0;
	}

	return m_nNativeSkill[nIndex].nSkillCode;
}

cSkillSource *cDigimonNativeSkillSource::GetIndexToSkillSource(u2 nIndex)
{
	if( nIndex >= m_nCount )
	{
		return null;
	}

	return m_nNativeSkill[nIndex].pSkillSource;
}

void cDigimonNativeSkillSource::CheckSkillOpenState(u2 nLevel, int *nStat)
{	// 본 함수 호출 이전 nStat는 모두 0으로 초기화 해서 넘기세요.
	if( nStat == NULL )
	{
		return;
	}

	for(int i = 0; i < m_nCount; i++)
	{
		if( nLevel >= m_nNativeSkill[i].nSkillCode == 0 )
		{
			continue;
		}

		if( nLevel < m_nNativeSkill[i].nOpenLevel )
		{			
			continue;			
		}

		nStat[i] = 1;	// 오픈 상태일 경우 1값			
	}
}