modded class JMAnimRegister
{
    override void RegisterFireArms(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        pType.AddItemInHandsProfileIK("GravityGun", "dz/anims/workspaces/player/player_main/weapons/player_main_repeater.asi", pBehavior, "dz/anims/anm/player/ik/weapons/repeater.anm");
    }
}