
#include "stdafx.h"
#include "ModelData.h"

void CsModelData::sSEQUENCE::Delete()
{
	int nSize = s_vpEvent.Size();
	for( int i=0; i<nSize; ++i )
	{
		NiDelete s_vpEvent[ i ];
	}
	s_vpEvent.Destroy();

	nSize = s_vpShader.Size();
	for( int i=0; i<nSize; ++i )
	{
		NiDelete s_vpShader[ i ];
	}
	s_vpShader.Destroy();
}

void CsModelData::sSEQUENCE::DeleteEvent( int nIndex )
{
	NiDelete s_vpEvent[ nIndex ];
	s_vpEvent.DeleteElement( nIndex );
	--s_SequenceInfo.s_nEventCount;
}

void CsModelData::sSEQUENCE::InsertEvent( sEVENT* pEvent )
{
	CsVectorPB< sEVENT* >	vpSort;
	int nSize = s_vpEvent.Size();
	for( int i=0; i<nSize; ++i )
	{
		vpSort.PushBack( s_vpEvent[ i ] );
	}
	vpSort.PushBack( pEvent );
	s_vpEvent.ClearElement();

	// Á¤·Ä
	sEVENT* pInsertEvent = NULL;
	nSize = vpSort.Size();
	while( nSize )
	{
		pInsertEvent = vpSort[ 0 ];
		for( int i=1; i<nSize; ++i )
		{
			if( pInsertEvent->s_fEventTime > vpSort[ i ]->s_fEventTime )
			{
				pInsertEvent = vpSort[ i ];
			}
		}
		vpSort.DeleteElement_FromData( pInsertEvent );
		s_vpEvent.PushBack( pInsertEvent );
		nSize = vpSort.Size();
	}

	s_SequenceInfo.s_nEventCount = s_vpEvent.Size();
}

void CsModelData::sSEQUENCE::DeleteShader( int nIndex )
{
	NiDelete s_vpShader[ nIndex ];
	s_vpShader.DeleteElement( nIndex );
	--s_SequenceInfo.s_nShaderCnt;
}

void CsModelData::sSEQUENCE::InsertShader( sSHADER* pShader )
{
	s_vpShader.PushBack( pShader );
	s_SequenceInfo.s_nShaderCnt = s_vpShader.Size();
}

//////////////////////////////////////////////////////////////////////////
CsModelData::CsModelData()
{

}

CsModelData::~CsModelData()
{

}

void CsModelData::Delete()
{
	std::map< DWORD, sSEQUENCE* >::iterator it = m_mapSequence.begin();
	std::map< DWORD, sSEQUENCE* >::iterator itEnd = m_mapSequence.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		NiDelete it->second;
	}
	m_mapSequence.clear();	
}

void CsModelData::InsertSequence( DWORD dwSequenceID, sSEQUENCE* pSequence )
{
	assert_cs( IsSequence( dwSequenceID ) == false );
	m_mapSequence[ dwSequenceID ] = pSequence;

	m_Info.s_nSequenceCount = (int)m_mapSequence.size();
}

void CsModelData::AddSequence( DWORD dwSequenceID )
{
	sSEQUENCE* pSequence = NiNew sSEQUENCE;
	pSequence->s_SequenceInfo.s_dwSequenceID = dwSequenceID;
	InsertSequence( dwSequenceID, pSequence );
}