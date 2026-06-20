
#include "stdafx.h"
#include "AlphaProperty.h"

// 알파블랜딩 타임관련
#define CHAR_DEST_ALPHA_VALUE		0.0f			// 아래 목적 알파 값
#define CHAR_DELTA_ALPHA_VALUE		1.0f			// 변화하는 알파 양


THREAD_MEMPOOL_CPP( CsC_AlphaProp )

#define PARENT_CALSS		CsC_Property

CsC_AlphaProp::CsC_AlphaProp():m_fDestAlphaTime(CHAR_DEST_ALPHA_TIME),m_fCurAlphaValue(0.0f)
,m_bUseDistAlpha(false),m_bHideDistOut(false)
{
	m_eType = CsC_Property::ALPHA;	
}

CsC_AlphaProp::~CsC_AlphaProp()
{
	m_eType = CsC_Property::ALPHA;	
}


void CsC_AlphaProp::PreDelete()
{
	PARENT_CALSS::PreDelete();
}

void CsC_AlphaProp::ChangeKFM()
{
	PARENT_CALSS::ChangeKFM();
	SetCurAlphaValue( AT_ALPHA_INCREASE, 0.0f );
}

void CsC_AlphaProp::Init( CsC_AvObject* pParent, eALPHA_TYPE eAlphaType, float fInitAlphaValue )
{
	PARENT_CALSS::Init( pParent );

	m_fCurAlphaValue = fInitAlphaValue;
	m_eAlphaType = eAlphaType;
	m_bUseDistAlpha = false;
	m_bHideDistOut = true;

	m_fDestAlphaTime = CHAR_DEST_ALPHA_TIME;
}

void CsC_AlphaProp::CheckModelComp()
{
	SetCurAlphaValue( m_fCurAlphaValue );
}

void CsC_AlphaProp::Update( float fDeltaTime )
{
	assert_cs( m_pParent->IsLoad() == true );

	m_bHideDistOut = false;
	_UpdateDist( fDeltaTime );
	_UpdateType( fDeltaTime );
}

void CsC_AlphaProp::_UpdateDist( float fDeltaTime )
{
	if( m_bUseDistAlpha == false )
		return;

	float fLength = ( CAMERA_ST.GetWorldTranslate() - m_pParent->GetPos() ).Length();

	if( fLength > m_pParent->GetAlphaHideDist() )
	{
		if( m_fCurAlphaValue > CHAR_DEST_ALPHA_VALUE )
		{
			SetType( AT_ALPHA_DECREASE );
		}		
		m_bHideDistOut = true;
	}
	else if( fLength < m_pParent->GetAlphaHideDist() - 100.0f )
	{
		if( m_fCurAlphaValue < 1.0f )
		{
			SetType( AT_ALPHA_INCREASE );
		}
	}
}

void CsC_AlphaProp::_UpdateType( float fDeltaTime )
{
	switch( m_eAlphaType )
	{
	case AT_STOP:
		break;
	case AT_KEEP:
		{
			m_eAlphaType = AT_STOP;
			if( m_fCurAlphaValue == 1.0f )
			{
				break;
			}
			assert_cs( m_fCurAlphaValue < 1.0f );

			_SetMaterialAlpha( m_fCurAlphaValue );
		}		
		break;
	case AT_ALPHA_DECREASE:
		if( ( m_fCurAlphaValue -= ( CHAR_DELTA_ALPHA_VALUE/m_fDestAlphaTime )*fDeltaTime ) <= CHAR_DEST_ALPHA_VALUE )
		{
			m_fCurAlphaValue = CHAR_DEST_ALPHA_VALUE;			
			m_eAlphaType = AT_STOP;
		}
		_SetMaterialAlpha( m_fCurAlphaValue );		
		break;
	case AT_ALPHA_INCREASE:
		if( ( m_fCurAlphaValue += ( CHAR_DELTA_ALPHA_VALUE/m_fDestAlphaTime )*fDeltaTime ) >= 1.0f )
		{	
			m_fCurAlphaValue = 1.0f;			
			m_eAlphaType = AT_STOP;
		}		
		_SetMaterialAlpha( m_fCurAlphaValue );
		break;	
	}
}

void CsC_AlphaProp::SetCurAlphaValue( float fAlphaValue )
{
	m_fCurAlphaValue = fAlphaValue;
	
	switch( m_eAlphaType )
	{
	case AT_STOP:
		{
			if( m_fCurAlphaValue < 1.0f )
			{
				m_eAlphaType = AT_KEEP;
			}
		}
		break;
	}
	if( m_fCurAlphaValue >= 1.0f )
	{
		m_fCurAlphaValue = 1.0f;
	}

	if( m_pParent->IsLoad() == true )
	{
		_SetMaterialAlpha( m_fCurAlphaValue );
	}
}

void CsC_AlphaProp::SetCurAlphaValue( eALPHA_TYPE type, float fAlphaValue )
{
	m_eAlphaType = type;
	SetCurAlphaValue( fAlphaValue );
}

void CsC_AlphaProp::_SetMaterialAlpha( float fAlpha )
{
	if(m_pParent == NULL )
		return;

	assert_cs( m_pParent->IsLoad() == true );	
	CsNodeObj* pCsNode = m_pParent->GetCsNode();
	if( pCsNode == NULL )
		return;

	int nCount = pCsNode->m_vpGeom.Size();
	CGeometry* pGeom = NULL;
	for( int i=0; i<nCount; ++i )
	{
		pGeom =	pCsNode->m_vpGeom[ i ];
		switch( pGeom->GetType() )
		{
		case CGeometry::CharOutLine:
		case CGeometry::CharOutLine_Gray:
		case CGeometry::MeshOutLine:
		case CGeometry::MeshOutLine_Gray:
			pGeom->SetAlpha( fAlpha );
			break;
		}
	}	
}

#undef PARENT_CLASS
