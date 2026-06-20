#include "StdAfx.h"
#include "AdaptTrade.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../00.Contents/Trade_Contents.h"

AdaptTradeContents::AdaptTradeContents(void):m_pContents(NULL)
{
}

AdaptTradeContents::~AdaptTradeContents(void)
{
}

bool AdaptTradeContents::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, false );
	if( E_CT_TRADE != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< CTradeContents* >( pContents );

	return true;
}

bool AdaptTradeContents::IsTrading() const
{
	SAFE_POINTER_RETVAL(m_pContents, false);
	return m_pContents->IsTrading();
}