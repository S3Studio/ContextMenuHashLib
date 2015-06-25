#include "StdAfx.h"
#include "ZlibAdler32Hash.h"
#include "ThirdParty/zlib/zlib.h"

#define ADLER32_SIZE 8

CZlibAdler32Hash::CZlibAdler32Hash(void)
: m_ulAlder(0)
{
}

CZlibAdler32Hash::~CZlibAdler32Hash(void)
{
}

BOOL CZlibAdler32Hash::CalcInit( LPVOID lpParam )
{
	m_strHash = _T("");
	m_ulAlder = adler32(0L, Z_NULL, 0);
	return TRUE;
}

BOOL CZlibAdler32Hash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	m_ulAlder = adler32(m_ulAlder, (LPBYTE)pBuffer, uSize);
	return TRUE;
}

BOOL CZlibAdler32Hash::CalcFinal()
{
	m_strHash.Format(_T("%0*x"), ADLER32_SIZE, m_ulAlder);

	return TRUE;
}
