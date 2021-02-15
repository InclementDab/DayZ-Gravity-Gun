modded class ItemBase
{
	GravityGun Grav;

	void ItemBase()
	{
		SetEventMask(EntityEvent.SIMULATE);
	}

	override void EOnSimulate(IEntity owner, float dt)
	{
		if (Grav && dBodyIsDynamic(this)) Grav.ControlObject(dt);
	}
};

modded class CarScript
{
	GravityGun Grav;

	void CarScript()
	{
		SetEventMask(EntityEvent.SIMULATE);
	}

	override void EOnSimulate(IEntity owner, float dt)
	{
		if (Grav && dBodyIsDynamic(this)) Grav.ControlObject(dt);
	}
};

modded class PlayerBase
{
	GravityGun Grav;

	void PlayerBase()
	{
		SetEventMask(EntityEvent.SIMULATE);
	}

	override void EOnSimulate(IEntity owner, float dt)
	{
		if (Grav && dBodyIsDynamic(this)) Grav.ControlKinematic(dt);
	}
};

modded class ZombieBase
{
	GravityGun Grav;

	void ZombieBase()
	{
		SetEventMask(EntityEvent.SIMULATE);
	}

	override void EOnSimulate(IEntity owner, float dt)
	{
		if (Grav && dBodyIsDynamic(this)) Grav.ControlKinematic(dt);
	}
};

modded class AnimalBase
{
	GravityGun Grav;

	void AnimalBase()
	{
		SetEventMask(EntityEvent.SIMULATE);
	}

	override void EOnSimulate(IEntity owner, float dt)
	{
		if (Grav && dBodyIsDynamic(this)) Grav.ControlKinematic(dt);
	}
};

class GravityGun: ItemBase
{
	static const float GUN_MIN_RANGE = 0.5;
	static const float GUN_PICKUP_RANGE = 10.0;
	static const float GUN_HOLD_RANGE = 1;
	static const float GUN_POWER = 250;
	
	protected ref Timer m_UpdateTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

	ScriptedLightBase m_GunLight;
	
	PlayerBase m_Player;

	int m_TimeDynamicCreated;
	Entity m_HoldingObject;

	float m_AimLR;
	float m_AimUD;

	vector m_AimDirection;
	vector m_AimPosition;
	vector m_ObjectTargetPosition;
	
	void GravityGun()
	{
		if (IsMissionClient()) {
			m_GunLight = ScriptedLightBase.CreateLight(GravityGunLight, ModelToWorld(GetMemoryPointPos("ce_radius")));
			m_GunLight.AttachOnMemoryPoint(this, "ce_radius");
		}
		
		m_UpdateTimer.Run(0.02, this, "OnUpdate", null, true);
	}
	
	void ControlObject(float pDt)
	{
		if (!m_Player) return;

		UpdateTargetPosition();
		
		float mass = dBodyGetMass(m_HoldingObject);
		vector playerVel = GetVelocity(m_Player);
		
		vector trans[4];
		GetTransform(trans);
		
		float distance = Math.Clamp(vector.Distance(GetPosition(), m_ObjectTargetPosition) - 2.0, 1.0, 10.0);
		
		trans[3] = m_ObjectTargetPosition + (playerVel * pDt * 3.0) + (GetVelocity(m_HoldingObject) * distance * 100.0);
		
		float playerVelLen = playerVel.Length();
		float timeToMove = mass * 0.1 * distance;
		if (playerVelLen > 1.0) timeToMove = timeToMove / playerVelLen;
		if (timeToMove < 1.0) timeToMove = 1.0;
		
		dBodySetTargetMatrix(m_HoldingObject, trans, pDt * timeToMove);
	}
	
	void ControlKinematic(float pDt)
	{
		if (!m_Player) return;

		UpdateTargetPosition();
		
		float mass = dBodyGetMass(m_HoldingObject);
		vector playerVel = GetVelocity(m_Player);
		
		float distance = Math.Clamp(vector.Distance(GetPosition(), m_ObjectTargetPosition) - 2.0, 1.0, 10.0);

		vector currentPosition = m_HoldingObject.GetPosition();
		vector targetPosition = m_ObjectTargetPosition + (playerVel * pDt * 3.0) + (GetVelocity(this) * distance * 100.0);
		
		float playerVelLen = playerVel.Length();
		float timeToMove = mass * 0.1 * distance;
		if (playerVelLen > 1.0) timeToMove = timeToMove / playerVelLen;

		m_HoldingObject.SetPosition(m_ObjectTargetPosition);
	}
	
	void OnUpdate()
	{
		Debug.DestroyAllShapes();
		
		m_Player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!m_Player)
		{
			if (m_HoldingObject) RemoveObjectInField();
			return;
		}
		
		m_AimLR = m_Player.GetCommandModifier_Weapons().GetBaseAimingAngleLR();
		m_AimUD = m_Player.GetCommandModifier_Weapons().GetBaseAimingAngleUD();
		m_AimLR = m_AimLR + m_Player.GetOrientation()[0];
		m_AimDirection = Vector(m_AimLR, m_AimUD, 0).AnglesToVector();
		m_AimPosition = m_Player.GetBonePositionWS(m_Player.GetBoneIndexByName("Head"));
		
