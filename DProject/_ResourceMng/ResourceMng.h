
#pragma once
#include "TextureMng.h"

class ResourceMng : public CSingleton<ResourceMng>
{
public:
	ResourceMng(void);
	~ResourceMng(void);

	void					Init();
	void					End();
	void					CleanUpResource();	// 사용하지 않는 리소스 정리
	size_t					GetTextureResourceSize() const;

	GUI::TexturePtr			GetTexture( const char *strFilename );
	GUI::TexturePtr			GetTexture( const wchar_t *strFilename );	
	void					RemoveTexture( GUI::TexturePtr& pRemove );	// 리소스 삭제
	void					RemoveAllResource();	// 리소스를 모드 지운다.

private:
	TextureMng				m_TextureMng;
};

#define RESOURCEMGR_ST		ResourceMng::GetSingleton()
#define RESOURCEMGR_STPTR	ResourceMng::GetSingletonPtr()