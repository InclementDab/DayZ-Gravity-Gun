class CfgPatches 
{
	class gravity_gun 
	{
		units[] = {"GravityGun"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[]={"DZ_Data", "DZ_Scripts","DZ_Weapons_Melee"};
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
		model = "\GravityGun\GravityGun\GravityGun.p3d";
		weight = 3500;
		rotationFlags = 1;
		itemBehaviour = 1;
		fragility = 0.008;
		attachments[] = {"RailgunBattery"};
		itemSize[] = {5,2};
		repairableWithKits[] = {7};
		repairCosts[] = {25.0};
		hiddenSelections[] = {"flame"};
		hiddenSelectionsTextures[] = {""};
		inventorySlot[] = {"Shoulder","Melee"};
		energyResources[] = {{"power",0.1}};

		class AnimationSources
		{
			class top_claw
			{
				source = "user";
				initPhase = 0;
				animPeriod = 0.01;
			};
			class left_claw
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 0;
			};
			class right_claw
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 0;
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.0,{"GravityGun\GravityGun\data\gravity_gun_emit.rvmat"}},{0.7,{"GravityGun\GravityGun\data\gravity_gun_emit.rvmat"}},{0.5,{"GravityGun\GravityGun\data\gravity_gun_damage.rvmat"}},{0.3,{"GravityGun\GravityGun\data\gravity_gun_damage.rvmat"}},{0.0,{"GravityGun\GravityGun\data\gravity_gun_destruct.rvmat"}}};
				};
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			plugType = 1;
			energyUsagePerSecond = 1.5;
			attachmentAction = 1;
			wetnessExposure = 0.1;
		};
		isMeleeWeapon = 1;
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeLightBlunt";
				range = 1.6;
			};
			class Heavy
			{
				ammo = "MeleeLightBlunt_Heavy";
				range = 1.6;
			};
			class Sprint
			{
				ammo = "MeleeLightBlunt_Heavy";
				range = 2.2;
			};
			class Default_SwitchedOn
			{
				ammo = "MeleeLightBluntShock";
				range = 1.6;
			};
			class Heavy_SwitchedOn
			{
				ammo = "MeleeLightBluntShock_Heavy";
				range = 1.6;
			};
			class Sprint_SwitchedOn
			{
				ammo = "MeleeLightBluntShock_Heavy";
				range = 2.2;
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