#include "stdafx.h"
#include "TableClass_DEvolution.h"

BEGIN_NAME_TABLE

CDEvolutionTable::CDEvolutionTable()
{

}

CDEvolutionTable::~CDEvolutionTable()
{

}

void CDEvolutionTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"DEvoBaseGroup.bin", this, &CDEvolutionTable::_LoadData_BaseGroup);
	AddLoadTableEvent(L"DEvolutionList.bin", this, &CDEvolutionTable::_LoadData_EvolutionList);
	AddLoadTableEvent(L"DigiEvoApplyData.bin", this, &CDEvolutionTable::_LoadData_EvoApplyData);
	AddChangeStringEvent(ESTRING_TABLE_DIGIEVO, this, &CDEvolutionTable::_InsertString);
}

void CDEvolutionTable::ReleaseTable()
{
	m_EvolApply.clear();
	m_EvoCont.clear();
}

int CDEvolutionTable::GetEvoApplyPercent(USHORT const& nEvoType) const
{
	ItemConts::const_iterator it = m_EvolApply.find(nEvoType);
	if (it == m_EvolApply.end())
		return 0;
	return it->second.nStatusPer;
}

std::wstring CDEvolutionTable::GetEvoTypeName(USHORT const& nEvoType) const
{
	ItemConts::const_iterator it = m_EvolApply.find(nEvoType);
	if (it == m_EvolApply.end())
		return L"";
	return it->second.name;
}

bool CDEvolutionTable::_InsertString(ITbImpl const* pData)
{
	CSTable_DigiEvo const* pStr = static_cast<CSTable_DigiEvo const*>(pData);
	ItemConts::iterator it = m_EvolApply.begin();
	for (; it != m_EvolApply.end(); ++it)
		it->second.name = pStr->GetData(it->first);

	return true;
}

bool CDEvolutionTable::_LoadData_EvoApplyData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		USHORT nEvoType;
		pReader.pop(nEvoType);
		sDigiEvoApplyData info;
		pReader.pop(info.nStatusPer);
		m_EvolApply.insert(std::make_pair(nEvoType, info));
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CDEvolutionTable::_LoadData_BaseGroup(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		USHORT nGroupIDx;
		pReader.pop(nGroupIDx);
		if (0 == nGroupIDx)
			break;
		DWORD dwBaseDigimonIDX = 0;
		pReader.pop(dwBaseDigimonIDX);
		m_Group.insert(std::make_pair(nGroupIDx, dwBaseDigimonIDX));
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CDEvolutionTable::_LoadData_EvolutionList(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		USHORT nEvoSlot = 0;
		pReader.pop(nEvoSlot);
		DWORD dwDigimonID = 0;
		pReader.pop(dwDigimonID);

		EvoConts::iterator it = m_EvoCont.find(dwDigimonID);
		if (it == m_EvoCont.end())
		{
			EvoConts::_Pairib rib = m_EvoCont.insert(std::make_pair(dwDigimonID, sDEvolutionInfo()));
			it = rib.first;
		}else{
			// Error
			MessageBox(NULL, _T("같은 디지몬 IDX"),_T("Error"),MB_OK);
			continue;
		}
		
		for (int n = 0; n < 9; ++n)
		{
			sDEvoTargetInfo targetInfo;
			pReader.pop(targetInfo.nSlot);
			pReader.pop(targetInfo.nType);
			if (0 == targetInfo.nSlot)
				continue;			
			it->second.m_nEvolutionList.push_back(targetInfo);
		}

		pReader.pop(it->second.m_UIIconPos.m_PosX);
		pReader.pop(it->second.m_UIIconPos.m_PosY);

		pReader.pop(it->second.m_bEnableSlot);
		pReader.pop(it->second.m_SlotOpenLimit.m_nOpenQualification);
		pReader.pop(it->second.m_SlotOpenLimit.m_nOpenLevel);
		pReader.pop(it->second.m_SlotOpenLimit.m_dwOpenQuest);
		pReader.pop(it->second.m_SlotOpenLimit.m_nOpenItemTypeS);
		pReader.pop(it->second.m_SlotOpenLimit.m_nOpenItemNum);
		pReader.pop(it->second.m_SlotOpenLimit.m_dwUseItem);
		pReader.pop(it->second.m_SlotOpenLimit.m_nUseItemNum);
		pReader.pop(it->second.m_SlotOpenLimit.m_nIntimacy);
		pReader.pop(it->second.m_SlotOpenLimit.m_nOpenCrest);

		pReader.pop(it->second.m_EvoCardInfo.m_EvoCard1);
		pReader.pop(it->second.m_EvoCardInfo.m_EvoCard2);
		pReader.pop(it->second.m_EvoCardInfo.m_EvoCard3);
		pReader.pop(it->second.m_EvoCardInfo.m_nEvoDigimental);

		pReader.pop(it->second.m_nEvoTamerDS);
		
		pReader.pop(it->second.m_3dRenderInfo.m_bRender);
		pReader.pop(it->second.m_3dRenderInfo.m_TargetStartPosX);
		pReader.pop(it->second.m_3dRenderInfo.m_TargetEndPosY);
		pReader.pop(it->second.m_3dRenderInfo.m_nStartHegiht);
		pReader.pop(it->second.m_3dRenderInfo.m_nStartRot);

		pReader.pop(it->second.m_3dRenderInfo.m_TargetEndPosX);
		pReader.pop(it->second.m_3dRenderInfo.m_TargetEndPosY);
		pReader.pop(it->second.m_3dRenderInfo.m_nEndHegiht);
		pReader.pop(it->second.m_3dRenderInfo.m_nEndRot);

		pReader.pop(it->second.m_3dRenderInfo.m_nSpeed);
		pReader.pop(it->second.m_3dRenderInfo.m_dwAni);
		pReader.pop(it->second.m_3dRenderInfo.m_fStartTime);
		pReader.pop(it->second.m_3dRenderInfo.m_fEndTime);
		pReader.pop(it->second.m_3dRenderInfo.m_nR);
		pReader.pop(it->second.m_3dRenderInfo.m_nG);
		pReader.pop(it->second.m_3dRenderInfo.m_nB);

		pReader.pop(it->second.m_EffectPos.m_sLevel);
		pReader.pop(it->second.m_EffectPos.m_sEnchant);
		pReader.pop(it->second.m_EffectPos.m_sSize);

		pReader.pop(it->second.m_nCapsuleEvo);
		pReader.pop(it->second.n_JogresEvo.m_nJoGressQuestCheck);
		pReader.pop(it->second.n_JogresEvo.m_nChipsetType);
		pReader.pop(it->second.n_JogresEvo.m_nChipsetTypeC);
		pReader.pop(it->second.n_JogresEvo.m_nChipsetNum);
		pReader.pop(it->second.n_JogresEvo.m_nChipsetTypeP);
		while (true)
		{
			DWORD dwNeedDigimonIDX = 0;
			pReader.pop(dwNeedDigimonIDX);		
			if (0 == dwNeedDigimonIDX)
				break;
			it->second.n_JogresEvo.m_NeedDigimons.push_back(dwNeedDigimonIDX);
		}

		bSuccess = pReader.MoveNext();
	}
	return true;
}

END_NAME_TABLE
