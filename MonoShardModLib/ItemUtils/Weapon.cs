using System.Reflection;

namespace MonoShardModLib.ItemUtils
{
    public class Weapon : Item
    {
        public Weapon(int self, int other) : base(self, other) { }

        public static Weapon CreateInstance(string typeName, int self, int other)
        {
            if(!ModLoader.ModObjects.Exists(t => t.Name == typeName))
                return new Weapon(self, other);
            else
                return Activator.CreateInstance(ModLoader.ModObjects.Find(t => t.Name == typeName), self, other) as Weapon;
        }

        public int BluntDamage
        {
            get => (int)Core.GetValueFromMapAsInteger("Blunt_Damage", Data, Self, Other);
            set => Core.SetValueInMapAsInteger("Blunt_Damage", Data, value, Self, Other);
        }

        public virtual void SetWeaponDefaults() { }
    }
}
