
#pragma once

#include "nlib/cProfile.h"

class cWeather : public NiMemObject
{
protected:
	enum eSTATE{ STATE_NONE, STATE_RAIN_BEGIN, STATE_RAINING, STATE_RAIN_END, };

public:
	virtual ~cWeather(){ Delete(); }

protected:
	CsNodeObj			m_NodeObject;
	NiTextureTransform*	m_ptrTextureSrc;
	NiTextureTransform	m_trTexture[ WEATHER_TEXTURE_DEPTH ];
	float				m_fVelocity;

	int					m_nCurLevel;
	CsTimeSeq			m_LevelTime;

	eSTATE				m_eCurState;


	cProfile*			m_pIniFile;
	

public:
	void				Delete();
	void				Init();

	void				Update();
	void				Render();

	void				BeginRain();
	void				EndRain();

protected:
	void				_ApplyDepth( int nDepth );
	void				_SetRainTexture( int nLevel );

protected:
	int					_GetIniValue( char* key );
};