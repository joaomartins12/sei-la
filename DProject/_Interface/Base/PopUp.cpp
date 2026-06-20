#include "stdafx.h"
#include "PopUp.h"

void cPopUp::Delete()
{
	for (int i = 0; i < 9; ++i)
	{
		SAFE_NIDELETE(m_pSprite[i]);
	}
}

void cPopUp::Init(cWindow* pParent, CsPoint pos, CsPoint size, CsPoint patch, char* cPathName, char* cExpName, bool bApplyWindowSize)
{
	if (size.x <= 9 || size.y <= 9)
		return;

	if (!cPathName || !cExpName)
		return;

	assert_cs(m_pSprite[0] == NULL);

	m_ptPos = pos;
	m_ptPatch = patch;
	m_ptSize = size;

	char cFull[MAX_FILENAME];

	sprintf_s(cFull, MAX_FILENAME, "%s1.%s", cPathName, cExpName);
	m_pSprite[0] = NiNew cSprite;
	if (m_pSprite[0])
		m_pSprite[0]->Init(pParent, pos, CsPoint(patch.x, patch.y), cFull, bApplyWindowSize);

	sprintf_s(cFull, MAX_FILENAME, "%s2.%s", cPathName, cExpName);
	m_pSprite[1] = NiNew cSprite;
	if (m_pSprite[1])
		m_pSprite[1]->Init(pParent, pos + CsPoint(patch.x, 0), CsPoint(size.x - patch.x * 2, patch.y), cFull, bApplyWindowSize);

	sprintf_s(cFull, MAX_FILENAME, "%s3.%s", cPathName, cExpName);
	m_pSprite[2] = NiNew cSprite;
	if (m_pSprite[2])
		m_pSprite[2]->Init(pParent, pos + CsPoint(size.x - patch.x, 0), CsPoint(patch.x, patch.y), cFull, bApplyWindowSize);

	sprintf_s(cFull, MAX_FILENAME, "%s4.%s", cPathName, cExpName);
	m_pSprite[3] = NiNew cSprite;
	if (m_pSprite[3])
		m_pSprite[3]->Init(pParent, pos + CsPoint(0, patch.y), CsPoint(patch.x, size.y - patch.y * 2), cFull, bApplyWindowSize);

	sprintf_s(cFull, MAX_FILENAME, "%s5.%s", cPathName, cExpName);
	m_pSprite[4] = NiNew cSprite;
	if (m_pSprite[4])
		m_pSprite[4]->Init(pParent, pos + CsPoint(patch.x, patch.y), CsPoint(size.x - patch.x * 2, size.y - patch.y * 2), cFull, bApplyWindowSize);

	sprintf_s(cFull, MAX_FILENAME, "%s6.%s", cPathName, cExpName);
	m_pSprite[5] = NiNew cSprite;
	if (m_pSprite[5])
		m_pSprite[5]->Init(pParent, pos + CsPoint(size.x - patch.x, patch.y), CsPoint(patch.x, size.y - patch.y * 2), cFull, bApplyWindowSize);

	sprintf_s(cFull, MAX_FILENAME, "%s7.%s", cPathName, cExpName);
	m_pSprite[6] = NiNew cSprite;
	if (m_pSprite[6])
		m_pSprite[6]->Init(pParent, pos + CsPoint(0, size.y - patch.y), CsPoint(patch.x, patch.y), cFull, bApplyWindowSize);

	sprintf_s(cFull, MAX_FILENAME, "%s8.%s", cPathName, cExpName);
	m_pSprite[7] = NiNew cSprite;
	if (m_pSprite[7])
		m_pSprite[7]->Init(pParent, pos + CsPoint(patch.x, size.y - patch.y), CsPoint(size.x - patch.x * 2, patch.y), cFull, bApplyWindowSize);

	sprintf_s(cFull, MAX_FILENAME, "%s9.%s", cPathName, cExpName);
	m_pSprite[8] = NiNew cSprite;
	if (m_pSprite[8])
		m_pSprite[8]->Init(pParent, pos + CsPoint(size.x - patch.x, size.y - patch.y), CsPoint(patch.x, patch.y), cFull, bApplyWindowSize);
}

void cPopUp::ReSize(CsPoint pos, CsPoint size)
{
	m_ptPos = pos;
	m_ptSize = size;

	if (size.x <= 0 || size.y <= 0)
		return;

	if (m_pSprite[0])
		m_pSprite[0]->SetPosSize(pos, CsPoint(m_ptPatch.x, m_ptPatch.y));

	if (m_pSprite[1])
		m_pSprite[1]->SetPosSize(pos + CsPoint(m_ptPatch.x, 0), CsPoint(size.x - m_ptPatch.x * 2, m_ptPatch.y));

	if (m_pSprite[2])
		m_pSprite[2]->SetPosSize(pos + CsPoint(size.x - m_ptPatch.x, 0), CsPoint(m_ptPatch.x, m_ptPatch.y));

	if (m_pSprite[3])
		m_pSprite[3]->SetPosSize(pos + CsPoint(0, m_ptPatch.y), CsPoint(m_ptPatch.x, size.y - m_ptPatch.y * 2));

	if (m_pSprite[4])
		m_pSprite[4]->SetPosSize(pos + CsPoint(m_ptPatch.x, m_ptPatch.y), CsPoint(size.x - m_ptPatch.x * 2, size.y - m_ptPatch.y * 2));

	if (m_pSprite[5])
		m_pSprite[5]->SetPosSize(pos + CsPoint(size.x - m_ptPatch.x, m_ptPatch.y), CsPoint(m_ptPatch.x, size.y - m_ptPatch.y * 2));

	if (m_pSprite[6])
		m_pSprite[6]->SetPosSize(pos + CsPoint(0, size.y - m_ptPatch.y), CsPoint(m_ptPatch.x, m_ptPatch.y));

	if (m_pSprite[7])
		m_pSprite[7]->SetPosSize(pos + CsPoint(m_ptPatch.x, size.y - m_ptPatch.y), CsPoint(size.x - m_ptPatch.x * 2, m_ptPatch.y));

	if (m_pSprite[8])
		m_pSprite[8]->SetPosSize(pos + CsPoint(size.x - m_ptPatch.x, size.y - m_ptPatch.y), CsPoint(m_ptPatch.x, m_ptPatch.y));
}

void cPopUp::Render()
{
	for (int i = 0; i < 9; ++i)
	{
		if (m_pSprite[i])
			m_pSprite[i]->Render();
	}
}

void cPopUp::Render(CsPoint ptPos)
{
	for (int i = 0; i < 9; ++i)
	{
		if (m_pSprite[i])
			m_pSprite[i]->Render(ptPos);
	}
}

void cPopUp::ResetDevice()
{
	for (int i = 0; i < 9; ++i)
	{
		if (m_pSprite[i])
			m_pSprite[i]->ResetDevice();
	}
}

void cPopUp::SetAlpha(float fAlpha)
{
	for (int i = 0; i < 9; ++i)
	{
		if (m_pSprite[i])
			m_pSprite[i]->SetAlpha(fAlpha);
	}
}