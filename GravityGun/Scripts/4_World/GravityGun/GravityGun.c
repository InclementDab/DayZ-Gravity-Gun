
modded class PlayerBase
{
	vector GGAim;
	
	override bool HeadingModel(float pDt, SDayZPlayerHeadingModel pModel)
	{
		//Print(pModel.m_fHeadingAngle);
		
		return super.HeadingModel(pDt, pModel);
	}
	
	override bool AimingModel(float pDt, SDayZPlayerAimingModel pModel)
	{
		//Print(pModel.m_fCurrentAimX);
		//Print(pModel.m_fCurrentAimY);
		
		GGAim[0] = pModel.m_fCurrentAimX;
		GGAim[2] = pModel.m_fCurrentAimY;
		
		return super.AimingModel(pDt, pModel);
	}
}

class GravityGun: ItemBase
{
	static const float GUN_PICKUP_RANGE = 5;
	static const float GUN_HOLD_RANGE = 1;
	static const float GUN_POWER = 250;
	
	protected ref Timer m_UpdateTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
	protected Object m_ObjectUnderCrosshair;
	protected Object m_HoldingObject;

	protected ref OpenableBehaviour m_Openable; //ZeRoY
	
	protected ScriptedLightBase m_GunLight;
	
	void GravityGun()
	{
		if (IsMissionClient()) {
			m_GunLight = ScriptedLightBase.CreateLight(GravityGunLight, ModelToWorld(GetMemoryPointPos("ce_radius")));
			m_GunLight.AttachOnMemoryPoint(this, "ce_radius");
		}
		
		m_UpdateTimer.Run(0.02, this, "OnUpdateTimer", null, true);
		m_Openable = new OpenableBehaviour(false);  //ZeRoY
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened"); //ZeRoY
	}
	
	void OnUpdateTimer()
	{
		Debug.DestroyAllShapes();
		
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		
		if (!player) {
			return;
		}
		
		if (m_HoldingObject) {
			m_HoldingObject.SetPosition(GetHoldingPosition());
			m_HoldingObject.Update();
			return;
		}
		/*
		int bone = player.GetBoneIndexByName("Head");
		
		vector tm[4];
		player.GetBoneTransformWS(bone, tm);
		
		vector head_ori = player.GetOrientation().AnglesToVector();
		head_ori[1] = tm[1][1];
		
		vector dir = MiscGameplayFunctions.GetHeadingVector(player);		
		vector begin = tm[3] + (GetGame().GetCurrentCameraDirection() * 0.02);
		vector end = begin + (GetGame().GetCurrentCameraDirection() * GUN_PICKUP_RANGE);
		
		Debug.DrawLine(begin, end);
		
		vector contact_pos, contact_dir;
		int contact_component;
		set<Object> results = new set<Object>();
		
		if (!DayZPhysics.RaycastRV(begin, end, contact_pos, contact_dir, contact_component, results, null, GetHierarchyRootPlayer(), false, false, ObjIntersectView, 0.25)) {
			return;
		}
		
		if (GetGame().IsClient()) {
			Debug.DrawCube(contact_pos, 0.25);
		}
		
		m_ObjectUnderCrosshair = null;
		if (results.Count() > 0) {
			m_ObjectUnderCrosshair = results[0];
		}*/
	}
	
	void TryPickUpItem(Object object)
	{		
		if (!object) {
			Print("No item to pick up!");
			return;
		}
		
		Print("Trying to pick up " + object);
		
		if (m_HoldingObject) {
			Print("Already holding item");
			TryDropItem();
			return;
		}
		
		m_HoldingObject = object;
		if (dBodyIsDynamic(m_HoldingObject)) {
			m_HoldingObject.SetDynamicPhysicsLifeTime(0.01);
		}
		
		EffectSound sound;
		PlaySoundSet(sound, "GravityGun_Pickup", 0, 0);
		SetClawState(0);
	}
	
	void TryDropItem()
	{
		m_HoldingObject.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);
		m_HoldingObject.SetDynamicPhysicsLifeTime(15.0);
		m_HoldingObject = null;
		
