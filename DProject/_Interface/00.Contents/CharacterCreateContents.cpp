#include "StdAfx.h"
#include "CharacterCreateContents.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

namespace
{
	// Posição ideal testada para o mapa 04 / DatsUnderground.
	// Coordenadas do jogo: X=3000, Y=245, Z=18120.00
	// Em NiPoint3/Gamebryo usamos: X=3000, Y(altura)=18120, Z=245.
	//
	// IMPORTANTE:
	// Estes valores têm de bater com o target da câmera no CharacterCreate.cpp.
	static float g_fCreateMapPosX = 3000.0f;
	static float g_fCreateMapPosY = 18114.0f;
	static float g_fCreateMapPosZ = 315.0f;

	static float g_fCreateDigimonOffsetX = 0.0f;
	static float g_fCreateDigimonOffsetZ = 0.0f;

	static float g_fCreateModelYaw = 0.0f;

	void CCContentLog(const char* pszText)
	{
		if (pszText)
		{
			OutputDebugStringA("[CharacterCreateContents] ");
			OutputDebugStringA(pszText);
			OutputDebugStringA("\n");
		}
	}

	void CCContentLogPos(const char* pszTag, const NiPoint3& kPos)
	{
		char szBuffer[256] = { 0, };
		sprintf_s(
			szBuffer,
			256,
			"[CharacterCreateContents] %s pos=(%.2f, %.2f, %.2f) yaw=%.2f\n",
			pszTag ? pszTag : "",
			kPos.x,
			kPos.y,
			kPos.z,
			g_fCreateModelYaw
		);
		OutputDebugStringA(szBuffer);
	}

	void CCContentLogCurrentCreatePos()
	{
		char szBuffer[256] = { 0, };
		sprintf_s(
			szBuffer,
			256,
			"[CharacterCreateContents][POS] Tamer=(%.2f, %.2f, %.2f) Digimon=(%.2f, %.2f, %.2f) yaw=%.2f\n",
			g_fCreateMapPosX,
			g_fCreateMapPosY,
			g_fCreateMapPosZ,
			g_fCreateMapPosX + g_fCreateDigimonOffsetX,
			g_fCreateMapPosY,
			g_fCreateMapPosZ + g_fCreateDigimonOffsetZ,
			g_fCreateModelYaw
		);
		OutputDebugStringA(szBuffer);
	}
}


void CharacterCreateContents::sDigimonEvolutionInfo::SetDigimonEvolType(nsCsDigimonTable::eEVOLUTION_TYPE const& eType)
{
	m_nDigimonEvolType = eType;

	if (nsCsFileTable::g_pBaseMng)
		m_strEvolType = nsCsFileTable::g_pBaseMng->GetEvoTypeName((int)eType);
}

void CharacterCreateContents::sDigimonEvolutionInfo::SetDigimonAttributeType(nsCsDigimonTable::eATTRIBUTE_TYPE const& eType)
{
	m_nDigimonAttributeType = eType;
	switch (m_nDigimonAttributeType)
	{
	case nsCsDigimonTable::AT_NONE:				m_strAttributeType = UISTRING_TEXT("COMMON_TXT_TYPE_NEUTRAL");	break;
	case nsCsDigimonTable::AT_DATA:				m_strAttributeType = UISTRING_TEXT("CHARACTER_CREATE_DIGIMON_TYPE_DATA");	break;
	case nsCsDigimonTable::AT_VACCINE:			m_strAttributeType = UISTRING_TEXT("COMMON_TXT_TYPE_VACCINE");		break;
	case nsCsDigimonTable::AT_VIRUS:			m_strAttributeType = UISTRING_TEXT("COMMON_TXT_TYPE_VIRUS");	break;
	case nsCsDigimonTable::AT_UNIDENTIFIED:		m_strAttributeType = UISTRING_TEXT("COMMON_TXT_TYPE_UNKNOWN");		break;
	}
}

