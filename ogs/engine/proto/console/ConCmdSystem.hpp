


class CConCmdSystem : public IConCmdSystem
{
public:

	
	
	virtual void			CommandCompletion( void(*callback)( const char *s ) );
	virtual void			ArgCompletion( const char *cmdString, void(*callback)( const char *s ) );
	
	virtual void			ExecuteCommandText( const char * text );
	virtual void			AppendCommandText( const char * text );

	virtual void			BufferCommandText( cmdExecution_t exec, const char *text );
	virtual void			ExecuteCommandBuffer();

	virtual void			ArgCompletion_FolderExtension( const idCmdArgs &args, void(*callback)( const char *s ), const char *folder, bool stripFolder, ... );
	virtual void			ArgCompletion_DeclName( const idCmdArgs &args, void(*callback)( const char *s ), int type );

	virtual void			BufferCommandArgs( cmdExecution_t exec, const idCmdArgs &args );

	virtual void			SetupReloadEngine( const idCmdArgs &args );
	virtual bool			PostReloadEngine();

	
private:
	static const int		MAX_CMD_BUFFER = 0x10000;

	int						textLength;
	byte					textBuf[MAX_CMD_BUFFER];

	idStr					completionString;
	idStrList				completionParms;

	// piggybacks on the text buffer, avoids tokenize again and screwing it up
	idList<idCmdArgs>		tokenizedCmds;

	// a command stored to be executed after a reloadEngine and all associated commands have been processed
	idCmdArgs				postReload;

private:	
	void					ExecuteTokenizedString( const idCmdArgs &args );
	void					InsertCommandText( const char *text );

	static void				ListByFlags( const idCmdArgs &args, cmdFlags_t flags );
	static void				List_f( const idCmdArgs &args );
	static void				SystemList_f( const idCmdArgs &args );
	static void				RendererList_f( const idCmdArgs &args );
	static void				SoundList_f( const idCmdArgs &args );
	static void				GameList_f( const idCmdArgs &args );
	static void				ToolList_f( const idCmdArgs &args );
	
	static void				Vstr_f( const idCmdArgs &args );
	
	static void				Parse_f( const idCmdArgs &args );
	
	static void				PrintMemInfo_f( const idCmdArgs &args );
};