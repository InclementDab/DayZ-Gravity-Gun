modded class ItemBase
{
	GravityGun Grav;

	void ItemBase()
	{
		SetEventMask(EntityEvent.SIMULATE);
	}

	override void EOnSimulate(IEntity owner, float dt)
	{
		peepee(dt);
	}
	
	void peepee(float pDt)
	{
		if (!Grav || !Grav.m_Ready || !dBodyIsDynamic(this)) return;

		float mass = dBodyGetMass(this);
		vector playerVel = GetVelocity(Grav.m_Player);
		
		vector trans[4];
		GetTransform(trans);
		
		float distance = Math.Clamp(vector.Distance(GetPosition(), Grav.m_ObjectTargetPosition) - 2.0, 1.0, 10.0);
		
		trans[3] = Grav.m_ObjectTargetPosition + (playerVel * pDt * 5.0);
		
		float playerVelLen = playerVel.Length();
		float timeToMove = mass * 0.1 * distance;
		if (playerVelLen > 1.0) timeToMove = timeToMove / playerVelLen;
		
		dBodySetTargetMatrix(this, trans, pDt * timeToMove);
	}
};

class GravityGun: ItemBase
{
	static const float GUN_PICKUP_RANGE = 10.0;
	static const float GUN_HOLD_RANGE = 1;
	static const float GUN_POWER = 250;
	
	protected ref Timer m_UpdateTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

	ScriptedLightBase m_GunLight;
	
	PlayerBase m_Player;
	bool m_Ready;

	int m_TimeDynamicCreated;
	Entity m_HoldingObject;

	float m_AimLR;
	float m_AimUD;

	vector m_AimDirection;
	vector m_ObjectTargetPosition;
	
	void GravityGun()
	{
		if (IsMissionClient()) {
			m_GunLight = ScriptedLightBase.CreateLight(GravityGunLight, ModelToWorld(GetMemoryPointPos("ce_radius")));
			m_GunLight.AttachOnMemoryPoint(this, "ce_radius");
		}
		
		m_UpdateTimer.Run(0.02, this, "OnUpdateTimer", null, true);
	}
	
	void OnUpdateTimer()
	{
		//Debug.DestroyAllShapes();
		
		m_Player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!m_Player) return;
		if (!m_HoldingObject) return;

		m_AimLR = m_Player.GetCommandModifier_Weapons().GetBaseAimingAngleLR();
		m_AimUD = m_Player.GetCommandModifier_Weapons().GetBaseAimingAngleUD();
		
		m_AimLR = m_AimLR + m_Player.GetOrientation()[0];

		m_AimDirection = Vector(m_AimLR, m_AimUD, 0).AnglesToVector();
		
		//Class dbg_GG = CF_Debugger_Get("GravityGun", this);
		//CF_Debugger_Display(dbg_GG, "AimLR", m_AimLR);
		//CF_Debugger_Display(dbg_GG, "AimUD", m_AimUD); 
		//CF_Debugger_Display(dbg_GG, "AimDirection", m_AimDirection); 
		
		vector begin = m_Player.GetBonePositionWS(m_Player.GetBoneIndexByName("Head"));
		vector end = begin + (m_AimDirection * GUN_HOLD_RANGE);		
		begin = begin + (m_AimDirection * 0.5);		
		
		Object hit_obj;
		vector contact_dir;
		float hit_fract;

		PhxInteractionLayers collisionLayerMask = PhxInteractionLayers.BUILDING | PhxInteractionLayers.DYNAMICITEM | PhxInteractionLayers.TERRAIN | PhxInteractionLayers.ROADWAY;

		bool hit = false;//DayZPhysics.RayCastBullet(begin, end, collisionLayerMask, m_HoldingObject, hit_obj, m_ObjectTargetPosition, contact_dir, hit_fract);
		if (!hit) m_ObjectTargetPosition = end;

		int colour = COLOR_GREEN;
		if (hit) colour = COLOR_RED;
		//Debug.DrawLine(begin, m_ObjectTargetPosition, colour, ShapeFlags.NOZBUFFER | ShapeFlags.TRANSP);
		
