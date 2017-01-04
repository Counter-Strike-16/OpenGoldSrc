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

#pragma once

#include "common/commontypes.h"

#define DELTA_MAX_FIELDS 56 // 7*8

#define DT_BYTE BIT(0)           // A byte
#define DT_SHORT BIT(1)          // 2 byte field
#define DT_FLOAT BIT(2)          // A floating point field
#define DT_INTEGER BIT(3)        // 4 byte integer
#define DT_ANGLE BIT(4)          // A floating point angle
#define DT_TIMEWINDOW_8 BIT(5)   // A floating point timestamp relative to server time
#define DT_TIMEWINDOW_BIG BIT(6) // A floating point timestamp relative to server time (with more precision and custom multiplier)
#define DT_STRING BIT(7)         // A null terminated string, sent as 8 byte chars
#define DT_SIGNED BIT(31)        // sign modificator

#define FDT_MARK BIT(0) // Delta mark for sending

typedef struct delta_s delta_t;
typedef void (*encoder_t)(delta_t *, const unsigned char *, const unsigned char *);

typedef struct delta_stats_s
{
	int sendcount;
	int receivedcount;
} delta_stats_t;

typedef struct delta_description_s
{
	int           fieldType;
	char          fieldName[32];
	int           fieldOffset;
	short int     fieldSize;
	int           significant_bits;
	float         premultiply;
	float         postmultiply;
	short int     flags;
	delta_stats_t stats;
} delta_description_t;

class CDeltaJit;

typedef struct delta_s
{
	int                  dynamic;
	int                  fieldCount;
	char                 conditionalencodename[32];
	encoder_t            conditionalencode;
	delta_description_t *pdd;

#ifdef REHLDS_FIXES
	CDeltaJit *jit;
#endif
} delta_t;

typedef struct delta_encoder_s delta_encoder_t;

struct delta_encoder_s
{
	delta_encoder_t *next;
	char *           name;
	encoder_t        conditionalencode;
};

typedef struct delta_link_s            delta_link_t;
typedef struct delta_definition_s      delta_definition_t;
typedef struct delta_definition_list_s delta_definition_list_t;
typedef struct delta_registry_s        delta_registry_t;
typedef struct delta_info_s            delta_info_t;

typedef struct delta_info_s
{
	delta_info_s *next;
	char *        name;
	char *        loadfile;
	delta_t *     delta;
} delta_info_t;

#ifdef HOOK_ENGINE
#define g_defs (*pg_defs)
#define g_encoders (*pg_encoders)
#define g_deltaregistry (*pg_deltaregistry)
#endif // HOOK_ENGINE

extern delta_definition_list_t *g_defs;
extern delta_encoder_t *        g_encoders;
extern delta_registry_t *       g_deltaregistry;
extern delta_t                  g_MetaDelta[];

delta_description_t *DELTA_FindField(delta_t *pFields, const char *pszField);
int DELTA_FindFieldIndex(struct delta_s *pFields, const char *fieldname);
void DELTA_SetField(struct delta_s *pFields, const char *fieldname);
void DELTA_UnsetField(struct delta_s *pFields, const char *fieldname);
void DELTA_SetFieldByIndex(struct delta_s *pFields, int fieldNumber);
void DELTA_UnsetFieldByIndex(struct delta_s *pFields, int fieldNumber);
void DELTA_ClearFlags(delta_t *pFields);
int DELTA_TestDelta(unsigned char *from, unsigned char *to, delta_t *pFields);
int DELTA_CountSendFields(delta_t *pFields);
void DELTA_MarkSendFields(unsigned char *from, unsigned char *to, delta_t *pFields);
void DELTA_SetSendFlagBits(delta_t *pFields, int *bits, int *bytecount);
qboolean DELTA_IsFieldMarked(delta_t *pFields, int fieldNumber);
void DELTA_WriteMarkedFields(unsigned char *from, unsigned char *to, delta_t *pFields);
qboolean DELTA_CheckDelta(unsigned char *from, unsigned char *to, delta_t *pFields);

#ifdef REHLDS_FIXES //Fix for https://github.com/dreamstalker/rehlds/issues/24
qboolean DELTA_WriteDeltaForceMask(unsigned char *from, unsigned char *to, qboolean force, delta_t *pFields, void (*callback)(void), void *pForceMask);
#endif

qboolean DELTA_WriteDelta(unsigned char *from, unsigned char *to, qboolean force, delta_t *pFields, void (*callback)(void));
qboolean _DELTA_WriteDelta(unsigned char *from, unsigned char *to, qboolean force, delta_t *pFields, void (*callback)(void), qboolean sendfields);
int DELTA_ParseDelta(unsigned char *from, unsigned char *to, delta_t *pFields);
void DELTA_AddEncoder(char *name, void (*conditionalencode)(struct delta_s *, const unsigned char *, const unsigned char *));
void      DELTA_ClearEncoders(void);
encoder_t DELTA_LookupEncoder(char *name);
int DELTA_CountLinks(delta_link_t *plinks);
void DELTA_ReverseLinks(delta_link_t **plinks);
void DELTA_ClearLinks(delta_link_t **plinks);
delta_t *DELTA_BuildFromLinks(delta_link_t **pplinks);
int DELTA_FindOffset(int count, delta_definition_t *pdef, char *fieldname);
qboolean DELTA_ParseType(delta_description_t *pdelta, char **pstream);
qboolean DELTA_ParseField(int count, delta_definition_t *pdefinition, delta_link_t *pField, char **pstream);
void DELTA_FreeDescription(delta_t **ppdesc);
void DELTA_AddDefinition(char *name, delta_definition_t *pdef, int numelements);
void                DELTA_ClearDefinitions(void);
delta_definition_t *DELTA_FindDefinition(char *name, int *count);
void DELTA_SkipDescription(char **pstream);
qboolean DELTA_ParseOneField(char **ppstream, delta_link_t **pplist, int count, delta_definition_t *pdefinition);
qboolean DELTA_ParseDescription(char *name, delta_t **ppdesc, char *pstream);
qboolean DELTA_Load(char *name, delta_t **ppdesc, char *pszFile);
void DELTA_RegisterDescription(char *name);
void      DELTA_ClearRegistrations(void);
delta_t **DELTA_LookupRegistration(const char *name);
void DELTA_ClearStats(delta_t *p);
void DELTA_ClearStats_f(void);
void DELTA_PrintStats(const char *name, delta_t *p);
void DELTA_DumpStats_f(void);
void DELTA_Init(void);
void DELTA_Shutdown(void);