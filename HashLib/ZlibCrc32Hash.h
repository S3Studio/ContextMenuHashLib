#pragma once
#include "IHashLib.h"

class CZlibCrc32Hash :
	public IHashLib
{
public:
	CZlibCrc32Hash(void);
	virtual ~CZlibCrc32Hash(void);

	virtual LPCTSTR GetHashMethodName() {return _T("CRC32");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

protected:
	CString m_strHash;
	ULONG m_ulCRC;
};
