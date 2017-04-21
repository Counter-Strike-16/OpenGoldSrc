#pragma once

/*
class CKeyboard
{
public:
	void Clear();
	
	bool IsKeyDown(int anKey);
	bool IsAnyKeyDown();
	
	keyNum_t StringToKeyNum( const char * str ); // This is used by the "bind" command
	const char *KeyNumToString( keyNum_t keyNum ); // This is the inverse of StringToKeyNum, used for config files
private:
	
};
*/

// 
struct keyBindings_t
{
	idStr keyboard;
	idStr mouse;
	idStr gamepad;
};

class idSerializer;

// Converts from a USB HID code to a K_ code
int Key_ConvertHIDCode( int hid );

// This class isn't oriented on use of input devices
// It checks for any pressed key of any input device (mouse/keyboard/joystick)
// Should be replaced with input devices if it's possible
class CKeyHandler
{
public:
	static void				Init();
	static void				Shutdown();

	static void				ArgCompletion_KeyName( const idCmdArgs &args, void(*callback)( const char *s ) );
	static void				PreliminaryKeyEvent( int keyNum, bool down );
	
	static int				GetUsercmdAction( int keyNum );
	static bool				GetOverstrikeMode();
	static void				SetOverstrikeMode( bool state );
	static void				ClearStates();
	
	static bool IsDown(int anKey);
	static bool IsAnyKeyDown();
	
	static keyNum_t			StringToKeyNum( const char * str );		// This is used by the "bind" command
	static const char *		KeyNumToString( keyNum_t keyNum );		// This is the inverse of StringToKeyNum, used for config files
	static const char *		LocalizedKeyName( keyNum_t keyNum );	// This returns text suitable to print on screen

	static void				SetBinding( int keyNum, const char *binding );
	static const char *		GetBinding( int keyNum );
	static bool				UnbindBinding( const char *bind );
	
	static int				NumBinds( const char *binding );
	
	static bool				ExecKeyBinding( int keyNum );
	
	static const char *		KeysFromBinding( const char *bind );
	static const char *		BindingFromKey( const char *key );
	
	static bool				KeyIsBoundTo( int keyNum, const char *binding );
	static void				WriteBindings( idFile *f );
	static keyBindings_t	KeyBindingsFromBinding( const char * bind, bool firstOnly = false, bool localized = false );
};