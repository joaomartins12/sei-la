
#include "stdafx.h"
#include "Thread_Unit.h"

CThreadSafeQueue< sTCUnit* >			sTCUnit::m_MemPool;
CThreadSafeQueue< sTCUnitPart* >		sTCUnitPart::m_MemPool;
//CThreadSafeQueue< sTCardPart* >			sTCardPart::m_MemPool;
CThreadSafeQueue< sDCUnitPart* >		sDCUnitPart::m_MemPool;

sTCUnit* sTCUnit::NewInstance( int nType, DWORD dwLoadID /*=-1*/ )
{
	switch( nType )
	{
	case LoadFileTable:
	case DeleteFile:
	case KFM_File:
	case NIF_File:{
		sTCUnit* pReturn;
		if (sTCUnit::m_MemPool.Get(pReturn) == false)
			pReturn = NiNew sTCUnit;

		assert_cs(pReturn->s_dwLoadID == 0);
		pReturn->s_Prop.Reset();
		pReturn->s_eThreadCharType = (eTYPE)nType;
		pReturn->s_eLoadType = eNormal;
		pReturn->s_dwLoadID = dwLoadID;
		assert_cs(pReturn->s_dwLoadID != 0);
		pReturn->s_dwChangeApplyModelID = 0;
		return pReturn;
	}break;
	case DPart_File:{
		sDCUnitPart* pReturn;
		if (sDCUnitPart::m_MemPool.Get(pReturn) == false)
			pReturn = NiNew sDCUnitPart;

		assert_cs(pReturn->s_dwLoadID == 0);
		pReturn->s_Prop.Reset();
		pReturn->s_eThreadCharType = (eTYPE)nType;
		pReturn->s_eLoadType = eNormal;
		pReturn->s_dwLoadID = dwLoadID;
		assert_cs(pReturn->s_dwLoadID != 0);
		pReturn->s_dwChangeApplyModelID = 0;
		return pReturn;
	}break;
	case All_Part_File:
	case One_Part_File:
	case Part_Delete:{
		sTCUnitPart* pReturn;
		if (sTCUnitPart::m_MemPool.Get(pReturn) == false)
		{
			pReturn = NiNew sTCUnitPart;
		}
		assert_cs(pReturn->s_dwLoadID == 0);
		pReturn->s_Prop.Reset();
		pReturn->s_eThreadCharType = (eTYPE)nType;
		pReturn->s_eLoadType = eNormal;
		pReturn->s_dwLoadID = dwLoadID;
		assert_cs(pReturn->s_dwLoadID != 0);
		pReturn->s_dwChangeApplyModelID = 0;
		return pReturn;
	}break;
// 	case CARD_File:
// 	{
// 		sTCardPart* pReturn;
// 		if (sTCardPart::m_MemPool.Get(pReturn) == false)
// 		{
// 			pReturn = NiNew sTCardPart;
// 		}
// 		assert_cs(pReturn->s_dwLoadID == 0);
// 		pReturn->s_Prop.Reset();
// 		pReturn->s_eThreadCharType = (eTYPE)nType;
// 		pReturn->s_eLoadType = eNormal;
// 		pReturn->s_dwLoadID = dwLoadID;
// 		assert_cs(pReturn->s_dwLoadID != 0);
// 		pReturn->s_dwChangeApplyModelID = 0;
// 		pReturn->s_bSkinObj = false;
// 		return pReturn;
// 	}
	}

	assert_cs( false );
	return NULL;
}

void sTCUnit::DeleteInstance( sTCUnit* pInst )
{
	pInst->Delete();
	switch( pInst->GetCharType() )
	{
	case LoadFileTable:
	case DeleteFile:
	case KFM_File:
	case NIF_File:
	{
		sTCUnit::m_MemPool.Put(pInst);
	}
	break;
	case DPart_File:
	{
		sDCUnitPart* pPartInst = (sDCUnitPart*)pInst;
		sDCUnitPart::m_MemPool.Put(pPartInst);
	}break;
	case All_Part_File:
	case One_Part_File:
	case Part_Delete:
	{
		sTCUnitPart* pPartInst = (sTCUnitPart*)pInst;
		sTCUnitPart::m_MemPool.Put(pPartInst);
	}
	break;
// 	case CARD_File:
// 	{
// 		sTCardPart* pPartInst = (sTCardPart*)pInst;
// 		sTCardPart::m_MemPool.Put(pPartInst);
// 	}
// 	break;
	default:
		assert_cs(false);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void sTCUnit::Delete()
{ 
	s_pLoadedObject = NULL; 
	s_Prop.Reset();
	s_dwLoadID = 0; 
	s_nValue1 = 0;
	s_pValue1 = NULL;
	s_dwChangeApplyModelID = 0;
	s_eLoadType = eNormal;
}

//////////////////////////////////////////////////////////////////////////
void sTCUnitPart::Delete()
{
	sTCUnit::Delete();
	s_nPartIndex = 0;
	s_ePartType = NONE_DATA;

	for( int n = 0; n < nsPART::MAX_APPLY_STATE_COUNT; ++n )
		s_PartInfo[n].Reset();
	for( int n = 0; n < nsPART::MAX_ATTACH_COUNT - nsPART::MAX_CHANGE_PART_COUNT; ++n )
		s_AttachInfo[n].Reset();
}


void sTCUnitPart::sPART_INFO::Reset()
{
	s_nFileTableID = 0;
	memset( s_cPartName, 0, sizeof(char)* OBJECT_NAME_LEN);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void sDCUnitPart::Delete()
{
	sTCUnit::Delete();
	s_nPartIndex = 0;
}


void sDCUnitPart::sDPART_INFO::Reset()
{
	memset(s_cPartPath, 0, sizeof(char)* OBJECT_PATH_LEN);
}
//////////////////////////////////////////////////////////////////////////

// void sTCardPart::Delete()
// {
// 	sTCUnit::Delete();
// 	s_nPartIndex = 0;
// 	s_ePartType = NONE_DATA;
// 
// 	for (int n = 0; n < nsCard::MAX_TOTAL_COUNT; ++n)
// 		s_PartInfo[n].Reset();
// }
// 
// 
// void sTCardPart::sPART_INFO::Reset()
// {
// 	m_pPixelData = NULL;
// 	memset(s_cPartPath, 0, sizeof(char)* OBJECT_PATH_LEN);
// }


