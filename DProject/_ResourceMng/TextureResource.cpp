
#include "StdAfx.h"
#include "TextureResource.h"


namespace GUI
{
	//--------------------------------------------------------------------------------------------------------------------------------
	// class Texture
	//--------------------------------------------------------------------------------------------------------------------------------
	Texture::Texture( const char *TexName ):m_strName ( TexName )
	{
		m_HashCode = CsFPS::GetHashCode( TexName );

		NiTexture::FormatPrefs kPrefs;
		kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::COMPRESSED;
		kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
		m_NiTexture = NiSourceTexture::Create(TexName , kPrefs );
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	Texture::~Texture()
	{
		if( m_NiTexture )
		{
			NiSourceTexturePtr TestTexture =  NiDynamicCast(NiSourceTexture,m_NiTexture);
			if( TestTexture )
				TestTexture->DestroyAppPixelData();
		}

		m_NiTexture = NULL;
		m_HashCode = 0;
		m_strName.clear();
	}
	
	NiTexturePtr Texture::GetTexture() const
	{ 
		return m_NiTexture;
	}
	
	unsigned int Texture::GetRefCountEx() const
	{
		if( m_NiTexture )
			return m_NiTexture->GetRefCount();
		return GetRefCount();
	}

	size_t Texture::GetHashCode() const
	{
		return m_HashCode;
	}

	std::string	Texture::GetFileName() const
	{
		return m_strName;
	}
}