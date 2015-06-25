#include "StdAfx.h"
#include "BtihHash.h"
#include "HashLibFactory.h"

#define LODWORD(l) ((DWORD)((LONG_PTR)(l) & 0xffffffff))
#define STR_LEN_LIMIT 100
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

CBtihHashSection::CBtihHashSection( void )
{
	ZeroMemory(&m_data, sizeof(m_data));
}

CBtihHashSection::~CBtihHashSection( void )
{

}

HRESULT CBtihHashSection::ParseBuffer( LPCH pBuffer, UINT uSize, LPCH* ppNextPos )
{
	if (pBuffer == NULL || ppNextPos == NULL || (*ppNextPos) == NULL)
		return E_FAIL;

	(*ppNextPos) = pBuffer;
	UINT uRemainSize = uSize;
	while (m_data.bFinished == FALSE && uRemainSize != 0)
	{
		if (m_data.Type == emST_UNKNOWN)
		{
			if (FAILED(ParseType((*ppNextPos), uRemainSize, ppNextPos)))
				return E_FAIL;
		}
		else
		{
			HRESULT hr = E_FAIL;
			switch (m_data.Type)
			{
			case emST_STRING:
				hr = ParseString((*ppNextPos), uRemainSize, ppNextPos);
				break;
			case emST_INTEGER:
				hr = ParseInteger((*ppNextPos), uRemainSize, ppNextPos);
				break;
			case emST_LIST:
				hr = ParseList((*ppNextPos), uRemainSize, ppNextPos);
				break;
			case emST_DICT:
				hr = ParseDict((*ppNextPos), uRemainSize, ppNextPos);
				break;
			}

			if (FAILED(hr))
				return E_FAIL;

			break;
		}

		if ((*ppNextPos) < pBuffer || (*ppNextPos) > pBuffer + uSize)
			return E_FAIL;
		uRemainSize = uSize - LODWORD((*ppNextPos) - pBuffer);
	}

	return (m_data.bFinished ? S_OK : S_FALSE);
}

