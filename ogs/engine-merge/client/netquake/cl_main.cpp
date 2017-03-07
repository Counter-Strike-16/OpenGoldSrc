
// these two are not intended to be set directly
cvar_t	cl_name = {"_cl_name", "player", true};
cvar_t	cl_color = {"_cl_color", "0", true};

cvar_t	cl_nolerp = {"cl_nolerp","0"};

entity_t		*cl_visedicts[MAX_VISEDICTS];

void CL_ClearState (void)
{

	if (!sv.active)
		Host_ClearMemory ();

	SZ_Clear (&cls.message);

// clear other arrays	
	memset (cl_entities, 0, sizeof(cl_entities));
	memset (cl_temp_entities, 0, sizeof(cl_temp_entities));
	memset (cl_beams, 0, sizeof(cl_beams));
}

void CL_Disconnect_f (void)
{
	CL_Disconnect ();
	if (sv.active)
		Host_ShutdownServer (false);
}

/*
=====================
CL_NextDemo

Called to play the next demo in the demo loop
=====================
*/
void CL_NextDemo (void)
{
	char	str[1024];

	if (cls.demonum == -1)
		return;		// don't play demos

	SCR_BeginLoadingPlaque ();

	if (!cls.demos[cls.demonum][0] || cls.demonum == MAX_DEMOS)
	{
		cls.demonum = 0;
		if (!cls.demos[cls.demonum][0])
		{
			Con_Printf ("No demos listed with startdemos\n");
			cls.demonum = -1;
			return;
		}
	}

	sprintf (str,"playdemo %s\n", cls.demos[cls.demonum]);
	Cbuf_InsertText (str);
	cls.demonum++;
}




/*
===============
SetPal

Debugging tool, just flashes the screen
===============
*/
void SetPal (int i)
{
#if 0
	static int old;
	byte	pal[768];
	int		c;
	
	if (i == old)
		return;
	old = i;

	if (i==0)
		VID_SetPalette (host_basepal);
	else if (i==1)
	{
		for (c=0 ; c<768 ; c+=3)
		{
			pal[c] = 0;
			pal[c+1] = 255;
			pal[c+2] = 0;
		}
		VID_SetPalette (pal);
	}
	else
	{
		for (c=0 ; c<768 ; c+=3)
		{
			pal[c] = 0;
			pal[c+1] = 0;
			pal[c+2] = 255;
		}
		VID_SetPalette (pal);
	}
#endif
}

