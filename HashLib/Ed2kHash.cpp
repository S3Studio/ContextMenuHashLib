#include "StdAfx.h"
#include "Ed2kHash.h"

#define ED2K_CHUNK_SIZE 9728000

CString strNullMd4 = _T("31d6cfe0d16ae931b73c59d7e0c089c0");

CEd2kHash::CEd2kHash(void)
: m_uChunkRemainSize(ED2K_CHUNK_SIZE)
, m_bFailed(TRUE)
{
	m_InitParam.bAddtionalChunkWhenAligned = FALSE;
	ZeroMemory(m_cHashFirst, MD4_SIZE / 2);
}

CEd2kHash::~CEd2kHash(void)
{
}

BOOL CEd2kHash::CalcInit( LPVOID lpParam )
{
	if (lpParam != NULL)
		m_InitParam = *((InitParam *)lpParam);

	m_pChunkHasher.Free();
	m_pOverallHasher.Free();
	m_strHash = _T("");
	ZeroMemory(m_cHashFirst, MD4_SIZE / 2);
	m_uChunkRemainSize = ED2K_CHUNK_SIZE;
	m_bFailed = FALSE;

	return TRUE;
}

BOOL CEd2kHash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	if (m_bFailed || pBuffer == NULL)
		return FALSE;

	LPVOID pPos = pBuffer;
	while (uSize > 0)
	{
		if (m_pChunkHasher == NULL)
		{
			m_pChunkHasher.Attach(new (std::nothrow)COSMd4Hash);
			if (m_pChunkHasher == NULL)
			{
				m_bFailed = TRUE;
				return FALSE;
			}

			if (m_pChunkHasher->CalcInit(NULL) == FALSE)
			{
				m_pChunkHasher.Free();
				m_bFailed = TRUE;
				return FALSE;
			}
		}

		UINT uHashSize = (uSize < m_uChunkRemainSize ? uSize : m_uChunkRemainSize);
		if (m_pChunkHasher->CalcStep(pPos, uHashSize) == FALSE)
		{
			m_pChunkHasher.Free();
			m_bFailed = TRUE;
			return FALSE;
		}

		uSize -= uHashSize;
		m_uChunkRemainSize -= uHashSize;
		pPos = (LPCH)pPos + uHashSize;

		if (m_uChunkRemainSize == 0)
		{
			if (m_pChunkHasher->CalcFinal() == FALSE)
			{
				m_pChunkHasher.Free();
				m_bFailed = TRUE;
				return FALSE;
			}

			if (OnChunkFinish() == FALSE)
			{
				m_bFailed = TRUE;
				return FALSE;
			}

			m_pChunkHasher.Free();
			m_uChunkRemainSize = ED2K_CHUNK_SIZE;

			if (m_InitParam.bAddtionalChunkWhenAligned == TRUE)
			{
				// Old method, see https://en.wikipedia.org/wiki/Ed2k_URI_scheme#eD2k_hash_algorithm
				m_pChunkHasher.Attach(new (std::nothrow)COSMd4Hash);
				if (m_pChunkHasher == NULL)
				{
					m_bFailed = TRUE;
					return FALSE;
				}

				if (m_pChunkHasher->CalcInit(NULL) == FALSE)
				{
					m_pChunkHasher.Free();
					m_bFailed = TRUE;
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CEd2kHash::CalcFinal()
{
	if (m_bFailed)
		return FALSE;

	if (m_pChunkHasher)
	{
		if (m_pChunkHasher->CalcFinal() == FALSE)
		{
			m_pChunkHasher.Free();
			m_bFailed = TRUE;
			return FALSE;
		}

		if (OnChunkFinish() == FALSE)
		{
			m_bFailed = TRUE;
			return FALSE;
		}

		m_pChunkHasher.Free();
	}

	if (m_InitParam.bAddtionalChunkWhenAligned == FALSE)
	{
		// new method
		if (m_pOverallHasher)
		{
			// two or more chunks
			if (m_pOverallHasher->CalcFinal() == FALSE)
			{
				m_pOverallHasher.Free();
				m_bFailed = TRUE;
				return FALSE;
			}

			m_strHash = m_pOverallHasher->GetHashStr();
			return TRUE;
		}
		else if (m_strHash != _T(""))
		{
			// one chunk
			// Hash already in m_strHash
			return TRUE;
		}
		else
		{
			// no data received
			m_strHash = strNullMd4;
			return TRUE;
		}
	}
	else
	{
		// old method
		if (m_pOverallHasher)
		{
			if (m_pOverallHasher->CalcFinal() == FALSE)
			{
				m_pOverallHasher.Free();
				m_bFailed = TRUE;
				return FALSE;
			}

			m_strHash = m_pOverallHasher->GetHashStr();
			return TRUE;
		}
		else if (m_strHash != _T(""))
		{
			// one chunk and not aligned
			// Hash already in m_strHash
			return TRUE;
		}
		else
		{
			// no data received
			m_strHash = strNullMd4;
			return TRUE;
		}
	}
}

BOOL CEd2kHash::OnChunkFinish()
{
	if (m_InitParam.bAddtionalChunkWhenAligned == FALSE)
	{
		// new method
		if (m_pOverallHasher == NULL)
		{
			if (m_strHash == _T(""))
			{
				// first chunk
				m_strHash = m_pChunkHasher->GetHashStr();
				memcpy(m_cHashFirst, m_pChunkHasher->GetHashDigest(), MD4_SIZE / 2);
			}
			else
			{
				// second chunk
				m_pOverallHasher.Attach(new (std::nothrow)COSMd4Hash);
				BOOL bRes = (m_pOverallHasher != NULL);
				bRes = bRes && m_pOverallHasher->CalcInit(NULL);
				bRes = bRes && m_pOverallHasher->CalcStep(m_cHashFirst, MD4_SIZE / 2);
				bRes = bRes && m_pOverallHasher->CalcStep(m_pChunkHasher->GetHashDigest(), MD4_SIZE / 2);

				if (bRes == FALSE)
				{
					m_pOverallHasher.Free();
					return FALSE;
				}
			}
		}
		else
		{
			// other situation
			if ((m_pOverallHasher->CalcStep(m_pChunkHasher->GetHashDigest(), MD4_SIZE / 2)) == FALSE)
			{
				m_pOverallHasher.Free();
				return FALSE;
			}
		}
	}
	else
	{
		// old method
		if (m_pOverallHasher == NULL)
		{
			// first chunk
			m_strHash = m_pChunkHasher->GetHashStr();

			if (m_uChunkRemainSize == 0)
			{
				// aligned
				m_pOverallHasher.Attach(new (std::nothrow)COSMd4Hash);
				if (m_pOverallHasher == NULL)
				{
					return FALSE;
				}

				if (m_pOverallHasher->CalcInit(NULL) == FALSE)
				{
					m_pOverallHasher.Free();
					return FALSE;
				}
			}
		}

		if (m_pOverallHasher)
		{
			if ((m_pOverallHasher->CalcStep(m_pChunkHasher->GetHashDigest(), MD4_SIZE / 2)) == FALSE)
			{
				m_pOverallHasher.Free();
				return FALSE;
			}
		}
	}

	return TRUE;
}