void CharacterCreateContents::sTamerCreatedInfo::SetEtcInfo()
{
	SAFE_POINTER_RET(nsCsFileTable::g_pTamerMng);
	CsTamer* pTamerInfo = nsCsFileTable::g_pTamerMng->GetTamer(m_nFileTableID);
	SAFE_POINTER_RET(pTamerInfo);

	CsTamer::sINFO* pInfo = pTamerInfo->GetInfo();
	SAFE_POINTER_RET(pInfo);

	m_szComment = pInfo->s_szComment;
	m_szName = pInfo->s_szName;
	m_szSeasonName = pInfo->s_szPart;
	for (int n = 0; n < TAMER_MAX_SKILL_COUNT; ++n)
	{
		DWORD dwSkillIdx = pInfo->s_Skill[n].s_dwID;
		if (0 == dwSkillIdx)
			break;

		CsTamerSkill* pTamerSkillInfo = nsCsFileTable::g_pSkillMng->GetTamer_Skill(dwSkillIdx, -1);
		SAFE_POINTER_CON(pTamerSkillInfo);

		CsTamerSkill::sTSkill_INFO* pSkillInfo = pTamerSkillInfo->Get_TSkill_Info();
		SAFE_POINTER_CON(pSkillInfo);

		CsSkill* pSkillData = nsCsFileTable::g_pSkillMng->GetSkill(pSkillInfo->s_dwSkillCode);
		SAFE_POINTER_CON(pSkillData);

		CsSkill::sINFO* pData = pSkillData->GetInfo();
		SAFE_POINTER_CON(pData);

		sSkillSimpleInfo skillInfo;
		skillInfo.m_dwIconIndex = pData->s_nIcon;
		skillInfo.m_dwSkillCode = pData->s_dwID;
		skillInfo.m_wsName = pData->s_szName;
		skillInfo.m_wsDesc = pData->s_szComment;
		skillInfo.m_nAttType = pData->s_nAttType;
		m_skilList.push_back(skillInfo);
	}
}

void CharacterCreateContents::sTamerCreatedInfo::SetTamerBaseState()
{
	m_mapTamerState.clear();

	SAFE_POINTER_RET(nsCsFileTable::g_pBaseMng);
	SAFE_POINTER_RET(nsCsFileTable::g_pTamerMng);

	CsTamer* pTamerInfo = nsCsFileTable::g_pTamerMng->GetTamer(m_nFileTableID);
	SAFE_POINTER_RET(pTamerInfo);
	CsTamer::sINFO* pInfo = pTamerInfo->GetInfo();
	SAFE_POINTER_RET(pInfo);
	CsBase* pTamerBase = nsCsFileTable::g_pBaseMng->GetTamerBase(1, pInfo->s_nTamerType);
	SAFE_POINTER_RET(pTamerBase);

	CsBase::sINFO* pBaseInfo = pTamerBase->GetInfo();
	SAFE_POINTER_RET(pBaseInfo);

	m_mapTamerState.insert(std::make_pair(eBaseStats::AP, pBaseInfo->s_nAttack));
	m_mapTamerState.insert(std::make_pair(eBaseStats::DP, pBaseInfo->s_nDefence));
	m_mapTamerState.insert(std::make_pair(eBaseStats::HP, pBaseInfo->s_nHP));
	m_mapTamerState.insert(std::make_pair(eBaseStats::DS, pBaseInfo->s_nDS));
}

//////////////////////////////////////////////////////////////////////////

void CharacterCreateContents::sDigimonCreatedInfo::SetEtcInfo()
{
	SAFE_POINTER_RET(nsCsFileTable::g_pDigimonMng);

	//////////////////////////////////////////////////////////////////////////
	// 디지몬 아이콘 이미지 셋팅
	if (g_pModelDataMng)
	{
		DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimonModelIdx(m_nFileTableID);
		m_sDigimonIconName = g_pModelDataMng->GetBigModelIconFile(dwModelID);
	}
	//////////////////////////////////////////////////////////////////////////


	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon(m_nFileTableID);
	SAFE_POINTER_RET(pDigimon);

	CsDigimon::sINFO* pTInfo = pDigimon->GetInfo();
	SAFE_POINTER_RET(pTInfo);

	m_szComment = UISTRING_TEXT("CHARACTER_CREATE_NO_INFORMATION");
	CsTacticsExplain* pTExplain = nsCsFileTable::g_pTacticsMng->GetTacticsExplain(m_nFileTableID);
	SAFE_POINTER_RET(pTExplain);
	CsTacticsExplain::sINFO* pInfo = pTExplain->GetInfo();
	SAFE_POINTER_RET(pInfo);
	m_szComment = pInfo->s_szTacticsExplain;
}

