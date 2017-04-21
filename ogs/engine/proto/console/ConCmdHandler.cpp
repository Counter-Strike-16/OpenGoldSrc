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

NOXREF cmd_function_t *CConCmdHandler::FindCmd(char *cmd_name)
{
	NOXREFCHECK;

	for(auto cmd = cmd_functions; cmd; cmd = cmd->next)
	{
		if(!Q_stricmp(cmd_name, cmd->name))
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