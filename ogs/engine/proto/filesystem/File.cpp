/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#include "precompiled.hpp"
#include "filesystem/File.hpp"
#include "filesystem/FileSystem.hpp"
#include "system/common.hpp"

CFile::CFile(const char *asName, IFileSystem *apFileSystem) : CFile(apFileSystem), msName(asName)
{
	assert(asName);
	
	//if(!Open(asName))
		//boom!
	
	//assert(mpFileHandle);
};

bool CFile::Open(const char *asName, const char *asOptions)
{
	Close();
	
	mpFileHandle = mpFileSystem->Open(asName, asOptions);
	
	if(!mpFileHandle)
		return false;
	
	return true;
};

int CFile::Printf(/*const*/ char *asData, ...)
{
	//
	assert(mpFileHandle);
	
	va_list arglist;
	char sText[256] = {'\0'};
	
	va_start(arglist, asData);
	Q_vsnprintf(sText, sizeof(sText), asData, arglist);
	va_end(arglist);
	
	//
	// original
	
	char data[8192];
	va_list va;

	va_start(va, pFormat);
	vsprintf(data, pFormat, va);
	va_end(va);
	
	//
	
	return mpFileSystem->FPrintf(mpFileHandle, "%s", sText);
};

void CFile::Seek(int anPos, FileSystemSeek_t seekType)
{
	mpFileSystem->Seek(mpFileHandle, anPos, seekType);
};

uint CFile::Tell() const
{
	return mpFileSystem->Tell(mpFileHandle);
};

void CFile::Flush()
{
	mpFileSystem->Flush(mpFileHandle);
};

int CFile::IsOk() const
{
	return mpFileSystem->IsOk(mpFileHandle);
};

int CFile::IsEOF() const
{
	return mpFileSystem->EndOfFile(mpFileHandle);
};

int CFile::Read(void *apOutput, int anSize, int anCount)
{
#ifdef REHLDS_FIXES
	return mpFileSystem->Read(apOutput, anSize * anCount, mpFileHandle);
#else
	return mpFileSystem->Read(apOutput, anSize, mpFileHandle);
#endif // REHLDS_FIXES
};

int CFile::Write(const void *apInput, int anSize, int anCount)
{
#ifdef REHLDS_FIXES
	return mpFileSystem->Write(apInput, anSize * anCount, mpFileHandle);
#else
	return mpFileSystem->Write(apInput, anSize, mpFileHandle); // where's count?
#endif // REHLDS_FIXES
};

char *CFile::ReadLine(char *asOutput, int anMaxChars)
{
	return mpFileSystem->ReadLine(asOutput, anMaxChars, mpFileHandle);
};

NOXREF byte CFile::GetCharacter() const
{
	NOXREFCHECK;

	uint8 retval;
	mpFileSystem->Read(&retval, 1, mpFileHandle);
	return retval;
};

uint CFile::GetSize() const // uint -> int ?
{
	if(!mpFileHandle)
		return 0;
	
	return mpFileSystem->Size(mpFileHandle);
};

void CFile::Close()
{
	if(mpFileHandle)
		mpFileSystem->Close(mpFileHandle);
};