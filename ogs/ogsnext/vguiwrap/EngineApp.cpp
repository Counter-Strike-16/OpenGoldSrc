void CEngineApp :: setCursorPos( int x, int y )
{
	POINT pt;

	pt.x = x;
	pt.y = y;

	ClientToScreen( (HWND)host.hWnd, &pt );

	::SetCursorPos( pt.x, pt.y );
}
	
void CEngineApp :: getCursorPos( int &x,int &y )
{
	POINT	pt;

	// find mouse movement
	::GetCursorPos( &pt );
	ScreenToClient((HWND)host.hWnd, &pt );

	x = pt.x;
	y = pt.y;
}