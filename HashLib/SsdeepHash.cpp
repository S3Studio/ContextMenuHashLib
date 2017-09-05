#include "StdAfx.h"
#include "SsdeepHash.h"
#include <math.h>

const ULONG c_ulDevide = (SPAMSUM_LENGTH * MIN_BLOCKSIZE);
const DOUBLE c_dLog2 = log(2.0f);

CSsdeepRollingHash::CSsdeepRollingHash( void )
: m_x(0)
, m_y(0)
, m_z(0)
, m_c(0)
{
}

CSsdeepRollingHash::~CSsdeepRollingHash( void )
{

}

BOOL CSsdeepRollingHash::CalcInit( LPVOID lpParam )
{
	m_x = m_y = m_z = 0;
	m_c = 0;
	m_vecWindow.clear();
	m_vecWindow.resize(ROLLING_WINDOW, 0);

	return TRUE;
}

BOOL CSsdeepRollingHash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	LPBYTE pbEnd = (LPBYTE)pBuffer + uSize;
	for (LPBYTE pbCurrent = (LPBYTE)pBuffer; pbCurrent < pbEnd; ++pbCurrent)
	{
		m_y -= m_x;
		m_y += ROLLING_WINDOW * (*pbCurrent);
		m_x += (*pbCurrent);
		m_x -= m_vecWindow[m_c];
		m_vecWindow[m_c] = (*pbCurrent);
		m_c = (m_c + 1) % ROLLING_WINDOW;
		m_z <<= 5;
		m_z ^= (*pbCurrent);
	}

	return TRUE;
}

BOOL CSsdeepRollingHash::CalcFinal()
{
	return TRUE;
}

const TCHAR CSsdeepHash::s_chB64[] = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

CSsdeepHash::CSsdeepHash(void)
: m_nMinBlock(0)
, m_ul64Size(0)
{
	m_FNVParam.bAlg1a = FALSE;
}

CSsdeepHash::~CSsdeepHash(void)
{
}

BOOL CSsdeepHash::CalcInit( LPVOID lpParam )
{
	BOOL bRet = TRUE;

	m_strHash = _T("");
	m_nMinBlock = 0;
	m_ul64Size = 0;

	m_aryHashStr.clear();
	m_aryHashStr.resize(TRAD_HASH_COUNT, _T(""));

	m_aryHashHalfTailChar.clear();
	m_aryHashHalfTailChar.resize(TRAD_HASH_COUNT, _T('\0'));

	m_aryTradHash.RemoveAll();
	m_aryTradHash.SetCount(TRAD_HASH_COUNT);
	m_aryTradHash[0].Attach(new (std::nothrow)CFNVHash(TRAD_HASH_BASIS, TRAD_HASH_PRIME));
	bRet = (m_aryTradHash[0] != NULL);
	bRet = bRet && (m_aryTradHash[0]->CalcInit(&m_FNVParam));

	m_aryTradHashHalfTail.RemoveAll();
	m_aryTradHashHalfTail.SetCount(TRAD_HASH_COUNT);

	bRet = bRet && m_RHash.CalcInit(NULL);

	return bRet;
}


