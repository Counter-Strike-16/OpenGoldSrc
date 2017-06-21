
#
#	GameUI CMake build file
#

#On Windows this is capitalized, on other platforms it's all lowercase.
if( WIN32 )
	set( PROJECT_NAME GameUI )
else()
	set( PROJECT_NAME gameui )
endif()

add_sources(
	${SRC_DIR}/common/BaseUI/IBaseUI.h
	${SRC_DIR}/common/GameUI/ICareerUI.h
	${SRC_DIR}/common/GameUI/IClientVGUI.h
	${SRC_DIR}/common/GameUI/IGameConsole.h
	${SRC_DIR}/common/GameUI/IGameUI.h
	${SRC_DIR}/common/GameUI/IMusicManager.h
	${SRC_DIR}/common/ServerBrowser/IServerBrowser.h
	${SRC_DIR}/common/IRunGameEngine.h
	${SRC_DIR}/common/IVGuiModule.h
	${SRC_DIR}/common/IVGuiModuleLoader.h
	${SRC_DIR}/common/port.h
	${SRC_DIR}/common/Random.cpp
	${SRC_DIR}/common/Random.h
	${SRC_DIR}/cstrike/dlls/weapontype.h
	${SRC_DIR}/game_shared/bot/bot_profile.cpp
	${SRC_DIR}/game_shared/bot/bot_profile.h
	${SRC_DIR}/game_shared/shared_util.cpp
	${SRC_DIR}/game_shared/shared_util.h
	${SRC_DIR}/game_shared/simple_checksum.h
	${SRC_DIR}/public/steam/friends/IFriendsUser.h
	${SRC_DIR}/public/igameuifuncs.h
	${SRC_DIR}/public/interface.cpp
	${SRC_DIR}/public/interface.h
	${SRC_DIR}/public/savegame_version.h
	BackgroundMenuButton.cpp
	BackgroundMenuButton.h
	BasePanel.cpp
	BasePanel.h
	BitmapButton.cpp
	BitmapButton.h
	BitmapImagePanel.cpp
	BitmapImagePanel.h
	CareerBaseFrame.cpp
	CareerBaseFrame.h
	CareerBox.cpp
	CareerBox.h
	CareerGame.cpp
	CareerGame.h
	CareerGameTasks.cpp
	CareerGameTasks.h
	CareerMapFrame.cpp
	CareerMapFrame.h
	CareerProfile.cpp
	CareerProfile.h
	CareerProfileFrame.cpp
	CareerProfileFrame.h
	CDKeyEntryDialog.cpp
	CDKeyEntryDialog.h
	CommandCheckButton.cpp
	CommandCheckButton.h
	ContentControlDialog.cpp
	ContentControlDialog.h
	CreateMultiplayerGameBotPage.cpp
	CreateMultiplayerGameBotPage.h
	CreateMultiplayerGameDialog.cpp
	CreateMultiplayerGameDialog.h
	CreateMultiplayerGameGameplayPage.cpp
	CreateMultiplayerGameGameplayPage.h
	CreateMultiplayerGameServerPage.cpp
	CreateMultiplayerGameServerPage.h
	CvarLatch.cpp
	CvarLatch.h
	CvarNegateCheckButton.cpp
	CvarNegateCheckButton.h
	CvarSlider.cpp
	CvarSlider.h
	CvarTextEntry.cpp
	CvarTextEntry.h
	CvarToggleCheckButton.cpp
	CvarToggleCheckButton.h
	DottedLabel.cpp
	DottedLabel.h
	EngineInterface.h
	GameConsole.cpp
	GameConsole.h
	GameConsoleDialog.cpp
	GameConsoleDialog.h
	GameUI_Interface.cpp
	GameUI_Interface.h
	HelpDialog.cpp
	HelpDialog.h
	ImageButton.cpp
	ImageButton.h
	KeyToggleCheckButton.cpp
	KeyToggleCheckButton.h
	LabeledCommandComboBox.cpp
	LabeledCommandComboBox.h
	labelslidercombo.cpp
	labelslidercombo.h
	LoadGameDialog.cpp
	LoadGameDialog.h
	LoadingDialog.cpp
	LoadingDialog.h
	LogoFile.cpp
	LogoFile.h
	ModInfo.cpp
	ModInfo.h
	MultiplayerAdvancedDialog.cpp
	MultiplayerAdvancedDialog.h
	MusicManager.cpp
	MusicManager.h
	NewGameDialog.cpp
	NewGameDialog.h
	OptionsDialog.cpp
	OptionsDialog.h
	OptionsSubAdvanced.cpp
	OptionsSubAdvanced.h
	OptionsSubAudio.cpp
	OptionsSubAudio.h
	OptionsSubKeyboard.cpp
	OptionsSubKeyboard.h
	OptionsSubMouse.cpp
	OptionsSubMouse.h
	OptionsSubMultiplayer.cpp
	OptionsSubMultiplayer.h
	OptionsSubVideo.cpp
	OptionsSubVideo.h
	OptionsSubVoice.cpp
	OptionsSubVoice.h
	PanelListPanel.cpp
	PanelListPanel.h
	PlayerListDialog.cpp
	PlayerListDialog.h
	RefreshSteamLogin.cpp
	RefreshSteamLogin.h
	RunGameEngine.cpp
	RunGameEngine.h
	SaveGameDialog.cpp
	SaveGameDialog.h
	ScriptObject.cpp
	ScriptObject.h
	Sys_Utils.cpp
	Sys_Utils.h
	TaskButton.cpp
	TaskButton.h
	Taskbar.cpp
	Taskbar.h
	taskframe.h
	TextEntryBox.cpp
	TextEntryBox.h
	vcontrolslistpanel.cpp
	vcontrolslistpanel.h
	VGuiSystemModuleLoader.cpp
	VGuiSystemModuleLoader.h
)

