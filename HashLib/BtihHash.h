#pragma once
#include "IHashLib.h"
#include <stack>

class CBtihHashSection
{
public:
	CBtihHashSection(void);
	virtual ~CBtihHashSection(void);

	enum em_SECTION_TYPE
	{
		emST_UNKNOWN = 0,
		emST_STRING,
		emST_INTEGER,
		emST_LIST,
		emST_DICT
	};

	em_SECTION_TYPE GetType() {return m_data.Type;};
	LPCSTR GetStringValueA() {return m_strStringValue;};

	HRESULT ParseBuffer(LPCH pBuffer, UINT uSize, LPCH* ppNextPos); // S_OK for section finished, S_FALSE for section unfinished, E_FAIL for error.

protected:
	struct SectionData
	{
		em_SECTION_TYPE Type;
		BOOL bFinished;
		union
		{
			struct
			{
				bool bStrLengthValid;
				UINT uStrRemainSize;
			} SData;
		} TypeData;
	};
	CStringA m_strStringValue;
	SectionData m_data;

	HRESULT ParseType(LPCH pBuffer, UINT uSize, LPCH* ppNextPos);
	HRESULT ParseString(LPCH pBuffer, UINT uSize, LPCH* ppNextPos);
	HRESULT ParseInteger(LPCH pBuffer, UINT uSize, LPCH* ppNextPos);
	HRESULT ParseList(LPCH pBuffer, UINT uSize, LPCH* ppNextPos);
	HRESULT ParseDict(LPCH pBuffer, UINT uSize, LPCH* ppNextPos);
};

class CBtihHash :
	public IHashLib
{
public:
	CBtihHash(void);
	virtual ~CBtihHash(void);

	virtual LPCTSTR GetHashMethodName() {return _T("BTIH");};

	virtual BOOL CalcInit(LPVOID lpParam);
	virtual BOOL CalcStep(LPVOID pBuffer, UINT uSize);
	virtual BOOL CalcFinal();
	virtual LPCTSTR GetHashStr() {return m_strHash;};

protected:
	enum em_State
	{
		emS_READY,
		emS_CALC,
		emS_SUCCESS,
		emS_FAIL,
	};
	struct InfoSectionState
	{
		BOOL bInInfoSection;
		LPVOID pBuffer;
		UINT uBegin;
		UINT uEnd;
	};

	CString m_strHash;
	em_State m_State;
	std::stack<CBtihHashSection> m_stackSection;
	InfoSectionState m_InfoSectionState;
	CAutoPtr<IHashLib> m_pSha1Hash;

	void InitInfoSection(LPVOID pBuffer, UINT uSize);
	BOOL CheckInfoSection(UINT uPos);
	BOOL CalcInfoSection();
};
