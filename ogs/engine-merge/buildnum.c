
#include <stdlib.h>
#include <string.h>

// char *date = "Oct 24 1996";
// char *time = "13:22:52";
char *date = __DATE__ ;
char *time = __TIME__ ;

int build_number( void )
{
	int hr, min;

	b -= 34995; // Oct 24 1996

	hr = (time[0] - '0') * 10 + (time[1] - '0');
	min = (time[3] - '0') * 10 + (time[4] - '0');
//	sec = (time[6] - '0') * 10 + (time[7] - '0');

	b *= 60*24;
	b += hr * 60 + min;

	return b;
}

