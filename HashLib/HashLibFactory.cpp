#include "StdAfx.h"
#include "HashLibFactory.h"
#include "OSMd5Hash.h"
#include "OSMd4Hash.h"
#include "WinCryptHash.h"
#include "BtihHash.h"
#include "Ed2kHash.h"
#include "ZLibCrc32Hash.h"
#include "ZlibAdler32Hash.h"
#include "WhirlpoolHash.h"

CHashLibFactory::CHashLibFactory(void)
{
}

CHashLibFactory::~CHashLibFactory(void)
{
}

IHashLib* CHashLibFactory::CreateHashLib( em_HASH_LIB_TYPE emType )
{
	IHashLib* pRet = NULL;
	switch (emType)
	{
	case emHLT_MD5:
		pRet = new(std::nothrow) COSMd5Hash;
		break;
	case emHLT_SHA:
		pRet = new(std::nothrow) CWinCryptHash(_T("SHA"), PROV_RSA_FULL, CALG_SHA);
		break;
	case emHLT_SHA256:
		pRet = new(std::nothrow) CWinCryptHash(_T("SHA256"), PROV_RSA_AES, CALG_SHA_256);
		break;
	case emHLT_SHA384: // insert
		pRet = new(std::nothrow) CWinCryptHash(_T("SHA384"), PROV_RSA_AES, CALG_SHA_384);
		break;
	case emHLT_SHA512:
		pRet = new(std::nothrow) CWinCryptHash(_T("SHA512"), PROV_RSA_AES, CALG_SHA_512);
		break;
	case emHLT_MD4:
		pRet = new(std::nothrow) COSMd4Hash;
		break;
	case emHLT_BTIH:
		pRet = new(std::nothrow) CBtihHash;
		break;
	case emHLT_ED2K:
		pRet = new(std::nothrow) CEd2kHash;
		break;
	case emHLT_CRC32:
		pRet = new(std::nothrow) CZlibCrc32Hash;
		break;
	case emHLT_ADLER32:
		pRet = new(std::nothrow) CZlibAdler32Hash;
		break;
	case emHLT_WHIRLPOOL:
		pRet = new(std::nothrow) CWhirlpoolHash;
		break;
	}

	return pRet;
}
