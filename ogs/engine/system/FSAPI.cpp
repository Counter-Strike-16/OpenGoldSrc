// FSAPI.cpp - filesystem api funcs

#pragma once

extern "C"
{

#if !(OGS_NULL_FS_API_IMPL || OGS_NULL_EXPORT_API_IMPL)
byte *FS_LoadFileForMe(char *filename, int *pLength)
{
};

void FS_FreeFile(void *buffer)
{
};

int FS_CompareFileTime(char *filename1, char *filename2, int *iCompare)
{
};

int FS_GetFileSize(char *filename)
{
};

void FS_EngineFprintf(void *pfile, char *szFmt, ...)
{
};
#else // OGS_NULL_FS_API_IMPL || OGS_NULL_EXPORT_API_IMPL
byte *COM_LoadFile(char *path, int usehunk, int *pLength)
{
};

char *COM_ParseFile(char *data, char *token)
{
	return NULL;
};

byte *FS_LoadFileForMe(char *filename, int *pLength)
{
};

void FS_FreeFile(void *buffer) // COM_FreeFile
{
};

int FS_CompareFileTime(char *filename1, char *filename2, int *iCompare)
{
	return 0;
};

int FS_GetFileSize(char *filename)
{
	return 0;
};

void FS_EngineFprintf(void *pfile, char *szFmt, ...)
{
};

void COM_AddAppDirectoryToSearchPath(const char *pszBaseDir, const char *appName)
{
};

int COM_ExpandFilename(const char *fileName, char *nameOutBuffer, int nameOutBufferSize)
{
	return 0;
};
#endif // OGS_NULL_FS_API_IMPL || OGS_NULL_EXPORT_API_IMPL

}; // extern "C"