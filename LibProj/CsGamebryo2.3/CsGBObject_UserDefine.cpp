
#include "stdafx.h"
#include "CsGBObject_UserDefine.h"

CSGBMEMPOOL_CPP( sUSER_DEFINE );

CALLBACK_QuestCheck		sUDD_BASE::m_CallBack_QuestCheck = NULL;

sUSER_DEFINE::sUSER_DEFINE():s_dwPlag(0),s_dwVisiblePlag(0)
{

}

sUSER_DEFINE::~sUSER_DEFINE()
{

}

void sUSER_DEFINE::Delete()
{
	std::map< DWORD, sUDD_BASE* >::iterator itUDD = s_mapUDData.begin();
	for( ; itUDD!=s_mapUDData.end(); ++itUDD )
	{
		itUDD->second->Delete();
		switch( itUDD->second->GetType() )
		{
		case sUSER_DEFINE::UD_DARKMAP:
			NiDelete ( (sUDD_DARKMAP*)itUDD->second );
			break;
		case sUSER_DEFINE::UD_ANIMATION_RANDOM:
			NiDelete ( (sUDD_ANI_RANDOM*)itUDD->second );
			break;
		case sUSER_DEFINE::UD_USE_TEXTYPE:
			NiDelete ( (sUDD_TexType*)itUDD->second );
			break;
		case sUSER_DEFINE::UDF_ROTATION:
			NiDelete ( (sUDD_Rotation*)itUDD->second );
			break;
		case sUSER_DEFINE::UD_MERGE:
			NiDelete ( (sUDD_Merge*)itUDD->second );
			break;
		case sUSER_DEFINE::UD_SOUND:
			NiDelete ( (sUDD_Sound*)itUDD->second );
			break;
		case sUSER_DEFINE::UD_QUEST_ON:
			NiDelete ( (sUDD_QuestOn*)itUDD->second );
			break;
		case sUSER_DEFINE::UD_QUEST_OFF:
			NiDelete ( (sUDD_QuestOff*)itUDD->second );
			break;
		case sUSER_DEFINE::UD_ACTORMANAGER:
			NiDelete ( (sUDD_ACTOR*)itUDD->second );
			break;
		case sUSER_DEFINE::UD_ACT:
			NiDelete ( (sUDD_ACT*)itUDD->second );
			break;
		case sUSER_DEFINE::UD_PATH:
			NiDelete ( (sUDD_PATH*)itUDD->second );
			break;
		default:
			assert_cs( false );
		}
	}
	s_mapUDData.clear();
}

void sUSER_DEFINE::DeleteData( eTYPE type )
{
	sUDD_BASE* pData = NULL;
	if( s_mapUDData.find( type ) != s_mapUDData.end() )
	{
		pData = s_mapUDData[ type ];
		pData->Delete();
		delete pData;
		s_mapUDData.erase( type );
	}
}


void sUDD_TexType::Delete()
{
	std::map< BYTE, sTEXTURE_GROUP* >::iterator it = s_mapTextureGroup.begin();
	for( ; it != s_mapTextureGroup.end(); ++it )
	{
		it->second->Delete();
		NiDelete it->second;
	}
	s_mapTextureGroup.clear();
}

void sUDD_TexType::sTEXTURE_GROUP::Delete()
{
	std::map< BYTE, nsCSFILE::sFileName_c* >::iterator it = s_mapFileName.begin();
	for( ; it != s_mapFileName.end(); ++it )
	{
		delete it->second;
	}
	s_mapFileName.clear();
}

void sUDD_Merge::Delete()
{
	std::map< DWORD, sINFO* >::iterator it = s_mapInfo.begin();
	for( ; it!=s_mapInfo.end(); ++it )
	{		
		SAFE_DELETE( it->second );
	}
	s_mapInfo.clear();
}

void sUDD_QuestOn::Delete()
{
	std::list< sINFO* >::iterator it = s_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = s_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		delete (*it);
	}
	s_listInfo.clear();
}

bool sUDD_QuestOn::IsOn()
{
	assert_cs( m_CallBack_QuestCheck != NULL );

	// 모든 정보가 완료 되야 성공
	std::list< sINFO* >::iterator it = s_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = s_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_bCompleate == true )
			continue;

		std::list< int >::iterator itInfo = (*it)->s_listQuest.begin();
		std::list< int >::iterator itInfoEnd = (*it)->s_listQuest.end();
		for( ; itInfo!=itInfoEnd; ++itInfo )
		{
			// 하나의 정보에서 하나라도 완료 되었다면 하나의 정보는 완료
			if( m_CallBack_QuestCheck( *itInfo ) == true )
			{
				(*it)->s_bCompleate = true;
				goto QUEST_CHECK_NEXT_STEP;
			}
		}
		// 하나의 정보가 실패라면 실패
		return false;

QUEST_CHECK_NEXT_STEP:;
	}
	return true;
}

bool sUDD_QuestOn::IsOn( int nCompQuest )
{
	assert_cs( m_CallBack_QuestCheck != NULL );

	std::list< sINFO* >::iterator it = s_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = s_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_bCompleate == true )
			continue;

		std::list< int >::iterator itInfo = (*it)->s_listQuest.begin();
		std::list< int >::iterator itInfoEnd = (*it)->s_listQuest.end();
		for( ; itInfo!=itInfoEnd; ++itInfo )
		{
			if( *itInfo == nCompQuest )
			{
				(*it)->s_bCompleate = true;
				goto QUEST_CHECK_NEXT_STEP;
			}
		}
		// 하나의 정보가 실패라면 실패
		return false;

QUEST_CHECK_NEXT_STEP:;
	}
	return true;
}

void sUDD_QuestOff::Delete()
{
	std::list< sINFO* >::iterator it = s_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = s_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		delete (*it);
	}
	s_listInfo.clear();
}

bool sUDD_QuestOff::IsOff()
{
	assert_cs( m_CallBack_QuestCheck != NULL );

	// 모든 정보가 완료 되야 성공
	std::list< sINFO* >::iterator it = s_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = s_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_bCompleate == true )
			continue;

		std::list< int >::iterator itInfo = (*it)->s_listQuest.begin();
		std::list< int >::iterator itInfoEnd = (*it)->s_listQuest.end();
		for( ; itInfo!=itInfoEnd; ++itInfo )
		{
			// 하나의 정보에서 하나라도 완료 되었다면 하나의 정보는 완료
			if( m_CallBack_QuestCheck( *itInfo ) == true )
			{
				(*it)->s_bCompleate = true;
				goto QUEST_CHECK_NEXT_STEP;
			}
		}
		// 하나의 정보가 실패라면 실패
		return false;

QUEST_CHECK_NEXT_STEP:;
	}
	return true;
}

bool sUDD_QuestOff::IsOff( int nCompQuest )
{
	assert_cs( m_CallBack_QuestCheck != NULL );

	// 모든 정보가 완료 되야 성공
	std::list< sINFO* >::iterator it = s_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = s_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_bCompleate == true )
			continue;

		std::list< int >::iterator itInfo = (*it)->s_listQuest.begin();
		std::list< int >::iterator itInfoEnd = (*it)->s_listQuest.end();
		for( ; itInfo!=itInfoEnd; ++itInfo )
		{
			// 하나의 정보에서 하나라도 완료 되었다면 하나의 정보는 완료
			if( *itInfo == nCompQuest )
			{
				(*it)->s_bCompleate = true;
				goto QUEST_CHECK_NEXT_STEP;
			}
		}
		// 하나의 정보가 실패라면 실패
		return false;

QUEST_CHECK_NEXT_STEP:;
	}
	return true;
}