#pragma once

enum em_HASH_LIB_TYPE
{
	emHLT_BEGIN = 0,
	emHLT_MD4 = emHLT_BEGIN,
	emHLT_MD5,
	emHLT_SHA,
	emHLT_SHA256,
	emHLT_SHA512,
	emHLT_BTIH,
	emHLT_ED2K,
	emHLT_CRC32,
	emHLT_ADLER32,
	emHLT_SHA384,
	emHLT_WHIRLPOOL,
	emHLT_SSDEEP,
	emHLT_KECCAK224,
	emHLT_KECCAK256,
	emHLT_KECCAK384,
	emHLT_KECCAK512,
	emHLT_MAX
};

class IHashLib
{
public:
	virtual LPCTSTR GetHashMethodName() = 0;

	virtual BOOL CalcInit(LPVOID lpParam) = 0;
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize) = 0;
	virtual BOOL CalcFinal() = 0;
	virtual LPCTSTR GetHashStr() = 0;
};

class IHashLibFactory
{
public:
	virtual IHashLib* CreateHashLib(em_HASH_LIB_TYPE emType) = 0;
};