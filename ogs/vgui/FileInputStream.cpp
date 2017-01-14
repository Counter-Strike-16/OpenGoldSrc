//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_FileInputStream.h"

using namespace vgui;

FileInputStream::FileInputStream(const char* fileName,bool textMode)
{
	_fp=fopen(fileName,textMode?"rt":"rb");
}

void FileInputStream::seekStart(bool& success)
{
	if(_fp)
	{
		success=fseek(_fp,0,SEEK_SET)!=0; // ?
		return;
	}
	success=false;
}

void FileInputStream::seekRelative(int count,bool& success)
{
	if(_fp)
	{
		success=fseek(_fp,SEEK_CUR,count)!=0; // ?
		return;
	}
	success=false;
}

void FileInputStream::seekEnd(bool& success)
{
	if(_fp)
	{
		success=fseek(_fp,SEEK_END,0)!=0; // ?
		return;
	}
	success=false;
}

int FileInputStream::getAvailable(bool& success)
{
	if(_fp)
	{
		success=false; // ?
		return 0;
	}
	success=false;
	return 0;
}

uchar FileInputStream::readUChar(bool& success)
{
	if(_fp)
	{
		uchar ret;
		success=(fread(&ret,sizeof(ret),1,_fp)==1);
		return ret;
	}
	success=false;
	return 0;
}

void FileInputStream::readUChar(uchar* buf,int count,bool& success)
{
	if(_fp)
	{
		success=(fread(buf,count,1,_fp)==1);
		return;
	}
	success=false;
}

void FileInputStream::close(bool& success)
{
	if(_fp)
	{
		success=(fclose(_fp)==0);
		return;
	}
	success=false;
}

void FileInputStream::close()
{
	bool success;
	close(success);
}
