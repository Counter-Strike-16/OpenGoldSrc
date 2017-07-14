#include <tier0/platform.h>

#include "weapontype.h"

const WeaponAliasInfo weaponAliasInfo[] = 
{
	{ "p228", WEAPON_P228 },
	{ "???", WEAPON_GLOCK },
	{ "scout", WEAPON_SCOUT },
	{ "hegren", WEAPON_HEGRENADE },
	{ "xm1014", WEAPON_XM1014 },
	{ "c4", WEAPON_C4 },
	{ "mac10", WEAPON_MAC10 },
	{ "aug", WEAPON_AUG },
	{ "sgren", WEAPON_SMOKEGRENADE },
	{ "elites", WEAPON_ELITE },
	{ "fn57", WEAPON_FIVESEVEN },
	{ "ump45", WEAPON_UMP45 },
	{ "sg550", WEAPON_SG550 },
	{ "galil", WEAPON_GALIL },
	{ "famas", WEAPON_FAMAS },
	{ "usp", WEAPON_USP },
	{ "glock", WEAPON_GLOCK18 },
	{ "awp", WEAPON_AWP },
	{ "mp5", WEAPON_MP5N },
	{ "m249", WEAPON_M249 },
	{ "m3", WEAPON_M3 },
	{ "m4a1", WEAPON_M4A1 },
	{ "tmp", WEAPON_TMP },
	{ "g3sg1", WEAPON_G3SG1 },
	{ "flash", WEAPON_FLASHBANG },
	{ "deagle", WEAPON_DEAGLE },
	{ "sg552", WEAPON_SG552 },
	{ "ak47", WEAPON_AK47 },
	{ "knife", WEAPON_KNIFE },
	{ "p90", WEAPON_P90 },
	{ "shield", WEAPON_SHIELDGUN },
	{ "none", WEAPON_NONE },
	{ "grenade", WEAPON_HEGRENADE },
	{ "hegrenade", WEAPON_HEGRENADE },
	{ "glock18", WEAPON_GLOCK18 },
	{ "elite", WEAPON_ELITE },
	{ "fiveseven", WEAPON_FIVESEVEN },
	{ "mp5navy", WEAPON_MP5N },
	{ nullptr, WEAPON_NONE }
};

const WeaponClassAliasInfo weaponClassAliasInfo[] = 
{
	{ "p228", WEAPONCLASS_PISTOL },
	{ "???", WEAPONCLASS_PISTOL },
	{ "scout", WEAPONCLASS_SNIPERRIFLE },
	{ "hegren", WEAPONCLASS_GRENADE },
	{ "xm1014", WEAPONCLASS_SHOTGUN },
	{ "c4", WEAPONCLASS_GRENADE },
	{ "mac10", WEAPONCLASS_SUBMACHINEGUN },
	{ "aug", WEAPONCLASS_RIFLE },
	{ "sgren", WEAPONCLASS_GRENADE },
	{ "elites", WEAPONCLASS_PISTOL },
	{ "fn57", WEAPONCLASS_PISTOL },
	{ "ump45", WEAPONCLASS_SUBMACHINEGUN },
	{ "sg550", WEAPONCLASS_SNIPERRIFLE },
	{ "galil", WEAPONCLASS_RIFLE },
	{ "famas", WEAPONCLASS_RIFLE },
	{ "usp", WEAPONCLASS_PISTOL },
	{ "glock", WEAPONCLASS_PISTOL },
	{ "awp", WEAPONCLASS_SNIPERRIFLE },
	{ "mp5", WEAPONCLASS_SUBMACHINEGUN },
	{ "m249", WEAPONCLASS_MACHINEGUN },
	{ "m3", WEAPONCLASS_SHOTGUN },
	{ "m4a1", WEAPONCLASS_RIFLE },
	{ "tmp", WEAPONCLASS_SUBMACHINEGUN },
	{ "g3sg1", WEAPONCLASS_SNIPERRIFLE },
	{ "flash", WEAPONCLASS_GRENADE },
	{ "deagle", WEAPONCLASS_PISTOL },
	{ "sg552", WEAPONCLASS_RIFLE },
	{ "ak47", WEAPONCLASS_RIFLE },
	{ "knife", WEAPONCLASS_KNIFE },
	{ "p90", WEAPONCLASS_SUBMACHINEGUN },
	{ "shield", WEAPONCLASS_PISTOL },
	{ "grenade", WEAPONCLASS_GRENADE },
	{ "hegrenade", WEAPONCLASS_GRENADE },
	{ "glock18", WEAPONCLASS_PISTOL },
	{ "elite", WEAPONCLASS_PISTOL },
	{ "fiveseven", WEAPONCLASS_PISTOL },
	{ "mp5navy", WEAPONCLASS_SUBMACHINEGUN },
	{ "grenade", WEAPONCLASS_GRENADE },	//TODO: listed twice - Solokiller
	{ "pistol", WEAPONCLASS_PISTOL },
	{ "smg", WEAPONCLASS_SUBMACHINEGUN },
	{ "machinegun", WEAPONCLASS_MACHINEGUN },
	{ "shotgun", WEAPONCLASS_SHOTGUN },
	{ "rifle", WEAPONCLASS_RIFLE },
	{ "sniper", WEAPONCLASS_SNIPERRIFLE },
	{ "none", WEAPONCLASS_NONE },
	{ nullptr, WEAPONCLASS_NONE }
};

