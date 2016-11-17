/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"


#ifdef HOOK_ENGINE
int (*pCheckIP)(netadr_t adr);
#endif //HOOK_ENGINE

class CIPRateLimit rateChecker;


/* <e7175> ../engine/ipratelimitWrapper.cpp:6 */
int CheckIP(netadr_t adr)
{
#ifdef HOOK_ENGINE
	int res = pCheckIP(adr);
	if (res != 1)
	{
		rehlds_syserror("CheckIP() is expected to return 1");
	}
	return res;
#else
	CRehldsPlatformHolder::get()->time(NULL); //time() is called inside IpRateLimiter
	return 1;
#endif
}
