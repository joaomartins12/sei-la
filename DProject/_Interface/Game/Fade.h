

#pragma once

// #define FADE_IN		0
// #define FADE_OUT	1

class CFade : public NiMemObject
{
public:
	enum eFadeType
	{
		FADE_NONE = 0,
		FADE_IN,
		FADE_OUT,
	};

	enum eFadeState
	{
		FADE_STATE_DOING,
		FADE_STATE_END,
	};

public:
	CFade();
	virtual ~CFade();

	void		Init(eFadeType type, float time = 1.0f);
	void		Reset( eFadeType type, float time = 1.0f );

	void		Update(float const& fDeltaTime);
	void		Render();

public:
	cSprite		m_Sprite;
	float		m_fTime;
	float		m_fMaxTime;
	eFadeType	m_nType;
	eFadeState	m_nState;

	float		m_fDeltaTime;
	DWORD		m_dwOldTime;
};

//extern CFade* g_pFade;

