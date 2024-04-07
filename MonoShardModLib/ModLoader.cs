﻿using MonoShardModLib.Hooks;
using MonoShardModLib.ModUtils;
using System;
using System.Collections.Generic;
using System.IO;

namespace MonoShardModLib
{
    public class ModLoader
    {
        public static string GenPath => Core.GetGenPath();
        public static string ModPath => Path.Join(GenPath, "Mods");
        public static string ModSourcePath => Path.Join(GenPath, "ModSources");

        public static List<Mod> Mods = new();
        public static List<Mod> EnabledMods = new();
        public static List<Type> ModObjects = new();
        public static List<GameHooks> Hooks = new();

        public static void Initialize()
        {
            //if (!Directory.Exists(ModPath)) Directory.CreateDirectory(ModPath);
            //if (!Directory.Exists(ModSourcePath)) Directory.CreateDirectory(ModSourcePath);
            Console.WriteLine("Successfully run MonoShard.");

            Console.WriteLine(ModPath);

            //ModPacker.Pack(Path.Combine(ModSourcePath, "testMod"));
        }

        public void LoadAllMods()
        {
            var mods = Directory.GetFiles(ModPath, "*.sml");
        }
    }
}