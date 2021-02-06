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

class CfgSoundShaders
{
	class GravityGun_SoundShader_Base
	{
		volume = 1;
		frequency = 1;
		limitation = 0;
		radius = 25;
		range = 500;
	};

	class GravityGun_Pickup_SoundShader: GravityGun_SoundShader_Base
	{
		samples[] = {{"GravityGun\GravityGun\sounds\physcannon_pickup", 1}};
	};
	
	class GravityGun_Hold_SoundShader: GravityGun_SoundShader_Base
	{
		samples[] = {{"GravityGun\GravityGun\sounds\superphys_hold_loop", 1}};
	};
	
	class GravityGun_Launch_SoundShader: GravityGun_SoundShader_Base
	{
		samples[] = {{"GravityGun\GravityGun\sounds\superphys_launch1", 1}};
	};
};

class CfgSoundSets
{
	class GravityGun_Soundset
	{
		sound3DProcessingType = "character3DProcessingType";
		volumeCurve = "characterAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
        distanceFilter = "defaultDistanceFilter";
	};

	class GravityGun_Pickup: GravityGun_Soundset
	{
		soundShaders[] = {"GravityGun_Pickup_SoundShader"};
	};
	
	class GravityGun_Hold: GravityGun_Soundset
	{
		soundShaders[] = {"GravityGun_Hold_SoundShader"};
	};
	
	class GravityGun_Launch: GravityGun_Soundset
	{
		soundShaders[] = {"GravityGun_Launch_SoundShader"};
	};

};