void CharacterCreateContents::sDigimonCreatedInfo::SetEvolutionInfo()
{
	SAFE_POINTER_RET(nsCsFileTable::g_pEvolMng);
	CDigimonEvolveInfo* pEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo(m_nFileTableID);
	SAFE_POINTER_RET(pEvolInfo);
	for (int i = 1; i < nLimit::EvoUnit; ++i)
	{
		CDigimonEvolveObj* pFTEvolObj = pEvolInfo->GetEvolveObjByEvoSlot(i);
		SAFE_POINTER_CON(pFTEvolObj);

		if (!pFTEvolObj->m_nEnableSlot)
			continue;

		CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon(pFTEvolObj->m_dwID);
		SAFE_POINTER_RET(pDigimon);

		CsDigimon::sINFO* pTInfo = pDigimon->GetInfo();
		SAFE_POINTER_RET(pTInfo);

		sDigimonEvolutionInfo addEvolInfo;
		addEvolInfo.m_nFileTableID = pFTEvolObj->m_dwID;
		addEvolInfo.m_szName = pTInfo->s_szName;
		addEvolInfo.m_szDigimonIcon = g_pModelDataMng->GetEvoModelIconFile(pTInfo->s_dwModelID);
		addEvolInfo.SetDigimonEvolType(pTInfo->s_eEvolutionType);// 성장 단계
		addEvolInfo.SetDigimonAttributeType(pTInfo->s_eAttributeType);// 성장 단계
		for (int n = 0; n < DIGIMON_MAX_SKILL_COUNT; ++n)
		{
			DWORD dwSkillCode = pTInfo->s_Skill[n].s_dwID;
			if (0 == dwSkillCode)
				break;

			CsSkill* pSkillData = nsCsFileTable::g_pSkillMng->GetSkill(dwSkillCode);
			SAFE_POINTER_CON(pSkillData);

			CsSkill::sINFO* pData = pSkillData->GetInfo();
			SAFE_POINTER_CON(pData);

			sSkillSimpleInfo skillInfo;
			skillInfo.m_dwIconIndex = pData->s_nIcon;
			skillInfo.m_dwSkillCode = pData->s_dwID;
			skillInfo.m_wsName = pData->s_szName;
			skillInfo.m_wsDesc = pData->s_szComment;
			skillInfo.m_nAttType = pData->s_nAttType;
			addEvolInfo.m_skilList.push_back(skillInfo);
		}

		m_EvolutionInfo.push_back(addEvolInfo);
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


int const CharacterCreateContents::IsContentsIdentity(void)
{
	return E_CT_CHARACTER_CREATE;
}

CharacterCreateContents::CharacterCreateContents(void) :m_nSelectedTamerTID(-1), m_nSelectedDigimonTID(-1), m_pTamerObject(NULL), m_pDigimonObject(NULL)
{
	GAME_EVENT_STPTR->AddEvent(EVENT_CODE::RECV_CHECK_DOUBLE_NAME, this, &CharacterCreateContents::RecvCheckDoubleName);
	GAME_EVENT_STPTR->AddEvent(EVENT_CODE::RECV_CHAR_CREATE_RESULT, this, &CharacterCreateContents::RecvCreateResult);
}

CharacterCreateContents::~CharacterCreateContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const CharacterCreateContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CharacterCreateContents::Initialize(void)
{
	return true;
}

void CharacterCreateContents::UnInitialize(void)
{
}

bool CharacterCreateContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void CharacterCreateContents::Update(float const& fElapsedTime)
{
}

void CharacterCreateContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void CharacterCreateContents::MakeMainActorData(void)
{
}

void CharacterCreateContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CharacterCreateContents::MakeCreatedTamerList()
{
	m_TamerListInfo.clear();

	SAFE_POINTER_RET(nsCsFileTable::g_pTamerMng);
	SAFE_POINTER_RET(nsCsFileTable::g_pCharCreateTBMng);
	LIST_TAMER_CREATE_INFO const& pTamerCreateInfo = nsCsFileTable::g_pCharCreateTBMng->GetTamerCreateInfo();
	LIST_TAMER_CREATE_INFO_CIT it = pTamerCreateInfo.begin();
	for (; it != pTamerCreateInfo.end(); ++it)
	{
		if (!(*it).m_bShow)
			continue;

		sTamerCreatedInfo addInfo;
		addInfo.m_nFileTableID = (*it).m_dwTalbeID;
		addInfo.m_nEnableCreated = (*it).m_bEnable;
		addInfo.m_soundFileName = (*it).m_sVoiceFile;
		addInfo.m_nSeasonType = (*it).m_nSeasonType;
		addInfo.m_listCostume = (*it).m_listCostume;
		addInfo.m_nIconIdx = (*it).m_nIconIdx;
		addInfo.SetEtcInfo();
		addInfo.SetTamerBaseState();

		m_TamerListInfo.push_back(addInfo);
	}
}

void CharacterCreateContents::MakeCreatedDigimonList()
{
	m_DigimonListInfo.clear();

	SAFE_POINTER_RET(nsCsFileTable::g_pDigimonMng);
	SAFE_POINTER_RET(nsCsFileTable::g_pTacticsMng);
	SAFE_POINTER_RET(nsCsFileTable::g_pCharCreateTBMng);
	LIST_DIGIMON_CREATE_INFO const& pTamerCreateInfo = nsCsFileTable::g_pCharCreateTBMng->GetDigimonCreateInfo();
	LIST_DIGIMON_CREATE_INFO_CIT it = pTamerCreateInfo.begin();
	for (; it != pTamerCreateInfo.end(); ++it)
	{
		if (!(*it).m_bShow)
			continue;

		sDigimonCreatedInfo addInfo;
		addInfo.m_nFileTableID = (*it).m_dwTalbeID;
		addInfo.m_nEnableCreated = (*it).m_bEnable;
		addInfo.m_soundFileName = (*it).m_sVoiceFile;
		addInfo.SetEtcInfo();
		addInfo.SetEvolutionInfo();
		m_DigimonListInfo.push_back(addInfo);
	}
}


CharacterCreateContents::LIST_TAMER_INFO const& CharacterCreateContents::GetMakeTamerList() const
{
	return m_TamerListInfo;
}

CharacterCreateContents::LIST_DIGIMON_INFO const& CharacterCreateContents::GetMakeDigimonList() const
{
	return m_DigimonListInfo;
}

void CharacterCreateContents::Render_TamerModel()
{
	if (m_pTamerObject)
		m_pTamerObject->Render();
}

void CharacterCreateContents::Render_DigimonModel()
{
	if (m_pDigimonObject)
		m_pDigimonObject->Render();
}

void CharacterCreateContents::Update_TamerModel(float fDeltaTime)
{
	if (m_pTamerObject)
		m_pTamerObject->CsC_PartObject::Update(fDeltaTime);
}

void CharacterCreateContents::Update_DigimonModel(float fDeltaTime)
{
	if (m_pDigimonObject)
		m_pDigimonObject->Update(fDeltaTime);
}

void CharacterCreateContents::ChangeTamerCostume(DWORD const& dwItemID)
{
	SAFE_POINTER_RET(m_pTamerObject);

	CsC_PartObject::sCHANGE_PART_INFO cp;
	cp.s_nFileTableID = dwItemID;
	cp.s_nPartIndex = nsPART::Costume;
	cp.s_nRemainTime = 0;

	m_pTamerObject->ChangePart(&cp, true);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CharacterCreateContents::ResetData()
{
	memset(m_szTamerName, 0, sizeof(TCHAR) * MAX_FILENAME);
	memset(m_szDigimonName, 0, sizeof(TCHAR) * MAX_FILENAME);

	m_nSelectedTamerTID = 0;
	m_nSelectedDigimonTID = 0;

	if (m_pTamerObject)
	{
		g_pCharMng->DeleteTempChar(m_pTamerObject);
		m_pTamerObject = NULL;
	}

	if (m_pDigimonObject)
	{
		g_pCharMng->DeleteTempChar(m_pDigimonObject);
		m_pDigimonObject = NULL;
	}

	m_TamerListInfo.clear();
	m_DigimonListInfo.clear();
}

CharacterCreateContents::sTamerCreatedInfo const* CharacterCreateContents::GetTamerInfo(unsigned int const& nTIdx) const
{
	LIST_TAMER_INFO_CIT it = m_TamerListInfo.begin();
	for (; it != m_TamerListInfo.end(); ++it)
	{
		if ((*it).m_nFileTableID != nTIdx)
			continue;

		return &(*it);
	}
	return NULL;
}

CharacterCreateContents::sDigimonCreatedInfo const* CharacterCreateContents::GetDigimonInfo(unsigned int const& nTIdx) const
{
	LIST_DIGIMON_INFO_CIT it = m_DigimonListInfo.begin();
	for (; it != m_DigimonListInfo.end(); ++it)
	{
		if ((*it).m_nFileTableID != nTIdx)
			continue;

		return &(*it);
	}
	return NULL;
}


CharacterCreateContents::sDigimonEvolutionInfo const* CharacterCreateContents::GetDigimonEvolutionInfo(unsigned int const& nBaseTIdx, unsigned int const& nEvolTIdx) const
{
	sDigimonCreatedInfo const* pBaseInfo = GetDigimonInfo(nBaseTIdx);
	SAFE_POINTER_RETVAL(pBaseInfo, NULL);
	LIST_DIGIMON_EVO_INFO_CIT it = pBaseInfo->m_EvolutionInfo.begin();
	for (; it != pBaseInfo->m_EvolutionInfo.end(); ++it)
	{
		if (nEvolTIdx != (*it).m_nFileTableID)
			continue;

		return &(*it);
	}
	return NULL;
}

void CharacterCreateContents::SetSelectTamerIdx(unsigned int const& nSelIdx)
{
	sTamerCreatedInfo const* pInfo = GetTamerInfo(nSelIdx);
	SAFE_POINTER_RET(pInfo);

	m_nSelectedTamerTID = nSelIdx;

	if (m_pTamerObject)
	{
		g_pCharMng->DeleteTempChar(m_pTamerObject);
		m_pTamerObject = NULL;
	}

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = OP_ABVWIRE | OP_LIGHT;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	CsC_PartObject::sCHANGE_PART_INFO cp[nsPART::MAX_TOTAL_COUNT];

	for (int j = 0; j < nsPART::MAX_TOTAL_COUNT; ++j)
	{
		cp[j].s_nPartIndex = j;
		cp[j].s_nFileTableID = 0;
		cp[j].s_nRemainTime = 0;
	}

	// Agora o Tamer deixa de usar a posição antiga de preview:
	// NiPoint3(fPos, 0, -110.0f)
	//
	// E passa a nascer no centro lógico do mapa 04 / DatsUnderground.
	NiPoint3 kCreatePos(g_fCreateMapPosX, g_fCreateMapPosY, g_fCreateMapPosZ);

	CCContentLogCurrentCreatePos();
	CCContentLogPos("SetSelectTamerIdx AddTempTamer", kCreatePos);

	m_pTamerObject = g_pCharMng->AddTempTamer(
		&prop,
		pInfo->m_nFileTableID,
		kCreatePos,
		g_fCreateModelYaw,
		NULL,
		cp,
		0
	);

	SAFE_POINTER_RET(m_pTamerObject);

	if (m_pTamerObject->GetProp_Alpha())
		m_pTamerObject->GetProp_Alpha()->SetDestAlphaTime(0.1f);

	ContentsStream kTmp;
	kTmp << m_nSelectedTamerTID;
	Notify(eTamer_Selected, kTmp);
}

void CharacterCreateContents::SetSelectDigimonIdx(unsigned int const& nSelBaseIdx, unsigned int const& nSelEvolIdx)
{
	sDigimonEvolutionInfo const* pInfo = GetDigimonEvolutionInfo(nSelBaseIdx, nSelEvolIdx);
	SAFE_POINTER_RET(pInfo);

	if (m_nSelectedDigimonTID != nSelBaseIdx)
		m_nSelectedDigimonTID = nSelBaseIdx;

	if (m_pDigimonObject)
	{
		g_pCharMng->DeleteTempChar(m_pDigimonObject);
		m_pDigimonObject = NULL;
	}

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = OP_ABVWIRE | OP_LIGHT;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	NiPoint3 kCreatePos(
		g_fCreateMapPosX + g_fCreateDigimonOffsetX,
		g_fCreateMapPosY,
		g_fCreateMapPosZ + g_fCreateDigimonOffsetZ
	);

	CCContentLogCurrentCreatePos();
	CCContentLogPos("SetSelectDigimonIdx AddTempDigimon", kCreatePos);

	m_pDigimonObject = g_pCharMng->AddTempDigimon(
		&prop,
		nSelEvolIdx,
		NULL,
		kCreatePos,
		g_fCreateModelYaw,
		NULL
	);

	SAFE_POINTER_RET(m_pDigimonObject);

	float fHight = m_pDigimonObject->CsC_AvObject::GetToolHeight();
	if (fHight > 250.0f)
	{
		float fScale = 250.0f / fHight;
		m_pDigimonObject->SetScale(fScale);
	}

	if (m_pDigimonObject->GetProp_Alpha())
		m_pDigimonObject->GetProp_Alpha()->SetDestAlphaTime(0.3f);

	ContentsStream kTmp;
	kTmp << nSelBaseIdx << nSelEvolIdx;
	Notify(eDigimon_Selected, kTmp);
}

void CharacterCreateContents::SetSelectDigimonEvolution(unsigned int const& nSelIdx, unsigned int const& nEvolutionIdx)
{
	SAFE_POINTER_RET(m_pDigimonObject);

	if (m_pDigimonObject)
	{
		g_pCharMng->DeleteTempChar(m_pDigimonObject);
		m_pDigimonObject = NULL;
	}

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = OP_ABVWIRE | OP_LIGHT;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	NiPoint3 kCreatePos(
		g_fCreateMapPosX + g_fCreateDigimonOffsetX,
		g_fCreateMapPosY,
		g_fCreateMapPosZ + g_fCreateDigimonOffsetZ
	);

	CCContentLogCurrentCreatePos();
	CCContentLogPos("SetSelectDigimonEvolution AddTempDigimon", kCreatePos);

	m_pDigimonObject = g_pCharMng->AddTempDigimon(
		&prop,
		nEvolutionIdx,
		NULL,
		kCreatePos,
		g_fCreateModelYaw,
		NULL
	);

	SAFE_POINTER_RET(m_pDigimonObject);

	float fHight = m_pDigimonObject->CsC_AvObject::GetToolHeight();
	if (fHight > 380.0f)
	{
		float fScale = 380.0f / fHight;
		m_pDigimonObject->SetScale(fScale);
	}
}

//------------------------------------------------------------------------
//
// 서버에 테이머 이름이 사용 가능하지 요청함
//
//------------------------------------------------------------------------
bool CharacterCreateContents::SendCheckTamerName(std::wstring const& pCheckName)
{
	if (m_nSelectedTamerTID == 0)
	{
		cPrintMsg::PrintMsg(10027);
		return false;
	}

	if (!nsCsFileTable::g_pCuidMng->CheckID(pCheckName.c_str()))
	{
		cPrintMsg::PrintMsg(10029);
		return false;
	}

	switch (Language::CheckTamerName(std::wstring(pCheckName)))
	{
	case Language::STRING_EMPTY:
	case Language::STRING_SIZE_MIN:
	case Language::STRING_SIZE_MAX:		cPrintMsg::PrintMsg(cPrintMsg::CHARCREATE_NAME_LEN_ERROR); return false;
	case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg(10029); return false;
	case Language::SUCCEEDED:
		break;
	default:
		return false;
	}

	bool bSuccess = net::gate->SendCheckDoubleName(pCheckName.c_str());
	assert_cs(bSuccess == true);

	if (bSuccess)
		wcscpy(&m_szTamerName[0], pCheckName.c_str());
	else
		memset(m_szTamerName, 0, sizeof(TCHAR) * MAX_FILENAME);

	return bSuccess;
}


bool CharacterCreateContents::SendCreate(std::wstring const& pCheckName)
{
	if (m_nSelectedDigimonTID == 0)
	{
		cPrintMsg::PrintMsg(10027);
		return false;
	}

	if (!nsCsFileTable::g_pCuidMng->CheckID(pCheckName.c_str()))
	{
		cPrintMsg::PrintMsg(10029);
		return false;
	}

	switch (Language::CheckTamerName(std::wstring(pCheckName)))
	{
	case Language::STRING_EMPTY:
	case Language::STRING_SIZE_MIN:
	case Language::STRING_SIZE_MAX:		cPrintMsg::PrintMsg(cPrintMsg::CHARCREATE_NAME_LEN_ERROR); return false;
	case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg(10029); return false;
	case Language::SUCCEEDED:
		break;
	default:
		return false;
	}

	int nEmptySlot = 0;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::GET_SELETECT_LIST_EMPTY_SLOT_NUMBER, &nEmptySlot);
	if (nEmptySlot == -1)
	{
		cPrintMsg::PrintMsg(cPrintMsg::CHARSELECT_MAX_CHAR_COUNT);
		return false;
	}

	wcscpy(&m_szDigimonName[0], pCheckName.c_str());

	nNet::CreateData data;
	data.m_nSlotNo = nEmptySlot;// 생성할 슬롯 번호
	strcpy_s(data.m_szTamerName, Language::pLength::name, (char*)LanConvert(m_szTamerName));
	strcpy_s(data.m_szDigimonName, Language::pLength::name, (char*)LanConvert(m_szDigimonName));
	data.m_nTamerType = m_nSelectedTamerTID;
	data.m_nDigimonType = m_nSelectedDigimonTID;

	if (!net::gate->SendCreate(data))
		return false;

	cPrintMsg::PrintMsg(10019);
	return true;
}




//------------------------------------------------------------------------
//
// 캐릭터 선택 화면으로 이동 시킴
//
//------------------------------------------------------------------------
void CharacterCreateContents::GotoBack()
{
	g_IME.SetEdit(false);

	ResetData();

	if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_CHARSEL))
		FLOWMGR_ST.PopFlow(FLOWMGR_ST.GetCurTopFlowID());
	else
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_CHARSEL);
}

