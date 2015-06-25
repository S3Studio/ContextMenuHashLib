#pragma once
#include "IHashLib.h"

class CFNVHash :
	public IHashLib
{
public:
	CFNVHash(UINT32 nBasis, UINT32 nPrime);
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
