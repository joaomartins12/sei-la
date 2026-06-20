#include "stdafx.h"
#include "Buff.h"

cBuffData::cBuffData()
{
}

void cBuffData::Init(CsC_AvObject* pObject)
{
	m_pObject = pObject;
	m_dwUnusualCond = UNUSUAL_NONE;
	m_bIsOnePlayEffect = false;
}

void cBuffData::Delete()
{
	m_ListBuff.clear();
	m_ListDeBuff.clear();
	m_ListSystemBuff.clear();
	m_ListEncyBuff.clear();
	m_dwUnusualCond = UNUSUAL_NONE;
}

void cBuffData::SetBuff(USHORT nBuffID, UINT nID, UINT nLifeTime, int nLV, u4 dwSkillCode)
{
	if (!nsCsFileTable::g_pBuffMng)
		return;

	CsBuff* pBuff = nsCsFileTable::g_pBuffMng->GetBuff(nBuffID);

	if (!pBuff)
		return;

	if (!pBuff->GetInfo())
		return;

	sBUFF_DATA pInfo;
	pInfo.s_pBuffTable = pBuff;
	pInfo.s_nTargetUID = nID;
	pInfo.s_nBuffCurLV = nLV;
	pInfo.s_nEndTime = (nLifeTime == UINT_MAX) ? UINT_MAX : nLifeTime;
	pInfo.s_nLifeTime = (nLifeTime == UINT_MAX) ? UINT_MAX : nLifeTime - _TIME_TS;
	pInfo.s_dwSkillCode = dwSkillCode;
	pInfo.s_nTimeIverse = pInfo.s_nLifeTime;

#ifdef MONSTER_SKILL_GROWTH
	pInfo.m_nStack = 1;
#endif

	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;
	bool bInsert = false;

	switch (pBuff->GetInfo()->s_nBuffType)
	{
	case 1:
	{
		it = m_ListBuff.begin();
		itEnd = m_ListBuff.end();

		for (; it != itEnd; it++)
		{
			if (pInfo.s_nEndTime < it->s_nEndTime)
			{
				m_ListBuff.insert(it, pInfo);
				bInsert = true;
				break;
			}
		}

		if (bInsert == false)
			m_ListBuff.push_back(pInfo);
	}
	break;

	case 2:
	{
		it = m_ListDeBuff.begin();
		itEnd = m_ListDeBuff.end();

		for (; it != itEnd; it++)
		{
			if (pInfo.s_nEndTime < it->s_nEndTime)
			{
				m_ListDeBuff.insert(it, pInfo);
				bInsert = true;
				break;
			}
		}

		if (bInsert == false)
			m_ListDeBuff.push_back(pInfo);
	}
	break;

	case 3:
	{
		it = m_ListSystemBuff.begin();
		itEnd = m_ListSystemBuff.end();

		for (; it != itEnd; it++)
		{
			if (pInfo.s_nEndTime < it->s_nEndTime)
			{
				m_ListSystemBuff.insert(it, pInfo);
				bInsert = true;
				break;
			}
		}

		if (bInsert == false)
			m_ListSystemBuff.push_back(pInfo);
	}
	break;

	default:
		return;
	}

	if (pBuff->GetInfo()->s_szEffectFile[0] != NULL)
	{
		if (m_pObject && m_pObject->GetProp_Effect())
		{
			char cEffectPath[MAX_PATH] = { 0, };
			sprintf_s(cEffectPath, MAX_PATH, "System\\Buff\\%s.nif", pBuff->GetInfo()->s_szEffectFile);
			m_pObject->GetProp_Effect()->AddEffect_FT(cEffectPath);
		}
	}

	_SetUnusalCond(_GetUnusalCondLv());
	_SetBuffEffect(pBuff, nBuffID, nID);
}

