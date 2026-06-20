

#include "StdAfx.h"

#include "SEffectMgr.h"

SEffectMgr*	g_pSEffectMgr = NULL;


// 셰이더 문자열 조합 검색어 ShaderPlagStr

SEffectMgr::SEffectMgr(void)
{
}

SEffectMgr::~SEffectMgr(void)
{
	for(int i = 0; i < ET_MAX; ++i)
	{
		MAP_IT iter_B = m_ShaderEffectMap[i].begin();
		MAP_IT iter_E = m_ShaderEffectMap[i].end();
		for(; iter_B != iter_E; ++iter_B )
			SAFE_DELETE(iter_B->second);
		m_ShaderEffectMap[i].clear();
	}
}

void SEffectMgr::AddSEffect( float fEnd, CsC_AvObject* pObj, const st_SHADING_FLAG& stFlag, float fCycle /*= 1.f*/ )
{
	if(! _Confirm(pObj, stFlag) )
		return;

	CShaderEffect_Base* pEffect = NULL;
	switch(stFlag.s_nEffectType)
	{
	case ET_COLOR:
		pEffect = csnew CShaderEffect_Color;
		pEffect->Init(stFlag, pObj, fEnd, fCycle);
		break;

	case ET_OUTLINE:
		pEffect = csnew CShaderEffect_OutLine;
		pEffect->Init(stFlag, pObj, fEnd, fCycle);
		break;

	default:
		assert_cs(false);
		return;
	}
	m_ShaderEffectMap[stFlag.s_nEffectType].insert(make_pair(pObj , pEffect));
}

void SEffectMgr::Update( float fDeltaTime )
{
	_PreUpdate();

	for(int i = 0; i < ET_MAX; ++i)
	{
		MAP_IT iter_B = m_ShaderEffectMap[i].begin();
		MAP_IT iter_E = m_ShaderEffectMap[i].end();
		for(; iter_B != iter_E; ++iter_B)
		{
			(iter_B->second)->Update(fDeltaTime);
		}
	}
}

void SEffectMgr::_PreUpdate()
{
	for(int i = 0; i < ET_MAX; ++i)
	{
		MAP_IT iter_B = m_ShaderEffectMap[i].begin();
		MAP_IT iter_E = m_ShaderEffectMap[i].end();
		for(; iter_B != iter_E; )
		{
			CShaderEffect_Base* pShader = (iter_B->second);
			if (pShader->m_stOption.s_nUpdateType == UT_NONE )
			{
				pShader->Delete();
				SAFE_DELETE(pShader);

				iter_B = m_ShaderEffectMap[i].erase(iter_B);
			}
			else
				++iter_B;
		}
	}
}


bool SEffectMgr::_Confirm( CsC_AvObject* ptr, const st_SHADING_FLAG& stFlag )
{
	MAP_IT iter = m_ShaderEffectMap[stFlag.s_nEffectType].find(ptr);

	// ptr이 없으니 add
	if( iter == m_ShaderEffectMap[stFlag.s_nEffectType].end() )
		return true;

	// 강제 갱신 요청 시
	if( stFlag.s_nPriorityType & PRIORITY_CHANGE )
	{
		_DeleteSDuplicateEffect(ptr, stFlag.s_nEffectType);
		return true;
	}

	return false;
}


void SEffectMgr::Render( CsC_AvObject* pObj )
{
	MAP_IT iter = m_ShaderEffectMap[ET_COLOR].find(pObj);
	
	// 만약 오브젝트를 찾을 수 없다면,
 	if(iter == m_ShaderEffectMap[ET_COLOR].end())
 	{
		CsNodeObj* pNiNode = pObj->GetCsNode();
		if( pNiNode )
 			pNiNode->Render();
 		return;
 	}

	st_SHADING_FLAG stOpton = (iter->second)->m_stOption;	
	if( stOpton.s_nEffectType != ET_COLOR )
	{
		CsNodeObj* pNiNode = pObj->GetCsNode();
		if( pNiNode )
 			pNiNode->Render();
		return;
	}

	(iter->second)->_SetGeomType();
	(iter->second)->ConstantTable();	// 셰이더 셋 -> 렌더 -> 원복
}

void SEffectMgr::OutLineRender()
{
	if( nsCsGBTerrain::g_Device.g_bUseMyShader == false )
		return;

	if( m_ShaderEffectMap[ET_OUTLINE].empty() )
		return;

	MAP_IT iter_B = m_ShaderEffectMap[ET_OUTLINE].begin();
	MAP_IT iter_E = m_ShaderEffectMap[ET_OUTLINE].end();
	for(; iter_B != iter_E; ++iter_B)
	{
// 		if( (iter_B->second)->m_stOption.s_nEffectType != ET_OUTLINE )
// 			continue;

		if( (iter_B->first)->IsEnableObject() == false )
			continue;

		if( (iter_B->first)->GetViewSight() == false )
			continue;
		
		CsNodeObj* pCsNode = (iter_B->first)->GetCsNode();
		if(!pCsNode)
			continue;

		CShaderEffect_Base* pShader = (iter_B->second);
		pShader->ConstantTable();
	}	
}

