#pragma once

class CMapHandler
{
	bool Load(const char *asFile, const char *asStartPos);
	
	void ChangeLevel(const char *asNewLevel, const char *asStartPos);
};