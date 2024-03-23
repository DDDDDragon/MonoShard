using System;
using System.IO;

namespace MonoShardModLib
{
    public class ModLoader
    {
        public string GenPath { get => Core.GetGenPath(); }
        public string ModPath => Path.Combine(GenPath, "Mods");

        public void Initialize()
        {
            if(!Directory.Exists(ModPath)) Directory.CreateDirectory(ModPath);
            Console.WriteLine(GenPath);
        }
    }
}