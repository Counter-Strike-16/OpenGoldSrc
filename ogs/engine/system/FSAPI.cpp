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
#endif // OGS_NULL_FS_API_IMPL || OGS_NULL_EXPORT_API_IMPL

}; // extern "C"