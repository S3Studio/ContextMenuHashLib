#pragma once
#include "IHashLib.h"

class CZlibAdler32Hash :
	public IHashLib
{
public:
	CZlibAdler32Hash(void);
	virtual ~CZlibAdler32Hash(void);

	virtual LPCTSTR GetHashMethodName() {return _T("Adler");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

protected:
	CString m_strHash;
	ULONG m_ulAlder;
};
