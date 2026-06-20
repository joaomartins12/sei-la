
#include "stdafx.h"
#include "11_LoadSequence.h"

THREAD_MEMPOOL_CPP( CsC_LoadSequence::sTEXTKEY_SHADER )
THREAD_MEMPOOL_CPP( CsC_LoadSequence::sTEXTKEY_EVENT )
THREAD_MEMPOOL_CPP( CsC_LoadSequence::sSEQUENCE_INFO )


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CsC_LoadSequence::sSEQUENCE_INFO::Delete()
{
	int nSize = s_vpTextKeyEvent.Size();
	for( int i=0; i<nSize; ++i )
	{
		CsC_LoadSequence::sTEXTKEY_EVENT::DeleteInstance( s_vpTextKeyEvent[ i ] );
	}
	s_vpTextKeyEvent.Destroy();	
	s_vpNAtt_EffectIndexArray.Destroy();

	nSize = s_vpTextKeyShader.Size();
	for( int i=0; i<nSize; ++i )
	{
		CsC_LoadSequence::sTEXTKEY_SHADER::DeleteInstance( s_vpTextKeyShader[ i ] );
	}
	s_vpTextKeyShader.Destroy();	

	s_dwSequenceID = 0;
	s_fEndTime = 0.0f;
	s_nLoopCount = 0;
	s_nUseStopFrameCount = 0;

	// 공격 정보 관련
	s_nNAtt_Count = 0;
	s_nLast_NAtt_SoundIndex = 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CsC_LoadSequence::CsC_LoadSequence()
{ 
	m_bApplyModel = false; 
	m_dwModelID = 0;
}

CsC_LoadSequence::~CsC_LoadSequence()
{ 
	m_bApplyModel = false; 
	m_dwModelID = 0;
}

void CsC_LoadSequence::Delete()
{
	std::map< DWORD, sSEQUENCE_INFO* >::iterator itSeq = m_mapSequence.begin();
	std::map< DWORD, sSEQUENCE_INFO* >::iterator itSeqEnd = m_mapSequence.end();
	for( ; itSeq!=itSeqEnd; ++itSeq )
	{
		itSeq->second->Delete();
		sSEQUENCE_INFO::DeleteInstance( itSeq->second );
	}
	m_mapSequence.clear();

	m_dwModelID = 0;
	m_bApplyModel = false;	
}

void CsC_LoadSequence::Init( NiActorManager* pActorMng, DWORD dwModelID )
{
	assert_cs( m_bApplyModel == false );
	assert_cs( m_mapSequence.empty() == true );

	UINT* nIDs = NULL;
	UINT nSeqCnt = 0;
	m_dwModelID = dwModelID;

	NiKFMTool* pKFMTool = pActorMng->GetKFMTool();
	pKFMTool->GetSequenceIDs( nIDs, nSeqCnt );

	if( nIDs == NULL )
		return;

	NiControllerSequence* pContSeq;
	UINT nSrcSeqID;
	for(UINT nSrc=0; nSrc<nSeqCnt; ++nSrc)
	{
		nSrcSeqID = pKFMTool->GetSequence( nIDs[ nSrc ] )->GetSequenceID();

		pContSeq = pActorMng->GetSequence( nSrcSeqID );
		if( pContSeq == NULL )
			continue;		

		sSEQUENCE_INFO* pInfo = sSEQUENCE_INFO::NewInstance();
		assert_csm1( pContSeq->GetBeginKeyTime() == 0.0f, "%f", pContSeq->GetBeginKeyTime() );
		pInfo->s_dwSequenceID	= nSrcSeqID;
		pInfo->s_fEndTime		= pContSeq->GetEndKeyTime();
		pInfo->s_eCyleType		= pContSeq->GetCycleType();

		// 텍스트키 지워주자 ( 괜히 콜백이라던지 연산 들어 가니까 )
		pContSeq->SetTextKeys( NULL );
		m_mapSequence[ nSrcSeqID ] = pInfo;
	}

	NiFree ( nIDs );
}


void CsC_LoadSequence::ApplyModelData( DWORD dwModelID )
{
	if( m_bApplyModel == true )
		return;

	m_bApplyModel = true;
	assert_cs( m_dwModelID == dwModelID );

	_SYNCHRONIZE_THREAD_(&m_CS);

	if( g_pModelDataMng->IsData( dwModelID ) == false )
	{
		assert_csm1( false, L"Model Data Empty = %d", dwModelID );
		return;
	}

	CsModelData* pModelData = g_pModelDataMng->GetData( dwModelID );
	CsModelData::sSEQUENCE* pModelSeq;

	std::map< DWORD, sSEQUENCE_INFO* >::iterator itSeq = m_mapSequence.begin();
	std::map< DWORD, sSEQUENCE_INFO* >::iterator itSeqEnd = m_mapSequence.end();
	for( ; itSeq!=itSeqEnd; ++itSeq )
	{
		assert_cs( itSeq->second->s_vpTextKeyEvent.Size() == 0 );
		assert_cs( itSeq->second->s_vpTextKeyShader.Size() == 0 );
		assert_cs( itSeq->second->s_vpNAtt_EffectIndexArray.Size() == 0 );
		assert_cs( itSeq->second->s_nNAtt_Count == 0 );
		itSeq->second->s_nUseStopFrameCount = 0;

		if( pModelData->IsSequence( itSeq->second->s_dwSequenceID ) == true )
		{
			pModelSeq = pModelData->GetSequence( itSeq->second->s_dwSequenceID );

			itSeq->second->s_nLoopCount = pModelSeq->s_SequenceInfo.s_nLoopCnt;
			_ApplyModel_Event( this, itSeq->second, pModelSeq );
			_ApplyModel_Shader( this, itSeq->second, pModelSeq );
		}		
	}
}

void CsC_LoadSequence::ResetApplyModelData( DWORD dwModelID )
{
	assert_cs( m_mapSequence.empty() == false );

	m_bApplyModel = true;
	assert_cs( m_dwModelID == dwModelID );

	_SYNCHRONIZE_THREAD_(&m_CS);

	if( g_pModelDataMng->IsData( dwModelID ) == false )
	{
		assert_csm1( false, L"Model Data Empty = %d", dwModelID );
		return;
	}

	CsModelData* pModelData = g_pModelDataMng->GetData( dwModelID );
	CsModelData::sSEQUENCE* pModelSeq;
	std::map< DWORD, sSEQUENCE_INFO* >::iterator itSeq = m_mapSequence.begin();
	std::map< DWORD, sSEQUENCE_INFO* >::iterator itSeqEnd = m_mapSequence.end();
	for( ; itSeq!=itSeqEnd; ++itSeq )
	{		
		// 기존 정보 제거
		itSeq->second->s_nUseStopFrameCount = 0;
		int nOldEventCount = itSeq->second->s_vpTextKeyEvent.Size();
		for( int nEvent=0; nEvent<nOldEventCount; ++nEvent )
		{
			sTEXTKEY_EVENT::DeleteInstance( itSeq->second->s_vpTextKeyEvent[ nEvent ] );
		}
		int nOldShaderCount = itSeq->second->s_vpTextKeyShader.Size();
		for( int nShader=0; nShader<nOldShaderCount; ++nShader )
		{
			sTEXTKEY_SHADER::DeleteInstance( itSeq->second->s_vpTextKeyShader[ nShader ] );
		}
		itSeq->second->s_vpTextKeyEvent.ClearElement();
		itSeq->second->s_vpTextKeyShader.ClearElement();
		itSeq->second->s_vpNAtt_EffectIndexArray.ClearElement();
		itSeq->second->s_nNAtt_Count = 0;

		if( pModelData->IsSequence( itSeq->second->s_dwSequenceID ) == true )
		{
			pModelSeq = pModelData->GetSequence( itSeq->second->s_dwSequenceID );

			itSeq->second->s_nLoopCount = pModelSeq->s_SequenceInfo.s_nLoopCnt;
			_ApplyModel_Event( this, itSeq->second, pModelSeq );
			_ApplyModel_Shader( this, itSeq->second, pModelSeq );
		}
	}
}

void CsC_LoadSequence::_ApplyModel_Event( CsC_LoadSequence* pParent, sSEQUENCE_INFO* pSeq, CsModelData::sSEQUENCE* pModelSeq )
{
	CsModelData::sEVENT* pModelEvent;
	int nEventCount = pModelSeq->s_SequenceInfo.s_nEventCount;
	for( int nEvent=0; nEvent<nEventCount; ++nEvent )
	{
		pModelEvent = pModelSeq->GetEvent( nEvent );

		sTEXTKEY_EVENT* pEvent = sTEXTKEY_EVENT::NewInstance();
		pEvent->s_fEventTime = pModelEvent->s_fEventTime;
		pEvent->s_eType = (CsC_LoadSequence::sTEXTKEY_EVENT::eEVENT_TYPE)pModelEvent->s_nType;
		pEvent->s_dwPlag = pModelEvent->s_dwPlag;
		pEvent->s_vOffset = pModelEvent->s_vOffset;
		pEvent->s_nStaticIndex = pModelEvent->s_nStaticIndex;
		pEvent->s_fEffectScale = pModelEvent->s_fEffectScale;
		pEvent->s_bParentScale = pModelEvent->s_bParentScale;
		pEvent->s_fFadeoutTime = pModelEvent->s_fFadeoutTime;
		pEvent->s_vValue1 = pModelEvent->s_vValue1;
		pEvent->s_vValue2 = pModelEvent->s_vValue2;
		strcpy_s( pEvent->s_cText, MAX_TEXTKEY_LEN, pModelEvent->s_cText );

		pSeq->s_vpTextKeyEvent.PushBack( pEvent );

		switch( pEvent->s_eType )
		{
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC:
			{
				switch( pEvent->s_nStaticIndex )
				{
				case nsSTATIC_EVENT::DYE_NORMAL_ATTACK:
				case nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE:
				case nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE:
					pSeq->s_vpNAtt_EffectIndexArray.PushBack( nEvent );
					++pSeq->s_nNAtt_Count;
					assert_cs( pSeq->s_vpNAtt_EffectIndexArray.Size() == pSeq->s_nNAtt_Count );
					break;
				}
			}
			break;
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_DS_STATIC:
			{
				if( ( pEvent->s_nStaticIndex >= nsSTATIC_EVENT::DYS_DMG_ATT_L )&&
					( pEvent->s_nStaticIndex <= nsSTATIC_EVENT::DYS_DMG_ATT_POWER ) )
				{
					pSeq->s_nLast_NAtt_SoundIndex = nEvent;
				}
			}
			break;
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_S_FILE:
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_E_FILE:
			break;
		default:
			assert_cs( false );
		}
	}
}


void CsC_LoadSequence::_ApplyModel_Shader( CsC_LoadSequence* pParent, sSEQUENCE_INFO* pSeq, CsModelData::sSEQUENCE* pModelSeq )
{
	CsModelData::sSHADER* pModelShader;
	int nShaderCount = pModelSeq->s_SequenceInfo.s_nShaderCnt;
	for( int nShader=0; nShader<nShaderCount; ++nShader )
	{
		pModelShader = pModelSeq->GetShader( nShader );

		sTEXTKEY_SHADER* pShader = sTEXTKEY_SHADER::NewInstance();
		pShader->s_eShaderType = pModelShader->s_eShaderType;
		pShader->s_nValue1 = pModelShader->s_nValue1;
		pShader->s_fValue1 = pModelShader->s_fValue1;
		pShader->s_fValue2 = pModelShader->s_fValue2;
		pShader->s_fValue3 = pModelShader->s_fValue3;
		strcpy_s( pShader->s_cApplyObjectName, 32, pModelShader->s_cApplyObjectName );

		pSeq->s_vpTextKeyShader.PushBack( pShader );
	}
}

CsC_LoadSequence::sSEQUENCE_INFO* CsC_LoadSequence::GetSequence( DWORD dwAniID )
{
	if( IsSequence( dwAniID ) )
		return m_mapSequence[ dwAniID ];
		
	return NULL;
}
