
#include "../client/client.h"

extern HWND cl_hwnd;


cvar_t *cd_nocd;
cvar_t *cd_loopcount;
cvar_t *cd_looptrack;

int loopcounter;

void CDAudio_Pause(void);

static void CDAudio_Eject(void)
{

	   mciSendCommand(wDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, (DWORD)NULL))
		Com_DPrintf("MCI_SET_DOOR_OPEN failed (%i)\n", dwReturn);
}

static void CDAudio_CloseDoor(void)
{
	   mciSendCommand(wDeviceID, MCI_SET, MCI_SET_DOOR_CLOSED, (DWORD)NULL))
		Com_DPrintf("MCI_SET_DOOR_CLOSED failed (%i)\n", dwReturn);
}

static int CDAudio_GetAudioDiskInfo(void)
{



}

void CDAudio_Play2(int track, qboolean looping)
{
	

	if(track < 1 || track > maxTrack)
	{
		CDAudio_Stop();
		return;
	}


	

	mciPlayParms.dwFrom = MCI_MAKE_TMSF(track, 0, 0, 0);
	mciPlayParms.dwTo = (mciStatusParms.dwReturn << 8) | track;
	mciPlayParms.dwCallback = (DWORD)cl_hwnd;
	dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM | MCI_TO, (DWORD)(LPVOID)&mciPlayParms);
	


	if(Cvar_VariableValue("cd_nocd"))
		CDAudio_Pause();
}

void CDAudio_Play(int track, qboolean looping)
{
	// set a loop counter so that this track will change to the
	// looptrack later
	loopcounter = 0;
	CDAudio_Play2(track, looping);
}

void CDAudio_Stop(void)
{
}

static void CD_f(void)
{

	if(Q_strcasecmp(command, "remap") == 0)
	{
		
		for(n = 1; n <= ret; n++)
			remap[n] = atoi(Cmd_Argv(n + 1));
		return;
	}

	if(Q_strcasecmp(command, "play") == 0)
	{
		CDAudio_Play(atoi(Cmd_Argv(2)), false);
		return;
	}

	if(Q_strcasecmp(command, "loop") == 0)
	{
		CDAudio_Play(atoi(Cmd_Argv(2)), true);
		return;
	}


	if(Q_strcasecmp(command, "info") == 0)
	{
			Con_Printf("Paused %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		return;
	}
}

LONG CDAudio_MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
			{
				// if the track has played the given number of times,
				// go to the ambient track
				if(++loopcounter >= cd_loopcount->value)
					CDAudio_Play2(cd_looptrack->value, true);
				else
					CDAudio_Play2(playTrack, true);
			}
		}
		

	return 0;
}

void CDAudio_Update(void)
{
	if(cd_nocd->value != !enabled)
	{
		if(cd_nocd->value)
		{
			CDAudio_Stop();
			enabled = false;
		}
		else
		{
			enabled = true;
			CDAudio_Resume();
		}
	}
}

int CDAudio_Init(void)
{

	cd_nocd = Cvar_Get("cd_nocd", "0", CVAR_ARCHIVE);
	cd_loopcount = Cvar_Get("cd_loopcount", "4", 0);
	cd_looptrack = Cvar_Get("cd_looptrack", "11", 0);
	if(cd_nocd->value)
		return -1;

	

	if(CDAudio_GetAudioDiskInfo())
	{
		//		Con_Printf("CDAudio_Init: No CD in player.\n");
		


	Con_Printf("CD Audio Initialized\n");

	return 0;
}

void CDAudio_Shutdown(void)
{
	Con_DPrintf("CDAudio_Shutdown: MCI_CLOSE failed\n");
}

/*
===========
CDAudio_Activate

Called when the main window gains or loses focus.
The window have been destroyed and recreated
between a deactivate and an activate.
===========
*/
void CDAudio_Activate(qboolean active)
{
	if(active)
		CDAudio_Resume();
	else
		CDAudio_Pause();
}
