#pragma once
#include "IHashLib.h"

struct MD5_CTX
{
	ULONG num[2];   /* number of _bits_ handled mod 2^64 */
	ULONG buf[4];   /* scratch buffer */
	BYTE  input[64];  /* input buffer */
	BYTE  digest[16];  /* actual digest after MD5Final call */
};
// // The MD5Init function initializes an MD5 message digest context.
typedef void (WINAPI *MD5INIT)(MD5_CTX*);
// // The MD5Update function updates the MD5 context by using the supplied buffer for the message whose MD5 digest is being generated
typedef void (WINAPI *MD5UPDATE)(MD5_CTX*, unsigned char* input, unsigned int inlen);
// // The MD5Final function ends an MD5 message digest previously started by a call to the MD5Init function
typedef void (WINAPI *MD5FINAL)(MD5_CTX*);

class COSMd5Hash
	: public IHashLib
{
public:
	COSMd5Hash(void);
	virtual ~COSMd5Hash(void);

	virtual LPCTSTR GetHashMethodName() {return _T("MD5");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

protected:
	MD5INIT m_fnMD5Init;
	MD5UPDATE m_fnMD5Update;
	MD5FINAL m_fnMD5Final;
	HMODULE m_hLibrary;
	MD5_CTX m_CTX;
	CString m_strHash;

	BOOL PrepareLib();
};