void cBuffData::ReleaseBuff(USHORT nBuffID)
{
	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;

	DWORD dwTargetUID = 0;

	it = m_ListBuff.begin();
	itEnd = m_ListBuff.end();
	for (; it != itEnd; it++)
	{
		if (it->s_pBuffTable && it->s_pBuffTable->GetInfo() && it->s_pBuffTable->GetInfo()->s_dwID == nBuffID)
		{
			dwTargetUID = it->s_nTargetUID;
			m_ListBuff.erase(it);
			break;
		}
	}

	it = m_ListDeBuff.begin();
	itEnd = m_ListDeBuff.end();
	for (; it != itEnd; it++)
	{
		if (it->s_pBuffTable && it->s_pBuffTable->GetInfo() && it->s_pBuffTable->GetInfo()->s_dwID == nBuffID)
		{
			dwTargetUID = it->s_nTargetUID;
			m_ListDeBuff.erase(it);
			break;
		}
	}

	it = m_ListSystemBuff.begin();
	itEnd = m_ListSystemBuff.end();
	for (; it != itEnd; it++)
	{
		if (it->s_pBuffTable && it->s_pBuffTable->GetInfo() && it->s_pBuffTable->GetInfo()->s_dwID == nBuffID)
		{
			m_ListSystemBuff.erase(it);
			break;
		}
	}

	CTamer* tempTamer = dynamic_cast<CTamer*>(m_pObject);
	if (tempTamer)
	{
		if (!tempTamer->IsRide())
			_ReleaseUnusalCond(_GetUnusalCondLv());
	}
	else
	{
		_ReleaseUnusalCond(_GetUnusalCondLv());
	}

	if (dwTargetUID != 0)
	{
		CsC_AvObject* pTarget = g_pMngCollector->GetObject(dwTargetUID);

		if (!pTarget)
			return;

		if (pTarget->GetProp_Effect() && pTarget->GetProp_Effect()->DeleteBuffLoopEffect(nBuffID))
		{
			ST_CHAT_PROTOCOL CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("Delete LoopEffect BuffID : %d / TargetID : %d"), nBuffID, dwTargetUID);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		}

		if (g_pSEffectMgr && g_pSEffectMgr->DeleteSEffect(pTarget, nBuffID))
		{
			ST_CHAT_PROTOCOL CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("Delete SEffect Shading TargetID : %d"), dwTargetUID);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		}
	}
}

cBuffData::sBUFF_DATA* cBuffData::GetBuffData(USHORT nID)
{
	LIST_BUFF_IT it;
	LIST_BUFF_IT itEnd;

	it = m_ListBuff.begin();
	itEnd = m_ListBuff.end();
	for (; it != itEnd; it++)
	{
		if (it->s_pBuffTable && it->s_pBuffTable->GetInfo() && it->s_pBuffTable->GetInfo()->s_dwID == nID)
			return &(*it);
	}

	it = m_ListDeBuff.begin();
	itEnd = m_ListDeBuff.end();
	for (; it != itEnd; it++)
	{
		if (it->s_pBuffTable && it->s_pBuffTable->GetInfo() && it->s_pBuffTable->GetInfo()->s_dwID == nID)
			return &(*it);
	}

	it = m_ListSystemBuff.begin();
	itEnd = m_ListSystemBuff.end();
	for (; it != itEnd; it++)
	{
		if (it->s_pBuffTable && it->s_pBuffTable->GetInfo() && it->s_pBuffTable->GetInfo()->s_dwID == nID)
			return &(*it);
	}

	return NULL;
}

int cBuffData::_GetUnusalCondLv()
{
	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;
	CsBuff* pBuff;
	int nLv = 0;

	it = m_ListDeBuff.begin();
	itEnd = m_ListDeBuff.end();

	for (; it != itEnd; it++)
	{
		pBuff = it->s_pBuffTable;

		if (!pBuff || !pBuff->GetInfo())
			continue;

		if (pBuff->GetInfo()->s_dwSkillCode == 0)
			continue;

		if (!nsCsFileTable::g_pSkillMng)
			continue;

		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill(pBuff->GetInfo()->s_dwSkillCode);
		if (!pSkill || !pSkill->GetInfo())
			continue;

		for (int i = 0; i < SKILL_APPLY_MAX_COUNT; i++)
		{
			CsSkill::sINFO::sAPPLY pSkillApply = pSkill->GetInfo()->s_Apply[i];

			if (pSkillApply.s_nID == 107 && pBuff->GetInfo()->s_nConditionLv > nLv)
				nLv = pBuff->GetInfo()->s_nConditionLv;
		}
	}

	return nLv;
}

void cBuffData::_SetUnusalCond(int nLv)
{
	if (!m_pObject)
		return;

	if (nLv >= 1)
	{
		m_pObject->DeletePath();
		m_pObject->SetPause(CsC_AvObject::PAUSE_PATH, true);
		m_dwUnusualCond = UNUSUAL_MOVE | UNUSUAL_CHANGE;
	}

	if (nLv >= 2)
		m_dwUnusualCond |= UNUSUAL_ITEM;

	if (nLv >= 3)
		m_dwUnusualCond |= UNUSUAL_ATTACK;
}