BOOL CSsdeepHash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	BOOL bRet = TRUE;

	m_ul64Size += uSize;
	LPBYTE pbEnd = (LPBYTE)pBuffer + uSize;
	std::vector<LPBYTE> aryLastPos;
	aryLastPos.resize(TRAD_HASH_COUNT, (LPBYTE)pBuffer);
	for (LPBYTE pbCurrent = (LPBYTE)pBuffer; pbCurrent < pbEnd; ++pbCurrent)
	{
		bRet = m_RHash.CalcStep(pbCurrent, 1);
		if (!bRet)
			break;

		for (UINT i = m_nMinBlock; i < TRAD_HASH_COUNT; ++i)
		{
			UINT32 uBlockSize = MIN_BLOCKSIZE << i;
			if ((m_RHash.GetHashDigest() % uBlockSize) == (uBlockSize - 1))
			{
				if (i < (TRAD_HASH_COUNT - 1) && m_aryTradHash[i + 1] == NULL)
				{
					// Clone from previous hash class
					m_aryTradHash[i + 1].Attach(new (std::nothrow)CFNVHash(*m_aryTradHash[i]));
				}

				bRet = m_aryTradHash[i]->CalcStep(aryLastPos[i], pbCurrent + 1 - aryLastPos[i]);
				if (!bRet)
					break;
				if (m_aryTradHashHalfTail[i] != NULL)
				{
					bRet = m_aryTradHashHalfTail[i]->CalcStep(aryLastPos[i], pbCurrent + 1 - aryLastPos[i]);
					if (!bRet)
						break;
				}
				aryLastPos[i] = pbCurrent + 1;

				//CalcFinal() makes no different to the hash of CFNVHash.
				/*bRet = m_aryTradHash[i]->CalcFinal();
				if (!bRet)
					break;*/

				if (m_aryHashStr[i].GetLength() < (SPAMSUM_LENGTH - 1))
				{
					m_aryHashStr[i] += s_chB64[m_aryTradHash[i]->GetHashDigest() % 0x40];
					if (m_aryHashStr[i].GetLength() == (SPAMSUM_LENGTH / 2))
					{
						m_aryTradHashHalfTail[i].Attach(m_aryTradHash[i].Detach());
						m_aryTradHash[i].Attach(new (std::nothrow)CFNVHash(TRAD_HASH_BASIS, TRAD_HASH_PRIME));
					}

					bRet = m_aryTradHash[i]->CalcInit(&m_FNVParam);
					if (!bRet)
						break;
				}
				else 
				{
					if (m_aryHashStr[i].GetLength() == (SPAMSUM_LENGTH - 1))
					{
						m_aryHashStr[i] += s_chB64[m_aryTradHash[i]->GetHashDigest() % 0x40];
					}
					else
					{
						m_aryHashStr[i].SetAt(SPAMSUM_LENGTH - 1, s_chB64[m_aryTradHash[i]->GetHashDigest() % 0x40]);
					}

					if (((ULONG64)(MIN_BLOCKSIZE << i) * SPAMSUM_LENGTH < m_ul64Size) && (m_aryTradHashHalfTail[i + 1] != NULL))
						m_nMinBlock = i + 1;
				}

				if (m_aryTradHashHalfTail[i] != NULL)
					m_aryHashHalfTailChar[i] = s_chB64[m_aryTradHashHalfTail[i]->GetHashDigest() % 0x40];
			}
			else
			{
				// HashBlock which index above i needs only to calculate hash value but not to update string
				break;
			}
		}
		if (!bRet)
			break;
	}
	for (UINT i = m_nMinBlock; i < TRAD_HASH_COUNT; ++i)
	{
		if (m_aryTradHash[i] == NULL)
			break;

		bRet = m_aryTradHash[i]->CalcStep(aryLastPos[i], pbEnd - aryLastPos[i]);
		if (!bRet)
			break;

		if (m_aryTradHashHalfTail[i] != NULL)
			bRet = m_aryTradHashHalfTail[i]->CalcStep(aryLastPos[i], pbEnd - aryLastPos[i]);
		if (!bRet)
			break;
	}

	return bRet;
}

BOOL CSsdeepHash::CalcFinal()
{
	UINT nGuessIndex = ((UINT) ceil(log((long double)m_ul64Size / c_ulDevide) / c_dLog2));
	if (nGuessIndex > NUM_BLOCKHASHES - 2)
		nGuessIndex = NUM_BLOCKHASHES - 2;
	for (UINT i = 0; i <= nGuessIndex; ++i)
	{
		UINT uHashIndex = nGuessIndex - i;
		if (m_aryHashStr[uHashIndex].GetLength() >= (SPAMSUM_LENGTH / 2))
		{
			SetHashVal(uHashIndex);

			return TRUE;
		}
	}

	// Use min blocksize
	SetHashVal(0);
	return TRUE;
}

void CSsdeepHash::SetHashVal( UINT nIndex )
{
	if (m_RHash.GetHashDigest() > 0)
	{
		m_aryTradHash[nIndex]->CalcFinal();
		if (m_aryHashStr[nIndex].GetLength() < SPAMSUM_LENGTH)
			m_aryHashStr[nIndex] += s_chB64[m_aryTradHash[nIndex]->GetHashDigest() % 0x40];
		else
			m_aryHashStr[nIndex].SetAt(SPAMSUM_LENGTH - 1, s_chB64[m_aryTradHash[nIndex]->GetHashDigest() % 0x40]);

		m_aryTradHash[nIndex + 1]->CalcFinal();
		if (m_aryHashStr[nIndex + 1].GetLength() < (SPAMSUM_LENGTH / 2))
		{
			m_aryHashStr[nIndex + 1] += s_chB64[m_aryTradHash[nIndex + 1]->GetHashDigest() % 0x40];
		}
		if (m_aryTradHashHalfTail[nIndex + 1] != NULL)
		{
			m_aryHashHalfTailChar[nIndex + 1] = s_chB64[m_aryTradHashHalfTail[nIndex + 1]->GetHashDigest() % 0x40];
		}
	}

	CString strHalfHash = m_aryHashStr[nIndex + 1];
	if (m_aryTradHashHalfTail[nIndex + 1] != NULL && m_aryHashHalfTailChar[nIndex + 1] != '\0')
	{
		CString strTemp;
		strTemp.Format(_T("%s%c"), strHalfHash.Left(SPAMSUM_LENGTH / 2 - 1), m_aryHashHalfTailChar[nIndex + 1]);
		strHalfHash = strTemp;
	}
	m_strHash.Format(_T("%u:%s:%s"), MIN_BLOCKSIZE << nIndex, m_aryHashStr[nIndex], strHalfHash);
}
