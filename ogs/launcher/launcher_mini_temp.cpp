int AppMain();

#ifdef WIN32
	int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		return AppMain();
	};
#else
	int main(int argc, char **argv)
	{
		return AppMain();
	};
#endif

int AppMain()
{
	return EXIT_SUCCESS;
};