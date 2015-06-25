#include "StdAfx.h"
#include "ZLibCrc32Hash.h"
#include "ThirdParty/zlib/zlib.h"

#define CRC32_SIZE 8

CZlibCrc32Hash::CZlibCrc32Hash(void)
: m_ulCRC(0)
{
}

CZlibCrc32Hash::~CZlibCrc32Hash(void)
{
}

BOOL CZlibCrc32Hash::CalcInit( LPVOID lpParam )
{
	m_strHash = _T("");
	m_ulCRC = crc32(0L, Z_NULL, 0);
	return TRUE;
}

BOOL CZlibCrc32Hash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	m_ulCRC = crc32(m_ulCRC, (LPBYTE)pBuffer, uSize);
	return TRUE;
}

BOOL CZlibCrc32Hash::CalcFinal()
{
	m_strHash.Format(_T("%0*x"), CRC32_SIZE, m_ulCRC);

	return TRUE;
}