void cBuffData::_ReleaseUnusalCond(int nLv)
{
	if (!m_pObject)
		return;

	m_pObject->SetPause(CsC_AvObject::PAUSE_PATH, false);

	_SetUnusalCond(nLv);
}

bool cBuffData::IsBuffData(USHORT nID)
{
	LIST_BUFF_IT it = m_ListBuff.begin();
	LIST_BUFF_IT itEnd = m_ListBuff.end();

	for (; it != itEnd; it++)
	{
		if (it->s_pBuffTable && it->s_pBuffTable->GetInfo() && it->s_pBuffTable->GetInfo()->s_dwID == nID)
			return true;
	}

	it = m_ListDeBuff.begin();
	itEnd = m_ListDeBuff.end();

	for (; it != itEnd; it++)
	{
		if (it->s_pBuffTable && it->s_pBuffTable->GetInfo() && it->s_pBuffTable->GetInfo()->s_dwID == nID)
			return true;
	}

	it = m_ListSystemBuff.begin();
	itEnd = m_ListSystemBuff.end();

	for (; it != itEnd; it++)
	{
		if (it->s_pBuffTable && it->s_pBuffTable->GetInfo() && it->s_pBuffTable->GetInfo()->s_dwID == nID)
			return true;
	}

	return false;
}

void cBuffData::Update()
{
	if (m_ListEncyBuff.size() != 0)
	{
		LIST_ENCY_BUFF_IT itEncy = m_ListEncyBuff.begin();
		LIST_ENCY_BUFF_IT itEncyEnd = m_ListEncyBuff.end();

		for (; itEncy != itEncyEnd; ++itEncy)
		{
			if (itEncy->s_nEndTime <= _TIME_TS)
			{
				m_ListEncyBuff.erase(itEncy);
				break;
			}
		}
	}

	if (m_ListBuff.empty() && m_ListDeBuff.empty() && m_ListSystemBuff.empty())
		return;

	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;

	it = m_ListBuff.begin();
	itEnd = m_ListBuff.end();

	for (; it != itEnd; it++)
	{
		if (it->s_nLifeTime == UINT_MAX)
			continue;

		it->s_nLifeTime = it->s_nEndTime - _TIME_TS;
	}

	it = m_ListDeBuff.begin();
	itEnd = m_ListDeBuff.end();

	for (; it != itEnd; it++)
	{
		if (it->s_nLifeTime == UINT_MAX)
			continue;

		it->s_nLifeTime = it->s_nEndTime - _TIME_TS;

		if (it->s_nLifeTime <= 0)
		{
			CsC_AvObject* pObject = g_pMngCollector->GetObject(it->s_nTargetUID);

			if (pObject && pObject->GetAniPause())
				pObject->SetAniPause(false);

			if (it->s_pBuffTable && it->s_pBuffTable->GetInfo())
				ReleaseBuff(it->s_pBuffTable->GetInfo()->s_dwID);

			break;
		}

		CsBuff* pBuff = it->s_pBuffTable;

		if (!pBuff || !pBuff->GetInfo())
			continue;

		if (pBuff->GetInfo()->s_dwSkillCode == 0)
			continue;

		if (!nsCsFileTable::g_pSkillMng)
			continue;

		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill(pBuff->GetInfo()->s_dwSkillCode);
		if (!pSkill || !pSkill->GetInfo())
			continue;

		for (int i = 0; i < SKILL_APPLY_MAX_COUNT; i++)
		{
			CsSkill::sINFO::sAPPLY pSkillApply = pSkill->GetInfo()->s_Apply[i];

			if (pSkillApply.s_nID == 107)
			{
				if (m_pObject && m_pObject->GetProp_Attack() && m_pObject->GetProp_Attack()->IsConditionProcess() == false)
				{
					CsC_AvObject* pObject = g_pMngCollector->GetObject(it->s_nTargetUID);

					CsC_AttackProp::sDAMAGE_INFO DInfo;
					DInfo.s_pHitter = (pObject != NULL) ? pObject : NULL;
					DInfo.s_eDamageType = CsC_AttackProp::DT_Condition;
					DInfo.s_nNumType = (m_pObject->GetLeafRTTI() == RTTI_MONSTER) ? NUMTYPE::MONSTER_ATTACK : NUMTYPE::DIGIMON_ATTACK;
					DInfo.s_nNumEffect = NUMTYPE::ET_NORMAL;
					DInfo.s_eActive = CsC_AttackProp::AT_ACTIVE;
					DInfo.s_eMarbleType = CsC_AttackProp::MB_NONE;
					DInfo.s_fHitEventTime = 1.5f;
					DInfo.s_dwResistTime = pBuff->GetInfo()->s_dwID;
					DInfo.s_pChildDamageInfo = NULL;

					m_pObject->GetProp_Attack()->InsertDamage(&DInfo);
				}
			}
		}
	}

	it = m_ListSystemBuff.begin();
	itEnd = m_ListSystemBuff.end();

	for (; it != itEnd; it++)
	{
		if (it->s_nLifeTime == UINT_MAX)
			continue;

		it->s_nLifeTime = it->s_nEndTime - _TIME_TS;
	}
}

