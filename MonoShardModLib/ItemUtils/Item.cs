using MonoShardModLib.ModUtils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MonoShardModLib.ItemUtils
{
    public class Item : GameObject
    {
        public Item(int self, int other) : base(self, other) { }

        public int Data
        {
            get => (int)Core.GetInstancePropertyAsInteger("data", Self, Other);
        }

        public string IdName
        {
            get => Core.GetInstancePropertyAsString("idName", Self, Other);
            set => Core.SetInstancePropertyAsString("idName", value, Self, Other);
        }
    }
}