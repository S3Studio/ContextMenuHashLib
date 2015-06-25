#pragma once
#include "IHashLib.h"

class CHashLibFactory :
	public IHashLibFactory
{
public:
	CHashLibFactory(void);
	virtual ~CHashLibFactory(void);

	virtual IHashLib* CreateHashLib(em_HASH_LIB_TYPE emType);
};
