
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
	
	protected ref Timer m_UpdateTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
	protected Object m_ObjectUnderCrosshair;
	protected Object m_HoldingObject;
	
	
	void GravityGun()
	{
		m_UpdateTimer.Run(0.02, this, "OnUpdateTimer", null, true);
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
		}
	}
	
	void TryPickUpItem()
	{
		if (!m_ObjectUnderCrosshair) {
			Print("No item to pick up!");
			return;
		}
		
		Print("Trying to pick up " + m_ObjectUnderCrosshair);
		
		if (m_HoldingObject) {
			Print("Already holding item");
			TryDropItem();
			return;
		}
		
		m_HoldingObject = m_ObjectUnderCrosshair;
		if (dBodyIsDynamic(m_HoldingObject)) {
			m_HoldingObject.SetDynamicPhysicsLifeTime(0.01);
		}
	}
	
	void TryDropItem()
	{
		m_HoldingObject.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);
		m_HoldingObject.SetDynamicPhysicsLifeTime(15.0);
		m_HoldingObject = null;
	}
	
	void TryLaunchItem(Object target)
	{
		m_HoldingObject = null;
		if (target) {
			target.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);
			target.SetDynamicPhysicsLifeTime(15.0);
			dBodyApplyImpulse(target, GetGame().GetCurrentCameraDirection() * 500);
			return;
		}
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
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
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
			gravity_gun.TryPickUpItem();
		}
	}
	
	override void OnStartServer(ActionData action_data)
	{
		GravityGun gravity_gun;
		if (Class.CastTo(gravity_gun, action_data.m_MainItem)) {
			gravity_gun.TryPickUpItem();
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

class GravityGunLaunchInput: DefaultActionInput
{
	void GravityGunLaunchInput(PlayerBase player)
	{
		SetInput("UAWalkRunToggle");
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