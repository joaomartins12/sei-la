#include "stdafx.h"
#include "04_PartObject.h"

#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP(PARENT_CLASS, CsC_PartObject, RTTI_PARTOBJECT)
THREAD_MEMPOOL_CPP(CsC_PartObject)

//CsCRTTI_CPP( PARENT_CLASS, CsC_CardObject, RTTI_CARDOBJECT )
//THREAD_MEMPOOL_CPP( CsC_CardObject )


#ifdef PC_BANG_SERVICE
#define PCBANG_COSTUME_ITEM_ID			17101
#endif

namespace
{
	static CsItem::sINFO* SafeGetItemInfo(UINT nFileTableID)
	{
		if (nFileTableID == 0)
			return NULL;

		if (nsCsFileTable::g_pItemMng == NULL)
			return NULL;

		if (nsCsFileTable::g_pItemMng->IsItem(nFileTableID) == false)
			return NULL;

		return nsCsFileTable::g_pItemMng->GetItem(nFileTableID)->GetInfo();
	}

	static bool IsValidCostumeCategory(int nTamerModelIDX, UINT nFileTableID)
	{
		CsItem::sINFO* pFTItem = SafeGetItemInfo(nFileTableID);

		if (pFTItem == NULL)
			return false;

		if (g_pModelDataMng == NULL)
			return false;

		return g_pModelDataMng->CostumeCategory(nTamerModelIDX, pFTItem->s_nType_S);
	}

	static bool SetDefaultPartFileName(char* pOut, int nPartIndex)
	{
		if (pOut == NULL)
			return false;

		switch (nPartIndex)
		{
		case nsPART::Head:
			strcpy_s(pOut, OBJECT_NAME_LEN, "Head.nif");
			return true;

		case nsPART::Coat:
			strcpy_s(pOut, OBJECT_NAME_LEN, "Up.nif");
			return true;

		case nsPART::Glove:
			strcpy_s(pOut, OBJECT_NAME_LEN, "Hand.nif");
			return true;

		case nsPART::Pants:
			strcpy_s(pOut, OBJECT_NAME_LEN, "Down.nif");
			return true;

		case nsPART::Shoes:
			strcpy_s(pOut, OBJECT_NAME_LEN, "Shoes.nif");
			return true;

		case nsPART::EquipAura:
			pOut[0] = '\0';
			return true;

#ifdef SDM_TAMER_XGUAGE_20180628
		case nsPART::XAI:
			pOut[0] = '\0';
			return true;
#endif

#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
		case nsPART::Digivice:
			pOut[0] = '\0';
			return true;
#endif
		}

		pOut[0] = '\0';
		return false;
	}

}

CsC_PartObject::CsC_PartObject()
{
	m_bInitialLoadPart = false;
	m_cWorkingFolder[0] = NULL;

	// 파츠 정보 리셋
	for (int i = 0; i < PART_INFO_COUNT; ++i)
		m_PartInfo[i].Reset();

	for (int i = 0; i < nsPART::MAX_CHANGE_PART_COUNT; ++i)
		m_SettingPart[i].Reset();
}

void CsC_PartObject::Delete()
{
	PARENT_CLASS::Delete();

	m_bInitialLoadPart = false;
	assert_cs(GetRefLinkPart() == 0);
	assert_cs(m_queueReadyPart.Empty() == true);
	/*std::vector< sLOAD_PART* >	vItem;
	m_queueReadyPart.Get( vItem );
	size_t nCount = vItem.size();
	for( size_t i=0; i<nCount; ++i )
	{
		if( vItem[ i ]->s_nPartIndex > -1 )
		{
			SAFE_DELETE( vItem[ i ]->s_pNiNode );
			if( vItem[ i ]->s_pRefLoad )
			{
				vItem[ i ]->s_pRefLoad->DecreaseRef();
				vItem[ i ]->s_pRefLoad = NULL;
			}
		}
		else
		{
			DecreaseRefLinkPart();
		}
		sLOAD_PART::DeleteInstance( vItem[ i ] );
	}*/

	m_cWorkingFolder[0] = NULL;

	// 파츠 정보 리셋
	for (int i = 0; i < PART_INFO_COUNT; ++i)
		m_PartInfo[i].Reset();

	for (int i = 0; i < nsPART::MAX_CHANGE_PART_COUNT; ++i)
		m_SettingPart[i].Reset();
}

