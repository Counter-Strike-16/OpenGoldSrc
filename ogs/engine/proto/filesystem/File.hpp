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
/// @brief generic file handle wrapper (RAII styled)

#pragma once

#include "public/FileSystem.h"

class CFile //: public opengoldsrc::IFile
{
public:
	CFile(IFileSystem *apFileSystem) : mpFileSystem(apFileSystem){}
	CFile(const char *asName, IFileSystem *apFileSystem);
	~CFile(){Close();}
	
	bool Open(const char *asName, const char *asOptions);
	
	int Printf(/*VERIFY_FORMAT_STRING*/ /*const*/ char *asData, ...);
	
	void Seek(int anPos, FileSystemSeek_t seekType);
	//int Seek(long offset, fsOrigin_t origin);
	
	uint Tell() const; // int?
	
	void Flush();
	
	//void ForceFlush();
	
	int IsOk() const;
	int IsEOF() const;
	
	int Read(void *apOutput, int anSize, int anCount); // const?
	
	int Write(const void *apInput, int anSize, int anCount); // const?
	
	char *ReadLine(char *asOutput, int anMaxChars); // const?
	
	NOXREF byte GetCharacter() const;
	
	const char *GetName() const {return msName;}
	
	//const char *GetFullPath() const;
	
	uint GetSize() const; // int GetLength
	
	operator bool(){return mpFileHandle != nullptr;}
private:
	void Close(); // make public?
	
	IFileSystem *mpFileSystem{nullptr};
	
	FileHandle_t mpFileHandle{nullptr};
	
	const char *msName{""};
};