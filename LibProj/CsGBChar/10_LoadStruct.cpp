

#include "stdafx.h"
#include "10_LoadStruct.h"

THREAD_MEMPOOL_CPP( sLOAD_PART )
THREAD_MEMPOOL_CPP( sLOAD_NIF )
THREAD_MEMPOOL_CPP( sLOAD_KFM )

void sRES_LOAD::GlobalShutDown()
{
	THREAD_MEMPOOL_DELETE( sLOAD_PART );
	THREAD_MEMPOOL_DELETE( sLOAD_NIF );
	THREAD_MEMPOOL_DELETE( sLOAD_KFM );
}

void sRES_LOAD::Delete()
{
	assert_cs( s_pRefLoad->GetRef() == 0 );
	SAFE_DELETE( s_pRefLoad );
	s_dwLoadID = 0;
	s_dwVisiblePlag = 0;
}

void sRES_LOAD::Clone( sRES_LOAD* pDestObject )
{
	pDestObject->s_dwVisiblePlag = s_dwVisiblePlag;
	pDestObject->s_pRefLoad = s_pRefLoad;
	pDestObject->s_dwLoadID = s_dwLoadID;
}


void sLOAD_PART::Clone( sLOAD_PART* pDestObject )
{
	sRES_LOAD::Clone( pDestObject );

	NiCloningProcess cp;
	cp.m_eCopyType = NiObjectNET::COPY_EXACT;
	pDestObject->s_pNiNode = NiDynamicCast( NiNode, s_pNiNode->Clone( cp ) );
	pDestObject->s_nFileTableID = s_nFileTableID;
}

void sLOAD_NIF::Clone( sLOAD_NIF* pDestObject, bool bABVWire )
{
	sRES_LOAD::Clone( pDestObject );

	NiCloningProcess cp;
	cp.m_eCopyType = NiObjectNET::COPY_EXACT;
	pDestObject->s_pNiNode = NiDynamicCast( NiNode, s_pNiNode->Clone( cp ) );

	assert_cs( pDestObject->s_pABVWire == NULL );
	if( bABVWire == true )
	{
		pDestObject->s_pABVWire = (NiNode*)pDestObject->s_pNiNode->GetObjectByName( "CS_ABVWire" );	
		assert_cs( pDestObject->s_pABVWire != NULL );
	}
}

void sLOAD_KFM::Clone( sLOAD_KFM* pDestObject, bool bABVWire )
{
	sRES_LOAD::Clone( pDestObject );
	pDestObject->s_pActorMng = s_pActorMng->Clone();

	assert_cs( pDestObject->s_pABVWire == NULL );
	if( bABVWire == true )
	{
		pDestObject->s_pABVWire = (NiNode*)pDestObject->s_pActorMng->GetNIFRoot()->GetObjectByName( "CS_ABVWire" );
		assert_csm1( pDestObject->s_pABVWire != NULL, "%s", s_pActorMng->GetKFMTool()->GetModelPath() );
	}

	// Delete 는 어차피 map에 보관된 놈만 이루어 진다.
	// 고로 포인터만 넘겨도 무관
	pDestObject->s_pSequenceMng = s_pSequenceMng;
}

void sLOAD_KFM::Delete()
{
#ifdef CRASHDUMP_0326		// chu8820 - ActorMng 삭제 실패 ( C014 )
	if( s_pActorMng != NULL )
	{
		if( s_pActorMng->GetRefCount() != 0 )
		{
			s_pActorMng = 0;
		}
		else
		{
			DUMPLOG( " C014 - 1 " );
		}
	}
	else
		DUMPLOG( " C014 - 2 " );	
#endif
//	s_pActorMng = 0;
	s_pABVWire = NULL;

	if( s_pSequenceMng )
	{
		s_pSequenceMng->Delete();
		SAFE_NIDELETE( s_pSequenceMng );
	}

	sRES_LOAD::Delete();
	s_dwChageApplyModel = 0;
}