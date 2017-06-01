char	findbase[MAX_OSPATH];
char	findpath[MAX_OSPATH];
int		findhandle;

static qboolean CompareAttributes( unsigned found, unsigned musthave, unsigned canthave )
{
	if ( ( found & _A_RDONLY ) && ( canthave & SFF_RDONLY ) )
		return false;
	if ( ( found & _A_HIDDEN ) && ( canthave & SFF_HIDDEN ) )
		return false;
	if ( ( found & _A_SYSTEM ) && ( canthave & SFF_SYSTEM ) )
		return false;
	if ( ( found & _A_SUBDIR ) && ( canthave & SFF_SUBDIR ) )
		return false;
	if ( ( found & _A_ARCH ) && ( canthave & SFF_ARCH ) )
		return false;

	if ( ( musthave & SFF_RDONLY ) && !( found & _A_RDONLY ) )
		return false;
	if ( ( musthave & SFF_HIDDEN ) && !( found & _A_HIDDEN ) )
		return false;
	if ( ( musthave & SFF_SYSTEM ) && !( found & _A_SYSTEM ) )
		return false;
	if ( ( musthave & SFF_SUBDIR ) && !( found & _A_SUBDIR ) )
		return false;
	if ( ( musthave & SFF_ARCH ) && !( found & _A_ARCH ) )
		return false;

	return true;
}

char *Sys_FindFirst (char *path, unsigned musthave, unsigned canthave )
{
	struct _finddata_t findinfo;

	if (findhandle)
		Sys_Error ("Sys_BeginFind without close");
	
	findhandle = 0;

	COM_FilePath (path, findbase);
	findhandle = _findfirst (path, &findinfo);
	
	if (findhandle == -1)
		return NULL;
	
	if ( !CompareAttributes( findinfo.attrib, musthave, canthave ) )
		return NULL;
	
	Q_sprintf (findpath, sizeof(findpath), "%s/%s", findbase, findinfo.name);
	return findpath;
};

char *Sys_FindNext (FileFindHandle_t handle, unsigned musthave, unsigned canthave )
{
	struct _finddata_t findinfo;

	if (findhandle == -1)
		return NULL;
	
	if (_findnext (handle, &findinfo) == -1)
		return NULL;
	
	if ( !CompareAttributes( findinfo.attrib, musthave, canthave ) )
		return NULL;

	Q_sprintf (findpath, sizeof(findpath), "%s/%s", findbase, findinfo.name);
	return findpath;
};

void Sys_FindClose(FileFindHandle_t handle)
{
	if (handle != -1)
		_findclose(handle);
	handle = 0;
};