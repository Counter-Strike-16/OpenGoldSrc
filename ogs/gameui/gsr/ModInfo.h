#ifndef GAMEUI_MODINFO_H
#define GAMEUI_MODINFO_H

class KeyValues;

/**
*	Stores data from the current game's liblist.gam.
*/
class CModInfo
{
public:
	CModInfo();
	~CModInfo();

	void FreeModInfo();

	const char* GetStartMap();

	const char* GetTrainMap();

	bool IsMultiplayerOnly();

	bool IsSinglePlayerOnly();

	/**
	*	Whether the map loading dialog should be shown as a simple progress dialog,
	*	or a more detailed dialog.
	*/
	bool BShowSimpleLoadingDialog();

	/**
	*	Whether the player model selection dialog should be disabled.
	*/
	bool NoModels();

	bool NoHiModel();

	/**
	*	Fallback directory for game content.
	*/
	const char* GetFallbackDir();

	/**
	*	Whether to use the fallback directory for maps.
	*/
	bool UseFallbackDirMaps();

	bool GetDetailedTexture();

	const char* GetGameDescription();

	/**
	*	Substring that cannot occur in a map name for listing in multiplayer server dialogs.
	*/
	const char* GetMPFilter();

	void LoadCurrentGameInfo();

private:
	void LoadGameInfoFromBuffer( const char* buffer );

private:
	KeyValues* m_pModData;

private:
	CModInfo( const CModInfo& ) = delete;
	CModInfo& operator=( const CModInfo& ) = delete;
};

CModInfo& ModInfo();

#endif //GAMEUI_MODINFO_H
