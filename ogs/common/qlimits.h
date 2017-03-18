//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ==========
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

// TODO: merge with procotol.hpp

#pragma once

// DATA STRUCTURE INFO

// SYSTEM INFO
#ifndef MAX_QPATH
const int MAX_QPATH = 64;   // max length of a game pathname
#endif

const int MAX_OSPATH = 260; // max length of a filesystem pathname

const float ON_EPSILON = 0.1f; // point on plane side epsilon

const int MAX_LIGHTSTYLE_INDEX_BITS = 6;
const int MAX_LIGHTSTYLES = (1 << MAX_LIGHTSTYLE_INDEX_BITS);

const int MAX_DECAL_INDEX_BITS = 9;
const int MAX_BASE_DECALS = (1 << MAX_DECAL_INDEX_BITS);