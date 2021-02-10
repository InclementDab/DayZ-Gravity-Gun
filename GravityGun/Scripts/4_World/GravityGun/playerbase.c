modded class PlayerBase extends ManBase
{
    override void Init()
    {
        if ( !GetGame().IsServer() || !GetGame().IsMultiplayer() ) 
        {

		    DayzPlayerItemBehaviorCfg 	fireArmsItemBehaviour = new DayzPlayerItemBehaviorCfg;
		    fireArmsItemBehaviour.SetFirearms();

            GetDayZPlayerType().AddItemInHandsProfileIK("GravityGun", "dz/anims/workspaces/player/player_main/weapons/player_main_repeater.asi", fireArmsItemBehaviour, "dz/anims/anm/player/ik/weapons/repeater.anm");
        }
        
        super.Init();
    }
}