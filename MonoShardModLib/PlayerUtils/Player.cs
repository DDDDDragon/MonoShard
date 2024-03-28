using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace MonoShardModLib.PlayerUtils
{
    public class Player : GameObject
    {
        public static Player Instance = null;

        public Player(int self, int other)
        {
            Instance = this;
            Self = self;
            Other = other;
        }

        public int HP 
        { 
            get => (int)Core.GetInstancePropertyAsInteger("HP", Self, Other);
            set => Core.SetInstancePropertyAsInteger("HP", value, Self, Other);
        }
    }
}
