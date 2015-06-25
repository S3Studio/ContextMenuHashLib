# ContextMenuHashLib

**ContextMenuHashLib** is a part of software [**ContextMenuHash**](http://s3studio2010.blogspot.com/2013/08/contextmenuhash-released.html) on Windows platform. It provides a unified way to calculate different types of hashes of the data.

## Environment Requirement

ContextMenuHashLib is able to be built under VisualStudio 2010.

Implements of some hash methods require corresponding Windows APIs or dlls.

## Supported Hash Method

* MD4

Use Windows api in advapi32.dll

* MD5

Use Windows api in advapi32.dll

* SHA Serial

Use Windows cryptographic service provider(CSP)

* BTIH

Use original code based on SHA1

* ED2K

Use original code based on MD4

* CRC32

Use code from zlib (version 1.2.5)

* Adler32

Use code from zlib (version 1.2.5)

* WHIRLPOOL

Use code from [official site](http://www.larc.usp.br/~pbarreto/WhirlpoolPage.html)

* Others

Implement of FNV hash method is also provided but not included in the project, which is original code.
