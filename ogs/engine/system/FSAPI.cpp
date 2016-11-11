// FSAPI.h - filesystem api funcs

#pragma once

extern "C"
{

#if !()
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
#else
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
#endif

};