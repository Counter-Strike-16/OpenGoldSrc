#ifndef GAMEUI_OPTIONSSUBKEYBOARD_H
#define GAMEUI_OPTIONSSUBKEYBOARD_H

#include <UtlSymbol.h>
#include <UtlVector.h>

#include <vgui_controls/PropertyPage.h>

class VControlsListPanel;

class COptionsSubKeyboard : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE( COptionsSubKeyboard, vgui2::PropertyPage );

private:
	//-----------------------------------------------------------------------------
	// Purpose: Used for saving engine keybindings in case user hits cancel button
	//-----------------------------------------------------------------------------
	struct KeyBinding
	{
		char* binding;
	};

public:
	COptionsSubKeyboard( vgui2::Panel* parent );
	virtual ~COptionsSubKeyboard();

private:
	virtual void Finish( int key, int button, const char* pchKeyName );

	/**
	*	User clicked on item: remember where last active row/column was
	*/
	MESSAGE_FUNC_INT( ItemSelected, "ItemSelected", itemID );

	void OnMouseDoublePressed( vgui2::MouseCode code ) override;

	void OnMousePressed( vgui2::MouseCode code ) override;

	void OnKeyTyped( wchar_t unichar ) override;

	void OnKeyCodeTyped( vgui2::KeyCode code ) override;

	void RemoveKeyFromBindItems( const char* key );

	void AddBinding( KeyValues* item, const char* keyname, const char* pchKeyName );

	void OnKeyCodePressed( vgui2::KeyCode code ) override;

	void OnMouseWheeled( int delta ) override;

	void CreateKeyBindingList();

	void ParseActionDescriptions();

	KeyValues* GetItemForBinding( const char* binding );

	const char* GetKeyName( int keynum );

	int FindKeyForName( const char* keyname );

	void ClearBindItems();

	void DeleteSavedBindings();

	void SaveCurrentBindings();

	void BindKey( const char* key, const char* binding );

	void UnbindKey( const char* key );

	void ApplyAllBindings();

	void OnApplyChanges() override;

	void FillInDefaultBindings();

	void OnCommand( const char* command ) override;

	void FillInCurrentBindings();

	void OnResetData();

private:
	VControlsListPanel* m_pKeyBindList;

	vgui2::Button* m_pSetBindingButton;
	vgui2::Button* m_pClearBindingButton;

	//TODO: define this constant somewhere - Solokiller
	KeyBinding m_Bindings[ 256 ];

	CUtlVector<CUtlSymbol> m_KeysToUnbind;

private:
	COptionsSubKeyboard( const COptionsSubKeyboard& ) = delete;
	COptionsSubKeyboard& operator=( const COptionsSubKeyboard& ) = delete;
};

#endif //GAMEUI_OPTIONSSUBKEYBOARD_H
