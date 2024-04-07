using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace MonoShardModLib.ModUtils
{
    public class Mod
    {
        public virtual bool IsEnable { get; set; }

        public ModFiles? ModFiles { get; set; }

        public Mod? Instance { get; set; }

        public override string ToString() => Name;

        public virtual string Name { get => GetType().Name; }

        public virtual string Author { get => "N/A"; }

        public virtual string Description { get => "N/A"; }

        public virtual string Version { get => "v0.0.0.0"; }

        public virtual string TargetVersion { get => "v0.0.0.0"; }

        public Mod() { }
    }
    public enum ModLanguage
    {
        Russian,
        English,
        Chinese,
        German,
        Spanish,
        French,
        Italian,
        Portuguese,
        Polish,
        Turkish,
        Japanese,
        Korean
    }
}
