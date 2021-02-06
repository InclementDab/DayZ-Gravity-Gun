class CfgPatches 
{
	class gravity_gun 
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[]={"DZ_Data", "DZ_Scripts"};
		author = "InclementDab";
		name = "GravityGun Core";
	};
};

class CfgVehicles
{
	class Weapon_Base;
	class GravityGun: Weapon_Base
	{
		scope=2;
		displayName = "Gravity Gun";
		descriptionShort = "A gun that harnesses Gravity to assist the user in moving heavy objects safely, without harm to the.... ah who am I kidding, go ahead, send your friend flying... you know you want to";
		model = "\dz\weapons\pistols\LongHorn\LongHorn.p3d";
		attachments[] = {};
		itemSize[] = {5,2};
	};
};