const WeaponBuyAliasInfo weaponBuyAliasInfo[] = 
{
	{ "galil", WEAPON_GALIL, "#Galil" },
	{ "defender", WEAPON_GALIL, "#Galil" },
	{ "ak47", WEAPON_AK47, "#Ak47" },
	{ "cv47", WEAPON_AK47, "#Ak47" },
	{ "scout", WEAPON_SCOUT, 0 },
	{ "sg552", WEAPON_SG552, "#SG552" },
	{ "krieg552", WEAPON_SG552, "#SG552" },
	{ "awp", WEAPON_AWP, nullptr },
	{ "magnum", WEAPON_AWP, nullptr },
	{ "g3sg1", WEAPON_G3SG1, "#G3SG1" },
	{ "d3au1", WEAPON_G3SG1, "#G3SG1" },
	{ "famas", WEAPON_FAMAS, "#Famas" },
	{ "clarion", WEAPON_FAMAS, "#Famas" },
	{ "m4a1", WEAPON_M4A1, "#M4A1" },
	{ "aug", WEAPON_AUG, "#Aug" },
	{ "bullpup", WEAPON_AUG, "#Aug" },
	{ "sg550", WEAPON_SG550, "#SG550" },
	{ "krieg550", WEAPON_SG550, "#SG550" },
	{ "glock", WEAPON_GLOCK18, nullptr },
	{ "9x19mm", WEAPON_GLOCK18, nullptr },
	{ "usp", WEAPON_USP, nullptr },
	{ "km45", WEAPON_USP, nullptr },
	{ "p228", WEAPON_P228, nullptr },
	{ "228compact", WEAPON_P228, nullptr },
	{ "deagle", WEAPON_DEAGLE, nullptr },
	{ "nighthawk", WEAPON_DEAGLE, nullptr },
	{ "elites", WEAPON_ELITE, "#Beretta96G" },
	{ "fn57", WEAPON_FIVESEVEN, "#FiveSeven" },
	{ "fiveseven", WEAPON_FIVESEVEN,  "#FiveSeven" },
	{ "m3", WEAPON_M3, nullptr },
	{ "12gauge", WEAPON_M3, nullptr },
	{ "xm1014", WEAPON_XM1014, nullptr },
	{ "autoshotgun", WEAPON_XM1014, nullptr },
	{ "mac10", WEAPON_MAC10, "#Mac10" },
	{ "tmp", WEAPON_TMP, "#tmp" },
	{ "mp", WEAPON_TMP, "#tmp" },
	{ "mp5", WEAPON_MP5N, nullptr },
	{ "smg", WEAPON_MP5N, nullptr },
	{ "ump45", WEAPON_UMP45, nullptr },
	{ "p90", WEAPON_P90, nullptr },
	{ "c90", WEAPON_P90, nullptr },
	{ "m249", WEAPON_M249, nullptr },
	{ nullptr, WEAPON_NONE, nullptr }
};

