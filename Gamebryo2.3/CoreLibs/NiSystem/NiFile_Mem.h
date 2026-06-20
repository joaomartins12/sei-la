
#ifndef NIFILE_MEM_H
#define NIFILE_MEM_H

#include "NiFile.h"

class NiMemFile
	: public NiFile
{
public:
	NiMemFile(void* pvBuffer, OpenMode eMode, unsigned int uiBytes);
	virtual ~NiMemFile(void);

	virtual char* GetBuffer();
	virtual void Seek(int iOffset, int iWhence);
	virtual unsigned int GetFileSize() const;

private:
	unsigned int m_uiSize;
};

#endif