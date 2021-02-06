
class CfgPatches
{
	class GravityGun_Scripts
	{
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Scripts"};
	};
};

class CfgMods 
{
	class DZ_GravityGun
	{
		name = "Mod Template";
		dir = "GravityGun";
		credits = "";
		author = "";
		creditsJson = "GravityGun/Scripts/Data/Credits.json";
		versionPath = "GravityGun/scripts/Data/Version.hpp";
		inputs = "GravityGun/Scripts/Data/Inputs.xml";
		type = "mod";
		dependencies[] =
		{
			"Game", "World", "Mission"
		};
		class defs
		{
			class imageSets
			{
				files[]=
				{
					"GravityGun/gui/imagesets/dayz_editor_gui.imageset"
				};
			};
			class engineScriptModule
			{
				value = "";
				files[] =
				{
					"GravityGun/scripts/common",
					"GravityGun/scripts/1_core"
				};
			};

			class gameScriptModule
			{
				value="";
				files[] = 
				{
					"GravityGun/scripts/common",
					"GravityGun/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[] = 
				{
					"GravityGun/scripts/common",
					"GravityGun/scripts/4_World"
				};
			};

			class missionScriptModule 
			{
				value="";
				files[] = 
				{
					"GravityGun/scripts/common",
					"GravityGun/scripts/5_Mission"
				};
			};
		};
	};
};
