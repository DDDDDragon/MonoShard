using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace MonoShardModLib.PlayerUtils
{
    public class Player
    {
        public static Player Instance = null;

        internal int Self { get; }
        internal int Other { get; }

        public Player(int self, int other)
        {
            Instance = this;
            Self = self;
            Other = other;
        }

        public object this[string type, string name]
        {
            get
            {
                switch (type)
                {
                    case "double":
                        return Core.GetInstancePropertyAsInteger(name, (int)Self, (int)Other);
                    default: return null;
                }
            }
        }

        public int HP 
        { 
            get => (int)Core.GetInstancePropertyAsInteger("HP", Self, Other);
            set => Core.SetInstancePropertyAsInteger("HP", value, Self, Other);
        }
    }
}
