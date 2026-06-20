
#include "stdafx.h"
#include "EffectProperty.h"

THREAD_MEMPOOL_CPP( CsC_EffectProp )

#define PARENT_CALSS		CsC_Property

void CsC_EffectProp::GlobalEffectProp( sCREATEINFO* pProp )
{
	pProp->s_dwPropPlag = PR_ANI_ENABLE;
	pProp->s_dwOptionPlag = 0;
	pProp->s_eInsertGeomType = CGeometry::Normal;
}

CsC_EffectProp::CsC_EffectProp()
{	
	m_eType = CsC_Property::EFFECT;		

	for( int i=0; i<LE_MAX_COUNT; ++i )
		m_LoopEffectArray[ i ] = NULL;
}

CsC_EffectProp::~CsC_EffectProp()
{	
}

void CsC_EffectProp::Delete()
{
	PARENT_CALSS::Delete();	

	for( int i=0; i<LE_MAX_COUNT; ++i )
	{
		m_LoopEffectArray[ i ] = NULL;
	}

	int nSize = m_vpEffect.Size();
	for( int i=0; i<nSize; ++i )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = m_vpEffect[ i ]; 
		g_pThread->DeleteChar( pUnit );
	}
	m_vpEffect.Destroy();

	nSize = m_vpEffectSound.Size();
	for( int i=0; i<nSize; ++i )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = m_vpEffectSound[ i ]->s_pEffect; 
		g_pThread->DeleteChar( pUnit );
		NiDelete m_vpEffectSound[ i ];
	}
	m_vpEffectSound.Destroy();
}

void CsC_EffectProp::DeleteEventEffect()
{
	int nSize = m_vpEffect.Size();	
	for( int i=nSize; i>0; --i )
	{
		CsC_EffectObject* pEffect = (CsC_EffectObject*)m_vpEffect[ i - 1 ];
		if( ( pEffect->GetEffectPlag() & nsEFFECT::EVENT_DEL_MASK ) == nsEFFECT::EVENT_DEL_ANI )
		{
			sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
			pUnit->s_pLoadedObject = m_vpEffect[ i - 1 ]; 
			g_pThread->DeleteChar( pUnit );	
			m_vpEffect.DeleteElement( i - 1 );	
		}			
	}	

	nSize = m_vpEffectSound.Size();
	for( int i=nSize; i>0; --i )
	{
		CsC_EffectObject* pEffect = (CsC_EffectObject*)m_vpEffectSound[ i - 1 ]->s_pEffect;		
		if( ( pEffect->GetEffectPlag() & nsEFFECT::EVENT_DEL_MASK ) == nsEFFECT::EVENT_DEL_ANI )
		{
			sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
			pUnit->s_pLoadedObject = m_vpEffectSound[ i - 1 ]->s_pEffect; 
			g_pThread->DeleteChar( pUnit );	
			NiDelete m_vpEffectSound[ i - 1 ];
			m_vpEffectSound.DeleteElement( i - 1 );	
		}			
	}
}

bool CsC_EffectProp::IsDelete()
{
	int nSize = m_vpEffect.Size();
	for( int i=0; i<nSize; ++i )
	{
		if( m_vpEffect[ i ]->IsDelete() == false )
			return false;
	}
	return true;
}

void CsC_EffectProp::DeleteEffect( int nEffectIndex )
{
	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
	pUnit->s_pLoadedObject = m_vpEffect[ nEffectIndex ]; 
	g_pThread->DeleteChar( pUnit );
	m_vpEffect.DeleteElement( nEffectIndex );
}

void CsC_EffectProp::DeleteEffect( CsC_AvObject* pEffect )
{
	if( pEffect == NULL )
		return;

	for( int i=0; i<LE_MAX_COUNT; ++i )
	{
		if( m_LoopEffectArray[ i ] == pEffect )
		{		
			m_LoopEffectArray[ i ] = NULL;			
		}
	}

	int nSize = m_vpEffect.Size();
	for( int i=0; i<nSize; ++i )
	{
		if( m_vpEffect[ i ] == pEffect )
		{
			sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
			pUnit->s_pLoadedObject = m_vpEffect[ i ]; 
			g_pThread->DeleteChar( pUnit );
			m_vpEffect.DeleteElement( i );
			return;
		}
	}	

	nSize = m_vpEffectSound.Size();
	for( int i=0; i<nSize; ++i )
	{
		if( m_vpEffectSound[ i ]->s_pEffect == pEffect )
		{
			sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
			pUnit->s_pLoadedObject = m_vpEffectSound[ i ]->s_pEffect; 
			g_pThread->DeleteChar( pUnit );
			NiDelete m_vpEffectSound[ i ];
			m_vpEffectSound.DeleteElement( i );
			return;
		}
	}
}