bool SEffectMgr::DeleteSEffect( CsC_AvObject * pObj )
{
	SAFE_POINTER_RETVAL( pObj, false );

	bool bFlag = false;
	for(int i = 0; i < ET_MAX; ++i)
	{
		MAP_IT it = m_ShaderEffectMap[i].find( pObj );
		if( it == m_ShaderEffectMap[i].end() )
			continue;
		if( it->second )
		{
			it->second->Delete();
			SAFE_DELETE(it->second);
		}
		
		m_ShaderEffectMap[i].erase(it);
		bFlag = true;
	}

	return bFlag;
}

bool SEffectMgr::_DeleteSDuplicateEffect( CsC_AvObject* pObj, int nEffectType )
{
	MAP_IT iter = m_ShaderEffectMap[nEffectType].find(pObj);

	if(iter == m_ShaderEffectMap[nEffectType].end())
		return false;

	(iter->second)->Delete();
	SAFE_DELETE(iter->second);
	m_ShaderEffectMap[nEffectType].erase(iter);

	return true;
}

bool SEffectMgr::DeleteSEffect( CsC_AvObject* pObj, DWORD dwBuffID )
{
	bool bDelete = false;
	for(int i = 0; i < ET_MAX; ++i)
	{
		MAP_IT iter_B = m_ShaderEffectMap[i].begin();
		MAP_IT iter_E = m_ShaderEffectMap[i].end();
		for(; iter_B != iter_E; )
		{
			// 존재하고, 입력된 버프ID가 일치하면 삭제
			if (pObj == (iter_B->first) )
			{
				assert_csm1( (iter_B->second)->m_stOption.s_dwBuffID == dwBuffID , "없는 버프에 대한 S이펙트 삭제요청", dwBuffID);
				(iter_B->second)->Delete();
				SAFE_DELETE(iter_B->second);

				iter_B = m_ShaderEffectMap[i].erase(iter_B);
				bDelete = true;
			}

			else
				++iter_B;
		}
	}

	return bDelete;
}


void SEffectMgr::CShaderEffect_Base::_Update_Time(float fDeltaTime)
{
	m_fCurrent_Time += fDeltaTime;

	if( m_fCurrent_Time >= m_fEnd_Time )
	{
		m_stOption.s_nUpdateType = UT_NONE;
		return;
	}

	m_stOption.s_nUpdateType = UT_TIME;
}

void SEffectMgr::CShaderEffect_Base::_Update_FPS()
{
	++m_fCurrent_Time;

	if( m_fCurrent_Time >= m_fEnd_Time )
	{
		m_stOption.s_nUpdateType = UT_NONE;
		return;
	}

	m_stOption.s_nUpdateType = UT_FPS;
}

void SEffectMgr::CShaderEffect_Base::Update(float fDeltaTime)
{
	if( m_stOption.s_nUpdateType == UT_NONE )
		return;

	switch(m_stOption.s_nUpdateType)
	{
	case UT_TIME:
		_Update_Time(fDeltaTime) ;
		break;

	case UT_FPS:
		_Update_FPS();
		break;

	case UT_NONE:
	case UT_INFINITY_TIME:
		break;

	default:
		assert_cs(false);
		break;
	}
}

void SEffectMgr::CShaderEffect_Base::Delete()
{
	m_stOption.s_nUpdateType = 0;
	m_stOption.s_nShaderType = CGeometry::None;
	m_pTarget->SetEnableColorEffect(false);
	Clear();
	_SetGeomType();
}

void SEffectMgr::CShaderEffect_Base::_SetSwitchColor()
{
	switch( m_stOption.s_nColorType )
	{
	case DC_NONE:	break;
	case DC_RED:	m_vOriginColor = D3DXVECTOR3(0.7f, -0.6f, -0.6f);	break;
	case DC_BLUE:	m_vOriginColor = D3DXVECTOR3(-0.8f, -0.8f, 0.8f);	break;
	case DC_YELLOW:	m_vOriginColor = D3DXVECTOR3(0.8f, 0.8f, -1.f);		break;
	case DC_WHITE:	m_vOriginColor = D3DXVECTOR3(1.f, 1.f, 1.f);		break;
	case DC_GREEN:	m_vOriginColor = D3DXVECTOR3(-0.8f, 0.8f, -0.8f);	break;
	case DC_TEST:	m_vOriginColor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		break;
	default:
		assert_cs(false);
		break;
	}
}

