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

#include "precompiled.h"

wadlist_t wads[NUM_WADS];

/* <c6c98> ../engine/wad.c:35 */
void W_CleanupName(char *in, char *out)
{
	int i;
	int c;

	for (i = 0; i < 16; i++)
	{
		c = in[i];
		if (!c)
			break;
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		out[i] = c;
	}
	if (i < 16)
		Q_memset(&out[i], 0, 16 - i);
}

/* <c6d06> ../engine/wad.c:62 */
int W_LoadWadFile(char *filename)
{
	int slot = 0;
	for (; slot < NUM_WADS; slot++)
	{
		if (!wads[slot].loaded)
			break;
	}

	if (slot >= NUM_WADS)
	{
		Con_Printf("No room for wad %s\n", filename);
		return -1;
	}

	wadlist_t *wad = &wads[slot];

	wad->wad_base = (byte *)COM_LoadHunkFile(filename);
	if (!wad->wad_base)
	{
		if (!slot)
			Sys_Error("W_LoadWadFile: couldn't load %s", filename);
		Con_Printf("WARNING:  W_LoadWadFile, couldn't load %s\n", filename);
		return -1;
	}

	Q_strncpy(wad->wadname, filename, sizeof(wad->wadname) - 1);
	wadinfo_t *header = (wadinfo_t *)wad->wad_base;
	wad->wadname[sizeof(wad->wadname) - 1] = 0;
	wad->loaded = TRUE;
	if (*(uint32 *)header->identification != MAKEID('W', 'A', 'D', '3'))
		Sys_Error("Wad file %s doesn't have WAD3 id\n", filename);
	wad->wad_numlumps = LittleLong(header->numlumps);

	lumpinfo_t * lump_p = (lumpinfo_t *)&wad->wad_base[LittleLong(header->infotableofs)];
	wad->wad_lumps = lump_p;

	for (int i = 0; i < wad->wad_numlumps; ++i, ++lump_p)
	{
		lump_p->filepos = LittleLong(lump_p->filepos);
		lump_p->size = LittleLong(lump_p->size);
		W_CleanupName(lump_p->name, lump_p->name);
		if (lump_p->type == 66)
			SwapPic((qpic_t *)&wad->wad_base[lump_p->filepos]);
	}

	return slot;
}

/* <c6c45> ../engine/wad.c:132 */
lumpinfo_t *W_GetLumpinfo(int wad, char *name, qboolean doerror)
{
	int i;
	lumpinfo_t *lump_p;
	char clean[16];

	W_CleanupName(name, clean);
	for (i = 0; i < wads[wad].wad_numlumps; i++)
	{
		lump_p = wads[i].wad_lumps;
		if (!Q_strcmp(clean, lump_p->name))
			return lump_p;
	}

	if (doerror)
		Sys_Error("W_GetLumpinfo: %s not found", name);

	return NULL;
}

/* <c6e6e> ../engine/wad.c:152 */
void *W_GetLumpName(int wad, char *name)
{
	lumpinfo_t *lump = W_GetLumpinfo(wad, name, TRUE);
	if (lump != NULL)
		return &wads[wad].wad_base[lump->filepos];
	return NULL;
}

/* <c6f2d> ../engine/wad.c:161 */
NOXREF void *W_GetLumpNum(int wad, int num)
{
	lumpinfo_t *lump;
	if (num < 0 || num > wads[wad].wad_numlumps)
		Sys_Error("W_GetLumpNum: bad number: %i", num);

	lump = wads[wad].wad_lumps;
	return (void *)&wads[wad].wad_base[lump->filepos];
}

/* <c6f87> ../engine/wad.c:173 */
void W_Shutdown(void)
{
	for (int slot = 0; slot < NUM_WADS; slot++)
	{
		wadlist_t *wad = &wads[slot];
		if (!wad->loaded)
			break;

		Q_memset(wad, 0, sizeof(wadlist_t));
	}
}

/* <c6c25> ../engine/wad.c:196 */
void SwapPic(qpic_t *pic)
{
	pic->width = LittleLong(pic->width);
	pic->height = LittleLong(pic->height);
}
