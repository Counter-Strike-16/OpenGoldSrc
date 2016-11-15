#include "GameInfo.h"

CGameInfo::CGameInfo()
{
};

CGameInfo::~CGameInfo()
{
};

// will load the mod info (game info) from gamefolder/asFileName (it should be liblist.gam but it would be hardcode)
void CGameInfo::LoadFromFile(const char *asFileName)
{
	Q_strncpy(szGameDir, com_gamedir, sizeof(szGameDir) - 1);
	if(Q_stricmp(szGameDir, "valve"))
		gmodinfo.bIsMod = 1;
	
	Q_snprintf(szDllListFile, sizeof(szDllListFile), "%s", "liblist.gam");
	hLibListFile = FS_Open(szDllListFile, "rb");
	
	if(hLibListFile)
	{
		nFileSize = FS_Size(hLibListFile);
		nFileSize2 = nFileSize;
		
		if(!nFileSize || (signed int)nFileSize > 262144)
			Sys_Error("Game listing file size is bogus [%s: size %i]", "liblist.gam", nFileSize);

		pszInputStream = (char *)Mem_Malloc(nFileSize + 1);
		
		if(!pszInputStream)
			Sys_Error("Could not allocate space for game listing file of %i bytes", nFileSize2 + 1);

		nBytesRead = FS_Read(pszInputStream, nFileSize2, 1, hLibListFile);
		
		if(nBytesRead != nFileSize2)
			Sys_Error("Error reading in game listing file, expected %i bytes, read %i", nFileSize2, nBytesRead);

		pszInputStream[nFileSize2] = 0;
		pStreamPos = pszInputStream;
		com_ignorecolons = 1;
		
		while(1)
		{
			pStreamPos = COM_Parse(pStreamPos);
			
			if(Q_strlen(com_token) <= 0)
				break;

			Q_strncpy(szKey, com_token, sizeof(szKey) - 1);
			szKey[sizeof(szKey) - 1] = 0;
			pStreamPos = COM_Parse(pStreamPos);
			Q_strncpy(szValue, com_token, sizeof(szValue) - 1);
			szValue[sizeof(szValue) - 1] = 0;
			
#ifdef _WIN32
			if(Q_stricmp(szKey, "gamedll"))
#else // _WIN32
			if(Q_stricmp(szKey, "gamedll_linux"))
#endif // _WIN32
			{
				DLL_SetModKey(&gmodinfo, szKey, szValue);
			}
			else
			{
				int index = COM_CheckParm("-dll");
				if(index && index < com_argc - 1)
				{
					Q_strncpy(szValue, com_argv[index + 1], sizeof(szValue) - 1);
					szValue[sizeof(szValue) - 1] = 0;
				};
				
#ifdef _WIN32
				if(Q_strstr(szValue, ".dll"))
#else // _WIN32
				if(Q_strstr(szValue, ".so"))
#endif // _WIN32
				{
					FS_GetLocalPath(szValue, szDllFilename, sizeof(szDllFilename));
					Con_DPrintf("\nAdding:  %s/%s\n", szGameDir, szValue);
					LoadThisDll(szDllFilename);
				}
				else
					Con_DPrintf("Skipping non-dll:  %s\n", szValue);
			};
		};
		
		com_ignorecolons = 0;
		Mem_Free(pszInputStream);
		FS_Close(hLibListFile);
	}
};

void CGameInfo::SetModKey(/*modinfo_t *pinfo,*/ char *pkey, char *pvalue)
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
		COM_AddDefaultDir(pvalue);
};