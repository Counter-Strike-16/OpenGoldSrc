



constexpr auto NUM_COLUMNS = 77; // 78 - 1
constexpr auto NUM_NAME_CHARS = 33;
constexpr auto NUM_DESCRIPTION_CHARS = ( NUM_COLUMNS - NUM_NAME_CHARS );
constexpr auto FORMAT_STRING = "%-32s ";

class CConVarSystem 
{
public:

	virtual bool			IsInitialized() const;

	virtual void			CommandCompletion( void(*callback)( const char *s ) );
	virtual void			ArgCompletion( const char *cmdString, void(*callback)( const char *s ) );

	virtual void			SetModifiedFlags( int flags );
	virtual int				GetModifiedFlags() const;
	virtual void			ClearModifiedFlags( int flags );

	virtual void			ResetFlaggedVariables( int flags );
	virtual void			RemoveFlaggedAutoCompletion( int flags );
	virtual void			WriteFlaggedVariables( int flags, const char *setCmd, idFile *f ) const;

	virtual void			MoveCVarsToDict( int flags, idDict & dict, bool onlyModified ) const;
	virtual void			SetCVarsFromDict( const idDict &dict );

	void					RegisterInternal( CConVar *cvar );
	idInternalCVar *		FindInternal( const char *name ) const;
	void					SetInternal( const char *name, const char *value, int flags );

private:
	bool					initialized;
	idList<idInternalCVar*, TAG_CVAR>	cvars;
	idHashIndex				cvarHash;
	int						modifiedFlags;

private:
	static void				Toggle_f( const idCmdArgs &args );
	static void				Set_f( const idCmdArgs &args );
	static void				Reset_f( const idCmdArgs &args );
	static void				ListByFlags( const idCmdArgs &args, cvarFlags_t flags );
	static void				List_f( const idCmdArgs &args );
	static void				Restart_f( const idCmdArgs &args );
	static void				CvarAdd_f( const idCmdArgs &args );
};