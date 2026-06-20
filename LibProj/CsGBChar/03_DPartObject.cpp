
#include "stdafx.h"
#include "03_DPartObject.h"

#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP(PARENT_CLASS, CsC_DPartObject, RTTI_DPARTOBJECT)
THREAD_MEMPOOL_CPP(CsC_DPartObject)


CsC_DPartObject::CsC_DPartObject():m_bInitialLoadPart(false)
{
}

void CsC_DPartObject::Delete()
{
	ClearAllParts();
	PARENT_CLASS::Delete();
	m_bInitialLoadPart = false;
	assert_cs(GetRefLinkPart() == 0);
	assert_cs(m_queueReadyPart.Empty() == true);	
}

void CsC_DPartObject::DeleteUpdate()
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

//====================================================================================
//
//		Load
//
//====================================================================================
void CsC_DPartObject::ChangeKFM(UINT nFTID)
{
	ClearAllParts();
	PARENT_CLASS::ChangeKFM(nFTID);
}

/**
* \author      :	      song
* \remarks	: 파트너몬 변경시 호출
* \param nFTID 파일테이블 ID
* \param szName 교체되는 몬스터의 이름
*/
void CsC_DPartObject::ChangeModel(UINT nFTID, LPCTSTR szName)
{
	ClearAllParts();
	PARENT_CLASS::ChangeModel(nFTID, szName);
}

void CsC_DPartObject::Update(float fDeltaTime, bool bAnimation)
{
	_UpdateReadyPart();
	PARENT_CLASS::Update(fDeltaTime, bAnimation);
}

void CsC_DPartObject::ThreadLoad_Part(std::list<nsDPART::sDPART_INFO> const* dPartsInfo, bool bIncludeBaseKFM/* true*/)
{
	//=======================================================================================
	//	기본 본 정보
	//=======================================================================================
	if (bIncludeBaseKFM == true)
	{
		assert_cs(GetFTID() != 0);
		sTCUnit* pUint = sTCUnit::NewInstance(sTCUnit::KFM_File, GetFTID());
		pUint->s_Prop = m_CharProp;
		pUint->s_pLoadedObject = (CsC_Object*)this;

		if (_IsFastPartLoad())
			pUint->SetLoadType(sTCUnit::eFast);

		// 루트 정보

		std::string strPath = GetModelPath();
		strcpy_s(pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str());
		assert_cs(pUint->s_dwLoadID != 0);
		g_pThread->LoadChar(pUint);
	}

	if (dPartsInfo)
	{
		std::list<nsDPART::sDPART_INFO>::const_iterator it = dPartsInfo->begin();
		for (; it != dPartsInfo->end(); ++it)
			ChangeParts(*it);
	}
}

void CsC_DPartObject::ThreadCallBack_LoadKFM(sLOAD_KFM* pLoad)
{
	m_bInitialLoadPart = true;
	PARENT_CLASS::ThreadCallBack_LoadKFM(pLoad);
}

void CsC_DPartObject::ThreadCallBack_AttachPart(sLOAD_PART* pPart)
{
	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
	m_queueReadyPart.Put(pPart);

	sLOAD_PART* pDecreaseRefPart = sLOAD_PART::NewInstance();
	pDecreaseRefPart->s_nPartIndex = -1;
	m_queueReadyPart.Put(pDecreaseRefPart);
}

void CsC_DPartObject::ChangeParts(nsDPART::sDPART_INFO const& pPartInfo)
{
	// 이미 있는 파트이면 삭제
	std::map<int, sParts >::iterator it = m_Parts.find(pPartInfo.s_nPartIndex);
	if (it != m_Parts.end())
		DetachParts(it->first);

	sParts info;
	info.s_FileName = pPartInfo.s_File;
	info.s_nFileTableID = pPartInfo.s_nFileTableID;
	info.s_nRemainTime = pPartInfo.s_nRemainTime;
	m_Parts.insert(std::make_pair(pPartInfo.s_nPartIndex, info));

	sDCUnitPart* pUint = (sDCUnitPart*)sTCUnit::NewInstance(sTCUnit::DPart_File, GetFTID());
	pUint->SetLoadType(sTCUnit::eNormal);
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ANI_LOOP;
	pUint->s_pLoadedObject = this;
	pUint->s_nPartIndex = pPartInfo.s_nPartIndex;

	sprintf_s(pUint->s_cFilePath, OBJECT_PATH_LEN, "%s", info.s_FileName.c_str());
	g_pThread->LoadChar(pUint);
}

