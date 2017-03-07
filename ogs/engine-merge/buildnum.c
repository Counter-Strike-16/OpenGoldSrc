
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdio.h>

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

char *pdate = __DATE__;
char *ptime = __TIME__;

char *mon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
char mond[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

char *build_number(void)
{
	int m = 0;
	int d = 0;
	int y = 0;
	static char result[32];
	static int b = 0;

	if (b)
		return result;

	for (m = 0; m < 11; m++)
	{
		if (!strnicmp(pdate, mon[m], 3))
			break;

		d += mond[m];
	}

	d += atoi(&pdate[4]) - 4;
	y = atoi(&pdate[7]) - 1900;
	b = d + (int)((y - 1) * 365.25);

	if (!(y % 4) && m > 1)
		b++;

	b -= 40966;
	sprintf(result, "%d", b);
	return result;
}