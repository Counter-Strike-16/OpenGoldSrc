extern "C" void ClientDLL_Init()
{
};

extern "C" void ClientDLL_UpdateClientData()
{
};

extern "C" void ClientDLL_HudVidInit()
{
};

extern "C" void ClientDLL_Shutdown()
{
};

void ClientDLL_MoveClient(struct playermove_s *pmove)
{
};

void ClientDLL_DeactivateMouse()
{
};

void ClientDLL_MouseEvent(int mstate)
{
};

extern "C" void ClientDLL_Frame(double time)
{
};

extern "C" void ClientDLL_CAM_Think()
{
};