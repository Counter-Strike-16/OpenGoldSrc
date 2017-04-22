


class CConCmdSystem : public IConCmdSystem
{
public:
	virtual void			CommandCompletion( void(*callback)( const char *s ) );
	virtual void			ArgCompletion( const char *cmdString, void(*callback)( const char *s ) );
	
	
	virtual void			ArgCompletion_FolderExtension( const idCmdArgs &args, void(*callback)( const char *s ), const char *folder, bool stripFolder, ... );
	virtual void			ArgCompletion_DeclName( const idCmdArgs &args, void(*callback)( const char *s ), int type );

	virtual void			BufferCommandArgs( cmdExecution_t exec, const idCmdArgs &args );
private:
	static const int		MAX_CMD_BUFFER = 0x10000;

	int						textLength;
	byte					textBuf[MAX_CMD_BUFFER];

	idStr					completionString;
	idStrList				completionParms;

	// piggybacks on the text buffer, avoids tokenize again and screwing it up
	idList<idCmdArgs>		tokenizedCmds;	
	
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