#pragma once
#include "IHashLib.h"
#include "OSMd4Hash.h"
#define MD4_SIZE 32

class CEd2kHash :
	public IHashLib
{
public:
	CEd2kHash(void);
	virtual ~CEd2kHash(void);

	struct InitParam
	{
		BOOL bAddtionalChunkWhenAligned;
	};

	virtual LPCTSTR GetHashMethodName() {return _T("Ed2k-Hash");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

protected:
	CString m_strHash;
	InitParam m_InitParam;
	UCHAR m_cHashFirst[MD4_SIZE / 2];
	CAutoPtr<COSMd4Hash> m_pChunkHasher, m_pOverallHasher;
	ULONG m_uChunkRemainSize;
	BOOL m_bFailed;

	BOOL OnChunkFinish();
};
