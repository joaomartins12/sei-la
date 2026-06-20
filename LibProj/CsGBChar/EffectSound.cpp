#include "stdafx.h"
#include "EffectSound.h"

CsEffectSound::CsEffectSound(void)
{	
	m_listSound.clear();
}

CsEffectSound::~CsEffectSound(void)
{	
	DeleteSoundList();		
}

void CsEffectSound::DeleteSoundList()
{
	std::list< sSound_Info* >::iterator it = m_listSound.begin();
	std::list< sSound_Info* >::iterator itEnd = m_listSound.end();
	for( ; it != itEnd; it++ )
	{
		SAFE_NIDELETE(*it);
	}
	m_listSound.clear();	
}



