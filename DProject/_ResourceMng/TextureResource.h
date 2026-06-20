//--------------------------------------------------------------------------------------------------------------------------------
// [2009/6/3 Th]
// Texture.h
//--------------------------------------------------------------------------------------------------------------------------------
#pragma once

#include <NiSourceTexture.h>

namespace GUI
{
	//--------------------------------------------------------------------------------------------------------------------------------
	// Texture
	//--------------------------------------------------------------------------------------------------------------------------------
	class Texture : public NiRefObject
	{
	public:
		Texture(const char *FileName);
		~Texture();

		NiTexturePtr GetTexture() const;
		unsigned int GetRefCountEx() const;
		size_t		 GetHashCode() const;
		std::string	 GetFileName() const;

	private:
		size_t				m_HashCode;
		std::string			m_strName;
		NiTexturePtr		m_NiTexture;
	};
	NiSmartPointer(Texture) ;
}
