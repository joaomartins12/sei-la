

#include "stdafx.h"

#include "GlobalMessage.h"

cGlobalMessage::~cGlobalMessage()
{
	Delete();
}

void cGlobalMessage::Delete()
{
	std::list< cGlobal* >::iterator it = m_listMsg.begin();
	std::list< cGlobal* >::iterator itEnd = m_listMsg.end();
	for( ; it != itEnd; ++it )
	{
		NiDelete *it;
	}
	m_listMsg.clear();

}

void cGlobalMessage::Update(float const& fDeltaTime)
{
	if( !m_listMsg.empty() )
	{
		std::list< cGlobal* >::iterator it = m_listMsg.begin();
		std::list< cGlobal* >::iterator itEnd = m_listMsg.end();

		while( it != itEnd )
		{
			switch( (*it)->Update(fDeltaTime) )
			{
			case cRandomRegen::ALPHA_INCREASE:
			case cRandomRegen::LIVE:
			case cRandomRegen::ALPHA_DECREASE:
				return;
				//break;

			case cRandomRegen::DIE:
				NiDelete *it;
				it = m_listMsg.erase( it );
				break;

			default:
				assert_cs( false );			
			}
		}
	}
}

bool cGlobalMessage::Render()
{
	if( !m_listMsg.empty() )
	{
		std::list< cGlobal* >::iterator it = m_listMsg.begin();
		std::list< cGlobal* >::iterator itEnd = m_listMsg.end();

		for( ; it != itEnd; ++it )
		{
			(*it)->Render();
		}
	}

	return true;
}

void cGlobalMessage::SetMessage( n2 nType, TCHAR const* szText, TCHAR const* szText2, TCHAR const* szText3, TCHAR const* szText4 )
{
	if( szText[ 0 ] == NULL )
		return;

	switch( nType )
	{
	case LEVELITEM:	// LevelUp Message
		{
			cLevelUpMsg* pMsg = NiNew cLevelUpMsg;
			pMsg->SetMessage( szText, szText2, szText3 );
			m_listMsg.push_back( pMsg );
		}
		break;

	case RANDOMREGENBEGIN:	// ·£´ý ¸®Á¨ ¸ó½ºÅÍ »ý¼º
		{
			cRandomRegen* pRandomMsg = NiNew cRandomRegen;
			pRandomMsg->SetRandomRegenStart( szText );
			m_listMsg.push_back( pRandomMsg );
		}
		break;

	case RANDOMREGENEND:	// ·£´ý ¸®Á¨ ¸ó½ºÅÍ ÆÄ±«
		{
			cRandomRegen* pRandomMsg = NiNew cRandomRegen;
			pRandomMsg->SetRandomRegenEnd( szText );
			m_listMsg.push_back( pRandomMsg );
		}
		break;

	case FILEBLESS:		// ÆÄÀÏ¼¶ Ãàº¹
		{
			cFileBless* pBless = NiNew cFileBless;
			pBless->SetRaidInfo( szText, szText2, szText3, szText4 );
			m_listMsg.push_back( pBless );
		}
		break;

	default:
		assert_cs( false );
	}
}

