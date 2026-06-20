#include "StdAfx.h"
#include "AdaptUnionStore.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../00.Contents/ServerRelocateContents.h"

AdaptUnionStore::AdaptUnionStore(void):m_pContents(NULL)
{
}

AdaptUnionStore::~AdaptUnionStore(void)
{
}

bool AdaptUnionStore::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, false );
	if( E_CT_UNIONSTORE_CONTENTS != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< UnionStoreContents* >( pContents );

	return true;
}

// 위탁 상점을 가지고 있는지 체크
bool AdaptUnionStore::IhaveUnionStore() const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IHaveUnionStore();
}
