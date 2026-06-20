
#include "stdafx.h"
#include "ResourceMng.h"

//--------------------------------------------------------------------------------------------------------------------------------
ResourceMng::ResourceMng(void)
{
	Init();
}
//--------------------------------------------------------------------------------------------------------------------------------
ResourceMng::~ResourceMng(void)
{
	End( );
}
//--------------------------------------------------------------------------------------------------------------------------------
void ResourceMng::Init(  )
{
}
//--------------------------------------------------------------------------------------------------------------------------------
void ResourceMng::End(  )
{
	RemoveAllResource();
}
//--------------------------------------------------------------------------------------------------------------------------------
void ResourceMng::CleanUpResource()
{
	m_TextureMng.CleanUpTexture();
}
//--------------------------------------------------------------------------------------------------------------------------------
size_t ResourceMng::GetTextureResourceSize() const
{
	return m_TextureMng.GetSize();
}
//--------------------------------------------------------------------------------------------------------------------------------
GUI::TexturePtr ResourceMng::GetTexture( const char *strFilename )
{
	return m_TextureMng.GetResource( strFilename );
}
//--------------------------------------------------------------------------------------------------------------------------------
GUI::TexturePtr ResourceMng::GetTexture( const wchar_t *strFilename )
{
	std::string file;
	DmCS::StringFn::ToMB(strFilename,file);
	return GetTexture( file.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------------------
void ResourceMng::RemoveTexture( GUI::TexturePtr& pRemove )
{
	SAFE_POINTER_RET( pRemove );
	m_TextureMng.RemoveResource( pRemove );
}
//--------------------------------------------------------------------------------------------------------------------------------
void ResourceMng::RemoveAllResource()
{
	m_TextureMng.RemoveAllResource();
}