void cBuffData::_SetEncyBuff(u2 nIdx, u4 nEndTime)
{
	LIST_ENCY_BUFF_IT it = m_ListEncyBuff.begin();
	LIST_ENCY_BUFF_IT itEnd = m_ListEncyBuff.end();

	bool bExist = false;

	for (; it != itEnd; ++it)
	{
		if (it->s_nOptIdx == nIdx)
		{
			it->s_nEndTime = nEndTime;
			bExist = true;
			break;
		}
	}

	if (bExist == false)
	{
		sENCY_BUFF_DATA pData;
		pData.s_nOptIdx = nIdx;
		pData.s_nEndTime = nEndTime;
		m_ListEncyBuff.push_back(pData);
	}
}

void cBuffData::_ReleaseEncyBuff(u2 nIdx)
{
	LIST_ENCY_BUFF_IT it = m_ListEncyBuff.begin();
	LIST_ENCY_BUFF_IT itEnd = m_ListEncyBuff.end();

	for (; it != itEnd; ++it)
	{
		if (it->s_nOptIdx == nIdx)
		{
			m_ListEncyBuff.erase(it);
			break;
		}
	}
}

void cBuffData::_ReleaseAllEncyBuff()
{
	m_ListEncyBuff.clear();
}

void cBuffData::ClearBuffLoopEffect()
{
	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;

	DWORD dwTargetUID = 0;
	DWORD dwBuffID = 0;

	if (!m_ListBuff.empty())
	{
		it = m_ListBuff.begin();
		itEnd = m_ListBuff.end();

		for (; it != itEnd; it++)
		{
			if (!it->s_pBuffTable || !it->s_pBuffTable->GetInfo())
				continue;

			dwTargetUID = it->s_nTargetUID;
			dwBuffID = it->s_pBuffTable->GetInfo()->s_dwID;

			CDigimon* pTarget = dynamic_cast<CDigimon*>(g_pMngCollector->GetObject(dwTargetUID));

			if (pTarget)
			{
				if (pTarget->GetProp_Effect())
					pTarget->GetProp_Effect()->DeleteBuffLoopEffect(dwBuffID);

				if (g_pSEffectMgr && g_pSEffectMgr->DeleteSEffect(pTarget, dwBuffID))
				{
					ST_CHAT_PROTOCOL CProtocol;
					CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
					DmCS::StringFn::Format(CProtocol.m_wStr, _T("Delete SEffect Shading TargetID : %d"), dwTargetUID);
					GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
				}
			}
		}

		m_ListBuff.clear();
	}

	if (!m_ListDeBuff.empty())
	{
		it = m_ListDeBuff.begin();
		itEnd = m_ListDeBuff.end();

		for (; it != itEnd; it++)
		{
			if (!it->s_pBuffTable || !it->s_pBuffTable->GetInfo())
				continue;

			dwTargetUID = it->s_nTargetUID;
			dwBuffID = it->s_pBuffTable->GetInfo()->s_dwID;

			CDigimon* pTarget = dynamic_cast<CDigimon*>(g_pMngCollector->GetObject(dwTargetUID));

			if (pTarget)
			{
				if (pTarget->GetProp_Effect())
					pTarget->GetProp_Effect()->DeleteBuffLoopEffect(dwBuffID);

				if (g_pSEffectMgr && g_pSEffectMgr->DeleteSEffect(pTarget, dwBuffID))
				{
					ST_CHAT_PROTOCOL CProtocol;
					CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
					DmCS::StringFn::Format(CProtocol.m_wStr, _T("Delete SEffect Shading TargetID : %d"), dwTargetUID);
					GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
				}
			}
		}

		m_ListDeBuff.clear();
	}
}

