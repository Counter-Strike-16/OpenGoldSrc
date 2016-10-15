#pragma once

class CCamera
{
public:
	CCamera();
	~CCamera();
	
	qboolean Cam_DrawViewModel(void);
qboolean Cam_DrawPlayer(int playernum);
void Cam_Track(usercmd_t *cmd);
void Cam_FinishMove(usercmd_t *cmd);
void Cam_Reset(void);
};