// We either using the cmd/var handlers with their's internal std lists
// or the cmd/var list classes which still internally contains std lists

CCmdBuffer gCmdBuffer;
gCmdBuffer.InsertText("Hello Gordon");
gCmdBuffer.InsertText("exit");
gCmdBuffer.Exec();

// rename CmdBuffer to CmdProcessor/CmdParser?

void CCmdBuffer::Exec()
{
	std::string line{""};
	
	while(has unprocessed data)
	{
		// Format our internal buffer
		// Current buffer:
		// "Hello Gordon\nexit\n"
		// Split the string into lines divided by ; or "\n"
		// And exec each of them
	
		// Exec our "Hello Gordon" string
		UNKNOWN->ExecCmd(line);
	};
};

void CCmdProcessor::ExecCmd(const char *text)
{
	// do CConCmdHandler::ExecCmd things here
};

void CConCmdHandler::TokenizeString(const char *text, IConCmdArgs &Args)
{
	// tokenize the passed string
};

// We don't need an interconnection between the components so they're need to be revisited
// in order to use a descending hierarchy model

// CmdBuffer - connected to ConCmdHandler
// ConCmdHandler - connected to CmdBuffer and ConVarHandler
// ConVarHandler - connected to ConCmdHandler (cmd_argc, cmd_argv will be changed with concmdargs)
// AddConVar - check that concmd with the same name isn't present
// AddConCmd - check than convar with the same name isn't present

// These two funcs below should be used as front-end funcs for client/server
// game dlls
// That means that if use create a concmd on dll side and registering it in
// system these funcs below should be used instead of direct AddCommand method
// to guarantee that the specific flags will be set

// Our CUnknown class should be some kind of front-end for user code
// That means that the cmdhandler/varhandler is operating on lower-level
// which is not directly acessible from a higher level (user code)
// Current GS code is already oriented on that so we just need to
// organize it properly

bool CUnknown::AddGameCommand(IConCmd *apCmd)
{
	apCmd->ClearFlags();
	apCmd->AddFlag(game command); // AddFlag as part of public interface?
	return mpCmdHandler->AddCommand(apCmd);
};

bool CUnknown::AddHUDCommand(IConCmd *apCmd)
{
	apCmd->ClearFlags(); //or ClearCmdFlags(IConCmd *apCmd)?
	apCmd->AddFlag(client command);
	return mpCmdHandler->AddCommand(apCmd);
};

//bool CUnknown::AddCommand(const char *asName, IConCmd *apCmd) // you can use something like an "Exec" method on that
bool CUnknown::AddCommand(const char *asName, pfnConCmdCallback afnCallback, const char *asDesc)
{
	if(host_initialized) // What's wrong with that?
		CSystem::Error("%s after host_initialized", __FUNCTION__);
	
	// GS is checking inside vars first
	// Can we change this logic?
	
	// Check in variables list
	if(mpVarHandler->Exists(asName)) // was Find; var list
	{
		mpConsole->Printf("ConCmd \"%s\" is already known as a var!", asName);
		//Con_Printf("%s: \"%s\" already defined as a var\n", __FUNCTION__, cmd_name);
		return false;
	};
	
	// Check if this command is already defined
	// Fail if the command already exists
	if(mpCmdHandler->Exists(asName)) // was Find; cmd list
	{
		mpConsole->Printf("ConCmd \"%s\" is already registered!", asName);
		//"%s: \"%s\" already defined\n", __FUNCTION__, asName
		return true; // false?
	};
	
	mpCmdHandler->AddCommand(asName, afnCallback, asDesc);
	return true;
};

// Need a some way to detect where did a cvar came from
//bool CUnknown::AddGameVar
//bool CUnknown::AddClientVar // AddHUDVar
//bool CUnknown::AddExternalVar
// What is CUnknown here? The console?
//
// Use different impl of IConsole for server/client sides?
//
// OGS_CLIENTCONSOLE_INTERFACE_VERSION
// OGS_SERVERCONSOLE_INTERFACE_VERSION
//
// Or what?
// IConsoleController?

bool CUnknown::AddVar(const char *asName, const char *asValue, const char *asDesc)
{
	if(mpVarHandler->Exists(asName)) // was Find; var list
	{
		mpConsole->Printf("ConVar \"%s\" is already registered!", asName);
		return true; // false?
	};
	
	if(mpCmdHandler->Exists(asName)) // was Find; cmd list
	{
		mpConsole->Printf("ConVar \"%s\" is already known as a cmd!", asName);
		return false;
	};
	
	mpVarHandler->AddVar(asName, asValue, asDesc);
	return true;
};

IConVar *CUnknown::GetVar(const char *asName)
{
	return mpVarHandler->Find(asName);
};