void SEffectMgr::CShaderEffect_Base::_SetGeomType()
{
	CsNodeObj*	pNode = m_pTarget->GetCsNode();
	CGeometry* pGeom = NULL;

	if( !pNode )
		return;

	int nCount = pNode->m_vpGeom.Size();

	for( int i=0; i<nCount; ++i )
	{
		pGeom = pNode->m_vpGeom[ i ];
		pGeom->SetStateType( m_stOption.s_nShaderType );
	}

	pNode->ActiveShader();
}

void SEffectMgr::CShaderEffect_Base::Init( const st_SHADING_FLAG& stFlag, CsC_AvObject* pTarget,
										  float fExit_Time, float fCycle)
{
	m_stOption = stFlag;
	m_pTarget = pTarget;
	m_pTarget->SetEnableColorEffect(true);
	// 시간
	m_fEnd_Time = fExit_Time;

	// 지속 형태에 대한 사이클
	m_fCycle_Time = fCycle;

	m_fCurrent_Time = 0.f;
	m_fCycle_CurrentTime = 0.f;
}


void SEffectMgr::CShaderEffect_Color::Init( const st_SHADING_FLAG& stFlag, CsC_AvObject* pTarget,
										   float fExit_Time, float fCycle)
{
	CShaderEffect_Base::Init(stFlag, pTarget, fExit_Time, fCycle);

	// 초기화
	m_vCurrentColor = D3DXVECTOR3(0.f,0.f,0.f);
	m_vAmbLight = D3DXVECTOR3(0.f,0.f,0.f);
	m_fFlag = 1.f;

	_SetSwitchColor();
	_SetVariableColor(fCycle);
}

void SEffectMgr::CShaderEffect_Color::Update( float fDeltaTime )
{
	CShaderEffect_Base::Update(fDeltaTime);
	FormUpdate(fDeltaTime);
}


void SEffectMgr::CShaderEffect_Color::ConstantTable()
{
	if( !m_bShow )
		return;

	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_fAddColor", sizeof(float) * 3, &m_vCurrentColor );
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_fAddLight", sizeof(float) * 3, &m_vAmbLight );

	m_pTarget->GetCsNode()->Render();

	D3DXVECTOR3 vZero(0.f,0.f,0.f);
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_fAddColor", sizeof(float) * 3, &vZero );
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_fAddLight", sizeof(float) * 3, &vZero );
}

void SEffectMgr::CShaderEffect_Color::FormUpdate( float fDeltaTime )
{
	switch(m_stOption.s_nFormType)
	{
	case FT_LOOPDEEPSOFT:
		{
			m_fCycle_CurrentTime += fDeltaTime;

			m_vCurrentColor += m_vVariableColor * m_fFlag * fDeltaTime;

			if(m_fCycle_CurrentTime >= m_fCycle_Time)
			{
				m_fFlag *= -1.f;
				m_fCycle_CurrentTime = 0.f;
			}
		}
		break;
	case FT_FLICKER:	// 깜빡임
		{
			m_fCycle_CurrentTime += fDeltaTime;
			if(m_fCycle_CurrentTime >= m_fCycle_Time)
			{
				m_fCycle_CurrentTime = 0.f;
				m_bShow = !m_bShow;
			}
		}break;
	case FT_ONECOLOR:
		break;
	case FT_LIGHTBEGINSOFT:
	case FT_BEGINDEEP:
	case FT_BEGINSOFT:
		m_vCurrentColor += m_vVariableColor * fDeltaTime;
		break;
	default:
		assert_cs(false);
		break;
	}
}

void SEffectMgr::CShaderEffect_Color::_SetVariableColor( float fCycleTime )
{
	m_vVariableColor = m_vOriginColor;
	switch(m_stOption.s_nFormType)
	{
	case FT_LOOPDEEPSOFT:
		m_fCycle_Time = fCycleTime;
		m_fCycle_CurrentTime = 0.f;

		m_vVariableColor /= fCycleTime*2.f;
		break;

	case FT_BEGINDEEP:
		m_vVariableColor /= m_fEnd_Time*2.f;
		break;

	case FT_ONECOLOR:
		break;

	case FT_BEGINSOFT:
		m_vVariableColor = m_vOriginColor;
		m_vVariableColor /= m_fEnd_Time*2.f;
		m_vVariableColor *= -1.f;
		m_vCurrentColor = m_vOriginColor;
		break;
	case FT_FLICKER:
		m_vCurrentColor = m_vOriginColor;
		break;
	case FT_LIGHTBEGINSOFT:
		m_vVariableColor = m_vOriginColor;
		m_vVariableColor /= m_fEnd_Time*2.f;
		m_vVariableColor *= -1.f;
		m_vCurrentColor = m_vOriginColor;
		m_vAmbLight = D3DXVECTOR3(0.75f,0.75f,0.75f);
		break;

	default:
		assert_cs(false);
		break;
	}
}

