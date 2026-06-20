

#include "stdafx.h"
#include "ThreadProc_Load.h"

void CsC_ThreadProc_Load::PushUnit_NoThread( sTCUnit* pUnit )
{
	if( pUnit->s_pLoadedObject )
		pUnit->s_pLoadedObject->IncreaseRefOnThreadLoad();
	m_queueLoad.Put( pUnit );

	CsC_AvObject* pLoaded = (CsC_AvObject*)pUnit->s_pLoadedObject;

	_LoadChar();
	switch( pLoaded->GetLeafRTTI() )
	{
	case RTTI_EFFECTOBJECT:
		((CsC_EffectObject*)pLoaded)->CheckEffectModel( 0.0f );
		break;
	default:
		pLoaded->CheckModel();
		pLoaded->Update( 0.0f );
	}	

	assert_cs( m_queueWaitLoad.empty() && m_PostWaitqueue.empty() );
}

void CsC_ThreadProc_Load::PushUnit_Thread( sTCUnit* pUnit )
{
	if( pUnit->s_pLoadedObject )
	{
		pUnit->s_pLoadedObject->IncreaseRefOnThreadLoad();
	}

	switch( pUnit->GetLoadType() )
	{
	case sTCUnit::eNormal:
		m_queueLoad.Put( pUnit );
		break;
	case sTCUnit::eFast:
		m_queuePostLoad.Put( pUnit );
		break;
	default:
		assert_cs( false );
	}	
}

DWORD CsC_ThreadProc_Load::ThreadProc( THREAD_PROC_ARG* pArg )
{	
	CsEvent *pevStop = pArg->pevStop;
	while( TRUE )
	{
		if( *pArg->m_pbStop == TRUE )
		{
			// 로드 할게 없으면 스레드를 종료한다.
			if( _IsEnableEnd() == true )
			{
				break;
			}
			m_TimeSeqExit.SetEnableTime( true );
			m_TimeSeqExit.OffReset();
		}
		else
		{
			pevStop->Wait( 30 );
		}
		
		{
			_SYNCHRONIZE_THREAD_(&m_CS_ProcLoad);
			_LoadChar();
			_WaitQueue2LoadQueue();
		}
	}
	return 0L;
}

void CsC_ThreadProc_Load::_LoadChar()
{
	sTCUnit* pUnit;	
	while( m_queuePostLoad.Get( pUnit ) || m_queueLoad.Get( pUnit ) )	
	{
		_LoadUnit( pUnit );
		_PostWaitQueue();
	}
}

