using MonoShardModLib.ModUtils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MonoShardModLib
{
    public abstract class GameObject : ILoadable
    {
        public virtual string Name => "";

        public int Self { get; internal set; }
        
        public int Other { get; internal set; }

        public int InstanceID { get; internal set; }

        public bool IsMod { get => ModLoader.ModObjects.Find(t => t.Name == (Name == "" ? GetType().Name : Name)) != default; }

        public Mod? Mod { get; internal set; }

        public GameObject(int self, int other)
        {
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
                        return Core.GetInstancePropertyAsInteger(name, Self, Other);
                    case "string":
                        return Core.GetInstancePropertyAsString(name, Self, Other);
                    default: return null;
                }
            }

            set
            {
                switch(type)
                {
                    case "double":
                        Core.SetInstancePropertyAsInteger(name, (float)value, Self, Other);
                        break;
                    case "string":
                        Core.SetInstancePropertyAsString(name, (string)value, Self, Other);
                        break;
                    default: return;
                }
            }
        }

        void ILoadable.Load(Mod mod)
        {
            Mod = mod;
        }

        public virtual void Load() { }

        public virtual void Register() { }
    }
}