void SEffectMgr::CShaderEffect_Color::Clear()
{
	D3DXVECTOR3 vZero(0.f,0.f,0.f);
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_fAddColor", sizeof(float) * 3, &vZero );
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_fAddLight", sizeof(float) * 3, &vZero );
}

void SEffectMgr::CShaderEffect_OutLine::Init( const st_SHADING_FLAG& stFlag, CsC_AvObject* pTarget,
											 float fExit_Time, float fCycle)
{
	CShaderEffect_Base::Init(stFlag, pTarget, fExit_Time, fCycle);

	// 초기화
	m_vCurrentColor = D3DXVECTOR3(0.f,0.f,0.f);
	m_fFlag = 1.f;

	_SetSwitchColor();
	_SetVariableColor(fCycle);
}

void SEffectMgr::CShaderEffect_OutLine::Update( float fDeltaTime )
{
	CShaderEffect_Base::Update(fDeltaTime);
	FormUpdate(fDeltaTime);
}

void SEffectMgr::CShaderEffect_OutLine::_SetVariableColor( float fCycleTime )
{
	m_vVariableColor = m_vOriginColor;
	switch(m_stOption.s_nFormType)
	{
	case FT_LOOPDEEPSOFT:
		m_fCycle_Time = fCycleTime;
		m_fCycle_CurrentTime = 0.f;
		//m_vVariableColor /= fCycleTime*2.f;
		m_vVariableColor /= fCycleTime;
		break;
	case FT_BEGINDEEP:
		m_vVariableColor /= m_fEnd_Time*2.f;
		break;
	case FT_ONECOLOR:
		break;
	case FT_BEGINSOFT:
		m_vVariableColor = m_vOriginColor;
		m_vVariableColor /= m_fEnd_Time*2.f;
		m_vVariableColor *= -1.f;
		m_vCurrentColor = m_vOriginColor;
		break;
	case FT_FLICKER:
		m_vCurrentColor = m_vOriginColor;
		break;
	case FT_LIGHTBEGINSOFT:
		m_vVariableColor = m_vOriginColor;
		m_vVariableColor /= m_fEnd_Time*2.f;
		m_vVariableColor *= -1.f;
		m_vCurrentColor = m_vOriginColor;
		break;

	default:
		assert_cs(false);
		break;
	}
}

void SEffectMgr::CShaderEffect_OutLine::ConstantTable()
{
	if( !m_bShow || NULL == m_pTarget )
		return;

	CsNodeObj* pCsNode = m_pTarget->GetCsNode();
	if( NULL == pCsNode )
		return;

	// 쉐이더 설정
 	float afSize[4] = { m_vCurrentColor.x, m_vCurrentColor.y, m_vCurrentColor.z, 1.f };

	float fWidth = 3.f;
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_OutLineWidth", sizeof(float), &fWidth );
 	NiD3DShaderFactory::UpdateGlobalShaderConstant( "BlurColor", sizeof(afSize), &afSize );

	
	// SkinShader는 아웃라인 그리기, MeshShader는 메쉬 늘리기
	// 따라서 메쉬를 늘려주는 것이 외곽선 크기를 늘려주는 것.
 	pCsNode->SetShader( "Char_OutLine_Blur_Eff", "OutLine_Blur_Eff" );
 	pCsNode->RenderAbsolute_ExceptPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE );
 	pCsNode->ActiveShader();
}

void SEffectMgr::CShaderEffect_OutLine::Clear()
{

}

void SEffectMgr::CShaderEffect_OutLine::FormUpdate( float fDeltaTime )
{
	switch(m_stOption.s_nFormType)
	{
	case FT_LOOPDEEPSOFT:
		{
			m_fCycle_CurrentTime += fDeltaTime;

			if(m_fCycle_CurrentTime >= m_fCycle_Time)
			{
				if( m_fFlag > 0 )
					m_vCurrentColor = m_vOriginColor;
				else
					m_vCurrentColor = D3DXVECTOR3(0.f,0.f,0.f);
					
				m_fFlag *= -1.f;
				m_fCycle_CurrentTime = 0.f;
			}
			else
				m_vCurrentColor += m_vVariableColor * m_fFlag * fDeltaTime;

		}
		break;
	case FT_FLICKER:	// 깜빡이 외곽선
		{
			m_fCycle_CurrentTime += fDeltaTime;

			if(m_fCycle_CurrentTime >= m_fCycle_Time)
			{
				m_fCycle_CurrentTime = 0.f;
				m_bShow = !m_bShow;
			}
		}
		break;
	case FT_ONECOLOR:
		break;
	case FT_LIGHTBEGINSOFT:
	case FT_BEGINDEEP:
	case FT_BEGINSOFT:
		m_vCurrentColor += m_vVariableColor * fDeltaTime;
		break;
	default:
		assert_cs(false);
		break;
	}
}