void CsC_PartObject::DeleteUpdate()
{
	PARENT_CLASS::DeleteUpdate();

	if (GetRefLinkPart() == 0)
	{
		// GetRefLinkPart 찰나의 순간에 들어 올수도 있다
		assert_cs(GetRefOnThreadLoad() || GetRefLinkPart() || (m_bInitialLoadPart == true));
		return;
	}

	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
	assert_cs(m_queueReadyPart.Empty() == false);

	sLOAD_PART* pReadyPart = NULL;
	while (m_queueReadyPart.Get(pReadyPart))
	{
		if (pReadyPart->s_nPartIndex > -1)
		{
			SAFE_DELETE(pReadyPart->s_pNiNode);
			if (pReadyPart->s_pRefLoad)
			{
				pReadyPart->s_pRefLoad->DecreaseRef();
				pReadyPart->s_pRefLoad = NULL;
			}
		}
		else
		{
			// -1 이라면 레퍼런스 감소로 놓겠다
			DecreaseRefLinkPart();
		}
		pReadyPart->s_pNiNode = NULL;
		pReadyPart->s_pRefLoad = NULL;
		sLOAD_PART::DeleteInstance(pReadyPart);
	}
	m_bInitialLoadPart = true;
}

void CsC_PartObject::Init(UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp)
{
	assert_cs(m_bInitialLoadPart == false);

	// 폴더 설정	
	std::string strPath = g_pModelDataMng->GetKfmPath(nFileTableID);
	nsCSFILE::GetFilePathOnly(m_cWorkingFolder, OBJECT_PATH_LEN, strPath.c_str(), true);

	PARENT_CLASS::Init(nIDX, nFileTableID, vPos, fRot, pProp);
}

void CsC_PartObject::Update(float fDeltaTime, bool bAnimation)
{
	_UpdateReadyPart();
	PARENT_CLASS::Update(fDeltaTime, bAnimation);
}

void CsC_PartObject::Render()
{
	PARENT_CLASS::Render();
}


void CsC_PartObject::SetScale(float fScale, bool bOrgScale)
{
	PARENT_CLASS::SetScale(fScale, bOrgScale);

	if (!IsProperty(CsC_Property::EFFECT))
		return;

	CsC_EffectProp* pEff = GetProp_Effect();
	SAFE_POINTER_RET(pEff);

	CsC_AvObject* pAura = pEff->GetLoopEffect(CsC_EffectProp::LE_AURA);
	if (pAura)
		pAura->SetScale(GetToolWidth() * 0.0334f);
}

//====================================================================================
//
//		Part
//
//====================================================================================

