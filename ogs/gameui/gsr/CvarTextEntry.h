#ifndef GAMEUI_CVARTEXTENTRY_H
#define GAMEUI_CVARTEXTENTRY_H

#include <vgui_controls/TextEntry.h>

class CCvarTextEntry : public vgui2::TextEntry
{
public:
	static const int MAX_VALUE_SIZE = 64;

public:
	DECLARE_CLASS_SIMPLE( CCvarTextEntry, vgui2::TextEntry );

public:
	CCvarTextEntry::CCvarTextEntry( vgui2::Panel* parent, const char* panelName, const char* cvarname );
	~CCvarTextEntry();

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	MESSAGE_FUNC( OnTextChanged, "TextChanged" );

	void ApplyChanges( bool immediate );

	void Reset();

	bool HasBeenModified();

private:
	char* m_pszCvarName;
	char m_pszStartValue[ MAX_VALUE_SIZE ];

private:
	CCvarTextEntry( const CCvarTextEntry& ) = delete;
	CCvarTextEntry& operator=( const CCvarTextEntry& ) = delete;
};


#endif //GAMEUI_CVARTEXTENTRY_H
