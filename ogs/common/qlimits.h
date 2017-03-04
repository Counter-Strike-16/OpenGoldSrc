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
const int MAX_QPATH = 64;   // max length of a game pathname
const int MAX_OSPATH = 260; // max length of a filesystem pathname

const float ON_EPSILON = 0.1f; // point on plane side epsilon

const int MAX_LIGHTSTYLE_INDEX_BITS = 6;
const int MAX_LIGHTSTYLES = (1 << MAX_LIGHTSTYLE_INDEX_BITS);

// Resource counts
const int MAX_MODEL_INDEX_BITS = 9; // sent as a short
const int MAX_MODELS = (1 << MAX_MODEL_INDEX_BITS);

const int MAX_SOUND_INDEX_BITS = 9;
const int MAX_SOUNDS = (1 << MAX_SOUND_INDEX_BITS);

const int MAX_SOUNDS_HASHLOOKUP_SIZE = (MAX_SOUNDS * 2 - 1);

const int MAX_GENERIC_INDEX_BITS = 9;
const int MAX_GENERIC = (1 << MAX_GENERIC_INDEX_BITS);

const int MAX_DECAL_INDEX_BITS = 9;
const int MAX_BASE_DECALS = (1 << MAX_DECAL_INDEX_BITS);

const int MAX_EVENTS = 256;
const int MAX_PACKET_ENTITIES = 256; // 256 visible entities per frame