void CsC_PartObject::ThreadLoad_Part(sCHANGE_PART_INFO* pItemCodeArray, bool bIncludeBaseKFM /*=true*/)
{
	if (pItemCodeArray == NULL)
		return;

	//=======================================================================================
	//	기본 본 정보
	//=======================================================================================
	if (bIncludeBaseKFM == true)
	{
		assert_cs(GetFTID() != 0);

		sTCUnitPart* pUintBase = (sTCUnitPart*)sTCUnit::NewInstance(sTCUnit::KFM_File, GetFTID());

		if (pUintBase)
		{
			pUintBase->s_Prop = m_CharProp;
			pUintBase->s_pLoadedObject = (CsC_Object*)this;

			if (_IsFastPartLoad())
				pUintBase->SetLoadType(sTCUnit::eFast);

			std::string strPath = GetModelPath();
			strcpy_s(pUintBase->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str());

			assert_cs(pUintBase->s_dwLoadID != 0);

			g_pThread->LoadChar(pUintBase);
		}
	}

	for (int i = 0; i < nsPART::MAX_TOTAL_COUNT; ++i)
	{
		m_SettingPart[i].s_nSettingID = pItemCodeArray[i].s_nFileTableID;
		m_SettingPart[i].s_nRemainTime = pItemCodeArray[i].s_nRemainTime;
	}

	//=======================================================================================
	//	파트 정보
	//=======================================================================================
	sTCUnitPart* pUint = (sTCUnitPart*)sTCUnit::NewInstance(sTCUnit::All_Part_File, GetFTID());

	if (pUint == NULL)
		return;

	pUint->s_Prop = m_CharProp;
	pUint->s_pLoadedObject = (CsC_Object*)this;

	if (_IsFastPartLoad())
		pUint->SetLoadType(sTCUnit::eFast);

	// 코스튬 상태가 아님
	if (pItemCodeArray[nsPART::Costume].s_nFileTableID == 0)
	{
		pUint->s_ePartType = sTCUnitPart::PT_PART;

		for (int i = 0; i < nsPART::MAX_APPLY_STATE_COUNT; ++i)
		{
			pUint->s_PartInfo[i].s_nFileTableID = pItemCodeArray[i].s_nFileTableID;

			GetFileName_FromID(pUint->s_PartInfo[i].s_cPartName, &pItemCodeArray[i]);

			if (pItemCodeArray[i].s_nFileTableID != 0)
			{
				int TamerModelIDX = GetFTID();

				if (IsValidCostumeCategory(TamerModelIDX, pItemCodeArray[i].s_nFileTableID) == false)
				{
					pUint->s_PartInfo[i].s_nFileTableID = 0;

					sCHANGE_PART_INFO sFakeItemCode;
					sFakeItemCode.s_nFileTableID = 0;
					sFakeItemCode.s_nPartIndex = pItemCodeArray[i].s_nPartIndex;
					sFakeItemCode.s_nRemainTime = 0;

					GetFileName_FromID(pUint->s_PartInfo[i].s_cPartName, &sFakeItemCode);
				}
			}
		}
	}
	// 코스튬 상태
	else
	{
		int TamerModelIDX = GetFTID();

		if (IsValidCostumeCategory(TamerModelIDX, pItemCodeArray[nsPART::Costume].s_nFileTableID) == false)
		{
			pUint->s_ePartType = sTCUnitPart::PT_PART;

			for (int i = 0; i < nsPART::MAX_APPLY_STATE_COUNT; ++i)
			{
				pUint->s_PartInfo[i].s_nFileTableID = 0;

				sCHANGE_PART_INFO sFakeItemCode;
				sFakeItemCode.s_nFileTableID = 0;
				sFakeItemCode.s_nPartIndex = pItemCodeArray[i].s_nPartIndex;
				sFakeItemCode.s_nRemainTime = 0;

				GetFileName_FromID(pUint->s_PartInfo[i].s_cPartName, &sFakeItemCode);
			}
		}
		else if (GetFileName_FromID(pUint->s_PartInfo[0].s_cPartName, &pItemCodeArray[nsPART::Costume]) == true)
		{
			char pPath[OBJECT_PATH_LEN];
			sprintf_s(pPath, OBJECT_PATH_LEN, "%s\\%s", GetWorkingFolder(), pUint->s_PartInfo[0].s_cPartName);

			if (!CsFPS::CsFPSystem::IsExist(0, pPath))
			{
				pUint->s_ePartType = sTCUnitPart::PT_PART;

				for (int i = 0; i < nsPART::MAX_APPLY_STATE_COUNT; ++i)
				{
					pUint->s_PartInfo[i].s_nFileTableID = 0;

					sCHANGE_PART_INFO sFakeItemCode;
					sFakeItemCode.s_nFileTableID = 0;
					sFakeItemCode.s_nPartIndex = pItemCodeArray[i].s_nPartIndex;
					sFakeItemCode.s_nRemainTime = 0;

					GetFileName_FromID(pUint->s_PartInfo[i].s_cPartName, &sFakeItemCode);
				}
			}
			else
			{
				pUint->s_ePartType = sTCUnitPart::PT_COSTUME_PUTON;
				pUint->s_PartInfo[0].s_nFileTableID = pItemCodeArray[nsPART::Costume].s_nFileTableID;
			}
		}
		else
		{
			sTCUnitPart::DeleteInstance(pUint);

			pItemCodeArray[nsPART::Costume].s_nFileTableID = 0;

			CsC_PartObject::ThreadLoad_Part(pItemCodeArray, false);
			return;
		}
	}

	// 어태치 정보
	for (int i = nsPART::MAX_CHANGE_PART_COUNT; i < nsPART::MAX_ATTACH_COUNT; ++i)
	{
		if (pItemCodeArray[i].s_nFileTableID != 0)
		{
			int TamerModelIDX = GetFTID();

			if (IsValidCostumeCategory(TamerModelIDX, pItemCodeArray[i].s_nFileTableID) == false)
			{
				pUint->s_AttachInfo[i - nsPART::MAX_CHANGE_PART_COUNT].s_nFileTableID = 0;
				continue;
			}

			if (GetFileName_FromID(pUint->s_AttachInfo[i - nsPART::MAX_CHANGE_PART_COUNT].s_cPartName, &pItemCodeArray[i]) == true)
			{
				assert_cs(pUint->s_ePartType != sTCUnitPart::NONE_DATA);

				pUint->s_ePartType = (sTCUnitPart::ePART_TYPE)(pUint->s_ePartType | sTCUnitPart::AT_ATTACH_PUTON);
				pUint->s_AttachInfo[i - nsPART::MAX_CHANGE_PART_COUNT].s_nFileTableID = pItemCodeArray[i].s_nFileTableID;
			}
			else
			{
				pUint->s_AttachInfo[i - nsPART::MAX_CHANGE_PART_COUNT].s_nFileTableID = 0;
			}
		}
	}

	assert_cs(pUint != NULL);

	g_pThread->LoadChar(pUint);
}