// This thing is temp
// It's meant to be used instead of IConVar to prevent checks for validity
// With this IConVarRef interface you don't need to check for var and
// you can directly set/get it's values; if it's pointing to a valid var then
// it's values will be modified/returned; otherwise, the internal values of
// convarref class will be modified
// This is planned to be used instead of setvar/getvar value methods
//
// It's may be better to use a CConVarRef class object in the user code
// and pass a IConVar pointer into it for the same functionality
//
// CConVarRef SomeVarRef;
// SomeVarRef = mpUnknown->GetVar("cl_hellogordon");
// SomeVarRef.SetString("bye"); // so this will set the value of "cl_hellogordon"
// cvar to "bye" in case it's registered
//
// CConVarRef NotRegisteredVarRef;
// NotRegisteredVarRef = mpUnknown->GetVar("killme");
// NotRegisteredVarRef.SetBool(false); // set false to internal value of CConVarRef object
// since "killme" wasn't found
const IConVarRef &CUnknown::GetVarRef(const char *asName)
{
	static CConVarRef ConVarRef;
	
	IConVar *pVar = mpVarHandler->Find(asName);
	
	if(pVar)
		ConVarRef.SetVar(pVar); // set our internal var to pVar
	
	return ConVarRef;
};

void CUnknown::SetVarString(const char *asName, const char *asValue)
{
	IConVar *pVar = mpVarHandler->Find(asName);
	
	if(pVar)
		pVar->SetString(asValue);
};

void CUnknown::SetVarInt(const char *asName, int anValue)
{
	IConVar *pVar = mpVarHandler->Find(asName);
	
	if(pVar)
		pVar->SetInt(anValue);
};

void CUnknown::SetVarFloat(const char *asName, float afValue)
{
	IConVar *pVar = mpVarHandler->Find(asName);
	
	if(pVar)
		pVar->SetFloat(afValue);
};

void CUnknown::SetVarBool(const char *asName, bool abValue)
{
	IConVar *pVar = mpVarHandler->Find(asName);
	
	if(pVar)
		pVar->SetBool(abValue);
};

const char *CUnknown::GetVarString(const char *asName, const char *asDefVal)
{
	IConVar *pVar = mpVarHandler->Find(asName);
	
	if(pVar)
		return pVar->GetString();
	
	return asDefVal;
};

int CUnknown::GetVarInt(const char *asName, int anDefVal)
{
	IConVar *pVar = mpVarHandler->Find(asName);
	
	if(pVar)
		return pVar->GetInt();
	
	return anDefVal;
};

float CUnknown::GetVarFloat(const char *asName, float afDefVal)
{
	IConVar *pVar = mpVarHandler->Find(asName);
	
	if(pVar)
		return pVar->GetFloat();
	
	return afDefVal;
};

bool CUnknown::GetVarBool(const char *asName, bool abDefVal)
{
	IConVar *pVar = mpVarHandler->Find(asName);
	
	if(pVar)
		return pVar->GetBool();
	
	return abDefVal;
};

void Cmd_Legacy_f()
{
	int nArgCount = Cmd_Argc();
	char *sArgs = Cmd_Args();
};

void CConCmdHandler::ExecCmd(const CConCmdArgs &CmdArgs)
{
	// Check in our registered cmds list
	
	for(auto It : mlstCmds)
		if(!Q_strcmp(It->GetName(), CmdArgs[0])) // 0 - cmdname; but should we remain it in args?
		{
			It->Exec(CmdArgs);
			return true;
		};
	
	// Legacy-mode commands support
	
	mpCurrentCmdArgs = CmdArgs; // To use Cmd_Argc/Cmd_Argv
	
	for(auto It : mlstOldCmds)
		if(!Q_strcmp(It->GetName(), CmdArgs[0])) // 0 - cmdname; but should we remain it in args?
		{
			It->Exec();
			return true;
		};
	
	// Check in cmd aliases
	
	for(auto It : mlstAliases)
		if(!Q_strcmp(It->GetName(), CmdArgs[0]))
		{
			mpCmdBuffer->InsertText(CmdArgs.ToString());
			return true;
		};
	
	return false;
};

// for just the 2 methods?
struct IConsoleHandler
{
	virtual bool AddCommand(IConCmd *apCmd) = 0;
	virtual bool AddVar(IConVar *apVar) = 0;
};

class CGameConsoleHandler : public IConsoleHandler
{
public:
	bool AddCommand(IConCmd *apCmd);
	bool AddVar(IConVar *apVar);
};

class CClientConsoleHandler : public IConsoleHandler
{
public:
	bool AddCommand(IConCmd *apCmd);
	bool AddVar(IConVar *apVar);
};