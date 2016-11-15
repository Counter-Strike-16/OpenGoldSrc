/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// Input.h -- external (non-keyboard) input devices

#pragma once

class CInput
{
public:
	CInput();
	~CInput();
	
	void Init();
	void Shutdown();
	
	void Frame();
	
	void SetActive(bool active);
	bool IsActive();
	
	void ShowMouse();
	void HideMouse();
	
	void ActivateMouse();
	void DeactivateMouse();
	
	void RestoreOriginalMouseState();
	void SetQuakeMouseState();
	
	void MouseEvent(int mstate);
	
	// Oportunity for devices to stick commands on the script buffer
	void Commands();
	
	// Add additional movement on top of the keyboard move cmd
	void Move(usercmd_t *cmd);
	
	// Restores all button and position states to defaults
	void ClearStates();
	
	// Called whenever screen dimensions change
	void ModeChanged();
};