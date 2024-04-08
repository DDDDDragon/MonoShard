using System;
using MonoShardModLib.ItemUtils;
using MonoShardModLib.PlayerUtils;

namespace MonoShardModLib
{
    public class Game
    {
        public void PreUpdatePlayer()
        {
            if (Player.Instance == null) return;
            var hp = Player.Instance.HP;
            if (hp < 50) Player.Instance.HP = 100;
        }

        public void PostUpdatePlayer()
        {
            if (Player.Instance == null) return;
            var hp = Player.Instance.HP;
            if (hp < 50) Player.Instance.HP = 100;
        }

        public void InitializeWeapon(Weapon weapon)
        {
            if(weapon.IsMod)
            {

            }
            else
            {
                if (weapon.IdName == "Travelling Staff")
                {
                    Console.WriteLine("You have travelling staff");
                    weapon.BluntDamage = 1000;
                }
            }
        }
    }
}