void CsC_PartObject::DeletePart(int nPartIndex)
{
	sTCUnitPart* pUint = (sTCUnitPart*)sTCUnit::NewInstance(sTCUnit::Part_Delete, GetFTID());
	pUint->s_pLoadedObject = (CsC_Object*)this;
	pUint->s_nPartIndex = nPartIndex;
	g_pThread->LoadChar(pUint);
}

void CsC_PartObject::ToOriginalPart(UINT nFileTableID)
{
	ChangeKFM(nFileTableID);

	sCHANGE_PART_INFO cpi[nsPART::MAX_TOTAL_COUNT];
	for (int i = 0; i < nsPART::MAX_TOTAL_COUNT; ++i)
	{
		cpi[i].s_nFileTableID = m_SettingPart[i].s_nSettingID;
		cpi[i].s_nRemainTime = m_SettingPart[i].s_nRemainTime;
		cpi[i].s_nPartIndex = i;
	}
	ThreadLoad_Part(cpi);
}

//void CsC_PartObject::ThreadCallBack_PreLoadAttachPart( sLOAD_PART* pPart, int nPartIndex )
//{
//	assert_cs( IsLoad() == false );
//
//	m_PartInfo[ nPartIndex ].ChangePart( pPart->s_nFileTableID );
//	m_PartInfo[ nPartIndex ].SetRefLoad( pPart->s_pRefLoad );
//	_SetPart( pPart->s_pNiNode, nPartIndex );
//}

void CsC_PartObject::ThreadCallBack_AttachPart(sLOAD_PART* pPart)
{
	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
	m_queueReadyPart.Put(pPart);

	sLOAD_PART* pDecreaseRefPart = sLOAD_PART::NewInstance();
	pDecreaseRefPart->s_nPartIndex = -1;
	m_queueReadyPart.Put(pDecreaseRefPart);
}

void CsC_PartObject::ThreadCallBack_AttachPart(std::queue< sLOAD_PART* >* pPartQueue)
{
	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
	assert_cs(pPartQueue->size() > 0);
	while (pPartQueue->size())
	{
		m_queueReadyPart.Put(pPartQueue->front());
		pPartQueue->pop();
	}

	sLOAD_PART* pDecreaseRefPart = sLOAD_PART::NewInstance();
	pDecreaseRefPart->s_nPartIndex = -1;
	m_queueReadyPart.Put(pDecreaseRefPart);
}

