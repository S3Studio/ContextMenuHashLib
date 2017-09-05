#pragma once
#include "IHashLib.h"

#define DEFAULT_BASIS_32BIT 2166136261
#define DEFAULT_PRIME_32BIT 16777619

class CFNVHash :
	public IHashLib
{
public:
	CFNVHash(UINT32 nBasis = DEFAULT_BASIS_32BIT, UINT32 nPrime = DEFAULT_PRIME_32BIT);
	CFNVHash(CFNVHash& CopyFrom);
	virtual ~CFNVHash(void);

	struct InitParam
	{
		BOOL bAlg1a;
	};

	virtual LPCTSTR GetHashMethodName() {return _T("FNV(32bit)");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

	// for CSsdeepHash
	UINT32 GetHashDigest() {return m_nHash;};

protected:
	CString m_strHash;
	InitParam m_InitParam;
	UINT32 m_nHash, m_nBasis, m_nPrime;
};
