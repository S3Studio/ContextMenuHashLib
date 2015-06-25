#pragma once
#include "IHashLib.h"

struct MD4_CTX
{
	unsigned int buf[4];
	unsigned int i[2];
	unsigned char in[64];
	unsigned char digest[16];
};

typedef void (WINAPI *MD4INIT)(MD4_CTX*);
typedef void (WINAPI *MD4UPDATE)(MD4_CTX*, unsigned char* input, unsigned int inlen);
typedef void (WINAPI *MD4FINAL)(MD4_CTX*);

class COSMd4Hash :
	public IHashLib
{
public:
	COSMd4Hash(void);
	virtual ~COSMd4Hash(void);

	virtual LPCTSTR GetHashMethodName() {return _T("MD4");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

	// for CEd2kHash
	PUCHAR GetHashDigest() {return m_CTX.digest;};

protected:
	MD4INIT m_fnMD4Init;
	MD4UPDATE m_fnMD4Update;
	MD4FINAL m_fnMD4Final;
	HMODULE m_hLibrary;
	MD4_CTX m_CTX;
	CString m_strHash;

	BOOL PrepareLib();

};