void CsC_PartObject::ChangePart(sCHANGE_PART_INFO* pPartInfo)
{
	if (pPartInfo == NULL)
		return;

	if (pPartInfo->s_nPartIndex >= PART_INFO_COUNT)
		return;

	sTCUnitPart* pUint = NULL;

	if (pPartInfo->s_nPartIndex < nsPART::MAX_TOTAL_COUNT)
	{
		m_SettingPart[pPartInfo->s_nPartIndex].s_nSettingID = pPartInfo->s_nFileTableID;
		m_SettingPart[pPartInfo->s_nPartIndex].s_nRemainTime = pPartInfo->s_nRemainTime;
	}

	// 장비 변경 쪽
	if (pPartInfo->s_nPartIndex < nsPART::MAX_CHANGE_PART_COUNT)
	{
		// 코스튬 아닐 때
		if (pPartInfo->s_nPartIndex != nsPART::Costume)
		{
			if ((m_SettingPart[nsPART::Costume].s_nSettingID) &&
				(pPartInfo->s_nPartIndex < nsPART::MAX_APPLY_STATE_COUNT))
			{
				m_SettingPart[pPartInfo->s_nPartIndex].s_nSettingID = pPartInfo->s_nFileTableID;
				m_SettingPart[pPartInfo->s_nPartIndex].s_nRemainTime = pPartInfo->s_nRemainTime;

				char cPartName[OBJECT_NAME_LEN] = { 0, };

				sCHANGE_PART_INFO settingInfo;
				settingInfo.s_nFileTableID = m_SettingPart[nsPART::Costume].s_nSettingID;
				settingInfo.s_nRemainTime = m_SettingPart[nsPART::Costume].s_nRemainTime;
				settingInfo.s_nPartIndex = nsPART::Costume;

				GetFileName_FromID(cPartName, &settingInfo);

				if (strcmp(cPartName, "NoneCostume.nif") != 0)
					return;
			}

			pUint = (sTCUnitPart*)sTCUnit::NewInstance(sTCUnit::One_Part_File, GetFTID());

			if (pUint == NULL)
				return;

			pUint->s_pLoadedObject = (CsC_Object*)this;
			pUint->s_ePartType = sTCUnitPart::PT_PART;
			pUint->s_nPartIndex = pPartInfo->s_nPartIndex;
			pUint->s_PartInfo[0].s_nFileTableID = pPartInfo->s_nFileTableID;

			GetFileName_FromID(pUint->s_PartInfo[0].s_cPartName, pPartInfo);

			if (pPartInfo->s_nFileTableID != 0)
			{
				int TamerModelIDX = GetFTID();

				if (IsValidCostumeCategory(TamerModelIDX, pPartInfo->s_nFileTableID) == false)
				{
					pUint->s_PartInfo[0].s_nFileTableID = 0;

					sCHANGE_PART_INFO fakeInfo;
					fakeInfo.s_nFileTableID = 0;
					fakeInfo.s_nPartIndex = pPartInfo->s_nPartIndex;
					fakeInfo.s_nRemainTime = 0;

					GetFileName_FromID(pUint->s_PartInfo[0].s_cPartName, &fakeInfo);
				}
			}
		}
		// 코스튬
		else
		{
			m_SettingPart[nsPART::Costume].s_nSettingID = pPartInfo->s_nFileTableID;
			m_SettingPart[nsPART::Costume].s_nRemainTime = pPartInfo->s_nRemainTime;

			pUint = (sTCUnitPart*)sTCUnit::NewInstance(sTCUnit::One_Part_File, GetFTID());

			if (pUint == NULL)
				return;

			pUint->s_pLoadedObject = (CsC_Object*)this;

			bool bDelCostume = true;

			if (pPartInfo->s_nFileTableID != 0)
			{
				if (GetFileName_FromID(pUint->s_PartInfo[0].s_cPartName, pPartInfo) == true)
				{
					bDelCostume = false;

					pUint->s_ePartType = sTCUnitPart::PT_COSTUME_PUTON;
					pUint->s_PartInfo[0].s_nFileTableID = pPartInfo->s_nFileTableID;
				}

				int TamerModelIDX = GetFTID();

				char pPath[OBJECT_PATH_LEN] = { 0, };
				sprintf_s(pPath, OBJECT_PATH_LEN, "%s\\%s", GetWorkingFolder(), pUint->s_PartInfo[0].s_cPartName);

				if (IsValidCostumeCategory(TamerModelIDX, pPartInfo->s_nFileTableID) == false ||
					!CsFPS::CsFPSystem::IsExist(0, pPath))
				{
					bDelCostume = true;
				}
			}

			if (bDelCostume == true)
			{
				pUint->s_ePartType = sTCUnitPart::PT_COSTUME_PUTOFF;

				for (int i = 0; i < nsPART::MAX_APPLY_STATE_COUNT; ++i)
				{
					pUint->s_PartInfo[i].s_nFileTableID = m_SettingPart[i].s_nSettingID;

					sCHANGE_PART_INFO cp;
					cp.s_nFileTableID = m_SettingPart[i].s_nSettingID;
					cp.s_nPartIndex = i;
					cp.s_nRemainTime = m_SettingPart[i].s_nRemainTime;

					GetFileName_FromID(pUint->s_PartInfo[i].s_cPartName, &cp);

					if (m_SettingPart[i].s_nSettingID != 0)
					{
						int TamerModelIDX = GetFTID();

						if (IsValidCostumeCategory(TamerModelIDX, pUint->s_PartInfo[i].s_nFileTableID) == false)
						{
							pUint->s_PartInfo[i].s_nFileTableID = 0;

							sCHANGE_PART_INFO fakeInfo;
							fakeInfo.s_nFileTableID = 0;
							fakeInfo.s_nPartIndex = i;
							fakeInfo.s_nRemainTime = 0;

							GetFileName_FromID(pUint->s_PartInfo[i].s_cPartName, &fakeInfo);
						}
					}
				}
			}
		}
	}
	// 어태치 쪽
	else
	{
		if ((pPartInfo->s_nPartIndex < nsPART::MAX_ATTACH_COUNT) &&
			(pPartInfo->s_nFileTableID == 0))
		{
			DeletePart(pPartInfo->s_nPartIndex);
			return;
		}
		else if (pPartInfo->s_nPartIndex < nsPART::MAX_ATTACH_COUNT)
		{
			int TamerModelIDX = GetFTID();

			if (IsValidCostumeCategory(TamerModelIDX, pPartInfo->s_nFileTableID) == false)
			{
				DeletePart(pPartInfo->s_nPartIndex);
				return;
			}

			pUint = (sTCUnitPart*)sTCUnit::NewInstance(sTCUnit::One_Part_File, GetFTID());

			if (pUint == NULL)
				return;

			if (GetFileName_FromID(pUint->s_PartInfo[0].s_cPartName, pPartInfo) == true)
			{
				pUint->s_pLoadedObject = (CsC_Object*)this;
				pUint->s_nPartIndex = pPartInfo->s_nPartIndex;
				pUint->s_ePartType = sTCUnitPart::AT_ATTACH_PUTON;
				pUint->s_PartInfo[0].s_nFileTableID = pPartInfo->s_nFileTableID;
			}
			else
			{
				sTCUnit::DeleteInstance(pUint);

				if ((pPartInfo->s_nPartIndex < nsPART::MAX_ATTACH_COUNT) &&
					(IsPart(pPartInfo->s_nPartIndex)))
				{
					DeletePart(pPartInfo->s_nPartIndex);
				}

				return;
			}
		}
		else if (pPartInfo->s_nPartIndex < PART_INFO_COUNT)
		{
			if (pPartInfo->s_nPartIndex == nsPART::EquipAura)
			{
				m_SettingPart[pPartInfo->s_nPartIndex].s_nSettingID = pPartInfo->s_nFileTableID;
				m_SettingPart[pPartInfo->s_nPartIndex].s_nRemainTime = pPartInfo->s_nRemainTime;
				return;
			}

#ifdef SDM_TAMER_XGUAGE_20180628
			if (pPartInfo->s_nPartIndex == nsPART::XAI)
			{
				m_SettingPart[pPartInfo->s_nPartIndex].s_nSettingID = pPartInfo->s_nFileTableID;
				m_SettingPart[pPartInfo->s_nPartIndex].s_nRemainTime = pPartInfo->s_nRemainTime;
				return;
			}
#endif

			if ((pPartInfo->s_nPartIndex != nsPART::Ring) &&
				(pPartInfo->s_nPartIndex != nsPART::Necklace) &&
				(pPartInfo->s_nPartIndex != nsPART::Earring)
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
				&& (pPartInfo->s_nPartIndex != nsPART::Bracelet)
#endif
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
				&& (pPartInfo->s_nPartIndex != nsPART::Digivice)
#endif
				)
			{
				pUint = (sTCUnitPart*)sTCUnit::NewInstance(sTCUnit::One_Part_File, GetFTID());

				if (pUint == NULL)
					return;

				bool bSuccess = GetFileName_FromID(pUint->s_PartInfo[0].s_cPartName, pPartInfo);

				if (bSuccess == false)
				{
					sTCUnit::DeleteInstance(pUint);
					return;
				}

				pUint->s_pLoadedObject = (CsC_Object*)this;
				pUint->s_nPartIndex = pPartInfo->s_nPartIndex;
				pUint->s_PartInfo[0].s_nFileTableID = pPartInfo->s_nFileTableID;
				pUint->s_ePartType = sTCUnitPart::AT_ATTACH_PUTON;
			}
		}
	}

	if (pUint != NULL)
	{
		if (_IsFastPartLoad())
			pUint->SetLoadType(sTCUnit::eFast);

		g_pThread->LoadChar(pUint);
	}
}