WeaponInfoStruct weaponInfo[ 27 ] = 
{
	{ WEAPON_P228, 600, 50, 13, 13, 52, 9, "weapon_p228" },
	{ WEAPON_GLOCK, 400,  20, 30,  20, 120, 1, "weapon_glock18" },
	{ WEAPON_GLOCK18, 400,  20, 30,  20, 120, 1, "weapon_glock18" },
	{ WEAPON_SCOUT, 2750, 80, 30, 10, 90, 4, "weapon_scout" },
	{ WEAPON_XM1014, 3000, 65, 8, 7, 32, 0, "weapon_xm1014" },
	{ WEAPON_MAC10, 5120, 25, 12, 30, 100, 5, "weapon_mac10" },
	{ WEAPON_AUG, 3500, 60, 30, 30, 90, 2, "weapon_aug" },
	{ WEAPON_ELITE, 332,  20, 30, 30, 120, 1, "weapon_elite" },
	{ WEAPON_FIVESEVEN, 750, 50, 50,  20, 100, 8, "weapon_fiveseven" },
	{ WEAPON_UMP45, 1700, 25, 12, 25, 100, 5, "weapon_ump45" },
	{ WEAPON_SG550, 4200, 60, 30, 30, 90, 2, "weapon_sg550" },
	{ WEAPON_GALIL, 2000, 60, 30, 35, 90, 2, "weapon_galil" },
	{ WEAPON_FAMAS, 2250, 60, 30, 25, 90, 2, "weapon_famas" },
	{ WEAPON_USP, 500, 25, 12, 12, 100, 5, "weapon_usp" },
	{ WEAPON_AWP, 4750, 125, 10, 10, 30, 7, "weapon_awp" },
	{ WEAPON_MP5N, 1500, 20, 30, 30, 120, 1, "weapon_mp5navy" },
	{ WEAPON_M249, 5750, 60, 30, 100, 200, 3, "weapon_m249" },
	{ WEAPON_M3, 1700, 65, 8, 8, 32, 0, "weapon_m3" },
	{ WEAPON_M4A1, 3100, 60, 30, 30, 90, 2, "weapon_m4a1" },
	{ WEAPON_TMP, 1250,  20, 30, 30, 120, 1, "weapon_tmp" },
	{ WEAPON_G3SG1, 5000, 80, 30,  20, 90, 4, "weapon_g3sg1" },
	{ WEAPON_DEAGLE, 650, 40, 7, 7, 35, 6, "weapon_deagle" },
	{ WEAPON_SG552, 3500, 60, 30, 30, 90, 2, "weapon_sg552" },
	{ WEAPON_AK47, 2500, 80, 30, 30, 90, 4, "weapon_ak47" },
	{ WEAPON_P90, 2350, 50, 50, 50, 100, 8, "weapon_p90" },
	{ WEAPON_SHIELDGUN, 2200, 0, 0, 0, 0, -1, nullptr },
	{ WEAPON_NONE, 0, 0, 0, 0, 0, -1, 0 },
};

const char* WeaponIDToAlias( int id )
{
	for( const auto& info : weaponAliasInfo )
	{
		if( info.id == id )
		{
			return info.alias;
		}
	}

	return nullptr;
}

WeaponIdType AliasToWeaponID( const char* alias )
{
	if( alias )
	{
		for( const auto& info : weaponAliasInfo )
		{
			if( !info.alias )
				break;

			if( !stricmp( info.alias, alias ) )
				return info.id;
		}
	}

	return WEAPON_NONE;
}

WeaponClassType AliasToWeaponClass( const char* alias )
{
	if( alias )
	{
		for( const auto& info : weaponClassAliasInfo )
		{
			if( !info.alias )
				break;

			if( !stricmp( info.alias, alias ) )
				return info.id;
		}
	}

	return WEAPONCLASS_NONE;
}

WeaponClassType WeaponIDToWeaponClass( int id )
{
	for( int i = 0; weaponAliasInfo[ i ].alias; ++i )
	{
		if( weaponAliasInfo[ i ].id == id )
		{
			for( int j = 0; weaponClassAliasInfo[ j ].alias; ++j )
			{
				if( !stricmp( weaponAliasInfo[ i ].alias, weaponClassAliasInfo[ j ].alias ) )
					return weaponClassAliasInfo[ j ].id;
			}

			break;
		}
	}

	return WEAPONCLASS_NONE;
}

const char* BuyAliasToWeaponID( const char* alias, WeaponIdType& id )
{
	if( alias )
	{
		for( int i = 0; weaponBuyAliasInfo[ i ].alias; ++i )
		{
			if( !stricmp( weaponBuyAliasInfo[ i ].alias, alias ) )
			{
				id = weaponBuyAliasInfo[ i ].id;
				return weaponBuyAliasInfo[ i ].failName;
			}
		}
	}

	id = WEAPON_NONE;

	return nullptr;
}

