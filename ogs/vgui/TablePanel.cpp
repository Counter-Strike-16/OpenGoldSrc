//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_TablePanel.h"

using namespace vgui;

TablePanel::TablePanel(int x,int y,int wide,int tall,int columnCount)
{
}

void TablePanel::setCellEditingEnabled(bool state)
{
}

void TablePanel::setColumnCount(int columnCount)
{
}

void TablePanel::setGridVisible(bool horizontal,bool vertical)
{
}

void TablePanel::setGridSize(int gridWide,int gridTall)
{
}

int TablePanel::getColumnCount()
{
	return 0;
}

void TablePanel::setColumnExtents(int column,int x0,int x1)
{
}

void TablePanel::setSelectedCell(int column,int row)
{
}

void TablePanel::getSelectedCell(int& column,int& row)
{
}

void TablePanel::setHeaderPanel(HeaderPanel* header)
{
}

void TablePanel::setColumnSelectionEnabled(bool state)
{
}

void TablePanel::setRowSelectionEnabled(bool state)
{
}

void TablePanel::setCellSectionEnabled(bool state)
{
}

void TablePanel::setEditableCell(int column,int row)
{
}

void TablePanel::stopCellEditing()
{
}

void TablePanel::getVirtualSize(int& wide,int& tall)
{
}

void TablePanel::paint()
{
}

Panel* TablePanel::isWithinTraverse(int x,int y)
{
	return null;
}

void TablePanel::privateMousePressed(MouseCode code,Panel* panel)
{
}

void TablePanel::privateMouseDoublePressed(MouseCode code,Panel* panel)
{
}

void TablePanel::privateKeyTyped(KeyCode code,Panel* panel)
{
}