		//Class dbg_GG = CF_Debugger_Get("GravityGun", this);
		//CF_Debugger_Display(dbg_GG, "m_HoldingObject", m_HoldingObject);
		//CF_Debugger_Display(dbg_GG, "m_Player.IsRaised()", m_Player.IsRaised());
		//CF_Debugger_Display(dbg_GG, "m_Player.GetPosition()", m_Player.GetPosition());
		
		int state = -1;
		
		if (m_Player.IsRaised() && !m_HoldingObject)
		{
			state = 0;
			
			UpdateHeldTarget();
		}
		else if (!m_Player.IsRaised() && m_HoldingObject)
		{
			state = 1;
			
			RemoveObjectInField();
		}	
		else if (m_HoldingObject)
		{
			state = 2;
			
			//UpdateTargetPosition();
		}
		
		//CF_Debugger_Display(dbg_GG, "state", state);
	}

	void UpdateHeldTarget()
	{
		//Class dbg_GG = CF_Debugger_Get("GravityGun", this);
		
		vector begin = m_AimPosition + (m_AimDirection * GUN_MIN_RANGE);
		vector end = m_AimPosition + (m_AimDirection * GUN_PICKUP_RANGE);
		vector contactPos = end;
		vector contactDir;
		int contactComponent;
		set<Object> results();
		bool hit = DayZPhysics.RaycastRV(begin, end, contactPos, contactDir, contactComponent, results, NULL, m_Player, false, false, ObjIntersectGeom);
				
		//CF_Debugger_Display(dbg_GG, "results", results.Count());
		//CF_Debugger_Display(dbg_GG, "begin", begin);
		//CF_Debugger_Display(dbg_GG, "end", end);
			
		//int colour = COLOR_GREEN;
		//if (hit) colour = COLOR_RED;
		//Debug.DrawLine(begin, contactPos, colour, ShapeFlags.TRANSP);
		
		if (results.Count() > 0) SetObjectInField(Entity.Cast(results[0]));
	}
	
	void UpdateTargetPosition()
	{
		vector minMax[2];
		float radius = m_HoldingObject.ClippingInfo(minMax);
	
		vector begin = m_AimPosition + (m_AimDirection * GUN_MIN_RANGE);
		vector end = m_AimPosition + (m_AimDirection * (GUN_HOLD_RANGE + radius));

		vector contactDir;
		int contactComponent;
		set<Object> results();
		bool hit = DayZPhysics.RaycastRV(begin, end, m_ObjectTargetPosition, contactDir, contactComponent, results, NULL, m_HoldingObject, false, false, ObjIntersectGeom);		
		if (!hit) m_ObjectTargetPosition = end;

		int colour = COLOR_GREEN;
		if (hit) colour = COLOR_RED;
		Debug.DrawLine(begin, m_ObjectTargetPosition, colour, ShapeFlags.TRANSP);
	}
	
	void SetObjectInField(Entity object)
	{
		if (!SetGravityGunOnObject(object, this)) return;
		
		m_HoldingObject = object;

		m_TimeDynamicCreated = m_HoldingObject.GetSimulationTimeStamp();
		if (dBodyIsDynamic(m_HoldingObject)) m_TimeDynamicCreated += 30.0;

		if (!dBodyIsDynamic(m_HoldingObject)) m_HoldingObject.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);

		m_HoldingObject.SetDynamicPhysicsLifeTime(-1);
		
		EffectSound sound;
		PlaySoundSet(sound, "GravityGun_Pickup", 0, 0);

		SetClawState(0);
	}

	private bool SetGravityGunOnObject(Entity object, GravityGun gun)
	{		
		ItemBase item;
		if (Class.CastTo(item, object)) {
			item.Grav = gun;
			return true;
		}
		
		CarScript car;
		if (Class.CastTo(car, object)) {
			car.Grav = gun;
			return true;
		}
		
		PlayerBase player;
		if (Class.CastTo(player, object)) {
			player.Grav = gun;
			return true;
		}
		
		ZombieBase zombie;
		if (Class.CastTo(zombie, object)) {
			zombie.Grav = gun;
			return true;
		}
		
		AnimalBase animal;
		if (Class.CastTo(animal, object)) {
			animal.Grav = gun;
			return true;
		}
		
		return false;
	}
	
	void RemoveObjectInField()
	{
		if (!SetGravityGunOnObject(m_HoldingObject, null)) return;

		m_HoldingObject.SetDynamicPhysicsLifeTime(m_HoldingObject.GetSimulationTimeStamp() - m_TimeDynamicCreated + 30.0);
		m_HoldingObject = null;
		
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
	}
	
	private void LaunchObject(notnull Entity target, bool isSelf = true)
	{
		if (!SetGravityGunOnObject(target, null)) return;
		
		if (!isSelf) target.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);
		
		if (isSelf)	target.SetDynamicPhysicsLifeTime(m_HoldingObject.GetSimulationTimeStamp() - m_TimeDynamicCreated + 30.0);
		else target.SetDynamicPhysicsLifeTime(30.0);
		
		float mass = dBodyGetMass(target);
		float massScale = Math.Clamp(0.2 / mass, 0.1, 2.0);
		
		dBodyApplyImpulse(target, m_AimDirection * GUN_POWER * massScale);
		
		EffectSound sound;
		PlaySoundSet(sound, "GravityGun_Launch", 0, 0);
		CreateCameraShake(1);
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

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		
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