bool CanBuyWeaponByMaptype( int playerTeam, WeaponIdType weaponID, bool useAssasinationRestrictions )
{
	if( !useAssasinationRestrictions )
	{
		if( playerTeam == CS_TEAM_CT )
		{
			switch( weaponID )
			{
			default:
				return false;

			case WEAPON_P228:
			case WEAPON_SCOUT:
			case WEAPON_XM1014:
			case WEAPON_AUG:
			case WEAPON_FIVESEVEN:
			case WEAPON_UMP45:
			case WEAPON_SG550:
			case WEAPON_FAMAS:
			case WEAPON_USP:
			case WEAPON_GLOCK18:
			case WEAPON_AWP:
			case WEAPON_MP5N:
			case WEAPON_M249:
			case WEAPON_M3:
			case WEAPON_M4A1:
			case WEAPON_TMP:
			case WEAPON_DEAGLE:
			case WEAPON_P90:
			case WEAPON_SHIELDGUN:
				return true;
			}
		}
		else if( playerTeam == CS_TEAM_T )
		{
			switch( weaponID )
			{
			default:
				return false;

			case WEAPON_P228:
			case WEAPON_SCOUT:
			case WEAPON_XM1014:
			case WEAPON_MAC10:
			case WEAPON_ELITE:
			case WEAPON_UMP45:
			case WEAPON_GALIL:
			case WEAPON_USP:
			case WEAPON_GLOCK18:
			case WEAPON_AWP:
			case WEAPON_MP5N:
			case WEAPON_M249:
			case WEAPON_M3:
			case WEAPON_G3SG1:
			case WEAPON_DEAGLE:
			case WEAPON_SG552:
			case WEAPON_AK47:
			case WEAPON_P90:
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		if( playerTeam == CS_TEAM_CT )
		{
			switch( weaponID )
			{
			default:
				return false;

			case WEAPON_P228:
			case WEAPON_XM1014:
			case WEAPON_AUG:
			case WEAPON_FIVESEVEN:
			case WEAPON_UMP45:
			case WEAPON_SG550:
			case WEAPON_FAMAS:
			case WEAPON_USP:
			case WEAPON_GLOCK18:
			case WEAPON_MP5N:
			case WEAPON_M249:
			case WEAPON_M3:
			case WEAPON_M4A1:
			case WEAPON_TMP:
			case WEAPON_DEAGLE:
			case WEAPON_P90:
			case WEAPON_SHIELDGUN:
				return true;
			}
		}
		else if( playerTeam == CS_TEAM_T )
		{
			switch( weaponID )
			{
			default:
				return false;

			case WEAPON_P228:
			case WEAPON_MAC10:
			case WEAPON_ELITE:
			case WEAPON_UMP45:
			case WEAPON_GALIL:
			case WEAPON_USP:
			case WEAPON_GLOCK18:
			case WEAPON_AWP:
			case WEAPON_DEAGLE:
			case WEAPON_AK47:
				return true;
			}
		}
		else
		{
			return false;
		}
	}
}

const WeaponInfoStruct* GetWeaponInfo( int weaponID )
{
	for( int i = 0; weaponInfo[ i ].id != WEAPON_NONE; ++i )
	{
		if( weaponInfo[ i ].id == weaponID )
			return &weaponInfo[ i ];
	}

	return nullptr;
}

bool IsPrimaryWeapon( int id )
{
	switch( id )
	{
	default:
		return false;

	case WEAPON_SCOUT:
	case WEAPON_XM1014:
	case WEAPON_MAC10:
	case WEAPON_AUG:
	case WEAPON_UMP45:
	case WEAPON_SG550:
	case WEAPON_GALIL:
	case WEAPON_FAMAS:
	case WEAPON_AWP:
	case WEAPON_MP5N:
	case WEAPON_M249:
	case WEAPON_M3:
	case WEAPON_M4A1:
	case WEAPON_TMP:
	case WEAPON_G3SG1:
	case WEAPON_SG552:
	case WEAPON_AK47:
	case WEAPON_P90:
	case WEAPON_SHIELDGUN:
		return true;
	}
}

bool IsSecondaryWeapon( int id )
{
	switch( id )
	{
	default:
		return false;

	case WEAPON_P228:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_USP:
	case WEAPON_GLOCK18:
	case WEAPON_DEAGLE:
		return true;
	}
}