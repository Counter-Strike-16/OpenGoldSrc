/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#ifndef TMESSAGE_H
#define TMESSAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "maintypes.h"

#define DEMO_MESSAGE		"__DEMOMESSAGE__"
#define NETWORK_MESSAGE1	"__NETMESSAGE__1"
#define NETWORK_MESSAGE2	"__NETMESSAGE__2"
#define NETWORK_MESSAGE3	"__NETMESSAGE__3"
#define NETWORK_MESSAGE4	"__NETMESSAGE__4"

#define MAX_NETMESSAGE		4

#define MSGFILE_NAME	0
#define MSGFILE_TEXT	1

#define NAME_HEAP_SIZE	16384
#define MAX_MESSAGES	1000

#ifdef HOOK_ENGINE

#define gMessageTable (*pgMessageTable)
#define gMessageTableCount (*pgMessageTableCount)
#define gMessageParms (*pgMessageParms)

#define gNetworkTextMessageBuffer (*pgNetworkTextMessageBuffer)
#define gNetworkMessageNames (*pgNetworkMessageNames)
#define gNetworkTextMessage (*pgNetworkTextMessage)

#endif // HOOK_ENGINE

extern client_textmessage_t *gMessageTable;
extern int gMessageTableCount;
extern client_textmessage_t gMessageParms;

extern char gNetworkTextMessageBuffer[MAX_NETMESSAGE][512];
extern const char *gNetworkMessageNames[MAX_NETMESSAGE];
extern client_textmessage_t gNetworkTextMessage[MAX_NETMESSAGE];

char *memfgets(unsigned char *pMemFile, int fileSize, int *pFilePos, char *pBuffer, int bufferSize);
int IsComment(char *pText);
int IsStartOfText(char *pText);
int IsEndOfText(char *pText);
int IsWhiteSpace(char space);
NOXREF const char *SkipSpace(const char *pText);
NOXREF const char *SkipText(const char *pText);
NOXREF int ParseFloats(const char *pText, float *pFloat, int count);
void TrimSpace(const char *source, char *dest);
NOXREF int IsToken(const char *pText, const char *pTokenName);
NOXREF int ParseDirective(const char *pText);
NOXREF void TextMessageParse(unsigned char *pMemFile, int fileSize);
NOXREF void TextMessageShutdown();
NOXREF void TextMessageInit();
NOXREF client_textmessage_t *TextMessageGet(const char *pName);

#endif // TMESSAGE_H
