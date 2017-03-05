#include "Host.hpp"

int CHost::Init(quakeparms_t *parms)
{
};

void CHost::InitLocal()
{
};

void CHost::Shutdown()
{
};

void CHost::EndGame(const char *message, ...)
{
};

//void NORETURN Error(const char *error, ...);

void CHost::WriteConfiguration()
{
};

void CHost::WriteCustomConfig()
{
};

void CHost::ClientCommands(const char *fmt, ...)
{
};

void CHost::ClearClients(qboolean bFramesOnly)
{
};

void CHost::ShutdownServer(qboolean crash)
{
};

void CHost::CheckDynamicStructures()
{
};

void CHost::ClearMemory(qboolean bQuiet)
{
};

qboolean CHost::FilterTime(float time)
{
};

void CHost::ComputeFPS(double frametime)
{
};

void CHost::GetInfo(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap)
{
};

void CHost::Speeds(double *time)
{
};

void CHost::UpdateScreen()
{
};

void CHost::UpdateSounds()
{
};

void CHost::CheckConnectionFailure()
{
};

void CHost::_Frame(float time)
{
};

int CHost::Frame(float time, int iState, int *stateInfo)
{
};

//void CheckGore();

qboolean CHost::IsSinglePlayerGame()
{
};

qboolean CHost::IsServerActive()
{
};

void CHost::PrintVersion()
{
};