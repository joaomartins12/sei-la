#pragma once
#include "ShaderHelper.h"

class CEffectView
{
public:
	CEffectView(void);
	~CEffectView(void);
public:
	static	bool		CALLBACK_ThreadDelete( sTCUnit* pUnit );

public:
	void			Init();
	void			Delete();	

	void			Update();
	void			Render();
	
	void			LoadEffect( size_t HashCode );

	void			DeleteEffect();	
	void			SetEffect( CString strFile );
	void			SetEffectOption();

	void			DeleteDestModel();
	CsC_AvObject*	SetDestModel( DWORD dwModelID );

	void			SetLoop( bool bLoop ) {	m_bLoop = bLoop; }
	void			SetSoundList( CString strSound, float fTime = -1.0f );

	void			EffectSoundPlay( float fDeltaTime );

	//=====================================================================================	
	//	이펙트 오브젝트	
	//=====================================================================================
protected:
	CsC_AvObject*			m_pDestModel;
	CsC_EffectObject*		m_pEffect;
	CString					m_strEffect;

	CsC_AniProp_Sequence*	m_pCsSequenceMng;

	float					m_fDeltaTime;
	float					m_fAniDeltaTime;
	float					m_fEffectTime;
	float					m_fEndAniTime;

	bool					m_bLoop;	

	//=====================================================================================	
	//	쉐이더	
	//=====================================================================================
protected:
	ShaderHelper			m_ShaderHelper;

	//=====================================================================================	
	//	디폴트 라이트 - 지형이 없을 경우에 쓰는 라이트	
	//=====================================================================================
protected:
	NiDirectionalLightPtr	m_pDefaultLight;
protected:
	void				_CreateDefaultLight();
	void				_DeleteDefaultLight();

	//=====================================================================================	
	//	이펙트 사운드 옵션 저장
	//=====================================================================================
public:	
	CsEffectSound*	m_pOption;	

	bool			SetOption( DWORD dwFlag, NiPoint3 vOffset, bool bScale, float fScale );
	void			DeleteOption() { SAFE_NIDELETE( m_pOption ); }
	void			ClearOption();	
		
};

extern CEffectView g_ViewEffect;