//------------------------------------------------------------------------
//
// 테이머 닉네임 사용 가능 여부 패킷 받음.
//
//------------------------------------------------------------------------
void CharacterCreateContents::RecvCheckDoubleName(void* pData)
{
	SAFE_POINTER_RET(pData);
	uint nRes = *static_cast<uint*>(pData);
	switch (nRes)
	{
	case 1:		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_DIGIMON_CREATE);			break;			// 사용 가능 이름
	default:	cPrintMsg::PrintMsg(cPrintMsg::CHARCREATE_TAMER_NAME_EXIST);	break;	// 사용 불가능한 이름
	}
}

//------------------------------------------------------------------------
//
// 캐릭터 생성 패킷 받음.
//
//------------------------------------------------------------------------
void CharacterCreateContents::RecvCreateResult(void* pData)
{
	SAFE_POINTER_RET(pData);
	uint uiResult = *static_cast<uint*>(pData);

	g_IME.SetEdit(false);
	cMessageBox::DelMsg(10019, false);

	switch (uiResult)
	{
	case nErrCode::ConflictTamerName:
		cPrintMsg::PrintMsg(cPrintMsg::CHARCREATE_TAMER_NAME_EXIST);
		break;
	case nErrCode::ConflictDigimonName:
		cPrintMsg::PrintMsg(cPrintMsg::CHARCREATE_DIGIMON_NAME_EXIST);
		break;
	case 0:		GotoBack();		break;// 생성 성공
	default:
		assert_cs(false);
		break;
	}
}


std::wstring CharacterCreateContents::GetItemName(DWORD const& dwItemCode)
{
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pItemMng, L"");
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem(dwItemCode);
	SAFE_POINTER_RETVAL(pItem, L"");
	SAFE_POINTER_RETVAL(pItem->GetInfo(), L"");
	return pItem->GetInfo()->s_szName;
}