void CsC_DPartObject::_UpdateReadyPart()
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
				_SetParts(pReadyPart->s_nPartIndex, pReadyPart->s_pNiNode);		
				bPartChange = true;
			}
			if (pReadyPart->s_pRefLoad)
				pReadyPart->s_pRefLoad->DecreaseRef();
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
}

void CsC_DPartObject::_SetParts(int const& nType, NiNodePtr pPart )
{
	std::map<int, sParts >::iterator it = m_Parts.find(nType);
	if (it == m_Parts.end())
		return;

	it->second.s_pPartObj = pPart;
	RemapObjectSkinInstance(m_Node.m_pNiNode, pPart);

	//========================================================================================
	// 부모에 붙인다 - 기존에 있는것이 리턴되서 나온다 ( 스마트 포인터라서 알아서 제거 )
	//========================================================================================
	m_Node.m_pNiNode->AttachChild(pPart);

	CsGBVisible::GetPlag(m_dwVisiblePlag, m_Node.m_pNiNode);
}

bool CsC_DPartObject::IsPartsApplied(int const& nType) const
{
	std::map<int, sParts >::const_iterator it = m_Parts.find(nType);
	if (it == m_Parts.end())
		return false;
	
	return true;
}

void CsC_DPartObject::DetachParts(int const& nType)
{
	if (NULL == m_Node.m_pNiNode)
		return;

	std::map<int, sParts >::iterator it = m_Parts.find(nType);
	if (it == m_Parts.end())
		return;

	if (NULL == it->second.s_pPartObj)
	{
		m_Parts.erase(it);
		return;
	}	

	m_Node.m_pNiNode->DetachChild(it->second.s_pPartObj);
	it->second.s_pPartObj = NULL;
	m_Parts.erase(it);

	m_Node.ResetNiObject(m_CharProp.s_eInsertGeomType);
	m_Node.ActiveShader();
}

void CsC_DPartObject::ClearAllParts()
{
	std::map<int, sParts >::iterator it = m_Parts.begin();
	for (; it != m_Parts.end(); ++it)
	{
		if(NULL == it->second.s_pPartObj)
			continue;

		if (m_Node.m_pNiNode)
			m_Node.m_pNiNode->DetachChild(it->second.s_pPartObj);
		it->second.s_pPartObj = NULL;
	}
	m_Parts.clear();
	m_Node.ResetNiObject(m_CharProp.s_eInsertGeomType);
	m_Node.ActiveShader();
}

bool CsC_DPartObject::RemapGeomSkinInstance(NiNode* p_pkSceneRoot, NiGeometry* p_pkGeom)
{
	if (0 == p_pkGeom)
		return false;

	NiSkinInstance* pkSkin = p_pkGeom->GetSkinInstance();
	if (pkSkin)
	{
		NiSkinData* pkSkinData = pkSkin->GetSkinData();
		assert(pkSkinData);

		unsigned int uiBoneCount = pkSkinData->GetBoneCount();
		NiAVObject* pkRootParent = pkSkin->GetRootParent();
		NiAVObject*const* pkBones = pkSkin->GetBones();

		NiAVObject* pkAVObject = 0;
		for (unsigned int ui = 0; ui < uiBoneCount; ui++)
		{
			NiAVObject* pkCurBone = pkBones[ui];
			pkAVObject = p_pkSceneRoot->GetObjectByName(pkCurBone->GetName());
			if (pkAVObject)
				pkSkin->SetBone(ui, pkAVObject);
			else
				return false;
		}

		pkAVObject = p_pkSceneRoot->GetObjectByName(pkRootParent->GetName());
		if (pkAVObject)
		{
			pkSkin->SetRootParent(pkAVObject);
			return true;
		}
	}

	return false;
}

void CsC_DPartObject::RemapObjectSkinInstance(NiNode* p_pkSceneRoot, NiAVObject* p_pkObject)
{
	if (NiIsKindOf(NiNode, p_pkObject))
	{
		NiNode* pkNode = static_cast<NiNode*>(p_pkObject);

		for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
		{
			NiAVObject* pkChild = pkNode->GetAt(i);
			if (pkChild != NULL)
			{
				RemapObjectSkinInstance(p_pkSceneRoot, pkChild);
			}
		}
	}
	else if (NiIsKindOf(NiTriBasedGeom, p_pkObject))
	{
		RemapGeomSkinInstance(p_pkSceneRoot, static_cast<NiTriBasedGeom*>(p_pkObject));
	}
}
#undef		PARENT_CLASS