add_subdirectory( ${SRC_DIR}/common common )
add_subdirectory( ${SRC_DIR}/public public )

preprocess_sources()

add_library( ${PROJECT_NAME} SHARED ${PREP_SRCS} )

target_include_directories( ${PROJECT_NAME} PRIVATE
	${SRC_DIR}/engine
	${SRC_DIR}/pm_shared
	${SRC_DIR}/cl_dll
	${SRC_DIR}/game_shared/bot
	${SRC_DIR}/game_shared
	${SRC_DIR}/common
	${SRC_DIR}/public
	${SRC_DIR}/public/tier1
	${SRC_DIR}
	${EXTERNAL_DIR}/SDL2/include
)

#CS_SHIELD_ENABLED turns on code that deals with the shield weapon
target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${SHARED_DEFS}
	NO_MALLOC_OVERRIDE
	GAMEUI_EXPORTS
	CS_SHIELD_ENABLED
)

set( PROJECT_LIBS )

#Link with dependencies
target_link_libraries( ${PROJECT_NAME} 
	${SDL2}
	vgui2_controls
	vstdlib
	tier1
	tier0
	${PROJECT_LIBS}
	${SHARED_DEPENDENCIES}
)

#CMake places libraries in /Debug or /Release on Windows, so explicitly set the paths for both.
#On Linux, it uses LIBRARY_OUTPUT_DIRECTORY
set_target_properties( ${PROJECT_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${GAME_BASE_PATH}/cl_dlls"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${GAME_BASE_PATH}/cl_dlls"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GAME_BASE_PATH}/cl_dlls"
)

set( PROJECT_LINK_FLAGS )

set_target_properties( ${PROJECT_NAME} PROPERTIES
	COMPILE_FLAGS "${LINUX_32BIT_FLAG}"
	LINK_FLAGS "${SHARED_ENGINE_LINKER_FLAGS} ${PROJECT_LINK_FLAGS} ${LINUX_32BIT_FLAG}"
)
	
#No lib prefix
set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "" )

#Create filters
create_source_groups( "${CMAKE_SOURCE_DIR}" )

clear_sources()