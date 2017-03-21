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
/// @brief Wrapper class around edict_t to interact with it in OOP way

#pragma once

/*
class CEdict : edict_t
{
	void SetSize(...){Edict_SetSize(this, ...)}
	void SetModel(...);
};

class CEdictHandle // reference to edict object; semantically it would work as EHANDLE in hlsdk
{
CEdictHandle(edict_t *aggr);
edict_t *privedict;
};
*/

class CEdictHandle
{
public:
	CEdictHandle(int id, edict_t *apEdict)
	    : mnID(id), mpHandlingEdict(apEdict)
	{
	}
	~CEdictHandle();

	void Clear();

	void SaveSpawnParms();

	void InitGameDLLFields();
	void ReleaseGameDLLFields();

	int GetID()
	{
		return mnID;
	} // IndexOfEdict
	entvars_t *GetEntVars();
	void *GetModelPtr();

	edict_t *operator->() const
	{
		return mpHandlingEdict;
	}
	//edict_t *operator&(){return *mpHandlingEdict;} // potential UB?

	const edict_t &operator*() const
	{
		return mpHandlingEdict;
	}
	edict_t &operator*() const
	{
		return mpHandlingEdict;
	}
	void SetHandlingEdict(edict_t *apHandlingEdict)
	{
		mpHandlingEdict = apHandlingEdict;
	}
	edict_t *GetHandlingEdict() const
	{
		return mpHandlingEdict;
	}

private:
	edict_t *mpHandlingEdict;
	int mnID;
};