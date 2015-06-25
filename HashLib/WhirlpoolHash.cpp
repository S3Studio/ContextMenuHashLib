#include "StdAfx.h"
#include "WhirlpoolHash.h"

extern "C" {
	// defined in Whirlpool.c
	void NESSIEinit(struct NESSIEstruct * const structpointer);
	void NESSIEadd(const unsigned char * const source,
		unsigned long sourceBits,
	struct NESSIEstruct * const structpointer);
	void NESSIEfinalize(struct NESSIEstruct * const structpointer,
		unsigned char * const result);
};

CWhirlpoolHash::CWhirlpoolHash(void)
{
}

CWhirlpoolHash::~CWhirlpoolHash(void)
{
}

BOOL CWhirlpoolHash::CalcInit( LPVOID lpParam )
{
	m_strHash = _T("");
	NESSIEinit(&m_w);
	return TRUE;
}

BOOL CWhirlpoolHash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	NESSIEadd((u8 *)pBuffer, 8 * uSize, &m_w);
	return TRUE;
}

BOOL CWhirlpoolHash::CalcFinal()
{
	u8 digest[DIGESTBYTES + 1];
	ZeroMemory(digest, sizeof(u8) * (DIGESTBYTES + 1));

	NESSIEfinalize(&m_w, digest);
	TCHAR szHash[DIGESTBYTES * 2 + 1] = {0};
	for (UINT i = 0; i < DIGESTBYTES; ++i)
	{
		_stprintf_s(szHash + i * 2, 3, _T("%02x"), digest[i]);
	}
	szHash[DIGESTBYTES * 2] = _T('\0');
	m_strHash = szHash;

	return TRUE;
}
