class CfgPatches 
{
	class gravity_gun 
	{
		units[] = {"GravityGun"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[]={"DZ_Data", "DZ_Scripts"};
		author = "InclementDab";
		name = "GravityGun Core";
	};
};

class CfgVehicles
{
	class Inventory_Base;
	class GravityGun: Inventory_Base
	{
		scope=2;
		displayName = "Gravity Gun";
		descriptionShort = "A gun that harnesses Gravity to assist the user in moving heavy objects safely, without harm to the.... ah who am I kidding, go ahead, send your friend flying... you know you want to";
		model = "\dz\weapons\pistols\LongHorn\LongHorn.p3d";
		attachments[] = {};
		itemSize[] = {5,2};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"DZ\weapons\pistols\LongHorn\data\LongHorn.rvmat"}},{0.7,{"DZ\weapons\pistols\LongHorn\data\LongHorn.rvmat"}},{0.5,{"DZ\weapons\pistols\LongHorn\data\LongHorn_damage.rvmat"}},{0.3,{"DZ\weapons\pistols\LongHorn\data\LongHorn_damage.rvmat"}},{0.0,{"DZ\weapons\pistols\LongHorn\data\LongHorn_destruct.rvmat"}}};
				};
			};
		};
	};
};