void CsC_EffectProp::Init( CsC_AvObject* pParent )
{
	PARENT_CALSS::Init( pParent );
}

void CsC_EffectProp::Reset()
{
	for( int i=0; i<LE_MAX_COUNT; ++i )
	{
		m_LoopEffectArray[ i ] = NULL;
	}

	int nSize = m_vpEffect.Size();
	for( int i=0; i<nSize; ++i )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = m_vpEffect[ i ]; 
		g_pThread->DeleteChar( pUnit );
	}
	m_vpEffect.ClearElement();
}


void CsC_EffectProp::PreUpdate( float fDeltaTime )
{
	int nSize = m_vpEffect.Size();
	CsC_EffectObject* pEffect;
	for( int i=0; i<nSize; ++i )
	{
		pEffect = (CsC_EffectObject*)m_vpEffect[ i ];
		pEffect->Update( fDeltaTime );
		if( pEffect->IsEndEffect() == true )
		{
			DeleteEffect( i );
			--i;
			--nSize;
		}
	}

	nSize = m_vpEffectSound.Size();
	for( int i=0; i<nSize; ++i )
	{
		pEffect = (CsC_EffectObject*)m_vpEffectSound[ i ]->s_pEffect;
		pEffect->Update( fDeltaTime );
		if( pEffect->IsEndEffect() == true )
		{
			DeleteEffect( pEffect );						
			--i;
			--nSize;
		}
		else
		{
			m_vpEffectSound[ i ]->s_fAccumTime += fDeltaTime;
			std::list< CsEffectSound::sSound_Info* >::iterator it = m_vpEffectSound[ i ]->s_pEffectSoundData->m_listSound.begin();
			std::list< CsEffectSound::sSound_Info* >::iterator itEnd = m_vpEffectSound[ i ]->s_pEffectSoundData->m_listSound.end();

			for( int j = 0; j < m_vpEffectSound[ i ]->s_nStep; j++ )
			{
				it++;
			}

			for( ;it != itEnd; it++ )
			{				
				if( m_vpEffectSound[ i ]->s_fAccumTime > (*it)->fSec )
				{
					PARENT_CALSS::m_pParent->PlaySound( (*it)->szFileName );
					m_vpEffectSound[ i ]->s_nStep++;
				}				
			}
		}
	}
}

void CsC_EffectProp::Render()
{
	int nSize = m_vpEffect.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_vpEffect[ i ]->Render();
	}

	nSize = m_vpEffectSound.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_vpEffectSound[ i ]->s_pEffect->Render();
	}
}

void CsC_EffectProp::RenderAbsolute()
{
	int nSize = m_vpEffect.Size();
	CsNodeObj* pNode;
	for( int i=0; i<nSize; ++i )
	{
		pNode = m_vpEffect[ i ]->GetCsNode();
		if( pNode )
		{
			pNode->RenderAbsolute();	
		}
	}

	nSize = m_vpEffectSound.Size();
	for( int i=0; i<nSize; ++i )
	{
		pNode = m_vpEffectSound[ i ]->s_pEffect->GetCsNode();
		if( pNode )
		{
			pNode->RenderAbsolute();	
		}
	}
}