void cBuffData::_SetBuffEffect(CsBuff* pBuff, USHORT nBuffID, UINT nTargetUID)
{
	if (!pBuff)
		return;

	if (!pBuff->GetInfo())
		return;

	DWORD dwBuffToSkillCode = pBuff->GetInfo()->s_dwSkillCode;

	if (dwBuffToSkillCode == 0)
		return;

	if (!nsCsFileTable::g_pSkillMng)
		return;

	CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill(dwBuffToSkillCode);

	if (!pSkill)
		return;

	if (!pSkill->GetInfo())
		return;

	CsC_AvObject* pTarget = g_pMngCollector->GetObject(nTargetUID);

	if (!pTarget)
		return;

	switch (pTarget->GetLeafRTTI())
	{
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
	case RTTI_MONSTER:
		break;

	default:
		return;
	}

	if (pSkill->GetInfo()->s_Apply[1].s_nID == 0)
		return;

	if (pBuff->GetInfo()->s_szEffectFile[0] == NULL)
		return;

	if (!pTarget->GetProp_Effect())
		return;

	char szBuf[OBJECT_PATH_LEN] = { 0, };

	if (pBuff->GetInfo()->s_nBuffType == 2)
		sprintf_s(szBuf, OBJECT_PATH_LEN, "System\\DeBuff\\%s.nif", pBuff->GetInfo()->s_szEffectFile);
	else
		sprintf_s(szBuf, OBJECT_PATH_LEN, "System\\Buff\\%s.nif", pBuff->GetInfo()->s_szEffectFile);

	TCHAR szWide[OBJECT_PATH_LEN] = { 0, };
	M2W(szWide, szBuf, OBJECT_PATH_LEN);

	ST_CHAT_PROTOCOL CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	DmCS::StringFn::Format(CProtocol.m_wStr, _T("EffectPath : %s"), szWide);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);

	char szSoundBuf[OBJECT_PATH_LEN] = { 0, };

#define CENTERFLAG	nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER
#define FOOTFLAG	nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS
#define HEIGHTFLAG	nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_THEIGHT

	DWORD dwFlag = FOOTFLAG;

	switch (pSkill->GetInfo()->s_Apply[1].s_nA)
	{
	case APPLY_CA:
	case APPLY_EV:
	case APPLY_SCD:
	{
		if (m_bIsOnePlayEffect)
		{
			pTarget->GetProp_Effect()->AddEffect(szBuf, 1.f, nsEFFECT::POS_CHARPOS);
			pTarget->PlaySound(SOUND_LEVEL_UP);
		}
		return;
	}

	case APPLY_UB:
	{
		dwFlag = CENTERFLAG;
		sprintf_s(szSoundBuf, "%s", SOUND_BUFF_UNBEATABLE);
	}
	break;

	case APPLY_DOT:
	case APPLY_DOT2:
	{
		if (m_bIsOnePlayEffect)
			pTarget->GetProp_Effect()->AddEffect(szBuf, 1.f, nsEFFECT::POS_CHARPOS);
		return;
	}

	case APPLY_STUN:
	{
		if (nBuffID == 40165 || nBuffID == 40172)
		{
			sprintf_s(szSoundBuf, "%s", SOUND_DEBUFF_STUN);
			dwFlag = HEIGHTFLAG;
		}
		else if (nBuffID == 40167)
		{
			sprintf_s(szSoundBuf, "%s", SOUND_DEBUFF_PETRIFY);

			if (g_pSEffectMgr)
			{
				SEffectMgr::st_SHADING_FLAG flag(
					SEffectMgr::FT_ONECOLOR,
					SEffectMgr::DC_NONE,
					CGeometry::Color_Gray,
					nBuffID,
					SEffectMgr::UT_INFINITY_TIME,
					SEffectMgr::ET_COLOR);

				g_pSEffectMgr->AddSEffect(0.f, pTarget, flag, 0.f);
			}
		}
	}
	break;

	case APPLY_DR:
	case APPLY_AB:
	{
		dwFlag = FOOTFLAG;
		sprintf_s(szSoundBuf, "%s", SOUND_LEVEL_UP);
	}
	break;

	default:
	{
		if (m_bIsOnePlayEffect)
		{
			pTarget->GetProp_Effect()->AddEffect(szBuf, 1.f, nsEFFECT::POS_CHARPOS);
			pTarget->PlaySound(SOUND_LEVEL_UP);
		}
		return;
	}
	}

	pTarget->GetProp_Effect()->AddBuffLoopEffect(szBuf, nBuffID, dwFlag);

	if (m_bIsOnePlayEffect)
		pTarget->PlaySound(szSoundBuf);
}