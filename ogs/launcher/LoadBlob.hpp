/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
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

#define BLOB_ALGORITHM 0x12345678

typedef struct BlobInfo_s
{
	char m_szPath[10];
	char m_szDescribe[32];
	char m_szCompany[22];
	DWORD m_dwAlgorithm;
} BlobInfo_t;

typedef struct BlobHeader_s
{
	DWORD m_dwCheckSum;
	WORD m_wSectionCount;
	DWORD m_dwExportPoint;
	DWORD m_dwImageBase;
	DWORD m_dwEntryPoint;
	DWORD m_dwImportTable;
} BlobHeader_t;

typedef struct BlobSection_s
{
	DWORD m_dwVirtualAddress;
	DWORD m_dwVirtualSize;
	DWORD m_dwDataSize;
	DWORD m_dwDataAddress;
	BOOL m_bIsSpecial;
} BlobSection_t;

typedef struct BlobFootprint_s
{
	HMODULE m_hDll;
} BlobFootprint_t;

BlobHeader_t *GetBlobHeader();
BOOL FIsBlob(const char *pstFileName);
DWORD NLoadBlobFile(const char *pstFileName, BlobFootprint_t *pblobfootprint, void **pv);
DWORD LoadBlobFile(BYTE *pBuffer, BlobFootprint_t *pblobfootprint, void **pv, DWORD dwSize);
void FreeBlob(BlobFootprint_t *pblobfootprint);