		m_Ready = true;
	}
	
	void SetObjectInField(notnull Entity object)
	{		
		m_HoldingObject = object;
		
		OnUpdateTimer();

		m_TimeDynamicCreated = m_HoldingObject.GetSimulationTimeStamp();
		if (dBodyIsDynamic(m_HoldingObject)) m_TimeDynamicCreated += 30.0;

		if (!dBodyIsDynamic(m_HoldingObject)) m_HoldingObject.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);

		m_HoldingObject.SetDynamicPhysicsLifeTime(-1);
		
		EffectSound sound;
		PlaySoundSet(sound, "GravityGun_Pickup", 0, 0);

		SetClawState(0);

		SetGravityGunOnObject(m_HoldingObject, this);
	}

	private void SetGravityGunOnObject(Entity object, GravityGun gun)
	{
		ItemBase itembase;
		if (!Class.CastTo(itembase, object))
		{

		} else
		{
			itembase.Grav = gun;
		}
	}
	
	void RemoveObjectInField()
	{
		SetGravityGunOnObject(m_HoldingObject, null);

		m_HoldingObject.SetDynamicPhysicsLifeTime(m_HoldingObject.GetSimulationTimeStamp() - m_TimeDynamicCreated + 30.0);
		m_HoldingObject = null;
		m_Ready = false;
		
		SetClawState(1);
	}
	
	void LaunchObjectInField(Entity target)
	{
		if (m_HoldingObject) {
			SetGravityGunOnObject(m_HoldingObject, null);
			LaunchObject(m_HoldingObject);
			m_HoldingObject = null;
		} else if (target) {
			LaunchObject(target);
		}

		SetClawState(1);
		m_Ready = false;
	}
	
	private void LaunchObject(notnull Entity target, bool isSelf = true)
	{
		SetGravityGunOnObject(target, null);
		
		if (!isSelf) target.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);
		
		if (isSelf)	target.SetDynamicPhysicsLifeTime(m_HoldingObject.GetSimulationTimeStamp() - m_TimeDynamicCreated + 30.0);
		else target.SetDynamicPhysicsLifeTime(30.0);
		
		dBodyApplyImpulse(target, GetGame().GetCurrentCameraDirection() * GUN_POWER);
		
		EffectSound sound;
		PlaySoundSet(sound, "GravityGun_Launch", 0, 0);
		CreateCameraShake(1);
		
		m_Ready = false;
	}
		
	void CreateCameraShake(float intensity)
	{
		GetGame().GetPlayer().GetCurrentCamera().SpawnCameraShake(Math.Clamp(intensity, 0.2, 1), 2, 5, 10);
	}

	void SetClawState(float value)
	{
		SetAnimationPhase("gravitygun", value);
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
			gravity_gun.LaunchObjectInField(Entity.Cast(action_data.m_Target.GetObject()));
		}
	}
	
	override void OnStartServer(ActionData action_data)
	{
		GravityGun gravity_gun;
		if (Class.CastTo(gravity_gun, action_data.m_MainItem)) {
			gravity_gun.LaunchObjectInField(Entity.Cast(action_data.m_Target.GetObject()));
		}
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		GravityGun gravity_gun;
		if (!Class.CastTo(gravity_gun, item)) return false;
		
		if (!gravity_gun.m_HoldingObject) return false;
		
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
			gravity_gun.SetObjectInField(Entity.Cast(action_data.m_Target.GetObject()));
		}
	}
	
	override void OnStartServer(ActionData action_data)
	{
		GravityGun gravity_gun;
		if (Class.CastTo(gravity_gun, action_data.m_MainItem)) {
			gravity_gun.SetObjectInField(Entity.Cast(action_data.m_Target.GetObject()));
		}
	}

	override bool HasTarget()
	{
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		GravityGun gravity_gun;
		if (!Class.CastTo(gravity_gun, item)) return false;
		
		if (gravity_gun.m_HoldingObject) return false;
		
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