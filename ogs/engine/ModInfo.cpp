#include "ModInfo.h"

CModInfo::CModInfo()
{
};

CModInfo::~CModInfo()
{
};

// will load the mod info (game info) from gamefolder/asFileName (it should be liblist.gam but it would be hardcode)
void CModInfo::LoadFromFile(const char *asFileName)
{
};

void CModInfo::SetModKey(/*modinfo_t *pinfo,*/ char *pkey, char *pvalue)
{
	if (!Q_stricmp(pkey, "url_info"))
	{
		pinfo->bIsMod = 1;
		Q_strncpy(pinfo->szInfo, pvalue, sizeof(pinfo->szInfo) - 1);
		pinfo->szInfo[sizeof(pinfo->szInfo) - 1] = 0;
	}
	else if (!Q_stricmp(pkey, "url_dl"))
	{
		pinfo->bIsMod = 1;
		Q_strncpy(pinfo->szDL, pvalue, sizeof(pinfo->szDL) - 1);
		pinfo->szDL[sizeof(pinfo->szDL) - 1] = 0;
	}
	else if (!Q_stricmp(pkey, "version"))
	{
		pinfo->bIsMod = 1;
		pinfo->version = Q_atoi(pvalue);
	}
	else if (!Q_stricmp(pkey, "size"))
	{
		pinfo->bIsMod = 1;
		pinfo->size = Q_atoi(pvalue);
	}
	else if (!Q_stricmp(pkey, "svonly"))
	{
		pinfo->bIsMod = 1;
		pinfo->svonly = Q_atoi(pvalue) != 0;
	}
	else if (!Q_stricmp(pkey, "cldll"))
	{
		pinfo->bIsMod = 1;
		pinfo->cldll = Q_atoi(pvalue) != 0;
	}
	else if (!Q_stricmp(pkey, "secure"))
	{
		pinfo->bIsMod = 1;
		pinfo->secure = Q_atoi(pvalue) != 0;
	}
	else if (!Q_stricmp(pkey, "hlversion"))
	{
		Q_strncpy(pinfo->szHLVersion, pvalue, sizeof(pinfo->szHLVersion) - 1);
		pinfo->szHLVersion[sizeof(pinfo->szHLVersion) - 1] = 0;
	}
	else if (!Q_stricmp(pkey, "edicts"))
	{
		pinfo->num_edicts = Q_atoi(pvalue);
		if (pinfo->num_edicts < NUM_EDICTS)
			pinfo->num_edicts = NUM_EDICTS;
	}
	else if (!Q_stricmp(pkey, "crcclientdll"))
	{
		pinfo->bIsMod = 1;
		pinfo->clientDllCRC = Q_atoi(pvalue) != 0;
	}
	else if (!Q_stricmp(pkey, "type"))
	{
		if (!Q_stricmp(pvalue, "singleplayer_only"))
			pinfo->type = SINGLEPLAYER_ONLY;
		else if (!Q_stricmp(pvalue, "multiplayer_only"))
			pinfo->type = MULTIPLAYER_ONLY;
		else
			pinfo->type = BOTH;
	}
	else if (!Q_stricmp(pkey, "fallback_dir"))
	{
		COM_AddDefaultDir(pvalue);
	}
}