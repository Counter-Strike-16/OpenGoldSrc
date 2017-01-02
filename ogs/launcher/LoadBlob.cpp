/*
 * This file is part of OGS Engine
 * Copyright (C) 2016-2017 OGS Dev Team
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the author gives permission to
 * link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 * Engine") and Modified Game Libraries ("MODs") developed by Valve,
 * L.L.C ("Valve").  You must obey the GNU General Public License in all
 * respects for all of the code used other than the GoldSrc Engine and MODs
 * from Valve.  If you modify this file, you may extend this exception
 * to your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from your
 * version.
 */

/// @file

#include <windows.h>
#include "LoadBlob.hpp"
#include <interface.h>
#include "IFileSystem.h"

extern IFileSystem *g_pFileSystem;

#pragma data_seg(".data")
BYTE         g_pBlobBuffer[0x2000000];
BlobHeader_t g_BlobHeader;

BlobHeader_t *GetBlobHeader(void)
{
	return &g_BlobHeader;
}

BOOL FIsBlob(const char *pstFileName)
{
	FileHandle_t file = g_pFileSystem->Open(pstFileName, "rb");

	if(file == FILESYSTEM_INVALID_HANDLE)
		return FALSE;

	BlobInfo_t info;
	g_pFileSystem->Read(&info, sizeof(BlobInfo_t), file);
	g_pFileSystem->Close(file);

	if(info.m_dwAlgorithm != BLOB_ALGORITHM)
		return FALSE;

	return TRUE;
}

DWORD NLoadBlobFile(const char *pstFileName, BlobFootprint_t *pblobfootprint, void **pv)
{
	FileHandle_t file = g_pFileSystem->Open(pstFileName, "rb");

	DWORD dwSize;
	BYTE *pBuffer;
	DWORD dwAddress;

	g_pFileSystem->Seek(file, 0, FILESYSTEM_SEEK_TAIL);
	dwSize = g_pFileSystem->Tell(file);
	g_pFileSystem->Seek(file, 0, FILESYSTEM_SEEK_HEAD);

	pBuffer = (BYTE *)malloc(dwSize);
	g_pFileSystem->Read(pBuffer, dwSize, file);

	dwAddress = LoadBlobFile(pBuffer, pblobfootprint, pv, dwSize);
	free(pBuffer);
	g_pFileSystem->Close(file);
	return dwAddress;
}

DWORD LoadBlobFile(BYTE *pBuffer, BlobFootprint_t *pblobfootprint, void **pv, DWORD dwSize)
{
	BYTE           bXor = 0x57;
	BlobHeader_t * pHeader;
	BlobSection_t *pSection;
	DWORD          dwAddress = 0;

	for(size_t i = sizeof(BlobInfo_t); i < dwSize; i++)
	{
		pBuffer[i] ^= bXor;
		bXor += pBuffer[i] + 0x57;
	}

	pHeader = (BlobHeader_t *)(pBuffer + sizeof(BlobInfo_t));
	pHeader->m_dwExportPoint ^= 0x7A32BC85;
	pHeader->m_dwImageBase ^= 0x49C042D1;
	pHeader->m_dwEntryPoint -= 12;
	pHeader->m_dwImportTable ^= 0x872C3D47;
	pSection = (BlobSection_t *)(pBuffer + sizeof(BlobInfo_t) + sizeof(BlobHeader_t));

	memcpy(&g_BlobHeader, pHeader, sizeof(BlobHeader_t));

	for(WORD j = 0; j <= pHeader->m_wSectionCount; j++)
	{
		if(pSection[j].m_bIsSpecial)
			dwAddress = pSection[j].m_dwDataAddress;

		if(pSection[j].m_dwVirtualSize > pSection[j].m_dwDataSize)
			memset((BYTE *)(pSection[j].m_dwVirtualAddress + pSection[j].m_dwDataSize), NULL, pSection[j].m_dwVirtualSize - pSection[j].m_dwDataSize);

		memcpy((BYTE *)pSection[j].m_dwVirtualAddress, pBuffer + pSection[j].m_dwDataAddress, pSection[j].m_dwDataSize);
	}

	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)pHeader->m_dwImportTable;

	while(pImport->Name)
	{
		HMODULE           hPorcDll = LoadLibrary((char *)(pHeader->m_dwImageBase + pImport->Name));
		PIMAGE_THUNK_DATA pThunk   = (PIMAGE_THUNK_DATA)(pHeader->m_dwImageBase + pImport++->FirstThunk);

		while(pThunk->u1.Function)
		{
			const char *pszProcName    = IMAGE_SNAP_BY_ORDINAL(pThunk->u1.Ordinal) ? (char *)((LONG)pThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32 - 1) : (char *)(pHeader->m_dwImageBase + ((IMAGE_IMPORT_BY_NAME *)((LONG)pThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32 - 1))->Name);
			pThunk++->u1.AddressOfData = (DWORD)GetProcAddress(hPorcDll, pszProcName);
		}
	}

	((BOOL(WINAPI *)(HINSTANCE, DWORD, void *))(pHeader->m_dwEntryPoint))(0, DLL_PROCESS_ATTACH, 0);
	((void (*)(void **))(pHeader->m_dwExportPoint))(pv);
	return dwAddress;
}

void FreeBlob(BlobFootprint_t *pblobfootprint)
{
	FreeLibrary(pblobfootprint->m_hDll);
}