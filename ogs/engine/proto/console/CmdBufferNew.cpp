CCmdBuffer gCmdBuffer;
gCmdBuffer.InsertText("Hello Gordon");
gCmdBuffer.InsertText("exit");
gCmdBuffer.Exec();

// rename CmdBuffer to CmdProcessor?

void CCmdBuffer::Exec()
{
	std::string line{""};
	
	// Format our internal buffer
	// Current buffer:
	// "Hello Gordon\nexit\n"
	// Split the string into tokens divided by ; or "\n"
	// And exec each of them
	
	// Exec our "Hello Gordon" string
	mpCmdHandler->ExecCmd(line);
};

void CCmdProcessor::ExecCmd(const char *text)
{
	// do CConCmdHandler::ExecCmd things here
};

IConCmdArgs *CConCmdHandler::TokenizeString(const char *text)
{
	return new CConCmdArgs(text); // temp
};

void CConCmdHandler::ExecCmd(const char *text)
{
	static IConCmdArgs CmdArgs; // crappy
	&CmdArgs = TokenizeString(text); // crappy
	
	for(auto It : mlstCmds)
		if(!Q_strcmp(It->GetName(), CmdArgs[0])) // 0 - cmdname; but should we remain it in args?
		{
			It->Exec(CmdArgs);
			return;
		};
	
	for(auto It : mlstAliases)
		if(!Q_strcmp(It->GetName(), CmdArgs[0]))
			mpCmdBuffer->InsertText(CmdArgs.ToString());
	
	if(!mpVarHandler->HandleCommand(CmdArgs))
		if(mpLocalClient->GetState() >= eClientState::Connected)
			mpNetwork->ForwardCmdToServer(CmdArgs); // network/client ?
			//mpLocalClient->ForwardCmdToServer(CmdArgs); // client has a ptr to net
};

// We don't need an interconnection between the components so they're need to be revisited
// in order to use a descending hierarchy model

// CmdBuffer - connected to ConCmdHandler
// ConCmdHandler - connected to CmdBuffer and ConVarHandler
// ConVarHandler - connected to ConCmdHandler (cmd_argc, cmd_argv will be changed with concmdargs)
// AddConVar - check that concmd with the same name isn't present
// AddConCmd - check than convar with the same name isn't present

bool CConsole::AddCommand(const char *asName)
{
	if(mpCmdHandler->Find(asName))
	{
		Printf("Blah-blah");
		return true;
	};
	
	if(mpVarHandler->Find(asName))
	{
		Printf("ConCmd is already known as a var");
		return false;
	};
	
	return true;
};

bool CConsole::AddVar(const char *asName)
{
	if(mpVarHandler->Find(asName))
	{
		Printf("Blah-blah");
		return true;
	};
	
	if(mpCmdHandler->Find(asName))
	{
		Printf("ConVar is already known as a cmd");
		return false;
	};
	
	return true;
};