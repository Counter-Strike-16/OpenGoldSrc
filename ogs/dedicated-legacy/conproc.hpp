//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

/// @file
/// @brief support for external server monitoring programs

#pragma once

#define CCOM_WRITE_TEXT 0x2
// Param1 : Text

#define CCOM_GET_TEXT 0x3
// Param1 : Begin line
// Param2 : End line

#define CCOM_GET_SCR_LINES 0x4
// No params

#define CCOM_SET_SCR_LINES 0x5
// Param1 : Number of lines

void InitConProc();
void DeinitConProc();

void WriteStatusText(char *psz);