void CsC_ThreadProc_Load::_LoadUnit( sTCUnit* pUnit )
{
	if( pUnit->s_pLoadedObject && ( pUnit->s_pLoadedObject->IsEnableThreadLoad( pUnit->GetCharType() == sTCUnit::KFM_File ) == false ) )
	{
		if( m_TimeSeqExit.IsEnableTime() )
		{
			// 시간 지났다 강제 종료 시키자
			if( m_TimeSeqExit.IsEnable() == true )
			{				
#ifndef _DEBUG
				assert_cs( false );
				sTCUnit::DeleteInstance( pUnit );
				m_TimeSeqExit.Reset();
				return;
#endif
			}
		}

		switch( pUnit->GetLoadType() )
		{
		case sTCUnit::eNormal:
			m_queueWaitLoad.push( pUnit );
			break;
		case sTCUnit::eFast:
			m_PostWaitqueue.push( pUnit );
			break;
		default:
			assert_cs( false );
			break;
		}		
		return;
	}

	if( m_TimeSeqExit.IsEnableTime() )
	{
		m_TimeSeqExit.OffReset();
	}

	int nIncreaseRefCount = 0;

	// 로드 구문
	switch( pUnit->GetCharType() )
	{
	case sTCUnit::LoadFileTable:
		{
			m_CallBack_LoadFileTable();
		}
		break;
	case sTCUnit::KFM_File:
		{		
			sLOAD_KFM* kfm = sLOAD_KFM::NewInstance();						
			g_pThread->GetResMng()->GetKFMObject( kfm, pUnit->s_cFilePath, &pUnit->s_Prop );

			kfm->s_dwLoadID = pUnit->s_dwLoadID;
			kfm->s_dwChageApplyModel = pUnit->s_dwChangeApplyModelID;

			assert_cs( kfm->s_dwLoadID != 0 );
			pUnit->s_pLoadedObject->ThreadCallBack_LoadKFM( kfm );
			++nIncreaseRefCount;
		}		
		break;

	case sTCUnit::NIF_File:
		{
			sLOAD_NIF* nif = sLOAD_NIF::NewInstance();			
			g_pThread->GetResMng()->GetNIFObject( nif, pUnit->s_cFilePath, &pUnit->s_Prop );

			nif->s_dwLoadID = pUnit->s_dwLoadID;
			assert_cs( nif->s_dwLoadID != 0 );
			pUnit->s_pLoadedObject->ThreadCallBack_LoadNIF( nif );
			++nIncreaseRefCount;
		}
		break;

	case sTCUnit::Part_Delete:
		{
			sTCUnitPart* pUnitPart = (sTCUnitPart*)pUnit;

			sLOAD_PART* part = sLOAD_PART::NewInstance();
			part->s_dwLoadID = pUnitPart->s_dwLoadID;
			part->s_nPartIndex = pUnitPart->s_nPartIndex;
			part->s_pRefLoad = NULL;
			part->s_pNiNode = NULL;

			CsC_PartObject* pPartObject = (CsC_PartObject*)pUnit->s_pLoadedObject;
			pPartObject->ThreadCallBack_AttachPart( part );
			
			++nIncreaseRefCount;
		}
		break;

	case sTCUnit::All_Part_File:
		{
			char cPartPath[ OBJECT_PATH_LEN ];

			assert_cs( pUnit->s_pLoadedObject->IsKindOf( RTTI_PARTOBJECT ) == true );

			sTCUnitPart* pUnitPart = (sTCUnitPart*)pUnit;

			CsC_PartObject* pPartObject = (CsC_PartObject*)pUnit->s_pLoadedObject;
			CsC_Thread_ResMng* pResMng = g_pThread->GetResMng();
			std::queue< sLOAD_PART* > queuePart;				

			switch( pUnitPart->s_ePartType & sTCUnitPart::MASK_PART )
			{
			case sTCUnitPart::PT_PART:
				{					
					for( int i=0; i<nsPART::MAX_APPLY_STATE_COUNT; ++i )	
					{
						sprintf_s( cPartPath, OBJECT_PATH_LEN, "%s\\%s", pPartObject->GetWorkingFolder(), pUnitPart->s_PartInfo[ i ].s_cPartName );

						sLOAD_PART* part = sLOAD_PART::NewInstance();						
						pResMng->GetPart( part, cPartPath );

						part->s_dwLoadID = pUnit->s_dwLoadID;
						part->s_nFileTableID = pUnitPart->s_PartInfo[ i ].s_nFileTableID;
						part->s_nPartIndex = i;
						queuePart.push( part );							
					}
					pPartObject->ThreadCallBack_AttachPart( &queuePart );
					++nIncreaseRefCount;
				}
				break;
			case sTCUnitPart::PT_COSTUME_PUTON:
				{					
					sprintf_s( cPartPath, OBJECT_PATH_LEN, "%s\\%s", pPartObject->GetWorkingFolder(), pUnitPart->s_PartInfo[ 0 ].s_cPartName );

					sLOAD_PART* part = sLOAD_PART::NewInstance();					
					pResMng->GetPart( part, cPartPath );

					part->s_dwLoadID = pUnit->s_dwLoadID;
					part->s_nFileTableID = pUnitPart->s_PartInfo[ 0 ].s_nFileTableID;
					part->s_nPartIndex = nsPART::Costume;
					pPartObject->ThreadCallBack_AttachPart( part );
					++nIncreaseRefCount;
				}
				break;
			default:
				assert_cs( false );
			}

			switch( pUnitPart->s_ePartType & sTCUnitPart::MASK_ATTACH )
			{
			case sTCUnitPart::NONE_DATA:
				break;
			case sTCUnitPart::AT_ATTACH_PUTON:
				{					
					for( int i=nsPART::MAX_CHANGE_PART_COUNT; i<nsPART::MAX_ATTACH_COUNT; ++i )	
					{
						if( pUnitPart->s_AttachInfo[ i - nsPART::MAX_CHANGE_PART_COUNT ].s_nFileTableID == 0 )
							continue;

						sprintf_s( cPartPath, OBJECT_PATH_LEN, "%s\\%s", pPartObject->GetWorkingFolder(), pUnitPart->s_AttachInfo[ i - nsPART::MAX_CHANGE_PART_COUNT ].s_cPartName );

						sLOAD_PART* part = sLOAD_PART::NewInstance();						
						pResMng->GetPart( part, cPartPath );

						part->s_dwLoadID = pUnit->s_dwLoadID;
						part->s_nFileTableID = pUnitPart->s_AttachInfo[ i - nsPART::MAX_CHANGE_PART_COUNT ].s_nFileTableID;
						part->s_nPartIndex = i;

						queuePart.push( part );
					}
					pPartObject->ThreadCallBack_AttachPart( &queuePart );
					++nIncreaseRefCount;
				}
				break;			
			default:
				assert_cs( false );
			}
		}
		break;
	// 	case sTCUnit::CARD_File:
	// 		{
	// 			char cPartPath[ OBJECT_PATH_LEN ];
	// 
	// 			assert_cs( pUnit->s_pLoadedObject->IsKindOf( RTTI_CARDOBJECT ) == true );
	// 
	// 			sTCardPart* pUnitPart = (sTCardPart*)pUnit;
	// 
	// 			CsC_CardObject* pPartObject = (CsC_CardObject*)pUnit->s_pLoadedObject;
	// 			CsC_Thread_ResMng* pResMng = g_pThread->GetResMng();
	// 			std::queue< sLOAD_PART* > queuePart;				
	// 
	// 			switch( pUnitPart->s_ePartType & sTCardPart::MASK_PART )
	// 			{
	// 			case sTCardPart::PT_PART:
	// 				{
	// 					for( int i=0; i<nsCard::MAX_TOTAL_COUNT; ++i )	
	// 					{
	// // 						if(i == nsCard::TD || i == nsCard::TE || i == nsCard::MFD )
	// // 							continue;
	// 
	// 						sprintf_s( cPartPath, OBJECT_PATH_LEN, "%s\\%s", pPartObject->GetWorkingFolder(), pUnitPart->s_PartInfo[ i ].s_cPartPath );
	// 
	// 						sLOAD_PART* part = sLOAD_PART::NewInstance();						
	// 						pResMng->GetPart( part, cPartPath );
	// 						
	// 						part->s_dwLoadID = pUnit->s_dwLoadID;
	// 						part->s_nFileTableID = 0;
	// 						part->s_nPartIndex = i;
	// 						queuePart.push( part );							
	// 					}
	// 					pPartObject->ThreadCallBack_AttachPart( &queuePart );
	// 					++nIncreaseRefCount;
	// 				}
	// 				break;	
	// 			default:
	// 				assert_cs( false );
	// 			}
	// 		}
	// 		break;
		case sTCUnit::DPart_File:
		{
			assert_cs(pUnit->s_pLoadedObject->IsKindOf(RTTI_DPARTOBJECT) == true);
			sDCUnitPart* pUnitPart = (sDCUnitPart*)pUnit;
			CsC_Thread_ResMng* pResMng = g_pThread->GetResMng();
			if (pResMng)
			{
				sLOAD_PART* part = sLOAD_PART::NewInstance();
				pResMng->GetPart(part, pUnit->s_cFilePath);

				part->s_dwLoadID = pUnitPart->s_dwLoadID;
				part->s_nPartIndex = pUnitPart->s_nPartIndex;
				//part->s_nFileTableID = pUnitPart->s_nFileTableID;
				//part->s_ePartType = sDCUnitPart::PT_SKIN;

				CsC_DPartObject* pPartObject = (CsC_DPartObject*)pUnit->s_pLoadedObject;
				pPartObject->ThreadCallBack_AttachPart(part);
			}
			++nIncreaseRefCount;
		}
		break;
	case sTCUnit::One_Part_File:
		{
			char cPartPath[ OBJECT_PATH_LEN ];

			assert_cs( pUnit->s_pLoadedObject->IsKindOf( RTTI_PARTOBJECT ) == true );

			sTCUnitPart* pUnitPart = (sTCUnitPart*)pUnit;

			CsC_PartObject* pPartObject = (CsC_PartObject*)pUnit->s_pLoadedObject;
			CsC_Thread_ResMng* pResMng = g_pThread->GetResMng();
			std::queue< sLOAD_PART* > queuePart;

			switch( pUnitPart->s_ePartType & sTCUnitPart::MASK_PART )
			{
			case sTCUnitPart::NONE_DATA:
				break;
			case sTCUnitPart::PT_PART:
				{					
					sprintf_s( cPartPath, OBJECT_PATH_LEN, "%s\\%s", pPartObject->GetWorkingFolder(), pUnitPart->s_PartInfo[ 0 ].s_cPartName );
					sLOAD_PART* part = sLOAD_PART::NewInstance();					
					pResMng->GetPart( part, cPartPath );

					part->s_dwLoadID = pUnit->s_dwLoadID;
					part->s_nFileTableID = pUnitPart->s_PartInfo[ 0 ].s_nFileTableID;
					part->s_nPartIndex = pUnitPart->s_nPartIndex;
					part->s_ePartType = sTCUnitPart::PT_PART;

					pPartObject->ThreadCallBack_AttachPart( part );
					++nIncreaseRefCount;
				}
				break;
			case sTCUnitPart::PT_COSTUME_PUTON:
				{					
					sprintf_s( cPartPath, OBJECT_PATH_LEN, "%s\\%s", pPartObject->GetWorkingFolder(), pUnitPart->s_PartInfo[ 0 ].s_cPartName );
					sLOAD_PART* part = sLOAD_PART::NewInstance();					
					pResMng->GetPart( part, cPartPath );

					part->s_dwLoadID = pUnit->s_dwLoadID;
					part->s_nFileTableID = pUnitPart->s_PartInfo[ 0 ].s_nFileTableID;
					part->s_nPartIndex = nsPART::Costume;
					part->s_ePartType = sTCUnitPart::PT_COSTUME_PUTON;

					pPartObject->ThreadCallBack_AttachPart( part );
					++nIncreaseRefCount;
				}
				break;
			case sTCUnitPart::PT_COSTUME_PUTOFF:
				{					
					for( int i=0; i<nsPART::MAX_APPLY_STATE_COUNT; ++i )	
					{
						sprintf_s( cPartPath, OBJECT_PATH_LEN, "%s\\%s", pPartObject->GetWorkingFolder(), pUnitPart->s_PartInfo[ i ].s_cPartName );
						sLOAD_PART* part = sLOAD_PART::NewInstance();						
						pResMng->GetPart( part, cPartPath );

						part->s_dwLoadID = pUnit->s_dwLoadID;
						part->s_nFileTableID = pUnitPart->s_PartInfo[ i ].s_nFileTableID;
						part->s_nPartIndex = i;
						part->s_ePartType = sTCUnitPart::PT_COSTUME_PUTOFF;

						queuePart.push( part );							
					}
					pPartObject->ThreadCallBack_AttachPart( &queuePart );
					++nIncreaseRefCount;
				}
				break;
			default:
				assert_cs( false );
			}

			switch( pUnitPart->s_ePartType & sTCUnitPart::MASK_ATTACH )
			{
			case sTCUnitPart::NONE_DATA:
				break;
			case sTCUnitPart::AT_ATTACH_PUTON:
				{					
					sprintf_s( cPartPath, OBJECT_PATH_LEN, "%s\\%s", pPartObject->GetWorkingFolder(), pUnitPart->s_PartInfo[ 0 ].s_cPartName );
					sLOAD_PART* part = sLOAD_PART::NewInstance();					
					pResMng->GetPart( part, cPartPath );

					part->s_dwLoadID = pUnit->s_dwLoadID;
					part->s_nFileTableID = pUnitPart->s_PartInfo[ 0 ].s_nFileTableID;
					part->s_nPartIndex = pUnitPart->s_nPartIndex;
					part->s_ePartType = sTCUnitPart::AT_ATTACH_PUTON;

					pPartObject->ThreadCallBack_AttachPart( part );
					++nIncreaseRefCount;
				}
				break;			
			default:
				assert_cs( false );
			}
		}
		break;
	default:
		assert_cs( false );
	}

	if( pUnit->s_pLoadedObject )
	{
		pUnit->s_pLoadedObject->ThreadCallBack_LoadCompleate( pUnit->GetCharType(), nIncreaseRefCount );
	}

	// 사용 했으니 집어 넣자
	sTCUnit::DeleteInstance( pUnit );	
}

void CsC_ThreadProc_Load::_WaitQueue2LoadQueue()
{
	while( m_queueWaitLoad.size() )
	{
		sTCUnit* pUnit = m_queueWaitLoad.front();
		m_queueWaitLoad.pop();
		m_queueLoad.Put( pUnit );
	}
}

void CsC_ThreadProc_Load::_PostWaitQueue()
{
	if( m_PostWaitqueue.empty() == true )
		return;

	while( m_PostWaitqueue.size() )
	{
		sTCUnit* pUnit = m_PostWaitqueue.front();
		m_PostWaitqueue.pop();
		m_queuePostLoad.Put( pUnit );
	}

	Sleep( 10 );
}

