

#include "stdafx.h"
#include "ThreadProc_Delete.h"

DWORD CsC_ThreadProc_Delete::ThreadProc( THREAD_PROC_ARG* pArg )
{
	CsEvent *pevStop = pArg->pevStop;	

	while( TRUE )
	{
		if( *pArg->m_pbStop == TRUE )
		{
			// 지울게 없다면
			if( _IsEnableEnd() == true )
				break;

			m_TimeSeqExit.SetEnableTime( true );
			m_TimeSeqExit.OffReset();
		}
		else
		{
			pevStop->Wait( 100 );
		}
		
		//if( CsKWaitTimeout( dwRet ) )
		{
			assert_cs( m_queueWaitDelete.empty() == true );

			_DeleteChar();
			_CheckWaitDelete();
		}	
	}
	return 0L;
}

void CsC_ThreadProc_Delete::_DeleteChar()
{
	sTCUnit* pUnit;
	while( m_queueDelete.Get( pUnit ) )
	{
		if( pUnit->s_pLoadedObject->IsDelete() == true )
		{
			if( m_TimeSeqExit.IsEnableTime() )
			{
				m_TimeSeqExit.OffReset();
			}


			pUnit->s_pLoadedObject->Delete();
			switch( pUnit->s_pLoadedObject->GetLeafRTTI() )
			{
			case RTTI_AVOBJECT:				
				CsC_AvObject::DeleteInstance( (CsC_AvObject*)pUnit->s_pLoadedObject );
				break;
			case RTTI_EFFECTOBJECT:
				CsC_EffectObject::DeleteInstance( (CsC_EffectObject*)pUnit->s_pLoadedObject );
				break;
			case RTTI_PARTOBJECT:
				CsC_PartObject::DeleteInstance( (CsC_PartObject*)pUnit->s_pLoadedObject );
				break;
			case RTTI_EVENTOBJECT:
				CsC_EventObject::DeleteInstance( (CsC_EventObject*)pUnit->s_pLoadedObject );
				break;
			case RTTI_EVENTTAMEROBJECT:
				CsC_EventTamerObject::DeleteInstance( (CsC_EventTamerObject*)pUnit->s_pLoadedObject );
				break;
// 			case RTTI_CARDOBJECT:
// 				CsC_CardObject::DeleteInstance( (CsC_CardObject*)pUnit->s_pLoadedObject );
// 				break;
			case RTTI_DPARTOBJECT:
				CsC_DPartObject::DeleteInstance((CsC_DPartObject*)pUnit->s_pLoadedObject);
				break;
			default:
				// 콜백 함수 호출
				if( m_CallBackDelete( pUnit ) == false )
				{
					assert_cs( false );
				}
			}
			// 사용 했으니 집어 넣자
			sTCUnit::DeleteInstance( pUnit );
		}
		else
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
					continue;
#endif
				}
			}

			// 제거가 안될때는 대기큐에 집어 넣자
			m_queueWaitDelete.push( pUnit );
		}
	}
}

void CsC_ThreadProc_Delete::_CheckWaitDelete()
{
	while( m_queueWaitDelete.empty() == false )
	{
		sTCUnit* pUnit = m_queueWaitDelete.front();
		m_queueWaitDelete.pop();
		m_queueDelete.Put( pUnit );
	}
}