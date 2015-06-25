#include "StdAfx.h"
#include "OSMd5Hash.h"
#include "wincrypt.h"

#define MD5_SIZE 32

COSMd5Hash::COSMd5Hash(void)
: m_fnMD5Init(NULL)
, m_fnMD5Update(NULL)
, m_fnMD5Final(NULL)
, m_hLibrary(NULL)
{
}

COSMd5Hash::~COSMd5Hash(void)
{
	if (m_hLibrary)
		FreeLibrary(m_hLibrary);
}

BOOL COSMd5Hash::CalcInit( LPVOID lpParam )
{
	if (PrepareLib() == FALSE)
		return FALSE;

	ZeroMemory(&m_CTX, sizeof(m_CTX));
	m_fnMD5Init(&m_CTX);
	m_strHash = _T("");
	
	return TRUE;
}

BOOL COSMd5Hash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	m_fnMD5Update(&m_CTX, (PUCHAR)pBuffer, uSize);

	return TRUE;
}

BOOL COSMd5Hash::CalcFinal()
{
	m_fnMD5Final(&m_CTX);

	TCHAR szHash[MD5_SIZE + 1] = {0};
	for (UINT i = 0; i < MD5_SIZE / 2; ++i)
	{
		_stprintf_s(szHash + i * 2, 3, _T("%02x"), m_CTX.digest[i]);
	}
	szHash[MD5_SIZE] = _T('\0');

	m_strHash = szHash;

	return TRUE;
}

BOOL COSMd5Hash::PrepareLib()
{
	if (m_hLibrary == NULL)
	{
		m_hLibrary = LoadLibrary(_T("advapi32.dll"));
		if (m_hLibrary == NULL)
			return FALSE;
	}

	m_fnMD5Init = (MD5INIT)GetProcAddress(m_hLibrary, "MD5Init");
	m_fnMD5Update = (MD5UPDATE)GetProcAddress(m_hLibrary, "MD5Update");
	m_fnMD5Final = (MD5FINAL)GetProcAddress(m_hLibrary, "MD5Final");

	return (m_fnMD5Init != NULL && m_fnMD5Update != NULL && m_fnMD5Final != NULL);
}
