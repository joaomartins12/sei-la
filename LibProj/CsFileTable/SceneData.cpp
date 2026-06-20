#include "stdafx.h"

CSceneData::CSceneData(void)
{
}

CSceneData::~CSceneData(void)
{
	Delete();
}

void CSceneData::Delete()
{
	std::list< sData* >::iterator it = m_pListData.begin();
	std::list< sData* >::iterator itEnd = m_pListData.end();
	for( ;it != itEnd; it++ )
	{
		SAFE_DELETE( *it ); 		
	}	
	m_pListData.clear();
}
