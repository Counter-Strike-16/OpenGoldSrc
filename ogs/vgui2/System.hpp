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

#pragma once

#include "vgui2/ISystem.h"

namespace vgui
{

class CSystem : public ISystem
{
public:
	void Shutdown();
	
	void RunFrame();
	
	void ShellExecute(const char *command, const char *file);
	
	double GetFrameTime();
	double GetCurrentTime();
	
	long GetTimeMillis();
	
	int GetClipboardTextCount();
	
	void SetClipboardText(const char *text, int textLen);
	void SetClipboardText(const wchar_t *text, int textLen);
	int GetClipboardText(int offset, char *buf, int bufLen);
	int GetClipboardText(int offset, wchar_t *buf, int bufLen);
	
	bool SetRegistryString(const char *key, const char *value);
	bool GetRegistryString(const char *key, char *value, int valueLen);
	bool SetRegistryInteger(const char *key, int value);
	bool GetRegistryInteger(const char *key, int &value);
	
	KeyValues *GetUserConfigFileData(const char *dialogName, int dialogID);
	void SetUserConfigFile(const char *fileName, const char *pathName);
	void SaveUserConfigFile();
	
	bool SetWatchForComputerUse(bool state);
	
	double GetTimeSinceLastUse();
	
	int GetAvailableDrives(char *buf, int bufLen);
	
	bool CommandLineParamExists(const char *paramName);
	const char *GetFullCommandLine();
	
	bool GetCurrentTimeAndDate(int *year, int *month, int *dayOfWeek, int *day, int *hour, int *minute, int *second);
	
	double GetFreeDiskSpace(const char *path);
	
	bool CreateShortcut(const char *linkFileName, const char *targetPath, const char *arguments, const char *workingDirectory, const char *iconFile);
	
	bool GetShortcutTarget(const char *linkFileName, char *targetPath, char *arguments, int destBufferSizes);
	bool ModifyShortcutTarget(const char *linkFileName, const char *targetPath, const char *arguments, const char *workingDirectory);
	
	bool GetCommandLineParamValue(const char *paramName, char *value, int valueBufferSize);
	
	bool DeleteRegistryKey(const char *keyName);
	
	const char *GetDesktopFolderPath();
};

}; // namespace vgui