CsC_AvObject* CsC_EffectProp::AddLoopEffect( eLOOP_EFFECT le, char const* cEffectPath )
{
	if( m_LoopEffectArray[ le ] != NULL )
	{
		DeleteEffect( m_LoopEffectArray[ le ] );
	}

	CsC_EffectObject* pEffect = CsC_EffectObject::NewInstance();
	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	GlobalEffectProp( &pUnit->s_Prop );
	
	size_t HashCode = CsFPS::GetHashCode( cEffectPath );	
	if( g_EffectSoundMng->IsData( HashCode ) == true )	// 이펙트 사운드 툴에서 설정되어 있다면
	{		
		CsEffectSound* pEffectSoundData = g_EffectSoundMng->GetData( HashCode );	
		
		pUnit->SetLoadType( sTCUnit::eFast );
		pUnit->s_pLoadedObject = pEffect;
		sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", pEffectSoundData->m_sESInfo.szFilePath );

		DWORD dwFlag = pEffectSoundData->m_sEffectOptin.dwEffectFlag & ~nsEFFECT::EVENT_DEL_ANI;
		if(le == LE_AURA)//오라 추가, 오라 이펙트 위치 관련 chu8820
		{
			dwFlag = nsEFFECT::LIVE_LOOP | nsEFFECT::POS_AURA | nsEFFECT::DIR_CHAR;

			pEffect->Init( m_pParent, &pUnit->s_Prop, dwFlag );
			pEffect->SetScale( m_pParent->GetToolWidth() * 0.0334f );
			m_vpEffect.PushBack( pEffect );

			m_LoopEffectArray[ le ] = pEffect;
			g_pThread->LoadChar( pUnit );

			return pEffect;
		}
#ifdef SDM_TAMER_XGUAGE_20180628
		if(le == LE_XJOGRESS)
		{
			dwFlag = nsEFFECT::LIVE_LOOP | nsEFFECT::POS_LEVEL | nsEFFECT::DIR_CAMERA_AXIS_Z;
		}
#endif
		pEffect->Init( m_pParent, &pUnit->s_Prop, dwFlag );

		float fScale;
		if( pEffectSoundData->m_sEffectOptin.bChrScale == true )
		{
			fScale = pEffectSoundData->m_sEffectOptin.fScale * m_pParent->GetToolWidth() * 0.027f;
		}
		else
		{
			fScale = pEffectSoundData->m_sEffectOptin.fScale;
		}	
		pEffect->SetScale( fScale );				

		if( ( pEffectSoundData->m_sEffectOptin.dwEffectFlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
		{
			pEffect->SetOffset( pEffectSoundData->m_sEffectOptin.vOffset );						
		}

		ES_PLAY* pData = NiNew ES_PLAY;
		pData->s_fAccumTime = 0.0f;
		pData->s_nStep = 0;
		pData->s_pEffect = pEffect;
		pData->s_pEffectSoundData = pEffectSoundData;

		m_vpEffectSound.PushBack( pData );
	}	
	else
	{
		pUnit->s_pLoadedObject = pEffect;
		sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", cEffectPath );


// #ifdef VERSION_USA
// 		DWORD dwPlag = nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::DIR_CHAR;
// #else
		DWORD dwPlag;
		switch( le )
		{
		case LE_DLEVEL:
		case LE_ENCHANT:
		case LE_SIZE:
			dwPlag = nsEFFECT::LIVE_LOOP | nsEFFECT::POS_LEVEL | nsEFFECT::DIR_CAMERA_AXIS_Z;
			break;
		case LE_PCBANG:
			dwPlag = nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::DIR_CHAR;
			break;
		//오라 이펙트 위치 관련 추가 chu8820
		case LE_AURA:
			dwPlag = nsEFFECT::LIVE_LOOP | nsEFFECT::POS_AURA | nsEFFECT::DIR_CHAR;
			break;
		default:
			dwPlag = nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::DIR_CHAR;
			break;
		}

		pEffect->Init( m_pParent, &pUnit->s_Prop, dwPlag );
		pEffect->SetScale( m_pParent->GetToolWidth() * 0.0334f );
		m_vpEffect.PushBack( pEffect );
	}

	m_LoopEffectArray[ le ] = pEffect;
	g_pThread->LoadChar( pUnit );

	return pEffect;
}

CsC_AvObject* CsC_EffectProp::GetLoopEffect( eLOOP_EFFECT le )
{
	return m_LoopEffectArray[ le ];
}

CsC_EffectObject* CsC_EffectProp::AddEffect( char* cEffectPath, CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption, float fScale )
{
	CsC_EffectObject* pEffect = CsC_EffectObject::NewInstance();

	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	GlobalEffectProp( &pUnit->s_Prop );	

	pUnit->s_pLoadedObject = pEffect;

	sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", cEffectPath );

	pEffect->Init( m_pParent, &pUnit->s_Prop, pEvent, pOption );
	pEffect->SetScale( pEvent->s_bParentScale == true ? m_pParent->GetScale() : fScale );
	pEffect->GetProp_Animation()->SetAniAccelRate( pOption->s_fAniAccelRate );

	m_vpEffect.PushBack( pEffect );
	g_pThread->LoadChar( pUnit );

	return pEffect;
}
CsC_EffectObject* CsC_EffectProp::AddEffect( const NiPoint3& kStartPos, char* cEffectPath)
{
	CsC_EffectObject* pEffect = CsC_EffectObject::NewInstance();

	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	GlobalEffectProp( &pUnit->s_Prop );	

	pUnit->s_pLoadedObject = pEffect;

	sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", cEffectPath );

	pEffect->InitForChainEffect( m_pParent, &pUnit->s_Prop, kStartPos );
	pEffect->GetProp_Animation()->SetAniAccelRate( 1.0f );
	m_vpEffect.PushBack( pEffect );
	g_pThread->LoadChar( pUnit );
	return pEffect;

}

#ifdef LEVELUP_EFFECT_LOOP
CsC_EffectObject* CsC_EffectProp::AddEffect( char* cEffectPath, float fScale, DWORD dwPlag, int eState, bool bPreLoad /*= false*/ )
{
	CsC_EffectObject* pEffect = CsC_EffectObject::NewInstance();

	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	GlobalEffectProp( &pUnit->s_Prop );
	if( bPreLoad )
	{
		pUnit->SetLoadType( sTCUnit::eFast );
	}
	pUnit->s_pLoadedObject = pEffect;

	sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", cEffectPath );

	pEffect->Init( m_pParent, &pUnit->s_Prop, dwPlag );
	pEffect->SetScale( fScale );
	pEffect->SetLevelEffect( (CsC_EffectObject::eLevelEffect)eState );
	m_vpEffect.PushBack( pEffect );
	g_pThread->LoadChar( pUnit );

	return pEffect;
}
#endif

CsC_EffectObject* CsC_EffectProp::AddEffect( char* cEffectPath, float fScale, DWORD dwPlag, bool bPreLoad /*=false*/ )
{
	CsC_EffectObject* pEffect = CsC_EffectObject::NewInstance();

	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	GlobalEffectProp( &pUnit->s_Prop );
	if( bPreLoad )
	{
		pUnit->SetLoadType( sTCUnit::eFast );
	}
	pUnit->s_pLoadedObject = pEffect;

	sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", cEffectPath );

	pEffect->Init( m_pParent, &pUnit->s_Prop, dwPlag );
	pEffect->SetScale( fScale );
	m_vpEffect.PushBack( pEffect );
	g_pThread->LoadChar( pUnit );

	return pEffect;
}

CsC_EffectObject* CsC_EffectProp::AddEffect( char* cEffectPath, NiPoint3 vPos, float fScale )
{
	CsC_EffectObject* pEffect = CsC_EffectObject::NewInstance();

	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	CsC_EffectProp::GlobalEffectProp( &pUnit->s_Prop );

	pUnit->s_pLoadedObject = pEffect;

	sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", cEffectPath );

	pEffect->Init( m_pParent, vPos, &pUnit->s_Prop );
	pEffect->SetScale( fScale );	
	m_vpEffect.PushBack( pEffect );
	g_pThread->LoadChar( pUnit );

	return pEffect;
}

CsC_EffectObject* CsC_EffectProp::AddEffect_FT_WithoutScale( char* cEffectPath, float fScale )
{
	size_t HashCode = CsFPS::GetHashCode( cEffectPath );

	CsC_EffectObject* pEffect = NULL;
	if( g_EffectSoundMng->IsData( HashCode ) == true )	
	{
		CsEffectSound* pEffectSoundData = g_EffectSoundMng->GetData( HashCode );	

		pEffect = CsC_EffectObject::NewInstance();

		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
		GlobalEffectProp( &pUnit->s_Prop );	
		pUnit->SetLoadType( sTCUnit::eFast );

		pUnit->s_pLoadedObject = pEffect;
		sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", pEffectSoundData->m_sESInfo.szFilePath );

		DWORD dwFlag = pEffectSoundData->m_sEffectOptin.dwEffectFlag & ~nsEFFECT::EVENT_DEL_ANI;
		pEffect->Init( m_pParent, &pUnit->s_Prop, dwFlag );		
		pEffect->SetScale( fScale );		

		g_pThread->LoadChar( pUnit );

		if( ( pEffectSoundData->m_sEffectOptin.dwEffectFlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
		{
			pEffect->SetOffset( pEffectSoundData->m_sEffectOptin.vOffset );						
		}

		ES_PLAY* pData = NiNew ES_PLAY;
		pData->s_fAccumTime = 0.0f;
		pData->s_nStep = 0;
		pData->s_pEffect = pEffect;
		pData->s_pEffectSoundData = pEffectSoundData;

		m_vpEffectSound.PushBack( pData );
	}	

	return pEffect;
}

CsC_EffectObject* CsC_EffectProp::AddEffect_FT( char* cEffectPath )
{
	size_t HashCode = CsFPS::GetHashCode( cEffectPath );

	CsC_EffectObject* pEffectObj = NULL;
	if( g_EffectSoundMng->IsData( HashCode ) == true )	
	{
		CsEffectSound* pEffectSoundData = g_EffectSoundMng->GetData( HashCode );	

		pEffectObj = CsC_EffectObject::NewInstance();

		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
		GlobalEffectProp( &pUnit->s_Prop );	
		pUnit->SetLoadType( sTCUnit::eFast );

		pUnit->s_pLoadedObject = pEffectObj;
		sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", pEffectSoundData->m_sESInfo.szFilePath );

		DWORD dwFlag = pEffectSoundData->m_sEffectOptin.dwEffectFlag & ~nsEFFECT::EVENT_DEL_ANI;
		pEffectObj->Init( m_pParent, &pUnit->s_Prop, dwFlag );
	
		float fScale;
		if( pEffectSoundData->m_sEffectOptin.bChrScale == true )
		{
			fScale = pEffectSoundData->m_sEffectOptin.fScale * m_pParent->GetToolWidth() * 0.027f;
		}
		else
		{
			fScale = pEffectSoundData->m_sEffectOptin.fScale;
		}	
		pEffectObj->SetScale( fScale );		

		g_pThread->LoadChar( pUnit );

		if( ( pEffectSoundData->m_sEffectOptin.dwEffectFlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
		{
			pEffectObj->SetOffset( pEffectSoundData->m_sEffectOptin.vOffset );						
		}

		ES_PLAY* pData = NiNew ES_PLAY;
		pData->s_fAccumTime = 0.0f;
		pData->s_nStep = 0;
		pData->s_pEffect = pEffectObj;
		pData->s_pEffectSoundData = pEffectSoundData;

		m_vpEffectSound.PushBack( pData );
	}	

	return pEffectObj;
}

CsC_EffectObject* CsC_EffectProp::AddEffect_FT( char* cEffectPath, float fEffecScale )
{
	size_t HashCode = CsFPS::GetHashCode( cEffectPath );

	CsC_EffectObject* pEffectObj = NULL;
	if( g_EffectSoundMng->IsData( HashCode ) == true )	
	{
		CsEffectSound* pEffectSoundData = g_EffectSoundMng->GetData( HashCode );	

		pEffectObj = CsC_EffectObject::NewInstance();

		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
		GlobalEffectProp( &pUnit->s_Prop );	
		pUnit->SetLoadType( sTCUnit::eFast );

		pUnit->s_pLoadedObject = pEffectObj;
		sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", pEffectSoundData->m_sESInfo.szFilePath );

		DWORD dwFlag = pEffectSoundData->m_sEffectOptin.dwEffectFlag & ~nsEFFECT::EVENT_DEL_ANI;
		pEffectObj->Init( m_pParent, &pUnit->s_Prop, dwFlag );

		float fScale;
		if( pEffectSoundData->m_sEffectOptin.bChrScale == true )
		{
			fScale = pEffectSoundData->m_sEffectOptin.fScale * fEffecScale;
		}
		else
		{
			fScale = pEffectSoundData->m_sEffectOptin.fScale;
		}	
		pEffectObj->SetScale( fScale );		

		g_pThread->LoadChar( pUnit );

		if( ( pEffectSoundData->m_sEffectOptin.dwEffectFlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
		{
			pEffectObj->SetOffset( pEffectSoundData->m_sEffectOptin.vOffset );						
		}

		ES_PLAY* pData = NiNew ES_PLAY;
		pData->s_fAccumTime = 0.0f;
		pData->s_nStep = 0;
		pData->s_pEffect = pEffectObj;
		pData->s_pEffectSoundData = pEffectSoundData;

		m_vpEffectSound.PushBack( pData );
	}	

	return pEffectObj;
}


CsC_EffectObject* CsC_EffectProp::AddFireWorkEffect( char* cEffectPath, float fScale, DWORD dwPlag, bool bPreLoad /*=false*/ )
{
	CsC_EffectObject* pEffect = CsC_EffectObject::NewInstance();

	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	GlobalEffectProp( &pUnit->s_Prop );
	if( bPreLoad )
	{
		pUnit->SetLoadType( sTCUnit::eFast );
	}
	pUnit->s_pLoadedObject = pEffect;

	sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\system\\%s", cEffectPath );

	pEffect->Init( m_pParent, &pUnit->s_Prop, dwPlag );
	pEffect->SetScale( fScale );
	m_vpEffect.PushBack( pEffect );
	g_pThread->LoadChar( pUnit );

	return pEffect;
}

void CsC_EffectProp::AddBuffLoopEffect( char* cEffectPath, DWORD dwBuffCode, 
									   DWORD dwFlag ,
									   bool bPreLoad /*= false */ )
{
	
	DeleteBuffLoopEffect(dwBuffCode);	// 기존에 있는 이펙트는 삭제
	
	CsC_EffectObject* pEffect = CsC_EffectObject::NewInstance();
	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	GlobalEffectProp( &pUnit->s_Prop );

	if( bPreLoad )
		pUnit->SetLoadType( sTCUnit::eFast );	

	pUnit->s_pLoadedObject = pEffect;
	sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", cEffectPath );

	pEffect->Init( m_pParent, &pUnit->s_Prop, dwFlag );
	pEffect->SetScale( m_pParent->GetToolWidth() * 0.00334f );
	m_vpEffect.PushBack( pEffect );
	g_pThread->LoadChar( pUnit );

	m_mapLoopBuffEffect.insert( make_pair(dwBuffCode, pEffect));
}

CsC_AvObject* CsC_EffectProp::_GetLoopBuffEffect( DWORD dwBuffCode )
{
	BUFF_EFF_MAP_IT iter = m_mapLoopBuffEffect.find(dwBuffCode);
	
	if( iter == m_mapLoopBuffEffect.end() )
		return NULL;

	return iter->second;
}

bool CsC_EffectProp::DeleteBuffLoopEffect( DWORD dwBuffCode )
{
	BUFF_EFF_MAP_IT iter = m_mapLoopBuffEffect.find(dwBuffCode);

	if( iter != m_mapLoopBuffEffect.end() )
	{
		CsC_AvObject* pBuffEffect = iter->second;
		m_mapLoopBuffEffect.erase(iter);
		DeleteEffect(pBuffEffect);
	}
	else
		return false;

	return true;	
}

CsC_AvObject* CsC_EffectProp::_GetLoopSkillEffect( DWORD dwSkillIdx )
{
	SKILL_EFF_MAP_IT iter = m_mapLoopSkillEffect.find( dwSkillIdx );

	if( iter == m_mapLoopSkillEffect.end() )
		return NULL;

	return iter->second;
}

void CsC_EffectProp::AddSkillLoopEffect( char* cEffectPath, DWORD dwSkillIdx, DWORD dwFlag /*= nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER*/, bool bPreLoad /*= false */, float fScale/* = 1.0f*/, float fLifeTime )
{
	DeleteSkillLoopEffect(dwSkillIdx);	// 기존에 있는 이펙트는 삭제

	CsC_EffectObject* pEffect = CsC_EffectObject::NewInstance();
	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	GlobalEffectProp( &pUnit->s_Prop );

	if( bPreLoad )
		pUnit->SetLoadType( sTCUnit::eFast );	

	pUnit->s_pLoadedObject = pEffect;
	sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Effect\\%s", cEffectPath );

	pEffect->Init( m_pParent, &pUnit->s_Prop, dwFlag );

	// 이펙트 스케일은 오브젝트와는 별개로 잡음
	pEffect->SetScale( fScale, false );
	pEffect->SetLifeTime( fLifeTime );

	m_vpEffect.PushBack( pEffect );
	g_pThread->LoadChar( pUnit );

	m_mapLoopSkillEffect.insert( make_pair( dwSkillIdx, pEffect ));
}

bool CsC_EffectProp::DeleteSkillLoopEffect( DWORD dwSkillIdx )
{
	SKILL_EFF_MAP_IT iter = m_mapLoopSkillEffect.find(dwSkillIdx);

	if( iter != m_mapLoopSkillEffect.end() )
	{
		CsC_AvObject* pBuffEffect = iter->second;
		m_mapLoopSkillEffect.erase(iter);
		DeleteEffect(pBuffEffect);
	}
	else
		return false;

	return true;
}


#undef PARENT_CLASS
