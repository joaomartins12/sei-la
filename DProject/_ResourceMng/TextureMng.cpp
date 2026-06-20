
#include "stdafx.h"
#include "TextureMng.h"

//--------------------------------------------------------------------------------------------------------------------------------
TextureMng::TextureMng(void)
{
}
//--------------------------------------------------------------------------------------------------------------------------------
TextureMng::~TextureMng(void)
{
	End( );
}
//--------------------------------------------------------------------------------------------------------------------------------
void TextureMng::Init(  )
{

}
//--------------------------------------------------------------------------------------------------------------------------------
void TextureMng::End(  )
{
	RemoveAllResource();
}
//--------------------------------------------------------------------------------------------------------------------------------
size_t TextureMng::GetSize() const
{
	return m_listTexture.size();
}
//--------------------------------------------------------------------------------------------------------------------------------
GUI::TexturePtr TextureMng::GetResource( const char *strFilename )
{
	size_t HashCode = CsFPS::GetHashCode( strFilename );

	STL_HASHMAP_TEXTURE_IT itor = m_listTexture.find( HashCode );
	if( itor == m_listTexture.end() ) // 존재 하지 않음
		return CreateResource(strFilename);

	// 있는거임
	return itor->second;
}
//--------------------------------------------------------------------------------------------------------------------------------
GUI::TexturePtr TextureMng::CreateResource( const char *strFilename )
{
	GUI::TexturePtr newTexture = NiNew GUI::Texture( strFilename );
	STL_HASHMAP_TEXTURE_VT vt ( newTexture->GetHashCode(), newTexture );
	m_listTexture.insert( vt );
//	BHPRT( "CreateResource : %lu, %s", newTexture->GetHashCode(), strFilename );
	return newTexture;
}
//--------------------------------------------------------------------------------------------------------------------------------
void TextureMng::RemoveResource( GUI::TexturePtr & pRemoveTexture)
{
	// 존재하는 파일인지 검사
	size_t hashcode = pRemoveTexture->GetHashCode();
	STL_HASHMAP_TEXTURE_IT itor = m_listTexture.find( hashcode );
	if( itor == m_listTexture.end() )
		return;
	
//	BHPRT( "RemoveResource : %lu, %s, %d", hashcode, itor->second->GetFileName().c_str(), itor->second->GetRefCount() );
	if( itor->second->GetRefCountEx() > 1 ) 
		pRemoveTexture = NULL;
	
	if( itor->second->GetRefCountEx() <= 1 )
	{
	//	BHPRT( "DeleteResource : %lu, %s, %d", hashcode, itor->second->GetFileName().c_str(), itor->second->GetRefCountEx() );
		itor->second = 0;
		m_listTexture.erase(itor);
	}	
}
//--------------------------------------------------------------------------------------------------------------------------------
void TextureMng::RemoveAllResource()
{
	STL_HASHMAP_TEXTURE_IT itor = m_listTexture.begin();
	for( ; itor != m_listTexture.end(); ++itor )
	{
		SAFE_POINTER_CON( itor->second );

		if( itor->second->GetRefCountEx() <= 1 )
			itor->second = 0;
		else
			assert_cs( true );
	}
	m_listTexture.clear();
}
//--------------------------------------------------------------------------------------------------------------------------------
void TextureMng::CleanUpTexture()
{
	size_t currSize = m_listTexture.size();	

	STL_HASHMAP_TEXTURE_IT itor = m_listTexture.begin();
	for( ; itor != m_listTexture.end();  )
	{
		if( NULL == itor->second )
		{
			itor = m_listTexture.erase(itor);
			continue;
		}

		if( itor->second->GetRefCountEx() > 1 ) 
		{
			++itor;
			continue;
		}

		itor = m_listTexture.erase(itor);
	}

	size_t changeSize = m_listTexture.size();	

	//DBG( "CleanUpTexture : %lu", currSize - changeSize );
}
//--------------------------------------------------------------------------------------------------------------------------------