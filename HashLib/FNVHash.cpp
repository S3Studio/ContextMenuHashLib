#include "StdAfx.h"
#include "FNVHash.h"

#define FNV_SIZE 8

CFNVHash::CFNVHash(UINT32 nBasis, UINT32 nPrime)
: m_nHash(0)
, m_nBasis(nBasis)
, m_nPrime(nPrime)
{
	m_InitParam.bAlg1a = TRUE;
}

CFNVHash::~CFNVHash(void)
{
}

BOOL CFNVHash::CalcInit( LPVOID lpParam )
{
	if (lpParam != NULL)
		m_InitParam = *((InitParam *)lpParam);

	m_strHash = _T("");
	m_nHash = m_nBasis;

	return TRUE;
}

BOOL CFNVHash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	LPBYTE pbEnd = (LPBYTE)pBuffer + uSize;
	for (LPBYTE pbCurrent = (LPBYTE)pBuffer; pbCurrent < pbEnd; ++pbCurrent)
	{
		if (m_InitParam.bAlg1a)
		{
			m_nHash = (m_nHash ^ (*pbCurrent)) * m_nPrime;
		} 
		else
		{
			m_nHash = (m_nHash * m_nPrime) ^ (*pbCurrent);
		}
	}

	return TRUE;
}

BOOL CFNVHash::CalcFinal()
{
	m_strHash.Format(_T("%0*x"), FNV_SIZE, m_nHash);

	return TRUE;
}