void CsC_PartObject::ChangePart_ByTool(sCHANGE_PART_INFO* pPartInfo)
{
}

void CsC_PartObject::_UpdateReadyPart()
{
	if (GetRefLinkPart() == 0)
		return;

	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
	assert_cs(m_queueReadyPart.Empty() == false);
	bool bPartChange = false;

	sLOAD_PART* pReadyPart = NULL;
	while (m_queueReadyPart.Get(pReadyPart))
	{
		if (pReadyPart->s_nPartIndex > -1)
		{
			// 현재 연결된 KFM 과 ID가 같을 시에만
			if (pReadyPart->s_dwLoadID == m_dwLoadID)
			{
				// 파트 추가
				if (pReadyPart->s_pNiNode)
				{
					m_PartInfo[pReadyPart->s_nPartIndex].SetRefLoad(pReadyPart->s_pRefLoad);
					_SetPart(pReadyPart->s_pNiNode, pReadyPart->s_nPartIndex);
				}
				// 파트 제거
				else
				{
					m_Node.m_pNiNode->DetachChildAt(pReadyPart->s_nPartIndex + nsPART::PART_NODE_CONSTANT);
					m_PartInfo[pReadyPart->s_nPartIndex].Reset();
				}
				bPartChange = true;
			}
		}
		else
		{
			// -1 이라면 레퍼런스 감소로 놓겠다
			DecreaseRefLinkPart();
			PostLinkObject();
		}
		pReadyPart->s_pNiNode = NULL;
		pReadyPart->s_pRefLoad = NULL;
		sLOAD_PART::DeleteInstance(pReadyPart);
	}

	if (bPartChange == true)
	{
		m_Node.ResetNiObject(m_CharProp.s_eInsertGeomType);
		m_Node.ActiveShader();
	}
	m_bInitialLoadPart = true;
}

