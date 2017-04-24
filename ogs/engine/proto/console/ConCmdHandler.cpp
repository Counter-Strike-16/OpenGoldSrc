void Cmd_Shutdown()
{
	cmd_functions = NULL; // TODO: Check that memory from functions is released too
};

// It almost fully duplicates the Cmd_AddCommand

// Use this for call from user code, because it alloc string for the name
//AddMallocCommand
IConCmd *CConCmdHandler::Create(char *cmd_name, xcommand_t function, int flag)
{
	// Check in variables list
	if(Cvar_FindVar(cmd_name) != NULL)
	{
		mpConsole->Printf("%s: \"%s\" already defined as a var\n", __FUNCTION__, cmd_name);
		return;
	};

	// Check if this command is already defined
	if(Exists(cmd_name))
	{
		mpConsole->Printf("%s: \"%s\" already defined\n", __FUNCTION__, cmd_name);
		return;
	};

	// Create cmd_function
	cmd_function_t *cmd = (cmd_function_t *)Mem_Malloc(sizeof(cmd_function_t));
	cmd->name = CopyString(cmd_name); // alloc string, so it will not dissapear on
	                                  // side modules unloading and to maintain
	                                  // the same name during run
	cmd->function = function ? function : Cmd_ForwardToServer;
	cmd->flags = flag;

	Insert(cmd);
	return cmd;
};

void CConCmdHandler::Insert(cmd_function_t *cmd)
{
	cmd_function_t *c, **p;

	// Commands list is alphabetically sorted, search where to push
	c = cmd_functions;
	p = &cmd_functions;
	
	while(c)
	{
		if(Q_stricmp(c->name, cmd->name) > 0)
		{
			// Current command name is bigger, insert before it
			cmd->next = c;
			*p = cmd;
			return;
		};
		
		p = &c->next;
		c = c->next;
	};

	// All commands in the list are lower then the new one
	cmd->next = nullptr;
	*p = cmd;
};

/*
============
Cmd_RemoveCommand
============
*/
void EXT_FUNC CConCmdHandler::RemoveCmd(char *cmd_name)
{
	auto prev = FindPrev(cmd_name);
	
	if(prev)
	{
		auto cmd = prev->next;
		prev->next = cmd->next;

		Z_Free(cmd->name);
		Mem_Free(cmd);
	};
};

NOXREF cmd_function_t *CConCmdHandler::Find(char *cmd_name)
{
	NOXREFCHECK;

	for(auto cmd = cmd_functions; cmd; cmd = cmd->next)
	{
		if(!Q_stricmp(cmd_name, cmd->name))
			return cmd;
	};

	return nullptr;
};

cmd_function_t *CConCmdHandler::FindPrev(char *cmd_name)
{
	if(!cmd_functions)
		return nullptr;

	for(cmd_function_t *cmd = cmd_functions; cmd->next; cmd = cmd->next)
	{
		if(!Q_stricmp(cmd_name, cmd->next->name))
			return cmd;
	};

	return nullptr;
};

bool CConCmdHandler::Exists(const char *cmd_name)
{
	auto cmd = cmd_functions;

	while(cmd)
	{
		if(!Q_stricmp(cmd_name, cmd->name))
			return true;

		cmd = cmd->next;
	};

	return false;
};