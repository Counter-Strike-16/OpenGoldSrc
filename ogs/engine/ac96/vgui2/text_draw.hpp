#pragma once

void VGUI2_Draw_Init();

void VGUI2_ResetCurrentTexture();

unsigned int VGUI2_GetConsoleFont();
unsigned int VGUI2_GetCreditsFont();

void VGUI2_Draw_SetTextColor( int r, int g, int b );
void VGUI2_Draw_SetTextColorAlpha( int r, int g, int b, int a );

const wchar_t* VGUI2_Find_String( const char* str );

int VGUI2_GetFontWide( wchar_t ch, unsigned int font );
int VGUI2_GetFontTall( unsigned int font );

int VGUI2_Draw_StringLen( const char* psz, unsigned int font );
int VGUI2_Draw_StringLenW( const wchar_t* wsz, unsigned int font );

int VGUI2_DrawString( int x, int y, const char* str, unsigned int font );

int VGUI2_DrawStringClient( int x, int y, const char* str, int r, int g, int b );

int VGUI2_DrawStringReverseClient( int x, int y, const char* str, int r, int g, int b );

int VGUI2_Draw_Character( int x, int y, int ch, unsigned int font );

int VGUI2_Draw_CharacterAdd( int x, int y, int ch, int r, int g, int b, unsigned int font );

/**
*	Fills the given array with character widths, and returns the height of the font
*	@param pWidth[ inout ] Pointer to an array that is at least 256 elements large
*	@param font Font to calculate the sizes of
*	@return Height of the font
*/
int VGUI2_MessageFontInfo( short* pWidth, unsigned int font );