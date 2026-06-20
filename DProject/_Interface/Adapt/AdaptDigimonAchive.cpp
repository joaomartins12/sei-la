#include "StdAfx.h"
#include "AdaptDigimonAchive.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../00.Contents/DigimonArchiveContents.h"

AdaptDigimonArchiveSystem::AdaptDigimonArchiveSystem(void):
m_pDigimonArchiveSystemContents(NULL)
{
}

AdaptDigimonArchiveSystem::~AdaptDigimonArchiveSystem(void)
{
}

bool AdaptDigimonArchiveSystem::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, NULL );
	if( E_CT_DIGIMON_ARCHIVE_CONTENTS != pContents->GetContentsIdentity() )
		return false;

	m_pDigimonArchiveSystemContents = static_cast< CDigimonArchiveContents* >( pContents );

	return true;
}
const int	AdaptDigimonArchiveSystem::GetOpenedArchiveCnt(void)
{
	return m_pDigimonArchiveSystemContents->GetOpenedArchiveCount();
}
const DWORD AdaptDigimonArchiveSystem::GetDigimonBaseIDInArchive(int ArchiveSlotIdx)
{
	return	m_pDigimonArchiveSystemContents->GetBaseIDInArchive(ArchiveSlotIdx);
}
