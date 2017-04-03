#pragma once

#include "public/interface.h"
#include "vstdlib/ICommandLine.h"

class CCommandLine : public ICommandLine
{
public:
	CCommandLine();
	~CCommandLine();
	
	void CreateCmdLine(const char *commandline);
	const char *GetCmdLine() const;
	const char *CheckParm(const char *psz, const char **ppszValue = 0) const;
	void RemoveParm(const char *parm);
	void AppendParm(const char *pszParm, const char *pszValues);
	void SetParm(const char *pszParm, const char *pszValues);
	void SetParm(const char *pszParm, int iValue);
private:
	enum
	{
		MAX_PARAMETER_LEN = 128,
		MAX_PARAMETERS = 256,
	};
	
	void LoadParametersFromFile(char *&pSrc, char *&pDst);
	
	char *m_pszCmdLine;
};