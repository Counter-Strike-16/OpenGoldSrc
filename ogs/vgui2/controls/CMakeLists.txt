
#
#	VGUI2 Controls CMake build file
#

set( PROJECT_NAME vgui2_controls )

add_sources(
	${CMAKE_CURRENT_SOURCE_DIR}/../src/vgui_key_translation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/vgui_key_translation.h
	AnimatingImagePanel.cpp
	AnimationController.cpp
	BitmapImagePanel.cpp
	BuildFactoryHelper.cpp
	BuildGroup.cpp
	BuildModeDialog.cpp
	Button.cpp
	CheckButton.cpp
	CheckButtonList.cpp
	ComboBox.cpp
	Controls.cpp
	DirectorySelectDialog.cpp
	Divider.cpp
	EditablePanel.cpp
	ExpandButton.cpp
	FileOpenDialog.cpp
	FocusNavGroup.cpp
	Frame.cpp
	GraphPanel.cpp
	HTML.cpp
	Image.cpp
	ImageList.cpp
	ImagePanel.cpp
	InputDialog.cpp
	KeyBindingHelpDialog.cpp
	KeyBoardEditorDialog.cpp
	Label.cpp
	ListPanel.cpp
	ListViewPanel.cpp
	MenuBar.cpp
	MenuButton.cpp
	Menu.cpp
	MenuItem.cpp
	MessageBox.cpp
	Panel.cpp
	PanelListPanel.cpp
	ProgressBar.cpp
	ProgressBox.cpp
	PropertyDialog.cpp
	PropertyPage.cpp
	PropertySheet.cpp
	QueryBox.cpp
	RadioButton.cpp
	RichText.cpp
	ScrollBar.cpp
	ScrollBarSlider.cpp
	SectionedListPanel.cpp
	Slider.cpp
	Splitter.cpp
	TextEntry.cpp
	TextImage.cpp
	ToggleButton.cpp
	Tooltip.cpp
	ToolWindow.cpp
	TreeView.cpp
	TreeViewListControl.cpp
	URLLabel.cpp
	WizardPanel.cpp
	WizardSubPanel.cpp
)

add_subdirectory( ${SRC_DIR}/common common )
add_subdirectory( ${SRC_DIR}/public public )

preprocess_sources()

add_library( ${PROJECT_NAME} STATIC ${PREP_SRCS} )

target_include_directories( ${PROJECT_NAME} PRIVATE
	${SRC_DIR}/common
	${SRC_DIR}/public
	${SRC_DIR}/public/html
	${SRC_DIR}/public/tier1
	${SRC_DIR}
	${EXTERNAL_DIR}/SDL2/include
)

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${SHARED_DEFS}
	NO_MALLOC_OVERRIDE
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