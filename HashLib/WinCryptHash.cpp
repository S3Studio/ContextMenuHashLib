#include "StdAfx.h"
#include "WinCryptHash.h"
#include <vector>
using std::vector;

CWinCryptHash::CWinCryptHash( LPCTSTR szHashMethodName, int nProvType, int nHashType )
: m_strHashMethodName(szHashMethodName)
, m_nProvType(nProvType)
, m_nHashType(nHashType)
, m_hProv(NULL)
, m_hHash(NULL)
{
}

CWinCryptHash::~CWinCryptHash(void)
{
	if (m_hHash)
	{
		CryptDestroyHash(m_hHash);
		m_hHash = NULL;
	}
	if (m_hProv)
	{
		CryptReleaseContext(m_hProv, 0);
		m_hProv = NULL;
	}
}

BOOL CWinCryptHash::CalcInit( LPVOID lpParam )
{
	if (!CreateHProv() || !CreateHHash())
		return FALSE;

	m_strHash = _T("");

	return TRUE;
}

BOOL CWinCryptHash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	if (m_hHash == NULL)
		return FALSE;

	return CryptHashData(m_hHash, (LPBYTE const)pBuffer, uSize, 0);
}

BOOL CWinCryptHash::CalcFinal()
{
	DWORD dwSize = 0;
	DWORD dwSizeByte = sizeof(dwSize);
	if (CryptGetHashParam(m_hHash, HP_HASHSIZE, (BYTE *)&dwSize, &dwSizeByte, 0) == FALSE)
		return FALSE;

	vector<BYTE> pHash;
	pHash.resize(dwSize, 0);
	if (CryptGetHashParam(m_hHash, HP_HASHVAL, &(pHash[0]), &dwSize, 0) == FALSE)
		return FALSE;

	CryptDestroyHash(m_hHash);
	m_hHash = NULL;

	vector<TCHAR> szHash;
	szHash.resize(dwSize * 2 + 1, 0);
	for (UINT i = 0; i < dwSize; ++i)
	{
		_stprintf_s(&(szHash[i * 2]), 3, _T("%02x"), pHash[i]);
	}

 	m_strHash = &(szHash[0]);

	return TRUE;
}

BOOL CWinCryptHash::CreateHProv()
{
	if (m_hProv == NULL)
	{
		return CryptAcquireContext(&m_hProv, NULL, NULL, m_nProvType, CRYPT_VERIFYCONTEXT);
	}
	else
	{
		return TRUE;
	}
}

BOOL CWinCryptHash::CreateHHash()
{
	if (m_hHash != NULL)
	{
		CryptDestroyHash(m_hHash);
		m_hHash = NULL;
	}

	if (m_hProv == NULL)
		return FALSE;

	return CryptCreateHash(m_hProv, m_nHashType, 0, 0, &m_hHash);
}
