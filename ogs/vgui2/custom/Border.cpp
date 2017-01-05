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

#include "Border.hpp"

namespace vgui
{

СBorder::СBorder()
{
	_inset[0] = 0;
	_inset[1] = 0;
	_inset[2] = 0;
	_inset[3] = 0;
	
	_name = NULL;
	
	m_eBackgroundType = IBorder::BACKGROUND_FILLED;
	
	memset(_sides, 0, sizeof(_sides));
};

СBorder::~СBorder()
{
	delete [] _name;
	
	for(int i = 0; i < 4; ++i)
		delete [] _sides[i].lines;
};

void CBorder::Paint(VPANEL panel)
{
	int wide, tall;
	((VPanel*)panel)->GetSize(wide, tall);
	
	Paint(0, 0, wide, tall, -1, 0, 0);

	m_bIsFristPaint = false;
};

void CBorder::Paint(int x0, int y0, int x1, int y1)
{
};

void CBorder::Paint(int x0, int y0, int x1, int y1, int breakSide, int breakStart, int breakStop)
{
	int i;
	
	for(i = 0; i < _sides[SIDE_LEFT].count; i++)
	{
		line_t *line = &(_sides[SIDE_LEFT].lines[i]);
		g_pSurface->DrawSetColor(line->col[0], line->col[1], line->col[2], line->col[3]);
		
		if(breakSide == SIDE_LEFT)
		{
			if(breakStart > 0)
				g_pSurface->DrawFilledRect(x + i, y + line->startOffset, x + i + 1, y + breakStart);

			if(breakEnd < (tall - line->endOffset))
				g_pSurface->DrawFilledRect(x + i, y + breakEnd + 1, x + i + 1, tall - line->endOffset);
		}
		else
			g_pSurface->DrawFilledRect(x + i, y + line->startOffset, x + i + 1, tall - line->endOffset);
	};
	
	for(i = 0; i < _sides[SIDE_TOP].count; i++)
	{
		line_t *line = &(_sides[SIDE_TOP].lines[i]);
		g_pSurface->DrawSetColor(line->col[0], line->col[1], line->col[2], line->col[3]);
		
		if(breakSide == SIDE_TOP)
		{
			if(breakStart > 0)
				g_pSurface->DrawFilledRect(x + line->startOffset, y + i, x + breakStart, y + i + 1);
			
			if(breakEnd < (wide - line->endOffset))
				g_pSurface->DrawFilledRect(x + breakEnd + 1, y + i, wide - line->endOffset, y + i + 1);
		}
		else
			g_pSurface->DrawFilledRect(x + line->startOffset, y + i, wide - line->endOffset, y + i + 1);
	};
	
	for(i = 0; i < _sides[SIDE_RIGHT].count; i++)
	{
		line_t *line = &(_sides[SIDE_RIGHT].lines[i]);
		g_pSurface->DrawSetColor(line->col[0], line->col[1], line->col[2], line->col[3]);
		g_pSurface->DrawFilledRect(wide - (i + 1), y + line->startOffset, (wide - (i + 1)) + 1, tall - line->endOffset);
	};
	
	for(i = 0; i < _sides[SIDE_BOTTOM].count; i++)
	{
		line_t *line = &(_sides[SIDE_BOTTOM].lines[i]);
		g_pSurface->DrawSetColor(line->col[0], line->col[1], line->col[2], line->col[3]);
		g_pSurface->DrawFilledRect(x + line->startOffset, tall - (i + 1), wide - line->endOffset, (tall - (i + 1)) + 1);
	};
	
	m_bIsFristPaint = false;
};

void CBorder::SetInset(int left, int top, int right, int bottom)
{
	_inset[SIDE_LEFT] = left;
	_inset[SIDE_TOP] = top;
	_inset[SIDE_RIGHT] = right;
	_inset[SIDE_BOTTOM] = bottom;
};

void CBorder::GetInset(int &left, int &top, int &right, int &bottom)
{
	left = _inset[SIDE_LEFT];
	top = _inset[SIDE_TOP];
	right = _inset[SIDE_RIGHT];
	bottom = _inset[SIDE_BOTTOM];
};

void CBorder::AddLine(sides_e side, Color color, int startOffset, int endOffset)
{
};

void CBorder::ApplySchemeSettings(IScheme *pScheme, KeyValues *inResourceData)
{
	const char *insetString = inResourceData->GetString("inset", "0 0 0 0");
	
	int left, top, right, bottom;
	GetInset(left, top, right, bottom);
	sscanf(insetString, "%d %d %d %d", &left, &top, &right, &bottom);
	SetInset(left, top, right, bottom);
	
	ParseSideSettings(SIDE_LEFT, inResourceData->FindKey("Left"),pScheme);
	ParseSideSettings(SIDE_TOP, inResourceData->FindKey("Top"),pScheme);
	ParseSideSettings(SIDE_RIGHT, inResourceData->FindKey("Right"),pScheme);
	ParseSideSettings(SIDE_BOTTOM, inResourceData->FindKey("Bottom"),pScheme);
	
	m_eBackgroundType = (backgroundtype_e)inResourceData->GetInt("backgroundtype");
	m_bIsFristPaint = true;
};

void CBorder::ParseSideSettings(int side_index, KeyValues *inResourceData, IScheme *pScheme)
{
	if(!inResourceData)
		return;
	
	int count = 0;
	KeyValues *kv;
	
	for(kv = inResourceData->GetFirstSubKey(); kv != NULL; kv = kv->GetNextKey())
		count++;
	
	_sides[side_index].count = count;
	_sides[side_index].lines = new line_t[count];
	
	int index = 0;
	
	for(kv = inResourceData->GetFirstSubKey(); kv != NULL; kv = kv->GetNextKey())
	{
		line_t *line = &(_sides[side_index].lines[index]);
		
		const char *col = kv->GetString("color", NULL);
		line->col = pScheme->GetColor(col, Color(0, 0, 0, 0));
		col = kv->GetString("offset", NULL);
		
		int Start = 0, end = 0;
		
		if(col)
			sscanf(col, "%d %d", &Start, &end);
		
		line->startOffset = Start;
		line->endOffset = end;
		
		index++;
	};
};

const char *CBorder::GetName()
{
	if(_name)
		return _name;
	
	return "";
};

void CBorder::SetName(const char *name)
{
	if(_name)
		delete [] _name;
	
	int len = Q_strlen(name) + 1;
	_name = new char[len];
	Q_strncpy(_name, name, len);
};

IBorder::backgroundtype_e CBorder::GetBackgroundType()
{
	return m_eBackgroundType;
};

}; // namespace vgui