		SetClawState(1);
	}
	
	void TryLaunchItem(Object target)
	{
		SetClawState(1);
		if (m_HoldingObject) {
			LaunchItem(m_HoldingObject);
			m_HoldingObject = null;
			return;
		}
				
		if (target) {
			LaunchItem(target);
		}
	}
	
	private void LaunchItem(notnull Object target)
	{
		target.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);
		target.SetDynamicPhysicsLifeTime(15.0);
		dBodyApplyImpulse(target, GetGame().GetCurrentCameraDirection() * GUN_POWER);
		
		EffectSound sound;
		PlaySoundSet(sound, "GravityGun_Launch", 0, 0);
		CreateCameraShake(1);
	}
	
	vector GetHoldingPosition()
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		
		if (!player) {
			return vector.Zero;
		}
		
		vector tm[4];
		player.GetBoneTransformWS(player.GetBoneIndexByName("Head"), tm);
		
		vector head_ori = player.GetOrientation().AnglesToVector();
		head_ori[1] = tm[1][1];
		
		vector begin = tm[3] + (GetGame().GetCurrentCameraDirection() * 0.02);
		vector end = begin + (GetGame().GetCurrentCameraDirection() * GUN_HOLD_RANGE);		
		vector contact_pos, contact_dir;
		
		Object hit_obj;
		float hit_fract;
		if (DayZPhysics.RayCastBullet(begin, end, PhxInteractionLayers.BUILDING | PhxInteractionLayers.DYNAMICITEM | PhxInteractionLayers.TERRAIN | PhxInteractionLayers.ROADWAY, m_HoldingObject, hit_obj, contact_pos, contact_dir, hit_fract)) {
			return contact_pos;
		}
		
		return end;
	}
	
	void CreateCameraShake(float intensity)
	{
		GetGame().GetPlayer().GetCurrentCamera().SpawnCameraShake(Math.Clamp(intensity, 0.2, 1), 2, 5, 10);
	}

	void SetClawState(float value)
	{
		SetAnimationPhase("gravitygun", value);
	}
	
	vector GetLookingDirection(DayZPlayerImplement player)
	{
		return player.GetInputController().GetAimChange();
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionGravityGunPickUpItem);
		AddAction(ActionGravityGunLaunchItem);
	}
}

class ActionGravityGunLaunchItem: ActionInteractBase
{
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override void OnStartClient(ActionData action_data)
	{
		GravityGun gravity_gun;
		if (Class.CastTo(gravity_gun, action_data.m_MainItem)) {
			gravity_gun.TryLaunchItem(action_data.m_Target.GetObject());
		}
	}
	
	override void OnStartServer(ActionData action_data)
	{
		GravityGun gravity_gun;
		if (Class.CastTo(gravity_gun, action_data.m_MainItem)) {
			gravity_gun.TryLaunchItem(action_data.m_Target.GetObject());
		}
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return true;
	}
		
	override string GetText()
	{
		return "Yeet";
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override typename GetInputType()
	{
		return WeaponManipulationActionInput;
	}
}

class ActionGravityGunPickUpItem: ActionSingleUseBase
{		
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}
		
	override void OnStartClient(ActionData action_data)
	{
		GravityGun gravity_gun;
		if (Class.CastTo(gravity_gun, action_data.m_MainItem)) {
			gravity_gun.TryPickUpItem(action_data.m_Target.GetObject());
		}
	}
	
	override void OnStartServer(ActionData action_data)
	{
		GravityGun gravity_gun;
		if (Class.CastTo(gravity_gun, action_data.m_MainItem)) {
			gravity_gun.TryPickUpItem(action_data.m_Target.GetObject());
		}
	}

	override bool HasTarget()
	{
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return true;
	}
	
	override typename GetInputType()
	{
		return DefaultActionInput;
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override string GetText()
	{
		return "Yoink";
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionGravityGunPickUpItem);
		actions.Insert(ActionGravityGunLaunchItem);
	}
}

class GravityGunLight: PointLightBase
{
	void GravityGunLight()
	{
		SetVisibleDuringDaylight(true);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetBrightnessTo(3);
		SetRadiusTo(3);
		// 255, 62, 0
		SetAmbientColor(1.0, 0.24, 0);
		SetDiffuseColor(1.0, 0.24, 0);
	}
}