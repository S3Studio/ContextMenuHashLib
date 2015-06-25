#pragma once
#include "IHashLib.h"
#include "ThirdParty/Whirlpool-nessie.h"

class CWhirlpoolHash :
	public IHashLib
{
public:
	CWhirlpoolHash(void);
	virtual ~CWhirlpoolHash(void);

	virtual LPCTSTR GetHashMethodName() {return _T("WHIRLPOOL");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

protected:
	CString m_strHash;
	NESSIEstruct m_w;
};
