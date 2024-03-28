using System;
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

        public void InitializeWeapon(string ID)
        {

        }
    }
}