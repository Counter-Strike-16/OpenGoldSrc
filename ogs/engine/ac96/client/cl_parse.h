#pragma once

#include "cdll_int.h"

extern int CL_UPDATE_BACKUP;
extern int CL_UPDATE_MASK;

void CL_ShutDownUsrMessages();

pfnUserMsgHook HookServerMsg(const char *pszName, pfnUserMsgHook pfn);

void CL_RemoveFromResourceList(resource_t *pResource);

void CL_ClearResourceLists();