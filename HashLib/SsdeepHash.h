#pragma once
#include "IHashLib.h"
#include "FNVHash.h"
#include <atlcoll.h>
#include <vector>

#define ROLLING_WINDOW 7

class CSsdeepRollingHash :
	public IHashLib
{
public:
	CSsdeepRollingHash(void);
	virtual ~CSsdeepRollingHash(void);

	virtual LPCTSTR GetHashMethodName() {return _T("");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return NULL;}; // Useless in CSsdeepHash

	// for CSsdeepHash
	UINT32 GetHashDigest() {return m_x + m_y + m_z;};

protected:
	UINT32 m_x, m_y, m_z;
	SIZE_T m_c;
	std::vector<UINT32> m_vecWindow;
};

#define SPAMSUM_LENGTH 64
#define MIN_BLOCKSIZE 3
#define TRAD_HASH_PRIME 0x01000193
#define TRAD_HASH_BASIS 0x28021967
#define TRAD_HASH_COUNT 32
#define NUM_BLOCKHASHES (TRAD_HASH_COUNT - 1)

class CSsdeepHash :
	public IHashLib
{
public:
	CSsdeepHash(void);
	virtual ~CSsdeepHash(void);

	virtual LPCTSTR GetHashMethodName() {return _T("ssdeep");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

protected:
	CString m_strHash;
	CSsdeepRollingHash m_RHash;
	//Max size of hash string is SPAMSUM_LENGTH, and when oversize the last char will be update.
	//Max size of half hash string is SPAMSUM_LENGTH / 2, and when oversize the HashHalfTailChar
	//    will be update, and when needed as half hash, the HashHalfTailChar will be added as a
	//    tail to the first SPAMSUM_LENGTH / 2 - 1 size of hash string.
	CAutoPtrArray<CFNVHash> m_aryTradHash;
	CAutoPtrArray<CFNVHash> m_aryTradHashHalfTail;
	std::vector<CString> m_aryHashStr;
	std::vector<TCHAR> m_aryHashHalfTailChar;
	CFNVHash::InitParam m_FNVParam;
	// HashBlock which index below m_nMinBlock need not to calculate later in CalcStep
	UINT m_nMinBlock;
	ULONG64 m_ul64Size;

	static const TCHAR s_chB64[];

	void SetHashVal(UINT nIndex);
};