void CsC_PartObject::_SetPart(NiNode* pPart, int nPartIndex)
{
	// 알아서 셋팅 된다.
	//// 본정보 제거 - 쓰레드에서 지우면 깨진다~ 왜? 나도 몰러 ㅡ.,ㅡ;
	//NiAVObject* pDetach = NULL;
	//int nChildCount = pPart->GetChildCount();
	//assert_csm2( nChildCount == 2, _T( "ModelID = %d, PartIndex = %d" ), GetFTID(), nPartIndex );
	//for( int i=0; i<nChildCount; ++i )
	//{
	//	pDetach = (NiNode*)pPart->GetAt( i );		
	//	if( strcmp( pDetach->GetName(), "Bip01" ) == 0 )
	//	{
	//		break;
	//	}
	//	// 하이드 된 오브젝트가 잇는가~ 경고
	//	assert_cs( pDetach->GetAppCulled() == false );
	//}
	//if( pDetach )
	//{
	//	pDetach->SetSelectiveUpdate( false );
	//	pDetach->SetSelectiveUpdateTransforms( false );
	//	pDetach->SetSelectiveUpdatePropertyControllers( false );
	//	pDetach->SetSelectiveUpdateRigid( true );
	//}

#ifndef _DEBUG
	assert_csm3(NiGetCollisionData(pPart) == NULL, _T("ModelID = %d, ItemID = %d, PartIndex = %d"), GetFTID(), m_PartInfo[nPartIndex].s_nFileTableID, nPartIndex);
#endif

	//========================================================================================
	// 새 메쉬에 본의 포인터 연결
	//========================================================================================
	// 본이 아닌 지오메트리를 찾는다
	std::list< NiAVObject* > list;
	nsCSGBFUNC::GetSkinGeometyList(&list, pPart);

#ifdef _GIVE
	if (list.empty() == true)
		return;
#else
	assert_cs(list.size());
#endif

	NiGeometry* pGeom = NULL;
	std::list< NiAVObject* >::iterator it = list.begin();
	std::list< NiAVObject* >::iterator itEnd = list.end();
	for (; it != itEnd; ++it)
	{
		pGeom = (NiGeometry*)(*it);

		assert_csm3(pGeom, _T("피직or스킨되어있는 메쉬를 찾을수 없음\n ModelID = %d, ItemID = %d, PartIndex = %d"), GetFTID(), m_PartInfo[nPartIndex].s_nFileTableID, nPartIndex);
		NiSkinInstance* pSkin = pGeom->GetSkinInstance();
		assert_cs(pSkin != NULL);
		int nBoneCount = pSkin->GetSkinData()->GetBoneCount();
		NiAVObject* const* pBones = pSkin->GetBones();
		NiAVObject* pBoneObject;
		for (int i = 0; i < nBoneCount; ++i)
		{
			// 이름으로 본 찾아서
			pBoneObject = m_Node.m_pNiNode->GetObjectByName(pBones[i]->GetName());
			if (pBoneObject != NULL)
			{
				// 포인터 연결
				pSkin->SetBone(i, pBoneObject);
			}
		}
	}

	// 장비 아이템일경우
	if (nPartIndex < nsPART::MAX_CHANGE_PART_COUNT)
	{
		// 코스츔 변경이면서 코스츔 테이블 인덱스가 0이 아니라면 - 코스츔 장착~ 파츠 제거
		if ((nPartIndex == nsPART::Costume) && (m_SettingPart[nsPART::Costume].s_nSettingID != 0))
		{
			for (int i = 0; i < nsPART::MAX_APPLY_STATE_COUNT; ++i)
			{
				m_Node.m_pNiNode->DetachChildAt(i + nsPART::PART_NODE_CONSTANT);
				m_PartInfo[i].Reset();
			}
		}
		else
		{
			m_Node.m_pNiNode->DetachChildAt(nsPART::Costume + nsPART::PART_NODE_CONSTANT);
			m_PartInfo[nsPART::Costume].Reset();
		}
	}
	//오라일 경우
	if (nPartIndex == nsPART::EquipAura)
	{

		//오라 아이템 차고 있는 상태에서 오라템 착용할 경우

		if ((nPartIndex == nsPART::EquipAura) && (m_SettingPart[nsPART::EquipAura].s_nSettingID != 0))
		{
			m_Node.m_pNiNode->DetachChildAt(nsPART::EquipAura + nsPART::PART_NODE_CONSTANT);
			m_PartInfo[nsPART::EquipAura].Reset();

			for (int i = 0; i < nsPART::MAX_APPLY_STATE_COUNT; ++i)
			{
				m_Node.m_pNiNode->DetachChildAt(i + nsPART::PART_NODE_CONSTANT);
				m_PartInfo[i].Reset();
			}
		}
		else
		{
			m_Node.m_pNiNode->DetachChildAt(nsPART::EquipAura + nsPART::PART_NODE_CONSTANT);
			m_PartInfo[nsPART::EquipAura].Reset();
		}

	}

	//========================================================================================
	// 부모에 붙인다 - 기존에 있는것이 리턴되서 나온다 ( 스마트 포인터라서 알아서 제거 )
	//========================================================================================
	NiAVObjectPtr pCheck = m_Node.m_pNiNode->SetAt(nPartIndex + nsPART::PART_NODE_CONSTANT, pPart);

	CsGBVisible::GetPlag(m_dwVisiblePlag, m_Node.m_pNiNode);
}


