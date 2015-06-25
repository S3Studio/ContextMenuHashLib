#include "StdAfx.h"
#include "OSMd4Hash.h"

#define MD4_SIZE 32

COSMd4Hash::COSMd4Hash(void)
: m_fnMD4Init(NULL)
, m_fnMD4Update(NULL)
, m_fnMD4Final(NULL)
, m_hLibrary(NULL)
{
}

COSMd4Hash::~COSMd4Hash(void)
{
	if (m_hLibrary)
		FreeLibrary(m_hLibrary);
}

BOOL COSMd4Hash::CalcInit( LPVOID lpParam )
{
	if (PrepareLib() == FALSE)
		return FALSE;

	ZeroMemory(&m_CTX, sizeof(m_CTX));
	m_fnMD4Init(&m_CTX);
	m_strHash = _T("");

	return TRUE;
}

BOOL COSMd4Hash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	m_fnMD4Update(&m_CTX, (PUCHAR)pBuffer, uSize);

	return TRUE;
}

BOOL COSMd4Hash::CalcFinal()
{
	m_fnMD4Final(&m_CTX);

	TCHAR szHash[MD4_SIZE + 1] = {0};
	for (UINT i = 0; i < MD4_SIZE / 2; ++i)
	{
		_stprintf_s(szHash + i * 2, 3, _T("%02x"), m_CTX.digest[i]);
	}
	szHash[MD4_SIZE] = _T('\0');

	m_strHash = szHash;

	return TRUE;
}

BOOL COSMd4Hash::PrepareLib()
{
	if (m_hLibrary == NULL)
	{
		m_hLibrary = LoadLibrary(_T("advapi32.dll"));
		if (m_hLibrary == NULL)
			return FALSE;
	}

	m_fnMD4Init = (MD4INIT)GetProcAddress(m_hLibrary, "MD4Init");
	m_fnMD4Update = (MD4UPDATE)GetProcAddress(m_hLibrary, "MD4Update");
	m_fnMD4Final = (MD4FINAL)GetProcAddress(m_hLibrary, "MD4Final");

	return (m_fnMD4Init != NULL && m_fnMD4Update != NULL && m_fnMD4Final != NULL);
}
