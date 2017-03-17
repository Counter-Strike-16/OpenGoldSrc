

//
// the net drivers should just set the apropriate bits in m_activenet,
// instead of having the menu code look through their internal tables
//
#define	MNET_IPX		1
#define	MNET_TCP		2

extern	int	m_activenet;

//
// menus
//
void M_Init ();
void M_Keydown (int key);
void M_Draw ();
void M_ToggleMenu_f ();
qpic_t	*M_CachePic (char *path);
void M_DrawTextBox (int x, int y, int width, int lines);
void M_Menu_Quit_f ();