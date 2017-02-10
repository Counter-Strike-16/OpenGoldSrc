#include "server/server.hpp"
#include "client/client.hpp"

client_state_t cl;
client_static_t cls;

server_t g_psv;
server_static_t g_psvs;

void Sys_Error(const char *, ...)
{
};

void Sys_Printf(const char *, ...)
{
};

void Sys_ShutdownGame()
{
};

void Con_Printf(const char *, ...)
{
};