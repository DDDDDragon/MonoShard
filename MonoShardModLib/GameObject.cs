using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MonoShardModLib
{
    public abstract class GameObject
    {
        public int Self { get; internal set; }
        
        public int Other { get; internal set; }

        public object this[string type, string name]
        {
            get
            {
                switch (type)
                {
                    case "double":
                        return Core.GetInstancePropertyAsInteger(name, Self, Other);
                    default: return null;
                }
            }
        }
    }
}
