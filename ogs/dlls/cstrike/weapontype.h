#ifndef CSTRIKE_DLLS_WEAPONTYPE_H
#define CSTRIKE_DLLS_WEAPONTYPE_H

enum CSTeam
{
	CS_TEAM_T = 1,
	CS_TEAM_CT = 2,
};

enum WeaponIdType
{
	WEAPON_NONE = 0,
	WEAPON_P228,
	WEAPON_GLOCK,
	WEAPON_SCOUT,
	WEAPON_HEGRENADE,
	WEAPON_XM1014,
	WEAPON_C4,
	WEAPON_MAC10,
	WEAPON_AUG,
	WEAPON_SMOKEGRENADE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_UMP45,
	WEAPON_SG550,
	WEAPON_GALIL,
	WEAPON_FAMAS,
	WEAPON_USP,
	WEAPON_GLOCK18,
	WEAPON_AWP,
	WEAPON_MP5N,
	WEAPON_M249,
	WEAPON_M3,
	WEAPON_M4A1,
	WEAPON_TMP,
	WEAPON_G3SG1,
	WEAPON_FLASHBANG,
	WEAPON_DEAGLE,
	WEAPON_SG552,
	WEAPON_AK47,
	WEAPON_KNIFE,
	WEAPON_P90,
	WEAPON_SHIELDGUN = 99
};

enum WeaponClassType
{
	WEAPONCLASS_NONE = 0,
	WEAPONCLASS_KNIFE,
	WEAPONCLASS_PISTOL,
	WEAPONCLASS_GRENADE,
	WEAPONCLASS_SUBMACHINEGUN,
	WEAPONCLASS_SHOTGUN,
	WEAPONCLASS_MACHINEGUN,
	WEAPONCLASS_RIFLE,
	WEAPONCLASS_SNIPERRIFLE,
	WEAPONCLASS_MAX
};

struct WeaponAliasInfo
{
	const char* alias;
	WeaponIdType id;
};

struct WeaponClassAliasInfo
{
	const char* alias;
	WeaponClassType id;
};

struct WeaponBuyAliasInfo
{
	const char* alias;
	WeaponIdType id;
	const char* failName;
};

struct WeaponInfoStruct
{
	int id;

	int cost;
	int clipCost;
	int buyClipSize;
	int gunClipSize;
	int maxRounds;
	int ammoType;

	const char* entityName;
};

const char* WeaponIDToAlias( int id );

WeaponIdType AliasToWeaponID( const char* alias );

WeaponClassType AliasToWeaponClass( const char* alias );

WeaponClassType WeaponIDToWeaponClass( int id );

const char* BuyAliasToWeaponID( const char* alias, WeaponIdType& id );

bool CanBuyWeaponByMaptype( int playerTeam, WeaponIdType weaponID, bool useAssasinationRestrictions );

bool IsPrimaryWeapon( int id );
bool IsSecondaryWeapon( int id );

#endif //CSTRIKE_DLLS_WEAPONTYPE_H