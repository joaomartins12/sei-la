
#pragma once

#include "TextureResource.h"

class TextureMng 
{
public:
	TextureMng(void);
	~TextureMng(void);

	void			Init();
	void			End();

	GUI::TexturePtr GetResource( const char *strFilename );
	void			RemoveResource( GUI::TexturePtr& hashCode );
	void			CleanUpTexture();
	void			RemoveAllResource();
	size_t			GetSize() const;

private:
	GUI::TexturePtr CreateResource( const char * strFilename );

	typedef std::map<size_t,GUI::TexturePtr>             STL_HASHMAP_TEXTURE;
	typedef std::map<size_t,GUI::TexturePtr>::iterator   STL_HASHMAP_TEXTURE_IT;
	typedef std::map<size_t,GUI::TexturePtr>::value_type STL_HASHMAP_TEXTURE_VT;

	STL_HASHMAP_TEXTURE m_listTexture;
};