HRESULT CBtihHashSection::ParseType( LPCH pBuffer, UINT uSize, LPCH* ppNextPos )
{
	if (pBuffer == NULL || ppNextPos == NULL || (*ppNextPos) == NULL)
		return E_FAIL;

	if (uSize < 1)
	{
		(*ppNextPos) = pBuffer;
		return S_OK;
	}

	switch (static_cast<char>(*pBuffer))
	{
	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		m_data.Type = emST_STRING;
		m_data.TypeData.SData.bStrLengthValid = FALSE;
		m_data.TypeData.SData.uStrRemainSize = 0;
		(*ppNextPos) = pBuffer;
		break;
	case 'i': case 'I':
		m_data.Type = emST_INTEGER;
		(*ppNextPos) = pBuffer + 1;
		break;
	case 'l': case 'L':
		m_data.Type = emST_LIST;
		(*ppNextPos) = pBuffer + 1;
		break;
	case 'd': case 'D':
		m_data.Type = emST_DICT;
		(*ppNextPos) = pBuffer + 1;
		break;
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBtihHashSection::ParseString( LPCH pBuffer, UINT uSize, LPCH* ppNextPos )
{
	if (pBuffer == NULL || ppNextPos == NULL || (*ppNextPos) == NULL)
		return E_FAIL;

	if (m_data.Type != emST_STRING)
		return E_FAIL;

	(*ppNextPos) = pBuffer;
	UINT uRemainSize = uSize;
	while (m_data.bFinished == FALSE && uRemainSize != 0)
	{
		if (m_data.TypeData.SData.bStrLengthValid == FALSE)
		{
			char chNext = static_cast<char>(**ppNextPos);
			if (chNext >= '0' && chNext <= '9')
			{
				m_data.TypeData.SData.uStrRemainSize *= 10;
				m_data.TypeData.SData.uStrRemainSize += (chNext - '0');
			}
			else if (chNext == ':')
			{
				m_data.TypeData.SData.bStrLengthValid = TRUE;
				m_strStringValue = "";
			}
			else
			{
				return E_FAIL;
			}

			++(*ppNextPos);
		}
		else
		{
			if (m_data.TypeData.SData.uStrRemainSize <= uRemainSize)
			{
				if (m_strStringValue.GetLength() < STR_LEN_LIMIT )
				{
					UINT uiLen = MIN(m_data.TypeData.SData.uStrRemainSize, STR_LEN_LIMIT);
					m_strStringValue.Append(*ppNextPos, uiLen);
				}
				(*ppNextPos) += m_data.TypeData.SData.uStrRemainSize;
				m_data.TypeData.SData.uStrRemainSize = 0;
				m_data.bFinished = TRUE;
			}
			else
			{
				if (m_strStringValue.GetLength() < STR_LEN_LIMIT )
				{
					UINT uiLen = MIN(uRemainSize, STR_LEN_LIMIT);
					m_strStringValue.Append(*ppNextPos, uiLen);
				}
				(*ppNextPos) += uRemainSize;
				m_data.TypeData.SData.uStrRemainSize -= uRemainSize;
			}

			break;
		}

		if ((*ppNextPos) < pBuffer || (*ppNextPos) > pBuffer + uSize)
			return E_FAIL;
		uRemainSize = uSize - LODWORD((*ppNextPos) - pBuffer);
	}

	return S_OK;
}

HRESULT CBtihHashSection::ParseInteger( LPCH pBuffer, UINT uSize, LPCH* ppNextPos )
{
	if (pBuffer == NULL || ppNextPos == NULL || (*ppNextPos) == NULL)
		return E_FAIL;

	if (m_data.Type != emST_INTEGER)
		return E_FAIL;

	(*ppNextPos) = pBuffer;
	UINT uRemainSize = uSize;
	while (m_data.bFinished == FALSE && uRemainSize != 0)
	{
		char chNext = static_cast<char>(**ppNextPos);
		++(*ppNextPos);
		if (chNext >= '0' && chNext <= '9')
		{
			/*
			// ingore the value
			m_data.TypeData.SData.uStrRemainSize *= 10;
			m_data.TypeData.SData.uStrRemainSize += (chNext - '0');
			*/
		}
		else if (chNext == '-')
		{
			/*
			// should be more rigorous
			*/
		}
		else if (chNext == 'e' || chNext == 'E')
		{
			m_data.bFinished = TRUE;
			break;
		}
		else
		{
			return E_FAIL;
		}

		if ((*ppNextPos) < pBuffer || (*ppNextPos) > pBuffer + uSize)
			return E_FAIL;
		uRemainSize = uSize - LODWORD((*ppNextPos) - pBuffer);
	}

	return S_OK;
}

HRESULT CBtihHashSection::ParseList( LPCH pBuffer, UINT uSize, LPCH* ppNextPos )
{
	if (pBuffer == NULL || ppNextPos == NULL || (*ppNextPos) == NULL)
		return E_FAIL;

	if (m_data.Type != emST_LIST)
		return E_FAIL;

	(*ppNextPos) = pBuffer;
	UINT uRemainSize = uSize;
	if (m_data.bFinished == FALSE && uRemainSize != 0)
	{
		char chNext = static_cast<char>(**ppNextPos);
		if (chNext == 'e' || chNext == 'E')
		{
			++(*ppNextPos);
			m_data.bFinished = TRUE;
		}
	}

	return S_OK;
}

HRESULT CBtihHashSection::ParseDict( LPCH pBuffer, UINT uSize, LPCH* ppNextPos )
{
	if (pBuffer == NULL || ppNextPos == NULL || (*ppNextPos) == NULL)
		return E_FAIL;

	if (m_data.Type != emST_DICT)
		return E_FAIL;

	(*ppNextPos) = pBuffer;
	UINT uRemainSize = uSize;
	if (m_data.bFinished == FALSE && uRemainSize != 0)
	{
		char chNext = static_cast<char>(**ppNextPos);
		if (chNext == 'e' || chNext == 'E')
		{
			++(*ppNextPos);
			m_data.bFinished = TRUE;
		}
	}

	return S_OK;
}

CBtihHash::CBtihHash(void)
: m_State(emS_READY)
{
	ZeroMemory(&m_InfoSectionState, sizeof(m_InfoSectionState));
}

CBtihHash::~CBtihHash(void)
{
}

BOOL CBtihHash::CalcInit( LPVOID lpParam )
{
	while (m_stackSection.size() > 0)
	{
		m_stackSection.pop();
	}
	CBtihHashSection secRoot;
	m_stackSection.push(secRoot);
	m_strHash = _T("");
	m_pSha1Hash.Free();
	m_State = emS_CALC;

	return TRUE;
}

BOOL CBtihHash::CalcStep( LPVOID pBuffer, UINT uSize )
{
	if (m_State != emS_CALC)
		return CalcFinal();

	InitInfoSection(pBuffer, uSize);

	LPVOID pPos = pBuffer;
	UINT uRemainSize = uSize;
	while (uRemainSize != 0)
	{
		if (m_stackSection.size() == 0)
		{
			m_State = emS_FAIL;
			return FALSE;
		}

		HRESULT hr = m_stackSection.top().ParseBuffer((LPCH)pPos, uRemainSize, (LPCH *)&pPos);
		if (FAILED(hr))
		{
			m_State = emS_FAIL;
			return FALSE;
		}
		else if (hr == S_OK)
		{
			if (CheckInfoSection(LODWORD((LPCH)pPos - (LPCH)pBuffer)) == FALSE)
			{
				m_State = emS_FAIL;
				return FALSE;
			}
			else if (m_State == emS_SUCCESS)
			{
				break;
			}
			m_stackSection.pop();
		}
		else if (pPos < (static_cast<LPCH>(pBuffer) + uSize))
		{
			CBtihHashSection secChild;
			m_stackSection.push(secChild);
		}

		if (pPos < pBuffer || pPos > (static_cast<LPCH>(pBuffer) + uSize))
		{
			m_State = emS_FAIL;
			return FALSE;
		}
		uRemainSize = uSize - LODWORD((LPCH)pPos - (LPCH)pBuffer);
	}

	if (CalcInfoSection() == FALSE)
		return FALSE;

	return TRUE;
}

BOOL CBtihHash::CalcFinal()
{
	return (m_State == emS_SUCCESS);
}

void CBtihHash::InitInfoSection( LPVOID pBuffer, UINT uSize )
{
	m_InfoSectionState.pBuffer = pBuffer;
	m_InfoSectionState.uBegin = 0;
	m_InfoSectionState.uEnd = uSize;
}

BOOL CBtihHash::CheckInfoSection( UINT uPos )
{
	if (m_InfoSectionState.bInInfoSection == FALSE)
	{
		if (m_stackSection.size() == 2 &&
			m_stackSection.top().GetType() == CBtihHashSection::emST_STRING &&
			CStringA("info") == m_stackSection.top().GetStringValueA()
			)
		{
			CHashLibFactory HLFac;
			m_pSha1Hash.Attach(HLFac.CreateHashLib(emHLT_SHA));
			if (m_pSha1Hash == NULL || m_pSha1Hash->CalcInit(NULL) == FALSE)
			{
				m_State = emS_FAIL;
				return FALSE;
			}

			m_InfoSectionState.uBegin = uPos;
			m_InfoSectionState.bInInfoSection = TRUE;
		}
	}
	else
	{
		if (m_stackSection.size() == 2)
		{
			m_InfoSectionState.uEnd = uPos;
			if (CalcInfoSection() == FALSE || m_pSha1Hash == NULL || m_pSha1Hash->CalcFinal() == FALSE)
			{
				m_State = emS_FAIL;
				return FALSE;
			}

			m_InfoSectionState.bInInfoSection = FALSE;
			m_strHash = m_pSha1Hash->GetHashStr();
			m_pSha1Hash.Free();
			m_State = emS_SUCCESS;
		}
	}

	return TRUE;
}

BOOL CBtihHash::CalcInfoSection()
{
	if (m_State != emS_CALC)
		return (m_State == emS_SUCCESS);

	if (m_InfoSectionState.bInInfoSection)
	{
		if (m_pSha1Hash == NULL)
			return FALSE;
		if (m_InfoSectionState.uEnd <= m_InfoSectionState.uBegin)
			return FALSE;

		if (m_pSha1Hash->CalcStep((LPCH)m_InfoSectionState.pBuffer + m_InfoSectionState.uBegin,
			m_InfoSectionState.uEnd - m_InfoSectionState.uBegin) == FALSE)
			return FALSE;
	}

	return TRUE;
}