//====================================================================================
//
//		Path
//
//====================================================================================
void CsC_PartObject::DeletePath()
{
	PARENT_CLASS::DeletePath();
}


bool CsC_PartObject::GetFileName_FromID(char* pOut, sCHANGE_PART_INFO* pPartInfo)
{
	if (pOut == NULL || pPartInfo == NULL)
		return false;

	pOut[0] = '\0';

	switch (pPartInfo->s_nPartIndex)
	{
	case nsCONDITION_PART::Barcode:
		if (pPartInfo->s_nFileTableID == 1)
			strcpy_s(pOut, OBJECT_NAME_LEN, "digi_clock.nif");
		else
			strcpy_s(pOut, OBJECT_NAME_LEN, "digi_clock2.nif");

		return true;
	}

	if (pPartInfo->s_nFileTableID == 0)
		return SetDefaultPartFileName(pOut, pPartInfo->s_nPartIndex);

	if (nsCsFileTable::g_pItemMng == NULL ||
		nsCsFileTable::g_pItemMng->IsItem(pPartInfo->s_nFileTableID) == false)
	{
		pPartInfo->s_nFileTableID = 0;
		return SetDefaultPartFileName(pOut, pPartInfo->s_nPartIndex);
	}

	CsItem::sINFO* pFTItem = SafeGetItemInfo(pPartInfo->s_nFileTableID);

	if (pFTItem == NULL)
	{
		pPartInfo->s_nFileTableID = 0;
		return SetDefaultPartFileName(pOut, pPartInfo->s_nPartIndex);
	}

	if (pFTItem->s_nType_L == 31)
	{
		if (pFTItem->s_nType_L - 22 != pPartInfo->s_nPartIndex)
		{
			pPartInfo->s_nFileTableID = 0;
			return SetDefaultPartFileName(pOut, pPartInfo->s_nPartIndex);
		}
	}
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
	else if (pFTItem->s_nType_L == 53)
	{
		if (pFTItem->s_nType_L - 40 != pPartInfo->s_nPartIndex)
		{
			pPartInfo->s_nFileTableID = 0;
			return SetDefaultPartFileName(pOut, pPartInfo->s_nPartIndex);
		}
	}
#endif
	else
	{
		if (pFTItem->s_nType_L - 21 != pPartInfo->s_nPartIndex)
		{
			pPartInfo->s_nFileTableID = 0;
			return SetDefaultPartFileName(pOut, pPartInfo->s_nPartIndex);
		}
	}

	if (pFTItem->s_nUseTime_Min > 0)
	{
		if (pFTItem->s_btUseTimeType == 2)
		{
			if ((pPartInfo->s_nRemainTime == (DWORD)-1) ||
				(pPartInfo->s_nRemainTime <= _GetTimeTS()))
			{
				return SetDefaultPartFileName(pOut, pPartInfo->s_nPartIndex);
			}
		}
	}

	sprintf_s(pOut, OBJECT_NAME_LEN, "%s.nif", pFTItem->s_cNif);

	return true;
}

#undef		PARENT_CLASS
