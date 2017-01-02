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

#include "InputInternal.hpp"

namespace vgui
{

void CInputInternal::RunFrame()
{
};

void CInputInternal::UpdateMouseFocus(int x, int y)
{
};

void CInputInternal::PanelDeleted(VPANEL panel)
{
};

bool CInputInternal::InternalCursorMoved(int x, int y)
{
	return false;
};

bool CInputInternal::InternalMousePressed(MouseCode code)
{
	return false;
};

bool CInputInternal::InternalMouseDoublePressed(MouseCode code)
{
	return false;
};

bool CInputInternal::InternalMouseReleased(MouseCode code)
{
	return false;
};

bool CInputInternal::InternalMouseWheeled(int delta)
{
	return false;
};

bool CInputInternal::InternalKeyCodePressed(KeyCode code)
{
	return false;
};

void CInputInternal::InternalKeyCodeTyped(KeyCode code)
{
};

void CInputInternal::InternalKeyTyped(wchar_t unichar)
{
};

bool CInputInternal::InternalKeyCodeReleased(KeyCode code)
{
	return false;
};

HInputContext CInputInternal::CreateInputContext()
{
	return 0;
};

void CInputInternal::DestroyInputContext(HInputContext context)
{
};

void CInputInternal::AssociatePanelWithInputContext(HInputContext context, VPANEL pRoot)
{
};

void CInputInternal::ActivateInputContext(HInputContext context)
{
};

VPANEL CInputInternal::GetMouseCapture()
{
	return 0;
};

bool CInputInternal::IsChildOfModalPanel(VPANEL panel)
{
	return false;
};

void CInputInternal::ResetInputContext(HInputContext context)
{
};

}; // namespace vgui