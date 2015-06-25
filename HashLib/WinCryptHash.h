#pragma once
#include "IHashLib.h"
#include <wincrypt.h>

class CWinCryptHash :
	public IHashLib
{
public:
	CWinCryptHash(LPCTSTR szHashMethodName, int ProvType, int HashType);
	virtual ~CWinCryptHash(void);

	virtual LPCTSTR GetHashMethodName() {return m_strHashMethodName;};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

protected:
	CString m_strHashMethodName;
	int m_nProvType, m_nHashType;
	HCRYPTPROV m_hProv;
	HCRYPTHASH m_hHash;
	CString m_strHash;

	BOOL CreateHProv();
	BOOL CreateHHash();
};
