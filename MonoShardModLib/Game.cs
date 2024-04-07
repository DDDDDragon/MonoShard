using System;
using MonoShardModLib.ItemUtils;
using MonoShardModLib.PlayerUtils;

namespace MonoShardModLib
{
    public class Game
    {
        public Game Instance { get; private set; }

        public Game()
        {
            Instance = this;
        }

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
            if (weapon.IsMod)
            {
                weapon.SetWeaponDefaults();
            }

            foreach (var hook in ModLoader.Hooks)
                hook.InitializeWeapon(weapon);
        }
    }
}