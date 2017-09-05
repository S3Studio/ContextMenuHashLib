#include "StdAfx.h"
#include "KeccakHash.h"

CKeccakHash::CKeccakHash(LPCTSTR szHashMethodName, int nHashBitLen)
: m_strHashMethodName(szHashMethodName)
, m_nHashBitLen(nHashBitLen)
{
}

CKeccakHash::~CKeccakHash(void)
{
}

BOOL CKeccakHash::CalcInit(LPVOID lpParam)
{
	return (Init(&m_State, m_nHashBitLen) == SUCCESS);
}

BOOL CKeccakHash::CalcStep(LPVOID pBuffer, UINT uSize)
{
	return (Update(&m_State, (const BitSequence *)pBuffer, uSize * 8) == SUCCESS);
}

BOOL CKeccakHash::CalcFinal()
{
	CStringA strHashA;
	BitSequence* pHashBuf = (BitSequence *)strHashA.GetBuffer(m_nHashBitLen / 8);
	ZeroMemory(pHashBuf, m_nHashBitLen / 8);
	HashReturn ret = Final(&m_State, pHashBuf);
	strHashA.ReleaseBuffer(m_nHashBitLen / 8);
	
	if (ret != SUCCESS)
		return FALSE;

	m_strHash = _T("");
	TCHAR szHash[3] = {0};
	for (UINT i = 0; i < m_nHashBitLen / 8; ++i)
	{
		_stprintf_s(szHash, 3, _T("%02x"), (UCHAR)strHashA.GetAt(i));
		m_strHash += szHash;
	}

	return TRUE;
}