#pragma once
#include "IHashLib.h"
extern "C" {
#include "ThirdParty/Keccak/KeccakNISTInterface.h"
}

class CKeccakHash :
	public IHashLib
{
public:
	CKeccakHash(LPCTSTR szHashMethodName, int nHashBitLen);
	virtual ~CKeccakHash(void);

	virtual LPCTSTR GetHashMethodName() {return m_strHashMethodName;};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

protected:
	CString m_strHashMethodName;
	CString m_strHash;
	hashState m_State;
	int m_nHashBitLen;
};
